\header {
  texidoc = "beamlets don't run to end of line if there are no other
  beamlets on the same height."
  
}
\version "2.11.51"

\paper {
  raggedright = ##t
}

\relative {
  \time 1/4
  \override Beam #'breakable = ##t
  r16 r16. c32[ c16 \break c8. ] r16
}
