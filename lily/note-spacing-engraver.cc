/* 
  note-spacing-engraver.cc -- implement Note_spacing_engraver
  
  source file of the GNU LilyPond music typesetter
  
  (c) 2006 Han-Wen Nienhuys <hanwen@lilypond.org>
  
*/

#include "engraver.hh"

#include "item.hh"
#include "pointer-group-interface.hh"

#include "translator.icc"

class Note_spacing_engraver : public Engraver
{
  Grob *last_spacing_;
  Grob *spacing_;

  void add_spacing_item (Grob *);

  TRANSLATOR_DECLARATIONS (Note_spacing_engraver);
protected:

  DECLARE_ACKNOWLEDGER (rhythmic_grob);
  DECLARE_ACKNOWLEDGER (note_column);
  void stop_translation_timestep ();
};

Note_spacing_engraver::Note_spacing_engraver ()
{
  last_spacing_ = 0;
  spacing_ = 0;
}

void
Note_spacing_engraver::add_spacing_item (Grob *g)
{
  if (!spacing_)
    {
      spacing_ = make_item ("NoteSpacing", g->self_scm ());
    }
  
  
  if (spacing_)
    {
      Pointer_group_interface::add_grob (spacing_,
					 ly_symbol2scm ("left-items"),
					 g);

      if (last_spacing_)
	{
	  Pointer_group_interface::add_grob (last_spacing_,
					     ly_symbol2scm ("right-items"),
					     g);
	}
    }
}


void
Note_spacing_engraver::acknowledge_note_column (Grob_info gi)
{
  add_spacing_item (gi.grob ());
}

void
Note_spacing_engraver::acknowledge_rhythmic_grob (Grob_info gi)
{
  add_spacing_item (gi.grob ());
}

void
Note_spacing_engraver::stop_translation_timestep ()
{
  if (spacing_)
    {
      last_spacing_ = spacing_;
      spacing_ = 0;
    }
}

ADD_ACKNOWLEDGER (Note_spacing_engraver, note_column);
ADD_ACKNOWLEDGER (Note_spacing_engraver, rhythmic_grob);

ADD_TRANSLATOR (Note_spacing_engraver,
		/* doc */ "Generates NoteSpacing, an object linking horizontal lines for use in spacing.",
		/* create */ "NoteSpacing",
		/* read */ "",
		/* write */ "");