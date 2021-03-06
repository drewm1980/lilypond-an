%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.64"

\header {
  lsrtags = "winds"

  texidoc = "
It is possible to indicate special articulation techniques such as
flute's \"tongue slap\", by replacing the note head with the
appropriate glyph.

"
  doctitle = "Flute slap notation"
} % begin verbatim

slap =
#(define-music-function (parser location music) (ly:music?)
#{\override NoteHead #'stencil = #ly:text-interface::print
  \override NoteHead #'text = \markup \musicglyph #"scripts.sforzato"
  \override NoteHead #'extra-offset = #'(0.1 . 0.0 )
  $music
  \revert NoteHead #'stencil
  \revert NoteHead #'text
  \revert NoteHead #'extra-offset #})

\relative c' {
  c \slap c d r \slap { g a } b r
}
