\version "2.11.65"
\header {
  lsrtags = "fretted-strings"
  texidoc = "Predefined fret diagrams can be added for new instruments
in addition to the standards used for guitar.  This file shows how
this is done by defining a new string-tuning and a few predefined
fretboards for the Venezuelan cuatro.

This file also shows how fingerings can be included in the chords
used as reference points for the chord lookup, and displayed in 
the fret diagram and the @code{TabStaff}, but not the music.

These fretboards are not transposable because they contain string
information.  This is planned to be corrected in the future.

"
  doctitle = "Defining predefined fretboards for other instruments"
}

%LSR: Thanks to Jesus Guillermo Andrade for the string-tuning
%LSR: and fretboard information.

% add FretBoards for the Cuatro
%   Note: This section could be put into a separate file
%      predefined-cuatro-fretboards.ly
%      and \included into each of your compositions

cuatroTuning = #'(11 18 14 9)

dSix = { <a\4 b\1 d\3 fis\2> }
dMajor = { <a\4 d\1 d\3 fis \2> }
aMajSeven = { <a\4 cis\1 e\3 g\2> }
dMajSeven = { <a\4 c\1 d\3 fis\2> }
gMajor = { <b\4 b\1 d\3 g\2> }

\storePredefinedDiagram \dSix
                        #cuatroTuning
                        #"o;o;o;o;"
\storePredefinedDiagram \dMajor
                        #cuatroTuning
                        #"o;o;o;3-3;"
\storePredefinedDiagram \aMajSeven
                        #cuatroTuning
                        #"o;2-2;1-1;2-3;"
\storePredefinedDiagram \dMajSeven
                        #cuatroTuning
                        #"o;o;o;1-1;"
\storePredefinedDiagram \gMajor
                        #cuatroTuning
                        #"2-2;o;1-1;o;"

% end of potential include file /predefined-cuatro-fretboards.ly


#(set-global-staff-size 16)

primerosNames = \chordmode {
  d:6 d a:maj7 d:maj7 
  g
}
primeros = {
  \dSix \dMajor \aMajSeven \dMajSeven
  \gMajor
}

\score {
  <<
    \new ChordNames {
      \set chordChanges = ##t
      \primerosNames
    }

    \new Staff {
      \new Voice \with {
        \remove "New_fingering_engraver"
      } 
      \relative c'' {
        \primeros
      }
    }

    \new FretBoards {
      \set stringTunings = #cuatroTuning
      \override FretBoard
        #'(fret-diagram-details string-count) = #'4
      \override FretBoard
        #'(fret-diagram-details finger-code) = #'in-dot
      \primeros
    }

    \new TabStaff \relative c'' {
      \set TabStaff.stringTunings = #cuatroTuning
      \primeros
    }
    
  >>

  \layout { 
    \context {
      \Score
      \override SpacingSpanner
        #'base-shortest-duration = #(ly:make-moment 1 16)
    }
  }
  \midi { }
}
