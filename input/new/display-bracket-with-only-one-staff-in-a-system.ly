\version "2.11.61"
\header {
  lsrtags = "staff-notation,tweaks-and-overrides"
  texidoc = "If there is only one staff in one of the staff types
@code{ChoirStaff} or @code{StaffGroup}, the bracket and the starting
bar line will not be displayed as standard behavior.  This can be changed
by overriding the relevant properties.

Note that in contexts such as @code{PianoStaff} and @code{GrandStaff}
where the systems begin with a brace instead of a bracket, another
property has to be set, as shown on the second system in the example.
"
  doctitle = "Display bracket with only one staff in a system"
}

\markup \left-column {
  \score {
    \new StaffGroup <<
      % Must be lower than the actual number of staff lines
      \override StaffGroup.SystemStartBracket #'collapse-height = #1
      \override Score.SystemStartBar #'collapse-height = #1
      \new Staff {
        c'1
      }
    >>
    \layout { }
  }
  \score {
    \new PianoStaff <<
      \override PianoStaff.SystemStartBrace #'collapse-height = #1
      \override Score.SystemStartBar #'collapse-height = #1
      \new Staff {
        c'1
      }
    >>
    \layout { }
  }
}
