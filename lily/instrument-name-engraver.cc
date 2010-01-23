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

#include "engraver.hh"
#include "pointer-group-interface.hh"
#include "side-position-interface.hh"
#include "axis-group-interface.hh"
#include "align-interface.hh"
#include "text-interface.hh"
#include "system.hh"

#include "translator.icc"

class Instrument_name_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (Instrument_name_engraver);

protected:
  Spanner *text_spanner_;

  SCM long_text_;
  SCM short_text_;

  vector<Grob *> axis_groups_;
  
  virtual void finalize ();
  DECLARE_ACKNOWLEDGER (axis_group);
  void process_music ();
  void start_spanner ();
  void consider_start_spanner ();
  void stop_spanner ();
};

Instrument_name_engraver::Instrument_name_engraver ()
{
  text_spanner_ = 0;

  long_text_ = SCM_EOL;
  short_text_ = SCM_EOL;
}

void
Instrument_name_engraver::process_music ()
{
  consider_start_spanner ();
}

void
Instrument_name_engraver::consider_start_spanner ()
{
  SCM long_text = get_property ("instrumentName");
  SCM short_text = get_property ("shortInstrumentName");

  if (!(Text_interface::is_markup (long_text)
	|| Text_interface::is_markup (short_text)))
    {
      long_text = get_property ("vocalName");
      short_text = get_property ("shortVocalName");
    }

  if ((Text_interface::is_markup (long_text)
       || Text_interface::is_markup (short_text))
      && (!text_spanner_
	  || short_text_ != short_text
	  || long_text_ != long_text))
    {
      if (text_spanner_)
	stop_spanner ();

      short_text_ = short_text;
      long_text_ = long_text;

      start_spanner ();
    }
}

void
Instrument_name_engraver::start_spanner ()
{
  text_spanner_ = make_spanner ("InstrumentName", SCM_EOL);

  Grob *col = unsmob_grob (get_property ("currentCommandColumn"));
  text_spanner_->set_bound (LEFT, col);
  text_spanner_->set_property ("text", short_text_);
  text_spanner_->set_property ("long-text", long_text_);

  /*
    UGH, should handle this in Score_engraver.
  */
  Grob *system = unsmob_grob (get_property ("rootSystem"));
  if (system)
    Axis_group_interface::add_element (system, text_spanner_);
  else
    text_spanner_->programming_error ("cannot find root system");
}

void
Instrument_name_engraver::acknowledge_axis_group (Grob_info info)
{
  if (dynamic_cast<Spanner *> (info.grob ())
      && Axis_group_interface::has_axis (info.grob (), Y_AXIS)

      /* ugh. */

      && !info.grob ()->internal_has_interface (ly_symbol2scm ("dynamic-interface"))
      && !info.grob ()->internal_has_interface (ly_symbol2scm ("piano-pedal-interface"))
      && !info.grob ()->internal_has_interface (ly_symbol2scm ("volta-interface"))
      && (!Align_interface::has_interface (info.grob ())))
    {
      axis_groups_.push_back (info.grob ());
    }
}

void
Instrument_name_engraver::finalize ()
{
  if (text_spanner_)
    stop_spanner ();
}

void
Instrument_name_engraver::stop_spanner ()
{
  for (vsize i = 0; i < axis_groups_.size (); i++)
    Pointer_group_interface::add_grob (text_spanner_,
				       ly_symbol2scm ("elements"),
				       axis_groups_[i]);
  
  text_spanner_->set_bound (RIGHT,
			    unsmob_grob (get_property ("currentCommandColumn")));

  Pointer_group_interface::set_ordered (text_spanner_,
					ly_symbol2scm ("elements"),
					false);

  text_spanner_ = 0;
}


ADD_ACKNOWLEDGER (Instrument_name_engraver, axis_group);

ADD_TRANSLATOR (Instrument_name_engraver,
		/* doc */
		"Create a system start text for instrument or vocal names.",
		
		/* create */
		"InstrumentName ",
		
		/* read */
		"currentCommandColumn "
		"instrumentName "
		"shortInstrumentName "
		"shortVocalName "
		"vocalName ",

		/* write */
		""
		);
