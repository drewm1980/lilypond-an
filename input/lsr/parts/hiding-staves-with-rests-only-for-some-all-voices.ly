%%  Do not edit this file; it is auto-generated from LSR!
\version "2.11.23"

\header { texidoc = "
If you have a choir score, or an orchestral score, where some voices
are quiet for a long time, you might want to hide staves containing
nothing (or only multi-measure rests). By default, lilypond will show
all staves, even if they only contain rests. To change this into what
is sometimes called a \"French Score\" style, simply add the
\RemoveEmptyStaffContext variable, in a \context block, to your \layout.

The first system would still show all staves for all voices. To force
this setting to also apply to the first system of a score, set
#'remove-first of VerticalAxisGroup to ##t.

If only one staff is displayed, the choir or the StaffGroup bracket
would also be hidden, so you will probably need to set
#'collapse-height of SystemStartBracket to #1 (or anything smaller than
5, which is the usual number of lines in a staff, see also snippet
\"Display bracket with only one stave in the system\").

If you have some voices where you still want to display all (even
empty) staves, you need to set the remove-empty property of the
VerticalAxisGroup to true for that one staff only. You can do this in
the \with section of the staff (in this example, the alto staff will
never erase empty lines, while the soprano staff will).
" }

sop = \relative c'' {
  R1*2 |\break 
  c4 c c c | R1 |\break
  R1*2 | \break
  R1*2 |\break 
  c4 c c c | R1 \bar"|."
}

alt = \relative c'' {
  g4 g g g | R1 | \break |
  R1*2 | \break
  R1*2 | \break
  g4 g g g | R1 | \break |
  g4 g g g | R1 \bar"|."
}

\layout {
  \context { 
    % add the RemoveEmptyStaffContext that erases rest-only staves
    \RemoveEmptyStaffContext 
  }
  \context {
    \Score
    % Remove all-rest staves also in the first system
    \override VerticalAxisGroup #'remove-first = ##t
  }
  \context {
    \ChoirStaff 
    % If only one non-empty staff in a system exists, still print the backet
    \override SystemStartBracket #'collapse-height = #1
  }
}

\score{
  \context ChoirStaff <<
    \context Staff=soprano <<
      \sop
      \set Staff.shortInstrumentName = "S"
    >>
    % never remove empty staves from the alto staff:
    \context Staff=alto \with { \override VerticalAxisGroup #'remove-empty = ##f }
    <<
      \alt
      \set Staff.shortInstrumentName = "A"
    >>
  >>
}
