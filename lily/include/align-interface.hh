/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2000--2010 Han-Wen Nienhuys <hanwen@xs4all.nl>

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

#ifndef ALIGN_INTERFACE_HH
#define ALIGN_INTERFACE_HH

#include "lily-proto.hh"
#include "std-vector.hh"
#include "grob-interface.hh"

struct Align_interface
{
  DECLARE_SCHEME_CALLBACK (align_to_minimum_distances, (SCM));
  DECLARE_SCHEME_CALLBACK (align_to_ideal_distances, (SCM));
  static void align_elements_to_minimum_distances(Grob *, Axis a);
  static void align_elements_to_ideal_distances(Grob *);
  static vector<Real> get_minimum_translations (Grob *, vector<Grob*> const&,
						Axis a,
						bool safe, int start, int end);
  static void set_ordered (Grob *);
  static Axis axis (Grob *);
  static void add_element (Grob *, Grob *);
  static int get_count (Grob *, Grob *);

  DECLARE_GROB_INTERFACE();

  static Real get_pure_child_y_translation (Grob *, Grob *child, int start, int end);
};

#endif /* ALIGN_INTERFACE_HH */

