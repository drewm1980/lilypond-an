\version "2.11.64"

\header {
  texidoc = "System separators may be defined as markups in the
@code{system-separator-markup} field of the paper block.  They are
centered between the boundary staves of each system."
}

\paper {
  system-separator-markup = \slashSeparator
}

foobar = \relative c' {
  c1 c \break
  c1 c \break
  c1 c
}
\book {
  \score {
    \new GrandStaff <<
      \new Staff \foobar 
      \new Staff \foobar 
    >>
  }
}
