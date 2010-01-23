/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 1996--2010 Jan Nieuwenhuizen <janneke@gnu.org>

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

#ifndef AUDIO_STAFF_HH
#define AUDIO_STAFF_HH

#include "std-vector.hh"
#include "lily-proto.hh"
#include "audio-element.hh"

struct Audio_staff : public Audio_element
{
  void add_audio_item (Audio_item *l);
  void output (Midi_stream &midi_stream_r, int track);

  Audio_staff ();
  
  vector<Audio_item*> audio_items_;
  int channel_;
};

#endif // AUDIO_STAFF_HH
