/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2005--2010 Han-Wen Nienhuys <hanwen@xs4all.nl>

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

#include "profile.hh"

void note_property_access (SCM *table, SCM sym);

SCM context_property_lookup_table;
SCM grob_property_lookup_table;
SCM prob_property_lookup_table;

LY_DEFINE (ly_property_lookup_stats, "ly:property-lookup-stats",
	   1, 0, 0, (SCM sym),
	   "Return hash table with a property access corresponding to"
	   " @var{sym}.  Choices are @code{prob}, @code{grob}, and"
	   " @code{context}.")
{
  if (sym == ly_symbol2scm ("context"))
    return context_property_lookup_table ? context_property_lookup_table
      : scm_c_make_hash_table (1);
  if (sym == ly_symbol2scm ("prob"))
    return prob_property_lookup_table ? prob_property_lookup_table
      : scm_c_make_hash_table (1);
  if (sym == ly_symbol2scm ("grob"))
    return grob_property_lookup_table ? grob_property_lookup_table
      : scm_c_make_hash_table (1);
  return scm_c_make_hash_table (1);
}


void
note_property_access (SCM *table, SCM sym)
{
  /*
    Statistics: which properties are looked up?
  */
  if (!*table)
    *table = scm_permanent_object (scm_c_make_hash_table (259));

  SCM hashhandle = scm_hashq_get_handle (*table, sym);
  if (hashhandle == SCM_BOOL_F)
    {
      scm_hashq_set_x (*table, sym, scm_from_int (0));
      hashhandle = scm_hashq_get_handle (*table, sym);
    }

  int count = scm_to_int (scm_cdr (hashhandle)) + 1;
  scm_set_cdr_x (hashhandle, scm_from_int (count));
}
