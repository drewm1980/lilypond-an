\version "2.11.51"

\header{
  texidoc="
The tempo command supports text markup and/or duration=count. Using the
Score.hideTempoNote, one can hide the duration=count in the tempo mark.
"
}

\relative c'' {
  \tempo "Allegro" c1
  \tempo "Allegro" c1
  \set Score.tempoText = #"blah" d1
  \tempo \markup{\italic \medium "Allegro"} c1\break
  \tempo 4=120 c1
  \tempo "Allegro" 4=120 c1
  \tempo "Allegro" 4=120 c1
  \tempo "Allegro" 4=110 c1
  \tempo "Allegretto" 4=110 c1\break

  \set Score.tempoHideNote = ##f
  \tempo "Allegro" 4=120 c1
  \set Score.tempoHideNote = ##t
  \tempo "No note" 8=160 c1
  \tempo "Still not" c1
  % No text and also no note => \null markup
  \tempo 4=100 c1
  \tempo "Allegro" 4=120 c1
  \set Score.tempoHideNote = ##f
  \tempo "With note" 8=80 c1\break

  % Unsetting the tempoText using only note=count:
  \tempo 8=80 c1
  \tempo "Allegro" 8=80 c1
  \tempo 8=80 c1
  
  % Unsetting the count using only text
  \tempo "no note (text-only)" c1
}
