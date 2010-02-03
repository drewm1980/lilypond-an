\version "2.11.51"

\header {
  texidoc = "By default, we start with page 1, which is on the right hand side
of a double page. In this example, auto-first-page-number is set to ##t.
ALthough the music will fit on a single page, it would require stretching the
first page badly, so we should automatically set the first page
number to 2 in order to avoid a bad page turn."
}

\paper {
  page-breaking = #ly:page-turn-breaking
  auto-first-page-number = ##t
  print-first-page-number = ##t
}

#(set-default-paper-size "a6")

\layout {
  \context {
    \Staff
    \consists "Page_turn_engraver"
  }
}

\book {
  \score {
    {
      a b c d R1
      \repeat unfold 26 {a4 b c d}
    }
  }
}
