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

#ifndef PAGE_SPACING_HH
#define PAGE_SPACING_HH

#include "constrained-breaking.hh"
#include "page-spacing-result.hh"

/* This is a penalty that we add whenever a page breaking solution
   is not bad enough to completely discard, but bad enough that
   it is worse than any "proper" solution. For example, if we didn't
   manage to fit systems on the desired number of pages or if there was
   too big for a page.

   This constant is large enough that it dominates any reasonable penalty,
   but small enough that nothing will overflow to infinity (so that we
   can still distinguish bad spacings by the number of BAD_SPACING_PENALTYs
   that they incur.

   BAD_SPACING_PENALTY is for occasions where the spacing is bad.
   TERRIBLE_SPACING_PENALTY is for when we are disregarding a user override
   (for example, we are failing to satisfy min-systems-per-page). These user
   overrides are more important than getting good spacing, so they get a
   larger penalty.
*/
const Real BAD_SPACING_PENALTY = 1e6;
const Real TERRIBLE_SPACING_PENALTY = 1e8;


/* for page_count > 2, we use a dynamic algorithm similar to
   constrained-breaking -- we have a class that stores the intermediate
   calculations so they can be reused for querying different page counts.
*/
class Page_spacer
{
public:
  Page_spacer (vector<Line_details> const &lines, vsize first_page_num, Page_breaking const*);
  Page_spacing_result solve (vsize page_count);

private:
  struct Page_spacing_node
  {
    Page_spacing_node ()
    {
      demerits_ = infinity_f;
      force_ = infinity_f;
      penalty_ = infinity_f;
      prev_ = VPOS;
      system_count_status_ = SYSTEM_COUNT_OK;
    }

    Real demerits_;
    Real force_;
    Real penalty_;
    vsize prev_;
    int system_count_status_;
  };

  Page_breaking const *breaker_;
  vsize first_page_num_;
  vector<Line_details> lines_;
  Matrix<Page_spacing_node> state_;
  vsize max_page_count_;

  bool ragged_;
  bool ragged_last_;

  void resize (vsize page_count);
  bool calc_subproblem (vsize page, vsize lines);
};

struct Page_spacing
{
  Real force_;
  Real page_height_;
  Real rod_height_;
  Real spring_len_;
  Real inverse_spring_k_;

  Line_details last_line_;
  Line_details first_line_;
  Page_breaking const *breaker_;

  Page_spacing (Real page_height, Page_breaking const *breaker)
  {
    page_height_ = page_height;
    breaker_ = breaker;
    clear ();
  }

  void calc_force ();
  void resize (Real new_height);
  void append_system (const Line_details &line);
  void prepend_system (const Line_details &line);
  void clear ();
};

#endif /* PAGE_SPACING_HH */
