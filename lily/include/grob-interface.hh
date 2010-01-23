/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2002--2010 Han-Wen Nienhuys <hanwen@xs4all.nl>

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

#ifndef INTERFACE_HH
#define INTERFACE_HH

#include "lily-guile.hh"

#define DECLARE_GROB_INTERFACE() \
  static SCM interface_symbol_;	   \
  static bool has_interface (Grob*)

#define ADD_INTERFACE(cl, b, c)				\
  SCM cl::interface_symbol_; \
  bool cl::has_interface (Grob *me)				\
  {								\
    return me->internal_has_interface (interface_symbol_);	\
  }								\
  void cl ## _init_ifaces ()					\
  {								\
    cl::interface_symbol_ = add_interface (#cl, b, c);		\
  }								\
  ADD_SCM_INIT_FUNC (cl ## ifaces, cl ## _init_ifaces);

SCM add_interface (char const *cxx_name,
		    char const *descr,
		    char const *vars);

SCM ly_add_interface (SCM, SCM, SCM);
void internal_add_interface (SCM, SCM, SCM);
SCM ly_all_grob_interfaces ();

#endif /* INTERFACE_HH */

