/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2002--2010 Juergen Reuter <reuter@ipd.uka.de>

  Han-Wen Nienhuys <hanwen@xs4all.nl

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

#include "accidental-placement.hh"
#include "axis-group-interface.hh"
#include "item.hh"
#include "note-head.hh"
#include "pitch-interval.hh"
#include "pointer-group-interface.hh"
#include "protected-scm.hh"
#include "side-position-interface.hh"
#include "separation-item.hh"
#include "staff-symbol-referencer.hh"
#include "stream-event.hh"

#include "translator.icc"

class Ambitus_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (Ambitus_engraver);
protected:
  DECLARE_ACKNOWLEDGER (note_head);

  void process_music ();
  void stop_translation_timestep ();
  virtual void finalize ();
  virtual void derived_mark () const;

private:
  void create_ambitus ();
  Item *ambitus_;
  Item *group_;
  Drul_array<Item *> heads_;
  Drul_array<Item *> accidentals_;
  Drul_array<Stream_event *> causes_;
  Pitch_interval pitch_interval_;
  bool is_typeset_;
  int start_c0_;
  SCM start_key_sig_;
};

void
Ambitus_engraver::derived_mark () const
{
  scm_gc_mark (start_key_sig_);
}

void
Ambitus_engraver::create_ambitus ()
{
  ambitus_ = make_item ("AmbitusLine", SCM_EOL);
  group_ = make_item ("Ambitus", SCM_EOL);
  Direction d = DOWN;
  do
    {
      heads_[d] = make_item ("AmbitusNoteHead", SCM_EOL);
      accidentals_[d] = make_item ("AmbitusAccidental", SCM_EOL);
      accidentals_[d]->set_parent (heads_[d], Y_AXIS);
      heads_[d]->set_object ("accidental-grob",
			     accidentals_[d]->self_scm ());
      Axis_group_interface::add_element (group_, heads_[d]);
      Axis_group_interface::add_element (group_, accidentals_[d]);
    }
  while (flip (&d) != DOWN);

  ambitus_->set_parent (heads_[DOWN], X_AXIS);
  Axis_group_interface::add_element (group_, ambitus_);

  is_typeset_ = false;
}

Ambitus_engraver::Ambitus_engraver ()
{
  ambitus_ = 0;
  heads_.set (0, 0);
  accidentals_.set (0, 0);
  group_ = 0;
  is_typeset_ = false;
  start_key_sig_ = SCM_EOL;
}

void
Ambitus_engraver::process_music ()
{
  /*
   * Ensure that ambitus is created in the very first timestep
   */
  if (!ambitus_)
    create_ambitus ();
}

void
Ambitus_engraver::stop_translation_timestep ()
{
  if (ambitus_ && !is_typeset_)
    {
      /*
       * Evaluate middleCPosition not until now, since otherwise we
       * may then oversee a clef that is defined in a staff context if
       * we are in a voice context; middleCPosition would then be
       * assumed to be 0.
       */
      start_c0_ = robust_scm2int (get_property ("middleCPosition"), 0);
      start_key_sig_ = get_property ("keySignature");

      is_typeset_ = true;
    }
}

void
Ambitus_engraver::acknowledge_note_head (Grob_info info)
{
  Stream_event *nr = info.event_cause ();
  if (nr && nr->in_event_class ("note-event"))
    {
      SCM p = nr->get_property ("pitch");
      /*
	If the engraver is added to a percussion context,
	filter out unpitched note heads.
      */
      if (!unsmob_pitch (p))
	return;
      Pitch pitch = *unsmob_pitch (p);
      Drul_array<bool> expands = pitch_interval_.add_point (pitch);
      if (expands[UP])
	causes_[UP] = nr;
      if (expands[DOWN])
	causes_[DOWN] = nr;
    }
}

void
Ambitus_engraver::finalize ()
{
  if (ambitus_ && !pitch_interval_.is_empty ())
    {
      Grob *accidental_placement =
	make_item ("AccidentalPlacement", accidentals_[DOWN]->self_scm ());

      Direction d = DOWN;
      do
	{
	  Pitch p = pitch_interval_[d];
	  heads_[d]->set_property ("cause", causes_[d]->self_scm());
	  heads_[d]->set_property ("staff-position",
				   scm_from_int (start_c0_ + p.steps ()));

	  SCM handle = scm_assoc (scm_cons (scm_from_int (p.get_octave ()),
					    scm_from_int (p.get_notename ())),
				  start_key_sig_);

	  if (handle == SCM_BOOL_F)
	    handle = scm_assoc (scm_from_int (p.get_notename ()),
				start_key_sig_);

	  Rational sig_alter = (handle != SCM_BOOL_F)
	    ? robust_scm2rational (scm_cdr (handle), Rational (0))
	    : Rational (0);

	  if (sig_alter == p.get_alteration ())
	    {
	      accidentals_[d]->suicide ();
	      heads_[d]->set_object ("accidental-grob", SCM_EOL);
	    }
	  else
	    accidentals_[d]->
	      set_property ("alteration",
			    ly_rational2scm (p.get_alteration ()));
	  Separation_item::add_conditional_item (heads_[d],
						 accidental_placement);
	  Accidental_placement::add_accidental (accidental_placement,
						accidentals_[d]);
	  Pointer_group_interface::add_grob (ambitus_,
					     ly_symbol2scm ("note-heads"),
					     heads_[d]);
	}
      while (flip (&d) != DOWN);

      Axis_group_interface::add_element (group_, accidental_placement);
    }
  else
    {
      Direction d = DOWN;
      do
	{
	  accidentals_[d]->suicide ();
	  heads_[d]->suicide ();
	}
      while (flip (&d) != DOWN);

      ambitus_->suicide ();
    }
}

ADD_ACKNOWLEDGER (Ambitus_engraver, note_head);
ADD_TRANSLATOR (Ambitus_engraver,
		/* doc */
		"Create an ambitus.",

		/* create */
		"AccidentalPlacement "
		"Ambitus "
		"AmbitusAccidental "
		"AmbitusLine "
		"AmbitusNoteHead ",

		/* read */
		"keySignature "
		"middleCPosition ",

		/* write */
		""
		);
