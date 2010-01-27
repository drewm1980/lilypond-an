/*
  note-heads-engraver.cc -- part of GNU LilyPond

  (c) 1997--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "engraver.hh"

#include <cctype>
using namespace std;

#include "duration.hh"
#include "item.hh"
#include "output-def.hh"
#include "pitch.hh"
#include "rhythmic-head.hh"
#include "staff-symbol-referencer.hh"
#include "stream-event.hh"
#include "warn.hh"

#include "translator.icc"
class Layout_pos 
{
public:
  virtual int pos(Pitch* pit) = 0;
  virtual ~Layout_pos()
  {
  }
};

class Layout_pos_traditional : public Layout_pos
{
public:
  virtual int pos(Pitch* pit)
  {
    return pit ? pit->steps () : 0;
  }
};

static Layout_pos_traditional layout_pos_traditional;

class Layout_pos_semitone : public Layout_pos
{
public:
  virtual int pos(Pitch* pit)
  {
    return pit ? pit->rounded_semitone_pitch() : 0;
  }
};

static Layout_pos_semitone layout_pos_semitone;
  

class Note_heads_engraver : public Engraver
{
  vector<Item*> notes_;
  vector<Stream_event*> note_evs_;

public:
  TRANSLATOR_DECLARATIONS (Note_heads_engraver);

protected:
  DECLARE_TRANSLATOR_LISTENER (note);
  void process_music ();
  void stop_translation_timestep ();
};

Note_heads_engraver::Note_heads_engraver ()
{
}

IMPLEMENT_TRANSLATOR_LISTENER (Note_heads_engraver, note);
void
Note_heads_engraver::listen_note (Stream_event *ev)
{
  note_evs_.push_back (ev);
}

void
Note_heads_engraver::process_music ()
{
  Layout_pos *layout_pos = &layout_pos_traditional;
  SCM layout_property = get_property("staff-line-layout");
  if (layout_property)
    {
      if (ly_is_equal(layout_property, ly_symbol2scm("traditional"))){
	layout_pos = &layout_pos_traditional;
      }
      else if (ly_is_equal(layout_property, ly_symbol2scm("semitone"))){
	layout_pos = &layout_pos_semitone;
      }
    }
      
  for (vsize i = 0; i < note_evs_.size (); i++)
    {
      Stream_event *ev = note_evs_[i];
      Item *note = make_item ("NoteHead", ev->self_scm ());

      Pitch *pit = unsmob_pitch (ev->get_property ("pitch"));

#if 0 /* TODO: should have a mechanism to switch off these warnings. */

      if (!pit)
	ev->origin ()->warning (_ ("NoteEvent without pitch"));
#endif

      SCM c0 = get_property ("middleCPosition");
      int middleC;
      if (scm_is_number (c0))
	middleC = scm_to_int(c0);
      else
	middleC = 0;
      int pos = layout_pos->pos(pit);
	    
      pos += middleC;
      
      note->set_property ("staff-position", scm_from_int (pos));

      /*
	Shape note heads change on step of the scale.
      */
      SCM shape_vector = get_property ("shapeNoteStyles");
      if (scm_is_vector (shape_vector))
	{
	  SCM scm_tonic = get_property ("tonic");
	  Pitch tonic (0, 0, 0);
	  if (unsmob_pitch (scm_tonic))
	    tonic = *unsmob_pitch (scm_tonic);

	  unsigned int delta = (pit->get_notename () - tonic.get_notename () + 7) % 7;

	  SCM style = SCM_EOL;
	  if (scm_c_vector_length (shape_vector) > delta
	      && scm_is_symbol (scm_vector_ref (shape_vector, scm_from_int (delta))))
	    style = scm_vector_ref (shape_vector, scm_from_int (delta));
	  if (scm_is_symbol (style))
	    note->set_property ("style", style);
	}

      notes_.push_back (note);
    }
}

void
Note_heads_engraver::stop_translation_timestep ()
{
  notes_.clear ();
  note_evs_.clear ();
}

ADD_TRANSLATOR (Note_heads_engraver,
		/* doc */
		"Generate note heads.",

		/* create */
		"NoteHead ",
		/* read */ "middleCPosition "
		"staffLineLayoutFunction "
		"staff-line-layout",
		/* write */ "");
