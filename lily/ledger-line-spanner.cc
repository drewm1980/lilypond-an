/*
  ledger-line-spanner.cc -- implement Ledger_line_spanner

  source file of the GNU LilyPond music typesetter
DECLARE_GROB_INTERFACE ();
  (c) 2004--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include <map>
#include <set>
using namespace std;

#include "item.hh"
#include "note-head.hh"
#include "staff-symbol-referencer.hh"
#include "staff-symbol.hh"
#include "lookup.hh"
#include "spanner.hh"
#include "pointer-group-interface.hh"
#include "paper-column.hh"

struct Ledger_request
{
  Interval ledger_extent_;
  Interval head_extent_;
  /* non-negative position, direction is removed */
  int position_;
  bool excentric_;
  Ledger_request ()
  {
    ledger_extent_.set_empty ();
    head_extent_.set_empty ();
    position_ = 0;
  }
};

typedef map < int, Drul_array<Ledger_request> > Ledger_requests;
typedef map <int, Ledger_request> Ledger_requests_internal;

/* This consists of a group of ledger lines internal to the staff.
   There should be no staff lines between these ledger lines
 */
class Internal_ledgers 
{
public:
  Internal_ledgers():
    halfspace_(1)
  {
  }
  void set_halfspace(const Real& halfspace)
  {
    halfspace_ = halfspace;
  }
  /** 
   * add new ledger line at \a new_ledger
   *
   * @param newLedger new ledger position
   * 
   */
  void add_ledger(int new_ledger)
  {
    ledgers_.insert(new_ledger);
  }
  
  /** must be performed before using contains */
  void calculate_extent()
  {
    ledger_extent_ = Interval(*min_element(ledgers_.begin(),
					   ledgers_.end())/halfspace_ - 1,
			     *max_element(ledgers_.begin(),
					  ledgers_.end())/halfspace_ + 1);
  }
  /** 
   * 
   * 
   * @param pos 
   * 
   * @return true if \a pos is contain in ledger
   */
  bool contains(int pos) const
  {
    return ledger_extent_.contains(pos/halfspace_);
  }
  const set<int>& ledger_set() const
  {
    return ledgers_;
  }
private:
  Interval ledger_extent_;
  set<int> ledgers_;
  /** set halfspace for rounding ledger extent */
  Real halfspace_;
};

  
/** 
 * 
 * 
 * @param pos 
 * @param internal_ledgers_container 
 * 
 * @return true if \a pos is contained in at least one item in Internal_ledgers
 */
bool contains(int pos, const vector<Internal_ledgers>& internal_ledgers_container)
{
  for (vector<Internal_ledgers>::const_iterator internal_ledgers = internal_ledgers_container.begin();
       internal_ledgers != internal_ledgers_container.end();
       ++internal_ledgers)
    {
      if ((*internal_ledgers).contains(pos))
	{
	  return true;
	}
    }
  return false;
}

struct Ledger_line_spanner
{
  DECLARE_SCHEME_CALLBACK (print, (SCM));
  DECLARE_SCHEME_CALLBACK (set_spacing_rods, (SCM));
  static Stencil brew_ledger_lines (Grob *me,
				    int pos,
				    Interval,
				    Real, Real,
				    Interval x_extent,
				    Real left_shorten);

  static Stencil brew_internal_ledger_lines (const Internal_ledgers& internal_ledgers,
					   Real, Real,
					   Interval x_extent,
					   Real left_shorten);
  DECLARE_GROB_INTERFACE ();
  /* returns ledger_size and left_shorten, given other values
   * helper method for brew_ledger_lines
   */
  static void find_ledger_size(const Item *head, Grob *common[],
			       Real length_fraction,
			       Ledger_request& req,
			       Interval& ledger_size,
			       Real& left_shorten
			       );

};

Stencil
Ledger_line_spanner::brew_ledger_lines (Grob *staff,
					int pos,
					Interval staff_extent,
					Real halfspace,
					Real ledgerlinethickness,
					Interval x_extent,
					Real left_shorten)
{
  int line_count = (staff_extent.contains (pos)
		    ? 0
		    : sign (pos) * int (rint (pos -  staff_extent[Direction (sign (pos))])) / 2);
  Stencil stencil;
  if (line_count)
    {
      Real blotdiameter = ledgerlinethickness;
      Interval y_extent
	= Interval (-0.5 * (ledgerlinethickness),
		    +0.5 * (ledgerlinethickness));
      Stencil proto_ledger_line
	= Lookup::round_filled_box (Box (x_extent, y_extent), blotdiameter);

      x_extent[LEFT] += left_shorten;
      Stencil proto_first_line
	= Lookup::round_filled_box (Box (x_extent, y_extent), blotdiameter);

      Direction dir = (Direction)sign (pos);
      Real offs = (Staff_symbol_referencer::on_line (staff, pos))
	? 0.0
	: -dir * halfspace;

      offs += pos * halfspace;
      for (int i = 0; i < line_count; i++)
	{
	  Stencil ledger_line ((i == 0)
			       ? proto_first_line
			       : proto_ledger_line);
	  ledger_line.translate_axis (-dir * halfspace * i * 2 + offs, Y_AXIS);
	  stencil.add_stencil (ledger_line);
	}
    }

  return stencil;
}
/** extra lines for internal ledger lines */ 
Stencil
Ledger_line_spanner::brew_internal_ledger_lines (const Internal_ledgers& internal_ledgers,
					       Real halfspace,
					       Real ledgerlinethickness,
					       Interval x_extent,
					       Real left_shorten)
{
  Stencil stencil;
  /* halfspace is 1/2 of staff-space */
  Real blotdiameter = ledgerlinethickness;
  Interval y_extent
    = Interval (-0.5 * (ledgerlinethickness),
		+0.5 * (ledgerlinethickness));
  Stencil proto_ledger_line
    = Lookup::round_filled_box (Box (x_extent, y_extent), blotdiameter);
  x_extent[LEFT] += left_shorten;
  Stencil proto_first_line
    = Lookup::round_filled_box (Box (x_extent, y_extent), blotdiameter);
  const set<int>& ledgers = internal_ledgers.ledger_set();
  for (set<int>::const_iterator currentLedger = ledgers.begin();
       currentLedger != ledgers.end();
       ++currentLedger)
  {
    int pos = *currentLedger;
    Real offs = pos * halfspace;
    Stencil ledger_line(proto_ledger_line);
    // Stencil ledger_line(proto_first_line);
    ledger_line.translate_axis(offs, Y_AXIS);
    stencil.add_stencil (ledger_line);
  }
  return stencil;
}

static void
set_rods (Drul_array<Interval> const &current_extents,
	  Drul_array<Interval> const &previous_extents,
	  Item *current_column,
	  Item *previous_column,
	  Real min_length_fraction)
{
  Direction d = UP;
  do
    {
      if (!current_extents[d].is_empty ()
	  && !previous_extents[d].is_empty ())
	{
	  Real total_head_length = previous_extents[d].length ()
	    + current_extents[d].length ();

	  Rod rod;
	  rod.distance_ = total_head_length
	    * (3 / 2 * min_length_fraction)
	    /*
	      we go from right to left.
	    */
	    - previous_extents[d][LEFT]
	    + current_extents[d][RIGHT];

	  rod.item_drul_[LEFT] = current_column;
	  rod.item_drul_[RIGHT] = previous_column;
	  rod.add_to_cols ();
	}
    }
  while (flip (&d) != DOWN);
}

MAKE_SCHEME_CALLBACK (Ledger_line_spanner, set_spacing_rods, 1);
SCM
Ledger_line_spanner::set_spacing_rods (SCM smob)
{
  Spanner *me = dynamic_cast<Spanner *> (unsmob_grob (smob));

  // find size of note heads.
  Grob *staff = Staff_symbol_referencer::get_staff_symbol (me);
  if (!staff)
    {
      me->suicide ();
      return SCM_EOL;
    }

  Real min_length_fraction
    = robust_scm2double (me->get_property ("minimum-length-fraction"), 0.15);

  Drul_array<Interval> current_extents;
  Drul_array<Interval> previous_extents;
  Item *previous_column = 0;
  Item *current_column = 0;

  Real halfspace = Staff_symbol::staff_space (staff) / 2;

  Interval staff_extent = staff->extent (staff, Y_AXIS);
  staff_extent *= 1 / halfspace;
    
  /*
    Run through heads using a loop. Since Ledger_line_spanner can
    contain a lot of noteheads, superlinear performance is too slow.
  */
  extract_item_set (me, "note-heads", heads);
  for (vsize i = heads.size (); i--;)
    {
      Item *h = heads[i];

      int pos = Staff_symbol_referencer::get_rounded_position (h);
      if (staff_extent.contains (pos))
	continue;

      Item *column = h->get_column ();
      if (current_column != column)
	{
	  set_rods (current_extents, previous_extents,
		    current_column, previous_column,
		    min_length_fraction);

	  previous_column = current_column;
	  current_column = column;
	  previous_extents = current_extents;

	  current_extents[DOWN].set_empty ();
	  current_extents[UP].set_empty ();
	}

      Interval head_extent = h->extent (column, X_AXIS);
      Direction vdir = Direction (sign (pos));
      if (!vdir)
	continue;

      current_extents[vdir].unite (head_extent);
    }

  if (previous_column && current_column)
    set_rods (current_extents, previous_extents,
	      current_column, previous_column,
	      min_length_fraction);

  return SCM_UNSPECIFIED;
}

/*
 * Calculate
 * ledger_size
 * left_shorten
 */
void
Ledger_line_spanner::find_ledger_size(const Item *head, Grob *common[],
				      Real length_fraction,
				      Ledger_request& req,
				      Interval& ledger_size,
				      Real& left_shorten
				      )
{
  
  Interval head_size = head->extent (common[X_AXIS], X_AXIS);
  ledger_size = head_size;
  ledger_size.widen (ledger_size.length () * length_fraction);

  Interval max_size = req.ledger_extent_;

  ledger_size.intersect (max_size);
  left_shorten = 0.0;
  if (Grob *g = unsmob_grob (head->get_object ("accidental-grob")))
    {
      Interval accidental_size = g->extent (common[X_AXIS], X_AXIS);
      Real d
	= linear_combination (Drul_array<Real> (accidental_size[RIGHT],
						head_size[LEFT]),
			      0.0);

      left_shorten = max (-ledger_size[LEFT] + d, 0.0);

      /*
	TODO: shorten 2 ledger lines for the case natural +
	downstem.
      */
    }
}

/*
  TODO: ledger share a lot of info. Lots of room to optimize away
  common use of objects/variables.
*/
MAKE_SCHEME_CALLBACK (Ledger_line_spanner, print, 1);
SCM
Ledger_line_spanner::print (SCM smob)
{
  Spanner *me = dynamic_cast<Spanner *> (unsmob_grob (smob));

  extract_grob_set (me, "note-heads", heads);

  if (heads.empty ())
    return SCM_EOL;

  // find size of note heads.
  Grob *staff = Staff_symbol_referencer::get_staff_symbol (me);
  if (!staff)
    return SCM_EOL;

  Real halfspace = Staff_symbol::staff_space (staff) / 2;

  Interval staff_extent = staff->extent (staff, Y_AXIS);
  staff_extent *= 1 / halfspace;
  
  Real length_fraction
    = robust_scm2double (me->get_property ("length-fraction"), 0.25);

  Stencil ledgers;
  Stencil default_ledger;

  Grob *common[NO_AXES];

  for (int i = X_AXIS; i < NO_AXES; i++)
    {
      Axis a = Axis (i);
      common[a] = common_refpoint_of_array (heads, me, a);
      for (vsize i = heads.size (); i--;)
	if (Grob *g = unsmob_grob (me->get_object ("accidental-grob")))
	  common[a] = common[a]->common_refpoint (g, a);
    }

  Ledger_requests reqs;
  Ledger_requests_internal reqs_internal;

  // add internal ledger lines to list
  vector<Internal_ledgers> internal_ledgers_container;
  SCM internal_ledgers = staff->get_property("internal-ledger-lines");
  set<int> ledger_set;
  if (scm_is_pair(internal_ledgers))
    {
      for (SCM s = internal_ledgers; scm_is_pair (s);
	   s = scm_cdr (s))
	{
	  SCM car = scm_car(s);
	  Internal_ledgers internal_ledgers;
	  internal_ledgers.set_halfspace(halfspace);
	  // ignore if s2 is not car
	  for (SCM s2 = car; scm_is_pair (s2);
	   s2 = scm_cdr (s2))
	    {
	      int pos = scm_to_int (scm_car(s2));
	      //	      ledger_set.insert(pos);
	      internal_ledgers.add_ledger(pos);
	    }
	  internal_ledgers.calculate_extent();
	  internal_ledgers_container.push_back(internal_ledgers);
	}
      
    }
  

  
  for (vsize i = heads.size (); i--;)
    {
      Item *h = dynamic_cast<Item *> (heads[i]);

      int pos = Staff_symbol_referencer::get_rounded_position (h);
      if (pos != 0 && !staff_extent.contains (pos))
	{
	  Interval head_extent = h->extent (common[X_AXIS], X_AXIS);
	  Interval ledger_extent = head_extent;
	  ledger_extent.widen (length_fraction * head_extent.length ());

	  Direction vdir = Direction (sign (pos));
	  int rank = h->get_column ()->get_rank ();

	  reqs[rank][vdir].ledger_extent_.unite (ledger_extent);
	  reqs[rank][vdir].head_extent_.unite (head_extent);
	  reqs[rank][vdir].position_
	    = vdir * max (vdir * reqs[rank][vdir].position_, vdir * pos);
	}
      if (contains(pos, internal_ledgers_container))
	{
	  Interval head_extent = h->extent (common[X_AXIS], X_AXIS);
	  Interval ledger_extent = head_extent;
	  ledger_extent.widen (length_fraction * head_extent.length ());

	  Direction vdir = Direction (sign (pos));
	  int rank = h->get_column ()->get_rank ();

	  reqs_internal[rank].ledger_extent_.unite (ledger_extent);
	  reqs_internal[rank].head_extent_.unite (head_extent);
	  reqs_internal[rank].position_
	    = vdir * max (vdir * reqs_internal[rank].position_, vdir * pos);
	}
      
    }

  // determine maximum size for non-colliding ledger.
  Real gap = robust_scm2double (me->get_property ("gap"), 0.1);
  Ledger_requests::iterator last (reqs.end ());
  for (Ledger_requests::iterator i (reqs.begin ());
       i != reqs.end (); last = i++)
    {
      if (last == reqs.end ())
	continue;

      Direction d = DOWN;
      do
	{
	  if (!staff_extent.contains (last->second[d].position_)
	      && !staff_extent.contains (i->second[d].position_))
	    {
	      Real center
		= (last->second[d].head_extent_[RIGHT]
		   + i->second[d].head_extent_[LEFT]) / 2;

	      Direction which = LEFT;
	      do
		{
		  Ledger_request &lr = ((which == LEFT) ? * last : *i).second[d];

		  // due tilt of quarter note-heads
		  /* FIXME */
		  bool both
		    = (!staff_extent.contains (last->second[d].position_
					       - sign (last->second[d].position_))
		       && !staff_extent.contains (i->second[d].position_
						  - sign (i->second[d].position_)));
		  Real limit = (center + (both ? which * gap / 2 : 0));
		  lr.ledger_extent_.at (-which)
		    = which * max (which * lr.ledger_extent_[-which], which * limit);
		}
	      while (flip (&which) != LEFT);
	    }
	}
      while (flip (&d) != DOWN);
    }

  // create ledgers for note heads
  Real ledgerlinethickness
    = Staff_symbol::get_ledger_line_thickness (staff);
  for (vsize i = heads.size (); i--;)
    {
      Item *h = dynamic_cast<Item *> (heads[i]);

      int pos = Staff_symbol_referencer::get_rounded_position (h);
      if (!staff_extent.contains (pos - sign (pos)))
	{
	  Interval ledger_size;
	  Real left_shorten;

	  find_ledger_size(h, common, length_fraction,
			   reqs[h->get_column ()->get_rank ()]
			   [Direction (sign (pos))],
			   ledger_size,
			   left_shorten);

	  ledgers.add_stencil (brew_ledger_lines (staff, pos, staff_extent,
						  halfspace,
						  ledgerlinethickness,
						  ledger_size,
						  left_shorten));
	}
      /* handle internal ledger lines */
      for (vector<Internal_ledgers>::const_iterator internal_ledgers = internal_ledgers_container.begin();
	   internal_ledgers != internal_ledgers_container.end();
	   ++internal_ledgers)
	{
	  if ((*internal_ledgers).contains(pos))
	    {
	      Interval ledger_size;
	      Real left_shorten;
	      
	      find_ledger_size(h, common, length_fraction,
			       reqs_internal[h->get_column ()->get_rank ()],
			       ledger_size,
			       left_shorten);

	      ledgers.add_stencil (brew_internal_ledger_lines (*internal_ledgers,
							     halfspace,
							     ledgerlinethickness,
							     ledger_size,
							     left_shorten));
	    }
	}
      
    }

  ledgers.translate_axis (-me->relative_coordinate (common[X_AXIS], X_AXIS),
			  X_AXIS);

  return ledgers.smobbed_copy ();
}

ADD_INTERFACE (Ledger_line_spanner,
	       "This spanner draws the ledger lines of a staff.  This is a"
	       " separate grob because it has to process all potential"
	       " collisions between all note heads.",

	       /* properties */
	       "gap "	
	       "length-fraction "	
	       "minimum-length-fraction "
	       "note-heads "
	       "thickness "
	       "minimum-length-fraction "
	       "length-fraction "
	       "gap "
	       "internal-ledger-lines");

struct Ledgered_interface
{
  DECLARE_GROB_INTERFACE ();
};

ADD_INTERFACE (Ledgered_interface,
	       "Objects that need ledger lines, typically note heads.  See"
	       " also @ref{ledger-line-spanner-interface}.",

	       /* properties */
	       "no-ledgers "
	       );
