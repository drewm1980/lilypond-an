%%  Do not edit this file; it is auto-generated from LSR!
\version "2.10.12"

\header { texidoc = "
The number of lines in a staff may changed by overriding
@code{line-count} in the properties of @code{StaffSymbol}.


" }

upper = \relative c'' {
  c1 d e f
}

lower = \relative c {
  c1 b a g
}

\score {
  \context PianoStaff <<
    \new Staff <<
      \upper
    >>  
    \new Staff  {
	\override Staff.StaffSymbol  #'line-count = #4 
        \clef bass
        \lower
    }
  >>

}
