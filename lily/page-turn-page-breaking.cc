/*
  page-turn-page-breaking.cc -- implement Page_turn_page_breaking

  source file of the GNU LilyPond music typesetter

  (c) 2006 Joe Neeman <joeneeman@gmail.com>
*/

#include "page-turn-page-breaking.hh"

#include "international.hh"
#include "item.hh"
#include "output-def.hh"
#include "page-spacing.hh"
#include "paper-book.hh"
#include "paper-score.hh"
#include "paper-system.hh"
#include "system.hh"
#include "warn.hh"

Page_turn_page_breaking::Page_turn_page_breaking (Paper_book *pb)
  : Page_breaking (pb, true)
{
}

Page_turn_page_breaking::~Page_turn_page_breaking ()
{
}

Real
Page_turn_page_breaking::calc_demerits (const Break_node &me)
{
  Real prev_f = 0;
  Real prev_dem = 0;
  Real page_weighting = robust_scm2double (book_->paper_->c_variable ("page-spacing-weight"), 1);
  if (me.prev_ != VPOS)
    {
      prev_f = state_[me.prev_].force_;
      prev_dem = state_[me.prev_].demerits_;
    }

  Real dem = me.force_ * me.force_ * page_weighting
           + me.line_force_ * me.line_force_
           + fabs (me.force_ - prev_f);
  if (isinf (me.line_force_) || isinf (me.force_))
    dem = infinity_f;

  return dem + prev_dem + me.penalty_ + me.line_penalty_;
}

Page_turn_page_breaking::Break_node
Page_turn_page_breaking::put_systems_on_pages (vsize start,
					       vsize end,
					       vector<Line_details> const &lines,
					       vector<vsize> const &div,
					       int page_number)
{
  bool last = end == breaks_.size () - 1;
  Real page_h = page_height (1, false); // FIXME
  SCM force_sym = last ? ly_symbol2scm ("blank-last-page-force") : ly_symbol2scm ("blank-page-force");
  Real blank_force = robust_scm2double (book_->paper_->lookup_variable (force_sym), 0);
  Spacing_result result = space_systems_on_min_pages (lines, page_h, blank_force);

  Break_node ret;
  ret.prev_ = start - 1;
  ret.break_pos_ = end;
  ret.first_page_number_ = page_number;
  ret.page_count_ = result.force_.size ();
  ret.force_ = 0;
  for (vsize i = 0; i < result.force_.size (); i++)
    ret.force_ += fabs (result.force_[i]);

  ret.penalty_ = result.penalty_;
  ret.div_ = div;
  ret.system_count_ = result.systems_per_page_;

  ret.too_many_lines_ = true;
  ret.line_force_ = 0;
  ret.line_penalty_ = 0;
  for (vsize i = 0; i < lines.size (); i++)
    {
      ret.line_force_ += fabs (lines[i].force_);
      ret.line_penalty_ += lines[i].break_penalty_;
      if (lines[i].force_ < 0)
	ret.too_many_lines_ = false;
    }
  return ret;
}

void
Page_turn_page_breaking::calc_subproblem (vsize ending_breakpoint)
{
  vsize end = ending_breakpoint + 1;
  Break_node best;
  Break_node cur;
  Break_node this_start_best;
  vsize prev_best_system_count = 0;

  for (vsize start = end; start--;)
    {
      if (start > 0 && best.demerits_ < state_[start-1].demerits_)
        continue;

      int p_num = 1;
      if (start > 0)
        {
          /* if the last node has an odd number of pages and is not the first page,
             add a blank page */
          int p_count = state_[start-1].page_count_;
          int f_p_num = state_[start-1].first_page_number_;
          p_num = f_p_num + p_count + ((f_p_num > 1) ? p_count % 2 : 0);
        }

      vector<vsize> min_systems;
      vector<vsize> max_systems;
      vsize min_system_count = 0;
      vsize max_system_count = 0;
      this_start_best.demerits_ = infinity_f;
      calc_system_count_bounds (start, end, &min_systems, &max_systems);

      /* heuristic: we've prepended some music, only the first system is allowed
         to get more lines */
      if (this_start_best.page_count_ == 2 && this_start_best.div_.size ())
        {
          vsize ds = max_systems.size () - this_start_best.div_.size ();
          for (vsize i = ds + 1; i < max_systems.size (); i++)
            {
              assert (this_start_best.div_[i - ds] <= max_systems[i]);
              max_systems[i] = this_start_best.div_[i - ds];
            }
        }

      for (vsize i = 0; i < min_systems.size (); i++)
        {
          min_system_count += min_systems[i];
          max_system_count += max_systems[i];
        }

      bool ok_page = true;

      /* heuristic: we've just added a breakpoint, we'll need at least as
         many systems as before */
      min_system_count = max (min_system_count, prev_best_system_count);
      for (vsize sys_count = min_system_count; sys_count <= max_system_count && ok_page; sys_count++)
        {
          vector<vector<vsize> > div;
          vector<vsize> blank;
          bool found = false;
          divide_systems (sys_count, min_systems, max_systems, &div, &blank);

          for (vsize d = 0; d < div.size (); d++)
            {
	      vector<Line_details> line = get_line_details (start, end, div[d]);

              cur = put_systems_on_pages (start, end, line, div[d], p_num);

              if (cur.page_count_ > 2 &&
		  (start < end - 1 || (!isinf (this_start_best.demerits_)
				       && cur.page_count_ + cur.page_count_ % 2
				          > this_start_best.page_count_ + this_start_best.page_count_ % 2)))
                {
                  ok_page = false;
                  break;
                }
	      cur.demerits_ = calc_demerits (cur);

              if (cur.demerits_ < this_start_best.demerits_)
                {
                  found = true;
                  this_start_best = cur;
                  prev_best_system_count = sys_count;
                }
            }
          if (!found && this_start_best.too_many_lines_)
            break;
        }
      if (isinf (this_start_best.demerits_))
        {
          assert (!isinf (best.demerits_) && start < end - 1);
          break;
        }
      if (this_start_best.demerits_ < best.demerits_)
        best = this_start_best;
    }
  state_.push_back (best);
}

SCM
Page_turn_page_breaking::solve ()
{
  state_.clear ();
  message (_f ("Calculating page and line breaks (%d possible page breaks)...",
               (int)breaks_.size () - 1) + " ");
  for (vsize i = 0; i < breaks_.size () - 1; i++)
    {
      calc_subproblem (i);
      progress_indication (string ("[") + to_string (i + 1) + "]");
    }
  progress_indication ("\n");

  vector<Break_node> breaking;
  int i = state_.size () - 1;
  while (i >= 0)
    {
      breaking.push_back (state_[i]);
      i = state_[i].prev_;
    }
  reverse (breaking);

  message (_ ("Drawing systems..."));
  SCM systems = make_lines (&breaking);
  return make_pages (breaking, systems);
}

/* do the line breaking in all the scores and return a big list of systems */
SCM
Page_turn_page_breaking::make_lines (vector<Break_node> *psoln)
{
  vector<Break_node> &soln = *psoln;
  for (vsize n = 0; n < soln.size (); n++)
    {
      vsize start = n > 0 ? soln[n-1].break_pos_ : 0;
      vsize end = soln[n].break_pos_;

      /* break each score into pieces */
      for (vsize i = next_system (start); i <= breaks_[end].sys_; i++)
        {
          vsize d = i - next_system (start);
          if (all_[i].pscore_)
            {
              vector<Column_x_positions> line_brk;

              line_brk = get_line_breaks (i, soln[n].div_[d], start, end);
              all_[i].pscore_->root_system ()->break_into_pieces (line_brk);
              assert (line_brk.size () == soln[n].div_[d]);
            }
        }
    }
  SCM ret = SCM_EOL;
  SCM *tail = &ret;
  for (vsize i = 0; i < all_.size (); i++)
    {
      if (all_[i].pscore_)
        for (SCM s = scm_vector_to_list (all_[i].pscore_->root_system ()->get_paper_systems ());
              scm_is_pair (s); s = scm_cdr (s))
          {
            *tail = scm_cons (scm_car (s), SCM_EOL);
            tail = SCM_CDRLOC (*tail);
          }
      else
        {
          *tail = scm_cons (all_[i].prob_->self_scm (), SCM_EOL);
          all_[i].prob_->unprotect ();
          tail = SCM_CDRLOC (*tail);
        }
    }
  return ret;
}

SCM
Page_turn_page_breaking::make_pages (vector<Break_node> const &soln, SCM systems)
{
  vector<vsize> lines_per_page;
  for (vsize i = 0; i < soln.size (); i++)
    {
      for (vsize j = 0; j < soln[i].page_count_; j++)
	lines_per_page.push_back (soln[i].system_count_[j]);

      if (i > 0 && i < soln.size () - 1 && soln[i].page_count_ % 2)
	/* add a blank page */
	lines_per_page.push_back (0);
    }
  return Page_breaking::make_pages (lines_per_page, systems);
}