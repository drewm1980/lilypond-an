/*
  page-turn-page-breaking.hh -- break lines and pages optimally
  for a whole Paper_book such that page turns can only occur
  at specific places.

  source file of the GNU LilyPond music typesetter

  (c) 2006 Joe Neeman <joeneeman@gmail.com>
*/

#ifndef PAGE_TURN_PAGE_BREAKING_HH
#define PAGE_TURN_PAGE_BREAKING_HH

#include "constrained-breaking.hh"
#include "page-breaking.hh"
#include "lily-guile.hh"

/*
  A dynamic programming solution to breaking pages
 */
class Page_turn_page_breaking: public Page_breaking
{
public:
  virtual SCM solve ();

  Page_turn_page_breaking (Paper_book *pb);
  virtual ~Page_turn_page_breaking ();

protected:
  struct Break_node
  {
    vsize prev_;
    int first_page_number_;
    vsize page_count_;

    Real force_;
    Real penalty_;

    Real line_force_;
    Real line_penalty_;

    /* true if every score here is too widely spaced */
    bool too_many_lines_;

    Real demerits_;
    vsize break_pos_; /* index into breaks_ */

    vector<vsize> div_;  /* our division of systems between scores on this page */
    vector<vsize> system_count_; /* systems per page */

    Break_node ()
    {
      prev_ = break_pos_ = VPOS;
      penalty_ = force_ = 0;
      line_penalty_ = line_force_ = 0;
      demerits_ = infinity_f;
      first_page_number_ = 0;
      too_many_lines_ = false;
    }
  };

  std::vector<Break_node> state_;

  Break_node put_systems_on_pages (vsize start,
				   vsize end,
				   vector<Line_details> const &lines,
				   vector<vsize> const &system_div,
				   int page_number);

  SCM make_lines (vector<Break_node> *breaks);
  SCM make_pages (vector<Break_node> const &breaks, SCM systems);

  Real calc_demerits (Break_node const &me);
  void calc_subproblem (vsize i);
};
#endif /* PAGE_TURN_PAGE_BREAKING_HH */