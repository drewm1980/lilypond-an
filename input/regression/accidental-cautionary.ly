\version "2.11.51"
\header {

texidoc = "Cautionary accidentals may be indicated using either
parentheses (default) or smaller accidentals.

"

}

\layout { ragged-right = ##t }

{
    c''4
    cis''?4
    \once \override Staff.AccidentalCautionary #'parenthesized = ##f
    \once \override Staff.AccidentalCautionary #'font-size = #-2
    cis''?4
    \once \override Staff.AccidentalCautionary #'parenthesized = ##t
    cis''?4
}

