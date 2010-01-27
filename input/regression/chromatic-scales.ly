\version "2.10"
\include "english.ly"
% This shows how to use internal-ledger-lines and staff-line-layout
% to produce alternative chromatic notation.
% This notation is 6-6 Tetragram by Richard Parncutt
% For more information,
% see http://web.syr.edu/~pwmorris/mnma/gallery/4LineNotations.html

scales = \relative {
  a, as b c cs d ds e f fs g gs
  a as b c cs d ds e f fs g gs
  a as b c cs d ds e f fs g gs
  a
}

\new Staff \with {
  \remove "Accidental_engraver"
  staff-line-layout = #'semitone
  middleCPosition = #-2
  clefGlyph = #"clefs.G"
  clefPosition = #(+ -2 7)
}
{
  \override Staff.StaffSymbol #'line-count = #8
  \override Staff.StaffSymbol #'line-positions = #'(-9 -7 -5 -3 3 5 7 9)
  \override Staff.StaffSymbol #'internal-ledger-lines = #'((-1 1))
  \time 4/4
  <<
    \scales
    \context NoteNames {
      \set printOctaveNames= ##f
      \scales
    }
  >>
}
