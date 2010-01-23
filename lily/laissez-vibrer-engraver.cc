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

#include "engraver.hh"
#include "item.hh"
#include "pointer-group-interface.hh"
#include "stream-event.hh"

#include "translator.icc"

class Laissez_vibrer_engraver : public Engraver
{
  Stream_event *event_;
  Grob *lv_column_;
  vector<Grob*> lv_ties_;

  void stop_translation_timestep ();
  DECLARE_ACKNOWLEDGER (note_head);
protected:
  DECLARE_TRANSLATOR_LISTENER (laissez_vibrer);
public:
  TRANSLATOR_DECLARATIONS (Laissez_vibrer_engraver);
};

Laissez_vibrer_engraver::Laissez_vibrer_engraver ()
{
  event_ = 0;
  lv_column_ = 0;
}

void
Laissez_vibrer_engraver::stop_translation_timestep ()
{
  event_ = 0;
  lv_column_ = 0;
  lv_ties_.clear ();
}

IMPLEMENT_TRANSLATOR_LISTENER (Laissez_vibrer_engraver, laissez_vibrer);
void
Laissez_vibrer_engraver::listen_laissez_vibrer (Stream_event *ev)
{
  ASSIGN_EVENT_ONCE (event_, ev);
}

void
Laissez_vibrer_engraver::acknowledge_note_head (Grob_info inf)
{
  if (!event_)
    return;

  SCM cause = event_->self_scm ();

  if (!lv_column_)
    lv_column_ = make_item ("LaissezVibrerTieColumn", cause);

  Grob *lv_tie = make_item ("LaissezVibrerTie", cause);
  lv_tie->set_object ("note-head", inf.grob ()->self_scm ());

  Pointer_group_interface::add_grob (lv_column_, ly_symbol2scm ("ties"),
				     lv_tie);

  if (is_direction (unsmob_stream_event (cause)->get_property ("direction")))
    {
      Direction d = to_dir (unsmob_stream_event (cause)->get_property ("direction"));
      lv_tie->set_property ("direction", scm_from_int (d));
    }

  lv_tie->set_parent (lv_column_, Y_AXIS);

  lv_ties_.push_back (lv_tie);
}

ADD_ACKNOWLEDGER (Laissez_vibrer_engraver, note_head);
ADD_TRANSLATOR (Laissez_vibrer_engraver,
		/* doc */
		"Create laissez vibrer items.",

		/* create */
		"LaissezVibrerTie "
		"LaissezVibrerTieColumn ",

		/* read */
		"",

		/* write */
		""
		);
