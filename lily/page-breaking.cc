/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2006--2009 Joe Neeman <joeneeman@gmail.com>

  LilyPond is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LilyPond is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with LilyPond.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
  This is a utility class for page-breaking algorithms. There are some complex
  parts of this class, some of which are useful to understand if you intend
  to write a page breaking algorithm (ie. a subclass of Page_breaking). Most
  of these complexities were introduced in order to break the problem of
  page-breaking into simpler subproblems and to hide some of the bookkeeping
  complexities of page breaking from the page breaking algorithms.

  COMPRESSED LINES
  There are several functions that actually distribute systems across pages
  (for example, the space_systems_XXX and pack_systems_XXX functions). If
  each of these functions had to handle \noPageBreak, it would be a mess.
  Therefore, we handle \noPageBreak by "compressing" the list of systems
  before doing any layout: we concatenate any two systems separated by a
  \noPageBreak into a single system. The page-breaking functions can do their
  magic without encountering a \noPageBreak; we then "uncompress" the systems
  at the end. We almost always work with cached_line_details_, which are
  "compressed."

  CHUNKS
  The basic operation of a page breaking algorithm is to repeatedly request
  some systems from the line-breaker and place those systems on some pages.
  With each repetition, the page breaking algorithm asks the line-breaker for
  some systems that it thinks will help it achieve a better layout. The
  Page_breaking class provides functionality to facilitate this in the case
  that the page breaking algorithm only cares about the number of systems.

  Even if a page breaking algorithm only cares number of systems, there may
  be many ways to satisfy its request. For example, in a piece with 2 scores
  and a request for 10 systems, we could return 5 systems from each score or
  4 from the first and 6 from the second. Even within a score, we might
  want to try several different line breaking configurations with a fixed
  system count; if there is a forced \pageBreak, for example, we might wish
  to tweak the number of systems on both sides of the \pageBreak independently.

  The Page_breaking class takes care of finding these configurations. It
  divides the piece into "chunks" and sets up the line-breaker in such a way
  that the number of systems in each chunk can be modified independently.
  Chunks never cross score boundaries; each title and markup is its own chunk.
  When a page breaking algorithm requests a number of systems, the Page_breaker
  stores an array of potential configurations, which the page breaking
  algorithm can iterate over using current_configuration(vsize).

  LINE_DIVISION
  A Line_division is simply a way of storing the exact way in which the
  total number of systems is distributed among chunks. Note that a
  Line_division may not (in fact, usually will not) describe all of the chunks
  in the entire book. Rather, it will describe the subset of chunks that lie
  between some fixed starting and ending point. This subset of chunks changes
  whenever a page breaking algorithm asks to consider a different pair of
  starting and ending breakpoints. In particular, a Line_division should be
  discarded after a call to set_current_breakpoints, since that Line_division
  refers to a subset of chunks which might be different from the current
  subset of chunks under consideration.
*/

#include "page-breaking.hh"

#include "international.hh"
#include "item.hh"
#include "output-def.hh"
#include "page-layout-problem.hh"
#include "page-spacing.hh"
#include "paper-book.hh"
#include "paper-score.hh"
#include "paper-system.hh"
#include "system.hh"
#include "warn.hh"

/* for each forbidden page break, merge the systems around it into one
   system. */
static vector<Line_details>
compress_lines (const vector<Line_details> &orig)
{
  vector<Line_details> ret;

  for (vsize i = 0; i < orig.size (); i++)
    {
      if (ret.size () && !scm_is_symbol (ret.back ().page_permission_))
	{
	  Line_details const &old = ret.back ();
	  Line_details compressed = orig[i];
	  Real padding = orig[i].title_ ? old.title_padding_ : old.padding_;

	  compressed.extent_[DOWN] = old.extent_[DOWN];
	  compressed.extent_[UP] = old.extent_[UP] + orig[i].extent_.length () + padding;
	  compressed.space_ += old.space_;
	  compressed.inverse_hooke_ += old.inverse_hooke_;

	  compressed.compressed_lines_count_ = old.compressed_lines_count_ + 1;
	  compressed.compressed_nontitle_lines_count_ =
	    old.compressed_nontitle_lines_count_ + (compressed.title_ ? 0 : 1);

	  // compressed.title_ is true if and only if the first of its
	  // compressed lines was a title.
	  compressed.title_ = old.title_;
	  ret.back () = compressed;
	}
      else
	{
	  ret.push_back (orig[i]);
	  ret.back ().force_ = 0;
	}
    }
  return ret;
}

/* translate the number of systems-per-page into something meaningful for
   the uncompressed lines.
*/
static vector<vsize>
uncompress_solution (vector<vsize> const &systems_per_page,
		     vector<Line_details> const &compressed)
{
  vector<vsize> ret;
  vsize start_sys = 0;

  for (vsize i = 0; i < systems_per_page.size (); i++)
    {
      int compressed_count = 0;
      for (vsize j = start_sys; j < start_sys + systems_per_page[i]; j++)
	compressed_count += compressed[j].compressed_lines_count_ - 1;

      ret.push_back (systems_per_page[i] + compressed_count);
      start_sys += systems_per_page[i];
    }
  return ret;
}

/* for Page_breaking, the start index (when we are dealing with the stuff
   between a pair of breakpoints) refers to the break_ index of the end of
   the previous page. So the system index of the start of the current page
   could either be the same as the end of the previous page or one more than
   it. */

/* Turn a break index into the sys index that starts the next page */
vsize
Page_breaking::next_system (Break_position const &break_pos) const
{
  vsize sys = break_pos.system_spec_index_;

  if (sys == VPOS) /* beginning of the book */
    return 0;
  if (system_specs_[sys].pscore_ && !break_pos.score_ender_)
    return sys; /* the score overflows the previous page */
  return sys + 1; /* this page starts with a new System_spec */
}

Page_breaking::Page_breaking (Paper_book *pb, Break_predicate is_break)
{
  book_ = pb;
  system_count_ = 0;
  ragged_ = to_boolean (pb->paper_->c_variable ("ragged-bottom"));
  ragged_last_ = to_boolean (pb->paper_->c_variable ("ragged-last-bottom"));
  systems_per_page_ = max (0, robust_scm2int (pb->paper_->c_variable ("systems-per-page"), 0));
  max_systems_per_page_ = max (0, robust_scm2int (pb->paper_->c_variable ("max-systems-per-page"), 0));
  min_systems_per_page_ = max (0, robust_scm2int (pb->paper_->c_variable ("min-systems-per-page"), 0));
  orphan_penalty_ = robust_scm2int (pb->paper_->c_variable ("orphan-penalty"), 100000);

  if (systems_per_page_ && (max_systems_per_page_ || min_systems_per_page_))
    {
      warning (_f ("ignoring min-systems-per-page and max-systems-per-page because systems-per-page was set"));
      min_systems_per_page_ = max_systems_per_page_ = 0;
    }
  if (max_systems_per_page_ && min_systems_per_page_ > max_systems_per_page_)
    {
      warning (_f ("min-systems-per-page is larger than max-systems-per-page, ignoring both values"));
      min_systems_per_page_ = max_systems_per_page_ = 0;
    }

  create_system_list ();
  find_chunks_and_breaks (is_break);
}

Page_breaking::~Page_breaking ()
{
}

bool
Page_breaking::ragged () const
{
  return ragged_;
}

bool
Page_breaking::ragged_last () const
{
  return ragged_last_;
}

int
Page_breaking::systems_per_page () const
{
  return systems_per_page_;
}

int
Page_breaking::max_systems_per_page () const
{
  return max_systems_per_page_;
}

int
Page_breaking::min_systems_per_page () const
{
  return min_systems_per_page_;
}

vsize
Page_breaking::system_count () const
{
  return system_count_;
}

bool
Page_breaking::too_many_lines (int line_count) const
{
  return max_systems_per_page () > 0 && line_count > max_systems_per_page ();
}

bool
Page_breaking::too_few_lines (int line_count) const
{
  return line_count < min_systems_per_page ();
}

Real
Page_breaking::line_count_penalty (int line_count) const
{
  if (too_many_lines (line_count))
    return (line_count - max_systems_per_page ()) * TERRIBLE_SPACING_PENALTY;
  if (too_few_lines (line_count))
    return (min_systems_per_page () - line_count) * TERRIBLE_SPACING_PENALTY;

  return 0;
}

int
Page_breaking::line_count_status (int line_count) const
{
  if (too_many_lines (line_count))
    return SYSTEM_COUNT_TOO_MANY;
  if (too_few_lines (line_count))
    return SYSTEM_COUNT_TOO_FEW;

  return SYSTEM_COUNT_OK;
}

/* translate indices into breaks_ into start-end parameters for the line breaker */
void
Page_breaking::line_breaker_args (vsize sys,
				  Break_position const &start,
				  Break_position const &end,
				  vsize *line_breaker_start,
				  vsize *line_breaker_end)
{
  assert (system_specs_[sys].pscore_);
  assert (next_system (start) <= sys && sys <= end.system_spec_index_);

  if (start.system_spec_index_ == sys)
    *line_breaker_start = start.score_break_;
  else
    *line_breaker_start = 0;

  if (end.system_spec_index_ == sys)
    *line_breaker_end = end.score_break_;
  else
    *line_breaker_end = VPOS;
}

void
Page_breaking::break_into_pieces (vsize start_break, vsize end_break,
				  Line_division const &div)
{
  vector<Break_position> chunks = chunk_list (start_break, end_break);
  bool ignore_div = false;
  if (chunks.size () != div.size () + 1)
    {
      programming_error ("did not find a valid page breaking configuration");
      ignore_div = true;
    }

  for (vsize i = 0; i + 1 < chunks.size (); i++)
    {
      vsize sys = next_system (chunks[i]);
      if (system_specs_[sys].pscore_)
	{
	  vsize start;
	  vsize end;
	  line_breaker_args (sys, chunks[i], chunks[i+1], &start, &end);

	  vector<Column_x_positions> pos = ignore_div
	    ? line_breaking_[sys].best_solution (start, end)
	    : line_breaking_[sys].solve (start, end, div[i]);
	  system_specs_[sys].pscore_->root_system ()->break_into_pieces (pos);
	}
    }
}

SCM
Page_breaking::systems ()
{
  SCM ret = SCM_EOL;
  for (vsize sys = 0; sys < system_specs_.size (); sys++)
    {
      if (system_specs_[sys].pscore_)
	{
	  system_specs_[sys].pscore_->root_system ()
	    ->do_break_substitution_and_fixup_refpoints ();
	  SCM lines = system_specs_[sys].pscore_->root_system ()
	    ->get_broken_system_grobs ();
	  ret = scm_cons (lines, ret);
	}
      else
	{
	  Prob *pb = system_specs_[sys].prob_;
	  ret = scm_cons (scm_list_1 (pb->self_scm ()), ret);
	  pb->unprotect ();
	}
    }
  return scm_append (scm_reverse (ret));
}

SCM
Page_breaking::make_page (int page_num, bool last) const
{
  bool last_part = ly_scm2bool (book_->paper_->c_variable ("is-last-bookpart"));
  SCM mod = scm_c_resolve_module ("scm page");
  SCM make_page_scm = scm_c_module_lookup (mod, "make-page");

  make_page_scm = scm_variable_ref (make_page_scm);

  return scm_apply_0 (make_page_scm,
		      scm_list_n (book_->self_scm (),
				  ly_symbol2scm ("page-number"), scm_from_int (page_num),
				  ly_symbol2scm ("is-last-bookpart"), scm_from_bool (last_part),
				  ly_symbol2scm ("is-bookpart-last-page"), scm_from_bool (last),
				  SCM_UNDEFINED));
}

Real
Page_breaking::page_height (int page_num, bool last) const
{
  SCM mod = scm_c_resolve_module ("scm page");
  SCM page = make_page (page_num, last);
  SCM calc_height = scm_c_module_lookup (mod, "calc-printable-height");
  calc_height = scm_variable_ref (calc_height);

  SCM height = scm_apply_1 (calc_height, page, SCM_EOL);
  return scm_to_double (height);
}

SCM
Page_breaking::breakpoint_property (vsize breakpoint, char const *str)
{
  Break_position const &pos = breaks_[breakpoint];

  if (pos.system_spec_index_ == VPOS)
    return SCM_EOL;
  if (system_specs_[pos.system_spec_index_].pscore_)
    return pos.col_->get_property (str);
  return system_specs_[pos.system_spec_index_].prob_->get_property (str);
}

SCM
Page_breaking::get_page_configuration (SCM systems, int page_num, bool ragged, bool last)
{
  SCM dummy_page = make_page (page_num, last);
  Page_layout_problem layout (book_, dummy_page, systems);
  return scm_is_pair (systems) ? layout.solution (ragged) : SCM_EOL;
}

SCM
Page_breaking::draw_page (SCM systems, SCM configuration, int page_num, bool last)
{
  // Create a stencil for each system.
  SCM paper_systems = SCM_EOL;
  for (SCM s = scm_reverse (systems); scm_is_pair (s); s = scm_cdr (s))
    {
      SCM paper_system = scm_car (s);
      if (Grob *g = unsmob_grob (scm_car (s)))
	{
	  System *sys = dynamic_cast<System*> (g);
	  paper_system = sys->get_paper_system ();
	}

      paper_systems = scm_cons (paper_system, paper_systems);
    }

  // Create the page and draw it.
  SCM page = make_page (page_num, last);
  SCM page_module = scm_c_resolve_module ("scm page");
  SCM page_stencil = scm_c_module_lookup (page_module, "page-stencil");
  page_stencil = scm_variable_ref (page_stencil);

  Prob *p = unsmob_prob (page);
  p->set_property ("lines", paper_systems);
  p->set_property ("configuration", configuration);
  scm_apply_1 (page_stencil, page, SCM_EOL);

  return page;
}

SCM
Page_breaking::make_pages (vector<vsize> lines_per_page, SCM systems)
{
  int first_page_number
    = robust_scm2int (book_->paper_->c_variable ("first-page-number"), 1);
  SCM ret = SCM_EOL;
  SCM label_page_table = book_->top_paper ()->c_variable ("label-page-table");
  if (label_page_table == SCM_UNDEFINED)
    label_page_table = SCM_EOL;

  // Build a list of (systems . configuration) pairs. Note that we lay out
  // the staves and find the configurations before drawing anything. Some
  // grobs (like tuplet brackets) look at their neighbours while drawing
  // themselves. If this happens before the neighbouring staves have
  // been laid out, bad side-effects could happen (in particular,
  // Align_interface::align_to_ideal_distances might be called).
  SCM systems_and_configs = SCM_EOL;

  for (vsize i = 0; i < lines_per_page.size (); i++)
    {
      int page_num = i + first_page_number;
      bool bookpart_last_page = (i == lines_per_page.size () - 1);
      bool rag = ragged () || (bookpart_last_page && ragged_last ());
      SCM line_count = scm_from_int (lines_per_page[i]);
      SCM lines = scm_list_head (systems, line_count);
      SCM config = get_page_configuration (lines, page_num, rag, bookpart_last_page);

      systems_and_configs = scm_cons (scm_cons (lines, config), systems_and_configs);
      systems = scm_list_tail (systems, line_count);
    }

  // Now it's safe to make the pages.
  int page_num = first_page_number + lines_per_page.size () - 1;
  for (SCM s = systems_and_configs; scm_is_pair (s); s = scm_cdr (s))
    {
      SCM lines = scm_caar (s);
      SCM config = scm_cdar (s);
      bool bookpart_last_page = (s == systems_and_configs);
      SCM page = draw_page (lines, config, page_num, bookpart_last_page);

      /* collect labels */
      SCM page_num_scm = scm_from_int (page_num);
      for (SCM l = lines ; scm_is_pair (l)  ; l = scm_cdr (l))
	{
	  SCM labels = SCM_EOL;
	  if (Grob * line = unsmob_grob (scm_car (l)))
	    {
	      System *system = dynamic_cast<System*> (line);
	      labels = system->get_property ("labels");
	    }
	  else if (Prob *prob = unsmob_prob (scm_car (l)))
	    labels = prob->get_property ("labels");

	  for (SCM lbls = labels ; scm_is_pair (lbls) ; lbls = scm_cdr (lbls))
	    label_page_table = scm_cons (scm_cons (scm_car (lbls), page_num_scm),
					 label_page_table);
	}

      ret = scm_cons (page, ret);
      --page_num;
    }
  book_->top_paper ()->set_variable (ly_symbol2scm ("label-page-table"), label_page_table);
  return ret;
}

/* The page-turn-page-breaker needs to have a line-breaker between any two
   columns with non-NULL page-turn-permission.

   The optimal-breaker needs to have a line-breaker between any two columns
   with page-break-permission = 'force.

   By using a grob predicate, we can accommodate both of these uses.
*/
void
Page_breaking::create_system_list ()
{
  SCM specs = book_->get_system_specs ();
  for (SCM s = specs; scm_is_pair (s); s = scm_cdr (s))
    {
      if (Paper_score *ps = dynamic_cast<Paper_score*> (unsmob_music_output (scm_car (s))))
	{
	  system_specs_.push_back (System_spec (ps));
	}
      else
        {
          Prob *pb = unsmob_prob (scm_car (s));
          assert (pb);

          pb->protect ();
          system_specs_.push_back (System_spec (pb));
        }
    }
}

void
Page_breaking::find_chunks_and_breaks (Break_predicate is_break)
{
  SCM force_sym = ly_symbol2scm ("force");

  chunks_.push_back (Break_position ());
  breaks_.push_back (Break_position ());

  for (vsize i = 0; i < system_specs_.size (); i++)
    {
      if (system_specs_[i].pscore_)
	{
	  vector<Grob*> cols;

	  SCM system_count = system_specs_[i].pscore_->layout ()->c_variable ("system-count");
	  if (scm_is_number (system_count))
	    {
	      // With system-count given, the line configuration for
	      // this score is fixed.  We need to ensure that chunk
	      // boundaries only occur at line breaks.
	      Constrained_breaking breaking (system_specs_[i].pscore_);
	      vector<Line_details> details = breaking.line_details (0, VPOS, scm_to_int (system_count));

	      cols.push_back (system_specs_[i].pscore_->root_system ()->used_columns ()[0]);
	      for (vsize j = 0; j < details.size (); j++)
		cols.push_back (details[j].last_column_);
	    }
	  else
	    cols = system_specs_[i].pscore_->root_system ()->used_columns ();

	  int last_chunk_idx = 0;
	  vector<vsize> line_breaker_columns;
	  line_breaker_columns.push_back (0);

	  for (vsize j = 1; j < cols.size (); j++)
	    {
	      bool last = (j == cols.size () - 1);
	      bool break_point = is_break (cols[j]);
	      bool chunk_end = cols[j]->get_property ("page-break-permission") == force_sym;
	      Break_position cur_pos = Break_position (i,
						       line_breaker_columns.size (),
						       cols[j],
						       last);

	      // NOTE: even in the breaks_ list, forced_line_count_
	      // refers to the number of lines between a
	      // Break_position and the start of that /chunk/.  This
	      // is needed for system_count_bounds to work correctly,
	      // since it mixes Break_positions from breaks_ and
	      // chunks_.
	      if (scm_is_number (system_count))
		cur_pos.forced_line_count_ = j - last_chunk_idx;

	      if (break_point || (i == system_specs_.size () - 1 && last))
		breaks_.push_back (cur_pos);
	      if (chunk_end || last)
		{
		  chunks_.push_back (cur_pos);
		  last_chunk_idx = j;
		}

	      if ((break_point || chunk_end) && !last)
		line_breaker_columns.push_back (j);
	    }
	  line_breaking_.push_back (Constrained_breaking (system_specs_[i].pscore_, line_breaker_columns));
	}
      else
	{
	  /* TODO: we want some way of applying Break_p to a prob? */
	  if (i == system_specs_.size () - 1)
	    breaks_.push_back (Break_position (i));

	  chunks_.push_back (Break_position (i));

	  /* FIXME: shouldn't we push a Null_breaker or similar dummy
	     class? --hwn */
	  line_breaking_.push_back (Constrained_breaking (NULL));
	}
    }
}

vector<Break_position>
Page_breaking::chunk_list (vsize start_index, vsize end_index)
{
  Break_position start = breaks_[start_index];
  Break_position end = breaks_[end_index];

  vsize i = 0;
  for (; i < chunks_.size () && chunks_[i] <= start; i++)
    ;

  vector<Break_position> ret;
  ret.push_back (start);
  for (; i < chunks_.size () && chunks_[i] < end; i++)
    ret.push_back (chunks_[i]);
  ret.push_back (end);
  return ret;
}

vsize
Page_breaking::min_system_count (vsize start, vsize end)
{
  vector<Break_position> chunks = chunk_list (start, end);
  Line_division div = system_count_bounds (chunks, true);
  vsize ret = 0;

  for (vsize i = 0; i < div.size (); i++)
    ret += div[i];
  return ret;
}

vsize
Page_breaking::max_system_count (vsize start, vsize end)
{
  vector<Break_position> chunks = chunk_list (start, end);
  Line_division div = system_count_bounds (chunks, false);
  vsize ret = 0;

  for (vsize i = 0; i < div.size (); i++)
    ret += div[i];
  return ret;
}

Page_breaking::Line_division
Page_breaking::system_count_bounds (vector<Break_position> const &chunks,
				    bool min)
{
  assert (chunks.size () >= 2);

  Line_division ret;
  ret.resize (chunks.size () - 1, 1);

  for (vsize i = 0; i + 1 < chunks.size (); i++)
    {
      vsize sys = next_system (chunks[i]);

      if (chunks[i+1].forced_line_count_)
	ret[i] = chunks[i+1].forced_line_count_;
      else if (system_specs_[sys].pscore_)
	{
	  vsize start;
	  vsize end;
	  line_breaker_args (sys, chunks[i], chunks[i+1], &start, &end);
	  ret[i] = min
	    ? line_breaking_[sys].min_system_count (start, end)
	    : line_breaking_[sys].max_system_count (start, end);
	}
    }

  return ret;
}

void
Page_breaking::set_current_breakpoints (vsize start,
					vsize end,
					vsize system_count,
					Line_division lower_bound,
					Line_division upper_bound)
{
  system_count_ = system_count;
  current_chunks_ = chunk_list (start, end);
  current_start_breakpoint_ = start;
  current_end_breakpoint_ = end;
  clear_line_details_cache ();

  if (!lower_bound.size ())
    lower_bound = system_count_bounds (current_chunks_, true);
  if (!upper_bound.size ())
    upper_bound = system_count_bounds (current_chunks_, false);

  assert (lower_bound.size () == current_chunks_.size () - 1);
  assert (upper_bound.size () == current_chunks_.size () - 1);

  Line_division work_in_progress;
  current_configurations_.clear ();
  line_divisions_rec (system_count,
		      lower_bound,
		      upper_bound,
		      &work_in_progress);

  /* we only consider a constant number of configurations. Otherwise,
     this becomes slow when there are many small scores. The constant
     5 is somewhat arbitrary. */
  if (current_configurations_.size () > 5)
    {
      vector<pair<Real,vsize> > dems_and_indices;

      for (vsize i = 0; i < current_configurations_.size (); i++)
	{
	  cache_line_details (i);
	  Real dem = 0;
	  for (vsize j = 0; j < cached_line_details_.size (); j++)
	    dem += cached_line_details_[j].force_ * cached_line_details_[j].force_
	      + cached_line_details_[j].break_penalty_;

	  dems_and_indices.push_back (pair<Real,vsize> (dem, i));
	}
      vector_sort (dems_and_indices, less<pair<Real,vsize> > ());

      vector<Line_division> best_5_configurations;
      for (vsize i = 0; i < 5; i++)
	best_5_configurations.push_back (current_configurations_[dems_and_indices[i].second]);

      clear_line_details_cache ();
      current_configurations_ = best_5_configurations;
    }
}

void
Page_breaking::set_to_ideal_line_configuration (vsize start, vsize end)
{
  current_chunks_ = chunk_list (start, end);
  current_start_breakpoint_ = start;
  current_end_breakpoint_ = end;
  clear_line_details_cache ();
  system_count_ = 0;

  Line_division div;
  for (vsize i = 0; i+1 < current_chunks_.size (); i++)
    {
      vsize sys = next_system (current_chunks_[i]);

      if (current_chunks_[i+1].forced_line_count_)
	div.push_back (current_chunks_[i+1].forced_line_count_);
      else if (system_specs_[sys].pscore_)
	{
	  line_breaker_args (sys, current_chunks_[i], current_chunks_[i+1], &start, &end);
	  div.push_back (line_breaking_[sys].best_solution (start, end).size ());
	}
      else
	div.push_back (1);

      system_count_ += div.back ();
    }
  current_configurations_.clear ();
  current_configurations_.push_back (div);
}

vsize
Page_breaking::current_configuration_count () const
{
  return current_configurations_.size ();
}

void
Page_breaking::cache_line_details (vsize configuration_index)
{
  if (cached_configuration_index_ != configuration_index)
    {
      cached_configuration_index_ = configuration_index;

      Line_division &div = current_configurations_[configuration_index];
      uncompressed_line_details_.clear ();
      for (vsize i = 0; i + 1 < current_chunks_.size (); i++)
	{
	  vsize sys = next_system (current_chunks_[i]);
	  if (system_specs_[sys].pscore_)
	    {
	      vsize start;
	      vsize end;
	      line_breaker_args (sys, current_chunks_[i], current_chunks_[i+1], &start, &end);

	      vector<Line_details> details = line_breaking_[sys].line_details (start, end, div[i]);
	      uncompressed_line_details_.insert (uncompressed_line_details_.end (), details.begin (), details.end ());
	    }
	  else
	    {
	      assert (div[i] == 1);
	      uncompressed_line_details_.push_back (Line_details (system_specs_[sys].prob_, book_->paper_));
	    }
	}
      cached_line_details_ = compress_lines (uncompressed_line_details_);
    }
}

void
Page_breaking::clear_line_details_cache ()
{
  cached_configuration_index_ = VPOS;
  cached_line_details_.clear ();
  uncompressed_line_details_.clear ();
}

void
Page_breaking::line_divisions_rec (vsize system_count,
				   Line_division const &min_sys,
				   Line_division const &max_sys,
				   Line_division *cur_division)
{
  vsize my_index = cur_division->size ();
  vsize others_min = 0;
  vsize others_max = 0;

  for (vsize i = my_index + 1; i < min_sys.size (); i++)
    {
      others_min += min_sys[i];
      others_max += max_sys[i];
    }
  others_max = min (others_max, system_count);
  vsize real_min = max (min_sys[my_index], system_count - others_max);
  vsize real_max = min (max_sys[my_index], system_count - others_min);

  if (real_min > real_max || real_min <= 0)
    {
      /* this should never happen within a recursive call. If it happens
	 at all, it means that we were called with an unsolvable problem
	 and we should return an empty result */
      assert (my_index == 0);
      return;
    }

  for (vsize i = real_min; i <= real_max; i++)
    {
      cur_division->push_back (i);
      if (my_index == min_sys.size () - 1)
	current_configurations_.push_back (*cur_division);
      else
        line_divisions_rec (system_count - i, min_sys, max_sys, cur_division);
      cur_division->pop_back ();
    }
}

vsize
Page_breaking::min_page_count (vsize configuration, vsize first_page_num)
{
  vsize ret = 1;
  vsize page_starter = 0;
  Real cur_rod_height = 0;
  Real cur_spring_height = 0;
  Real cur_page_height = page_height (first_page_num, false);
  int line_count = 0;

  cache_line_details (configuration);

  if (cached_line_details_.size ())
    cur_page_height -= min_whitespace_at_top_of_page (cached_line_details_[0]);

  for (vsize i = 0; i < cached_line_details_.size (); i++)
    {
      Real ext_len = cached_line_details_[i].extent_.length ();
      Real padding = 0;
      if (cur_rod_height > 0)
	padding = cached_line_details_[i].title_ ?
	  cached_line_details_[i-1].title_padding_ : cached_line_details_[i-1].padding_;

      Real next_rod_height = cur_rod_height + ext_len + padding;
      Real next_spring_height = cur_spring_height + cached_line_details_[i].space_;
      Real next_height = next_rod_height + (ragged () ? next_spring_height : 0)
	+ min_whitespace_at_bottom_of_page (cached_line_details_[i]);
      int next_line_count = line_count + cached_line_details_[i].compressed_nontitle_lines_count_;

      if ((!too_few_lines (line_count) && (next_height > cur_page_height && cur_rod_height > 0))
	  || too_many_lines (next_line_count)
	  || (i > 0
	      && cached_line_details_[i-1].page_permission_ == ly_symbol2scm ("force")))
	{
	  line_count = cached_line_details_[i].compressed_nontitle_lines_count_;
	  cur_rod_height = ext_len;
	  cur_spring_height = cached_line_details_[i].space_;
	  page_starter = i;

	  cur_page_height = page_height (first_page_num + ret, false);
	  cur_page_height -= min_whitespace_at_top_of_page (cached_line_details_[i]);

	  ret++;
	}
      else
	{
	  cur_rod_height = next_rod_height;
	  cur_spring_height = next_spring_height;
	  line_count = next_line_count;
	}
    }

  /* there are two potential problems with the last page (because we didn't know
     it was the last page until after we managed to fit all the systems to it):
     - we are ragged-last but the last page has a compressed spring
     - the value returned by page_height (num, true) is smaller than the
       value returned by page_height (num, false) and it causes the page not to
       fit.

     In either case, we just need to add one more page. This is because the last
     line will always fit on the extra page and by adding one more page to the
     end, the previous page is no longer the last page, so our previous
     calculations that treated it as a non-last page were ok.
  */

  cur_page_height = page_height (first_page_num + ret - 1, true);
  cur_page_height -= min_whitespace_at_top_of_page (cached_line_details_[page_starter]);
  cur_page_height -= min_whitespace_at_bottom_of_page (cached_line_details_.back ());

  Real cur_height = cur_rod_height + ((ragged_last () || ragged ()) ? cur_spring_height : 0);
  if (!too_few_lines (line_count - cached_line_details_.back ().compressed_nontitle_lines_count_)
      && cur_height > cur_page_height
      /* don't increase the page count if the last page had only one system */
      && cur_rod_height > cached_line_details_.back ().extent_.length ())
    ret++;

  assert (ret <= cached_line_details_.size ());
  return ret;
}

// If systems_per_page_ is positive, we don't really try to space on N pages;
// we just put the requested number of systems on each page and penalize
// if the result doesn't have N pages.
Page_spacing_result
Page_breaking::space_systems_on_n_pages (vsize configuration, vsize n, vsize first_page_num)
{
  Page_spacing_result ret;

  if (systems_per_page_ > 0)
    {
      Page_spacing_result ret = space_systems_with_fixed_number_per_page (configuration, first_page_num);
      ret.demerits_ += (ret.force_.size () == n) ? 0 : BAD_SPACING_PENALTY;
      return ret;
    }

  cache_line_details (configuration);
  bool valid_n = (n >= min_page_count (configuration, first_page_num)
		  && n <= cached_line_details_.size ());

  if (!valid_n)
    programming_error ("number of pages is out of bounds");

  if (n == 1 && valid_n)
    ret = space_systems_on_1_page (cached_line_details_,
				   page_height (first_page_num, is_last ()),
				   ragged () || (is_last () && ragged_last ()));
  else if (n == 2 && valid_n)
    ret = space_systems_on_2_pages (configuration, first_page_num);
  else
    {
      Page_spacer ps (cached_line_details_, first_page_num, this);
      ret = ps.solve (n);
    }

  return finalize_spacing_result (configuration, ret);
}

Real
Page_breaking::blank_page_penalty () const
{
  SCM penalty_sym;

  if (is_last ())
    penalty_sym = ly_symbol2scm ("blank-last-page-force");
  else if (ends_score ())
    penalty_sym = ly_symbol2scm ("blank-after-score-page-force");
  else
    penalty_sym = ly_symbol2scm ("blank-page-force");

  Break_position const &pos = breaks_[current_end_breakpoint_];
  if (Paper_score *ps = system_specs_[pos.system_spec_index_].pscore_)
    return robust_scm2double (ps->layout ()->lookup_variable (penalty_sym), 0.0);

  return robust_scm2double (book_->paper_->lookup_variable (penalty_sym), 0.0);
}

// If systems_per_page_ is positive, we don't really try to space on N
// or N+1 pages; see the comment to space_systems_on_n_pages.
Page_spacing_result
Page_breaking::space_systems_on_n_or_one_more_pages (vsize configuration, vsize n, vsize first_page_num,
						     Real penalty_for_fewer_pages)
{
  Page_spacing_result n_res;
  Page_spacing_result m_res;

  if (systems_per_page_ > 0)
    {
      Page_spacing_result ret = space_systems_with_fixed_number_per_page (configuration, first_page_num);
      ret.demerits_ += (ret.force_.size () == n || ret.force_.size () == (n-1)) ? 0 : BAD_SPACING_PENALTY;
      return ret;
    }

  cache_line_details (configuration);
  vsize min_p_count = min_page_count (configuration, first_page_num);
  bool valid_n = n >= min_p_count || n <= cached_line_details_.size ();

  if (!valid_n)
    programming_error ("both page counts are out of bounds");

  if (n == 1 && valid_n)
    {
      bool rag = ragged () || (is_last () && ragged_last ());
      Real height = page_height (first_page_num, is_last ());

      if (1 >= min_p_count)
	n_res = space_systems_on_1_page (cached_line_details_, height, rag);
      if (1 < cached_line_details_.size ())
	m_res = space_systems_on_2_pages (configuration, first_page_num);
    }
  else
    {
      Page_spacer ps (cached_line_details_, first_page_num, this);
      
      if (n >= min_p_count || !valid_n)
	n_res = ps.solve (n);
      if (n < cached_line_details_.size () || !valid_n)
	m_res = ps.solve (n+1);
    }

  m_res = finalize_spacing_result (configuration, m_res);
  n_res = finalize_spacing_result (configuration, n_res);

  Real page_spacing_weight = robust_scm2double (book_->paper_->c_variable ("page-spacing-weight"), 10);
  n_res.demerits_ += penalty_for_fewer_pages * page_spacing_weight;

  if (n_res.force_.size ())
    n_res.force_.back () += penalty_for_fewer_pages;

  return (m_res.demerits_ < n_res.demerits_) ? m_res : n_res;
}

Page_spacing_result
Page_breaking::space_systems_on_best_pages (vsize configuration, vsize first_page_num)
{
  vsize min_p_count = min_page_count (configuration, first_page_num);

  cache_line_details (configuration);
  Page_spacer ps (cached_line_details_, first_page_num, this);
  Page_spacing_result best = ps.solve (min_p_count);

  for (vsize i = min_p_count+1; i <= cached_line_details_.size (); i++)
    {
      Page_spacing_result cur = ps.solve (i);
      if (cur.demerits_ < best.demerits_)
	best = cur;
    }

  Page_spacing_result ret = finalize_spacing_result (configuration, best);
  return ret;
}

Page_spacing_result
Page_breaking::space_systems_with_fixed_number_per_page (vsize configuration,
							 vsize first_page_num)
{
  Page_spacing_result res;
  Page_spacing space (page_height (first_page_num, false), this);
  vsize line = 0;
  vsize page = 0;
  vsize page_first_line = 0;

  cache_line_details (configuration);
  while (line < cached_line_details_.size ())
    {
      page++;
      space.clear ();
      space.resize (page_height (first_page_num + page, false));

      int system_count_on_this_page = 0;
      while (system_count_on_this_page < systems_per_page_
	     && line < cached_line_details_.size ())
	{
	  Line_details const &cur_line = cached_line_details_[line];
	  space.append_system (cur_line);
	  system_count_on_this_page += cur_line.compressed_nontitle_lines_count_;
	  line++;

	  if (cur_line.page_permission_ == ly_symbol2scm ("force"))
	    break;
	}

      res.systems_per_page_.push_back (line - page_first_line);

      res.force_.push_back (space.force_);
      res.penalty_ += cached_line_details_[line-1].page_penalty_;
      if (system_count_on_this_page != systems_per_page_)
	{
	  res.penalty_ += TERRIBLE_SPACING_PENALTY;
	  res.system_count_status_ |= ((system_count_on_this_page < systems_per_page_))
	    ? SYSTEM_COUNT_TOO_FEW : SYSTEM_COUNT_TOO_MANY;
	}

      page_first_line = line;
    }

  /* Recalculate forces for the last page because we know now that is
     really the last page. */
  space.resize (page_height (first_page_num + page, true));
  res.force_.back () = space.force_;
  return finalize_spacing_result (configuration, res);
}

Page_spacing_result
Page_breaking::pack_systems_on_least_pages (vsize configuration, vsize first_page_num)
{
  // TODO: add support for min/max-systems-per-page.
  Page_spacing_result res;
  vsize page = 0;
  vsize page_first_line = 0;
  Page_spacing space (page_height (first_page_num, false), this);

  cache_line_details (configuration);
  for (vsize line = 0; line < cached_line_details_.size (); line++)
    {
      Real prev_force = space.force_;
      space.append_system (cached_line_details_[line]);
      if ((line > page_first_line)
	  && (isinf (space.force_)
	      || ((line > 0)
                  && (cached_line_details_[line-1].page_permission_ == ly_symbol2scm ("force")))))
	{
	  res.systems_per_page_.push_back (line - page_first_line);
	  res.force_.push_back (prev_force);
          res.penalty_ += cached_line_details_[line-1].page_penalty_;
	  page++;
	  space.resize (page_height (first_page_num + page, false));
	  space.clear ();
          space.append_system (cached_line_details_[line]);
	  page_first_line = line;
	}

      if (line == cached_line_details_.size () - 1)
	{
	  /* This is the last line */
	  /* When the last page height was computed, we did not know yet that it
	   * was the last one. If the systems put on it don't fit anymore, the last
	   * system is moved to a new page */
	  space.resize (page_height (first_page_num + page, true));
	  if ((line > page_first_line) && (isinf (space.force_)))
	    {
	      res.systems_per_page_.push_back (line - page_first_line);
	      res.force_.push_back (prev_force);
	      /* the last page containing the last line */
	      space.resize (page_height (first_page_num + page + 1, true));
	      space.clear ();
	      space.append_system (cached_line_details_[line]);
	      res.systems_per_page_.push_back (1);
	      res.force_.push_back (space.force_);
              res.penalty_ += cached_line_details_[line-1].page_penalty_;
              res.penalty_ += cached_line_details_[line].page_penalty_;
	    }
	  else
	    {
	      res.systems_per_page_.push_back (line + 1 - page_first_line);
	      res.force_.push_back (space.force_);
              res.penalty_ += cached_line_details_[line].page_penalty_;
	    }
	}
    }
  return finalize_spacing_result (configuration, res);
}

/* Calculate demerits and fix res.systems_per_page_ so that
   it refers to the original line numbers, not the ones given by compress_lines (). */
Page_spacing_result
Page_breaking::finalize_spacing_result (vsize configuration, Page_spacing_result res)
{
  if (res.force_.empty ())
    return res;

  cache_line_details (configuration);
  res.systems_per_page_ = uncompress_solution (res.systems_per_page_, cached_line_details_);

  Real line_force = 0;
  Real line_penalty = 0;
  Real page_demerits = res.penalty_;
  Real page_weighting = robust_scm2double (book_->paper_->c_variable ("page-spacing-weight"), 10);

  for (vsize i = 0; i < uncompressed_line_details_.size (); i++)
    {
      line_force += uncompressed_line_details_[i].force_ * uncompressed_line_details_[i].force_;
      line_penalty += uncompressed_line_details_[i].break_penalty_;
    }

  for (vsize i = 0; i < res.force_.size (); i++)
    {
      Real f = res.force_[i];

      page_demerits += min(f*f, BAD_SPACING_PENALTY);
    }

  /* for a while we tried averaging page and line forces across pages instead
     of summing them, but it caused a problem: if there is a single page
     with a very bad page force (for example because of a forced page break),
     the page breaker will put in a _lot_ of pages so that the bad force
     becomes averaged out over many pages. */
  res.demerits_ = line_force + line_penalty + page_demerits * page_weighting;
  return res;

}

/* the cases for page_count = 1 or 2 can be done in O (n) time. Since they
   are by far the most common cases, we have special functions for them.

   space_systems_on_1_page has a different calling convention than most of the
   space_systems functions. This is because space_systems_on_1_page is (unlike
   the other space_systems functions) sometimes called on subsets of a full
   configuration. */
Page_spacing_result
Page_breaking::space_systems_on_1_page (vector<Line_details> const &lines, Real page_height, bool ragged)
{
  Page_spacing space (page_height, this);
  Page_spacing_result ret;
  int line_count = 0;

  for (vsize i = 0; i < lines.size (); i++)
    {
      space.append_system (lines[i]);
      line_count += lines[i].compressed_nontitle_lines_count_;
    }

  ret.systems_per_page_.push_back (lines.size ());
  ret.force_.push_back (ragged ? min (space.force_, 0.0) : space.force_);
  ret.penalty_ = line_count_penalty (line_count) + lines.back ().page_penalty_ + lines.back ().turn_penalty_;
  ret.system_count_status_ |= line_count_status (line_count);

  /* don't do finalize_spacing_result () because we are only an internal function */
  return ret;
}

Page_spacing_result
Page_breaking::space_systems_on_2_pages (vsize configuration, vsize first_page_num)
{
  Real page1_height = page_height (first_page_num, false);
  Real page2_height = page_height (first_page_num + 1, is_last ());
  bool ragged1 = ragged ();
  bool ragged2 = ragged () || (is_last () && ragged_last ());

  /* if there is a forced break, this reduces to 2 1-page problems */
  cache_line_details (configuration);
  for (vsize i = 0; i + 1 < cached_line_details_.size (); i++)
    if (cached_line_details_[i].page_permission_ == ly_symbol2scm ("force"))
      {
	vector<Line_details> lines1 (cached_line_details_.begin (), cached_line_details_.begin () + i + 1);
	vector<Line_details> lines2 (cached_line_details_.begin () + i + 1, cached_line_details_.end ());
	Page_spacing_result p1 = space_systems_on_1_page (lines1, page1_height, ragged1);
	Page_spacing_result p2 = space_systems_on_1_page (lines2, page2_height, ragged2);

	p1.systems_per_page_.push_back (p2.systems_per_page_[0]);
	p1.force_.push_back (p2.force_[0]);
	p1.penalty_ += p2.penalty_ - cached_line_details_[i].turn_penalty_;
	return p1;
      }

  vector<Real> page1_force;
  vector<Real> page2_force;

  // page1_penalty and page2_penalty store the penalties based
  // on min-systems-per-page and max-systems-per-page.
  vector<Real> page1_penalty;
  vector<Real> page2_penalty;

  // page1_status and page2_status keep track of whether the min-systems-per-page
  // and max-systems-per-page constraints are satisfied.
  vector<int> page1_status;
  vector<int> page2_status;

  Page_spacing page1 (page1_height, this);
  Page_spacing page2 (page2_height, this);
  int page1_line_count = 0;
  int page2_line_count = 0;

  page1_force.resize (cached_line_details_.size () - 1, infinity_f);
  page2_force.resize (cached_line_details_.size () - 1, infinity_f);
  page1_penalty.resize (cached_line_details_.size () - 1, infinity_f);
  page2_penalty.resize (cached_line_details_.size () - 1, infinity_f);
  page1_status.resize (cached_line_details_.size () - 1, 0);
  page2_status.resize (cached_line_details_.size () - 1, 0);

  /* find the page 1 and page 2 forces for each page-breaking position */
  for (vsize i = 0; i < page1_force.size (); i++)
    {
      page1.append_system (cached_line_details_[i]);
      page2.prepend_system (cached_line_details_[cached_line_details_.size () - 1 - i]);
      page1_line_count += cached_line_details_[i].compressed_nontitle_lines_count_;
      page2_line_count += cached_line_details_[cached_line_details_.size () - 1 - i].compressed_nontitle_lines_count_;

      page1_force[i] = (ragged1 && page1.force_ < 0 && i > 0) ? infinity_f : page1.force_;
      page1_penalty[i] = line_count_penalty (page1_line_count);
      page1_status[i] = line_count_status (page1_line_count);

      if (ragged2)
	page2_force[page2_force.size () - 1 - i] =
	  (page2.force_ < 0 && i + 1 < page1_force.size ()) ? infinity_f : 0;
      else
	page2_force[page2_force.size () - 1 - i] = page2.force_;
      page2_penalty[page2_penalty.size () - 1 - i] = line_count_penalty (page2_line_count);
      page2_status[page2_penalty.size () - 1 - i] = line_count_status (page2_line_count);
    }

  /* find the position that minimises the sum of the page forces */
  vsize best_sys_count = 1;
  Real best_demerits = infinity_f;
  for (vsize i = 0; i < page1_force.size (); i++)
    {
      Real f = page1_force[i] * page1_force[i] + page2_force[i] * page2_force[i];

      // NOTE: we treat max-systems-per-page and min-systems-per-page as soft
      // constraints. That is, we penalize harshly when they don't happen
      // but we don't completely reject.
      Real dem = f
	+ page1_penalty[i] + page2_penalty[i]
	+ cached_line_details_[i+1].page_penalty_
	+ cached_line_details_.back ().page_penalty_ + cached_line_details_.back ().turn_penalty_;
      if (dem < best_demerits)
	{
	  best_demerits = dem;
	  best_sys_count = i+1;
	}
    }

  Page_spacing_result ret;
  ret.systems_per_page_.push_back (best_sys_count);
  ret.systems_per_page_.push_back (cached_line_details_.size () - best_sys_count);
  ret.force_.push_back (page1_force[best_sys_count-1]);
  ret.force_.push_back (page2_force[best_sys_count-1]);
  ret.system_count_status_ = page1_status[best_sys_count-1] | page2_status[best_sys_count-1];
  ret.penalty_ = cached_line_details_[best_sys_count-1].page_penalty_
    + cached_line_details_.back ().page_penalty_
    + cached_line_details_.back ().turn_penalty_
    + page1_penalty[best_sys_count-1] + page2_penalty[best_sys_count-1];

  /* don't do finalize_spacing_result () because we are only an internal function */
  return ret;
}

bool
Page_breaking::all_lines_stretched (vsize configuration)
{
  cache_line_details (configuration);
  for (vsize i = 0; i < cached_line_details_.size (); i++)
    if (cached_line_details_[i].force_ < 0)
      return false;

  return true;
}

Page_breaking::Line_division
Page_breaking::current_configuration (vsize configuration_index) const
{
  return current_configurations_[configuration_index];
}

bool
Page_breaking::is_last () const
{
  return current_end_breakpoint_ == last_break_position ();
}

bool
Page_breaking::ends_score () const
{
  return breaks_[current_end_breakpoint_].score_ender_;
}

vsize
Page_breaking::last_break_position () const
{
  return breaks_.size () - 1;  
}

// This gives the minimum distance between the top of the
// printable area (ie. the bottom of the top-margin) and
// the extent box of the topmost system.
Real
Page_breaking::min_whitespace_at_top_of_page (Line_details const &line) const
{
  SCM first_system_spacing = book_->paper_->c_variable ("top-system-spacing");
  if (line.title_)
    first_system_spacing = book_->paper_->c_variable ("top-title-spacing");

  Real min_distance = -infinity_f;
  Real padding = 0;

  Page_layout_problem::read_spacing_spec (first_system_spacing,
					  &min_distance,
					  ly_symbol2scm ("minimum-distance"));
  Page_layout_problem::read_spacing_spec (first_system_spacing,
					  &padding,
					  ly_symbol2scm ("padding"));

  // FIXME: take into account the height of the header
  return max (0.0, max (padding, min_distance - line.extent_[UP]));
}

Real
Page_breaking::min_whitespace_at_bottom_of_page (Line_details const &line) const
{
  SCM last_system_spacing = book_->paper_->c_variable ("bottom-system-spacing");
  Real min_distance = -infinity_f;
  Real padding = 0;

  Page_layout_problem::read_spacing_spec (last_system_spacing,
					  &min_distance,
					  ly_symbol2scm ("minimum-distance"));
  Page_layout_problem::read_spacing_spec (last_system_spacing,
					  &padding,
					  ly_symbol2scm ("padding"));

  // FIXME: take into account the height of the footer
  return max (0.0, max (padding, min_distance + line.extent_[DOWN]));
}

int
Page_breaking::orphan_penalty () const
{
  return orphan_penalty_;
}
