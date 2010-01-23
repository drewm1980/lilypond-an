/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2004--2010 Han-Wen Nienhuys <hanwen@xs4all.nl>

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
#include "context.hh"
#include "warn.hh"

class Grace_engraver : public Engraver
{
  void consider_change_grace_settings ();
protected:
  void start_translation_timestep ();
  virtual void derived_mark () const;
  virtual void initialize ();

  TRANSLATOR_DECLARATIONS (Grace_engraver);
  Moment last_moment_;
  SCM grace_settings_;
public:
};

Grace_engraver::Grace_engraver ()
{
  grace_settings_ = SCM_EOL;
  last_moment_ = Moment (Rational (-1, 1));
}

void
Grace_engraver::initialize ()
{
  consider_change_grace_settings ();
}

void
Grace_engraver::consider_change_grace_settings ()
{
  Moment now = now_mom ();
  if (last_moment_.grace_part_ && !now.grace_part_)
    {
      for (SCM s = grace_settings_; scm_is_pair (s); s = scm_cdr (s))
	{
	  SCM context = scm_caar (s);
	  SCM entry = scm_cdar (s);
	  SCM grob = scm_cadr (entry);
	  SCM sym = scm_caddr (entry);

	  execute_pushpop_property (unsmob_context (context),
				    grob, sym, SCM_UNDEFINED);
	}

      grace_settings_ = SCM_EOL;
    }
  else if (!last_moment_.grace_part_ && now.grace_part_)
    {
      SCM settings = get_property ("graceSettings");

      grace_settings_ = SCM_EOL;
      for (SCM s = settings; scm_is_pair (s); s = scm_cdr (s))
	{
	  SCM entry = scm_car (s);
	  SCM context_name = scm_car (entry);
	  SCM grob = scm_cadr (entry);
	  SCM sym = scm_caddr (entry);
	  SCM val = scm_cadr (scm_cddr (entry));

	  Context *c = context ();
	  while (c && !c->is_alias (context_name))
	    c = c->get_parent_context ();

	  if (c)
	    {
	      execute_pushpop_property (c,
					grob, sym, val);
	      grace_settings_
		= scm_cons (scm_cons (c->self_scm (), entry), grace_settings_);
	    }
	  else
	      programming_error ("cannot find context from graceSettings: "
				 + ly_symbol2string (context_name));
	}
    }

  last_moment_ = now_mom ();
}

void
Grace_engraver::derived_mark () const
{
  scm_gc_mark (grace_settings_);
  Engraver::derived_mark ();
}

void
Grace_engraver::start_translation_timestep ()
{
  consider_change_grace_settings ();
}

#include "translator.icc"

ADD_TRANSLATOR (Grace_engraver,
		/* doc */
		"Set font size and other properties for grace notes.",

		/* create */
		"",

		/* read */
		"graceSettings ",

		/* write */
		""
		);
