/*
  grid-line-span-engraver.cc --  implement Grid_line_span_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2005--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "engraver.hh"
#include "item.hh"
#include "grid-line-interface.hh"

class Grid_line_span_engraver : public Engraver
{
  Item *spanline_;
  vector<Item*> lines_;

public:
  TRANSLATOR_DECLARATIONS (Grid_line_span_engraver);
protected:
  DECLARE_ACKNOWLEDGER (grid_point);
  void stop_translation_timestep ();
};

Grid_line_span_engraver::Grid_line_span_engraver ()
{
  spanline_ = 0;
}

void
Grid_line_span_engraver::acknowledge_grid_point (Grob_info i)
{
  int depth = i.origin_contexts (this).size ();
  if (depth)
    {
      Item *it = dynamic_cast<Item *> (i.grob ());
      lines_.push_back (it);

      if (lines_.size () >= 2 && !spanline_)
	{
	  spanline_ = make_item ("GridLine", SCM_EOL);
	  spanline_->set_parent (lines_[0], X_AXIS);
	}
    }
}

void
Grid_line_span_engraver::stop_translation_timestep ()
{
  if (spanline_)
    {
      for (vsize i = 0; i < lines_.size (); i++)
	Grid_line_interface::add_grid_point (spanline_, lines_[i]);

      spanline_ = 0;
    }
  lines_.resize (0);
}

#include "translator.icc"
ADD_ACKNOWLEDGER (Grid_line_span_engraver, grid_point);
ADD_TRANSLATOR (Grid_line_span_engraver,
		/* doc */
		"This engraver makes cross-staff lines: It catches all normal"
		" lines and draws a single span line across them.",

		/* create */
		"GridLine ",

		/* read */
		"",

		/* write */
		""
		);
