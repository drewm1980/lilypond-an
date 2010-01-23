/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2006--2010 Joe Neeman <joeneeman@gmail.com>

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

#ifndef SKYLINE_HH
#define SKYLINE_HH

#include "lily-proto.hh"
#include "axis.hh"
#include "box.hh"
#include "interval.hh"
#include "direction.hh"
#include "std-vector.hh"
#include "smobs.hh"

#include <list>

struct Building
{
  Real end_;
  Real y_intercept_;
  Real slope_;

  void precompute (Real start, Real start_height, Real end_height, Real end);
  Building (Real start, Real start_height, Real end_height, Real end);
  Building (Box const &b, Real horizon_padding, Axis a, Direction d);
  void print () const;

  Real height (Real x) const;
  Real intersection_x (Building const &other) const;
  void leading_part (Real chop);
  bool conceals (Building const &other, Real x) const;
  Building sloped_neighbour (Real start, Real horizon_padding, Direction d) const;
};

class Skyline
{
private:
  list<Building> buildings_;
  Direction sky_;
  
  void internal_merge_skyline (list<Building>*, list<Building>*,
			       list<Building> *const result);
  list<Building> internal_build_skyline (list<Box>*, Real, Axis, Direction);

  DECLARE_SIMPLE_SMOBS(Skyline);

public:
  Skyline ();
  Skyline (Skyline const &src);
  Skyline (Direction sky);
  Skyline (vector<Box> const &bldgs, Real horizon_padding, Axis a, Direction sky);
  Skyline (Box const &b, Real horizon_padding, Axis a, Direction sky);
  
  vector<Offset> to_points (Axis) const;
  void merge (Skyline const &);
  void insert (Box const &, Real horizon_padding, Axis);
  void print () const;
  void print_points () const;
  void raise (Real);
  void shift (Real);
  Real distance (Skyline const &) const;
  Real height (Real airplane) const;
  Real max_height () const;
  void set_minimum_height (Real height);
  void clear ();
  bool is_empty () const;
};

extern bool debug_skylines;

#endif /* SKYLINE_HH */

