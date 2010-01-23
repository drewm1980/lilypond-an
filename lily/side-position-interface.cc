/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 1998--2010 Han-Wen Nienhuys <hanwen@xs4all.nl>

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

#include "side-position-interface.hh"

#include <cmath>		// ceil.
#include <algorithm>

using namespace std;

#include "axis-group-interface.hh"
#include "directional-element-interface.hh"
#include "grob.hh"
#include "grob-array.hh"
#include "main.hh"
#include "misc.hh"
#include "note-head.hh"
#include "pointer-group-interface.hh"
#include "staff-symbol-referencer.hh"
#include "staff-symbol.hh"
#include "stem.hh"
#include "string-convert.hh"
#include "system.hh"
#include "warn.hh"

void
Side_position_interface::add_support (Grob *me, Grob *e)
{
  Pointer_group_interface::add_unordered_grob (me, ly_symbol2scm ("side-support-elements"), e);
}

/* Put the element next to the support, optionally taking in
   account the extent of the support.

   Does not take into account the extent of ME.
*/
SCM
Side_position_interface::general_side_position (Grob *me, Axis a, bool use_extents,
						bool include_my_extent,
						bool pure, int start, int end,
						Real *current_offset)
{
  Real ss = Staff_symbol_referencer::staff_space (me);

  extract_grob_set (me, "side-support-elements", support);

  Grob *common = common_refpoint_of_array (support, me->get_parent (a), a);
  Grob *staff_symbol = Staff_symbol_referencer::get_staff_symbol (me);
  bool include_staff = 
    staff_symbol
    && a == Y_AXIS
    && scm_is_number (me->get_property ("staff-padding"))
    && !to_boolean (me->get_property ("quantize-position"));

  Interval dim;
  Interval staff_extents;
  if (include_staff)
    {
      common = staff_symbol->common_refpoint (common, Y_AXIS);
      staff_extents = staff_symbol->maybe_pure_extent (common, Y_AXIS, pure, start, end);

      if (include_staff)
	dim.unite (staff_extents);
    }

  Direction dir = get_grob_direction (me);

  for (vsize i = 0; i < support.size (); i++)
    {
      Grob *e = support[i];

      // In the case of a stem, we will find a note head as well
      // ignoring the stem solves cyclic dependencies if the stem is
      // attached to a cross-staff beam.
      if (a == Y_AXIS
	  && Stem::has_interface (e)
	  && dir == - get_grob_direction (e))
	continue;
      
      if (e)
	{
	  if (use_extents)
	    dim.unite (e->maybe_pure_extent (common, a, pure, start, end));
	  else
	    {
	      Real x = e->maybe_pure_coordinate (common, a, pure, start, end);
	      dim.unite (Interval (x, x));
	    }
	}
    }

  if (dim.is_empty ())
    dim = Interval (0, 0);

  Real off = me->get_parent (a)->maybe_pure_coordinate (common, a, pure, start, end);
  Real minimum_space = ss * robust_scm2double (me->get_property ("minimum-space"), -1);

  Real total_off = dim.linear_combination (dir) - off;
  total_off += dir * ss * robust_scm2double (me->get_property ("padding"), 0);

  if (minimum_space >= 0
      && dir
      && total_off * dir < minimum_space)
    total_off = minimum_space * dir;

  if (include_my_extent)
    {
      Interval iv = me->maybe_pure_extent (me, a, pure, start, end);
      if (!iv.is_empty ())
	{
	  if (!dir)
	    {
	      programming_error ("direction unknown, but aligned-side wanted");
	      dir = DOWN;
	    }
	  total_off += -iv[-dir];
	}
    }

  if (current_offset)
    total_off = dir * max (dir * total_off,
			   dir * (*current_offset));
  
  
  /* FIXME: 1000 should relate to paper size.  */
  if (fabs (total_off) > 1000)
    {
      string msg
	= String_convert::form_string ("Improbable offset for grob %s: %f",
				       me->name ().c_str (), total_off);

      programming_error (msg);
      if (strict_infinity_checking)
	scm_misc_error (__FUNCTION__, "Improbable offset.", SCM_EOL);
    }
  return scm_from_double (total_off);
}


MAKE_SCHEME_CALLBACK (Side_position_interface, y_aligned_on_support_refpoints, 1);
SCM
Side_position_interface::y_aligned_on_support_refpoints (SCM smob)
{
  return general_side_position (unsmob_grob (smob), Y_AXIS, false, false, false, 0, 0, 0); 
}

MAKE_SCHEME_CALLBACK (Side_position_interface, pure_y_aligned_on_support_refpoints, 3);
SCM
Side_position_interface::pure_y_aligned_on_support_refpoints (SCM smob, SCM start, SCM end)
{
  return general_side_position (unsmob_grob (smob), Y_AXIS, false, false, 
				true, scm_to_int (start), scm_to_int (end), 0); 
}


/*
  Position next to support, taking into account my own dimensions and padding.
*/
SCM
axis_aligned_side_helper (SCM smob, Axis a, bool pure, int start, int end, SCM current_off_scm)
{
  Real r;
  Real *current_off_ptr = 0;
  if (scm_is_number (current_off_scm))
    {
      r = scm_to_double (current_off_scm);
      current_off_ptr = &r; 
    }
  
  return Side_position_interface::aligned_side (unsmob_grob (smob), a, pure, start, end, current_off_ptr);
}


MAKE_SCHEME_CALLBACK_WITH_OPTARGS (Side_position_interface, x_aligned_side, 2, 1, "");
SCM
Side_position_interface::x_aligned_side (SCM smob, SCM current_off)
{
  return axis_aligned_side_helper (smob, X_AXIS, false, 0, 0, current_off);
}

MAKE_SCHEME_CALLBACK_WITH_OPTARGS (Side_position_interface, y_aligned_side, 2, 1, "");
SCM
Side_position_interface::y_aligned_side (SCM smob, SCM current_off)
{
  return axis_aligned_side_helper (smob, Y_AXIS, false, 0, 0, current_off);
}

MAKE_SCHEME_CALLBACK_WITH_OPTARGS (Side_position_interface, pure_y_aligned_side, 4, 1, "");
SCM
Side_position_interface::pure_y_aligned_side (SCM smob, SCM start, SCM end, SCM cur_off)
{
  return axis_aligned_side_helper (smob, Y_AXIS, true,
				   scm_to_int (start),
				   scm_to_int (end),
				   cur_off);
}

MAKE_SCHEME_CALLBACK (Side_position_interface, calc_cross_staff, 1)
SCM
Side_position_interface::calc_cross_staff (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  extract_grob_set (me, "side-support-elements", elts);

  for (vsize i = 0; i < elts.size (); i++)
    if (to_boolean (elts[i]->get_property ("cross-staff")))
      return SCM_BOOL_T;

  Grob *common = common_refpoint_of_array (elts, me->get_parent (Y_AXIS), Y_AXIS);
  return scm_from_bool (common != me->get_parent (Y_AXIS));
}

SCM
Side_position_interface::aligned_side (Grob *me, Axis a, bool pure, int start, int end,
				       Real *current_off)
{
  Direction dir = get_grob_direction (me);

  Real o = scm_to_double (general_side_position (me, a, true, true, pure, start, end, current_off));

  /*
    Maintain a minimum distance to the staff. This is similar to side
    position with padding, but it will put adjoining objects on a row if
    stuff sticks out of the staff a little.
  */
  Grob *staff = Staff_symbol_referencer::get_staff_symbol (me);
  if (staff && a == Y_AXIS)
    {
      if (to_boolean (me->get_property ("quantize-position")))
	{
	  Grob *common = me->common_refpoint (staff, Y_AXIS);
	  Real my_off = me->get_parent (Y_AXIS)->maybe_pure_coordinate (common, Y_AXIS, pure, start, end);
	  Real staff_off = staff->maybe_pure_coordinate (common, Y_AXIS, pure, start, end);
	  Real ss = Staff_symbol::staff_space (staff);
	  Real position = 2 * (my_off + o - staff_off) / ss;
	  Real rounded = directed_round (position, dir);
	  Grob *head = me->get_parent (X_AXIS);

	  if (fabs (position) <= 2 * Staff_symbol_referencer::staff_radius (me) + 1
	      /* In case of a ledger lines, quantize even if we're outside the staff. */
	      || (Note_head::has_interface (head)
		  
		  && abs (Staff_symbol_referencer::get_position (head)) > abs (position)))
	    {
	      o += (rounded - position) * 0.5 * ss;
	      if (Staff_symbol_referencer::on_line (me, int (rounded)))
		o += dir * 0.5 * ss;
	    }
	}
      else if (scm_is_number (me->get_property ("staff-padding")) && dir)
	{
	  Interval iv = me->maybe_pure_extent (me, a, pure, start, end);
	  
 	  Real padding
	    = Staff_symbol_referencer::staff_space (me)
	    * scm_to_double (me->get_property ("staff-padding"));

	  Grob *common = me->common_refpoint (staff, Y_AXIS);

	  Interval staff_size = staff->maybe_pure_extent (common, Y_AXIS, pure, start, end);
	  Real diff = dir*staff_size[dir] + padding - dir * (o + iv[-dir]);
	  o += dir * max (diff, 0.0);
	}
    }
  return scm_from_double (o);
}

void
Side_position_interface::set_axis (Grob *me, Axis a)
{
  if (!scm_is_number (me->get_property ("side-axis")))
    {
      me->set_property ("side-axis", scm_from_int (a));
      chain_offset_callback (me,
			     (a==X_AXIS)
			     ? x_aligned_side_proc
			     : y_aligned_side_proc,
			     a);
    }
}

Axis
Side_position_interface::get_axis (Grob *me)
{
  if (scm_is_number (me->get_property ("side-axis")))
    return Axis (scm_to_int (me->get_property ("side-axis")));
  
  string msg = String_convert::form_string ("side-axis not set for grob %s.",
					    me->name ().c_str ());
  me->programming_error (msg);
  return NO_AXES;
}

MAKE_SCHEME_CALLBACK (Side_position_interface, move_to_extremal_staff, 1);
SCM
Side_position_interface::move_to_extremal_staff (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  System *sys = dynamic_cast<System*> (me->get_system ());
  Direction dir = get_grob_direction (me);
  if (dir != DOWN)
    dir = UP;

  Interval iv = me->extent (sys, X_AXIS);
  iv.widen (1.0);
  Grob *top_staff = sys->get_extremal_staff (dir, iv);

  if (!top_staff)
    return SCM_BOOL_F;

  // Only move this grob if it is a direct child of the system.  We
  // are not interested in moving marks from other staves to the top
  // staff; we only want to move marks from the system to the top
  // staff.
  if (sys != me->get_parent (Y_AXIS))
    return SCM_BOOL_F;

  me->set_parent (top_staff, Y_AXIS);
  me->flush_extent_cache (Y_AXIS);
  Axis_group_interface::add_element (top_staff, me);

  // Remove any cross-staff side-support dependencies
  Grob_array *ga = unsmob_grob_array (me->get_object ("side-support-elements"));
  if (ga)
    {
      vector<Grob*> const& elts = ga->array ();
      vector<Grob*> new_elts;
      for (vsize i = 0; i < elts.size (); ++i)
	{
	  if (me->common_refpoint (elts[i], Y_AXIS) == top_staff)
	    new_elts.push_back (elts[i]);
	}
      ga->set_array (new_elts);
    }
  return SCM_BOOL_T;
}


ADD_INTERFACE (Side_position_interface,
	       "Position a victim object (this one) next to other objects"
	       " (the support).  The property @code{direction} signifies where"
	       " to put the victim object relative to the support (left or"
	       " right, up or down?)\n"
	       "\n"
	       "The routine also takes the size of the staff into account if"
	       " @code{staff-padding} is set.  If undefined, the staff symbol"
	       " is ignored.",

	       /* properties */
	       "direction "
	       "minimum-space "
	       "padding "
	       "quantize-position "
	       "side-axis "
	       "side-support-elements "
	       "slur-padding "
	       "staff-padding "
	       );
