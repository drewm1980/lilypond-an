%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.38"

\header {
  lsrtags = "expressive-marks, keyboards, unfretted-strings"

  texidoc = "
In some situations, you may want to create slurs between notes from
different voices.

The solution is to add invisible notes to one of the voices, using
\\hideNotes.

This example is bar 235 of the Ciaconna from Bach's 2nd Partita for
solo violin, BWV 1004.

"
  doctitle = "Creating slurs across voices"
} % begin verbatim
\relative
<<
  {d16( a') s a s a[ s a] s a[ s a] } \\
  {\slurUp bes,[ s e]( \hideNotes a) \unHideNotes f[( \hideNotes a) \unHideNotes fis]( \hideNotes a) \unHideNotes g[( \hideNotes a) \unHideNotes gis]( \hideNotes a) }
>>
