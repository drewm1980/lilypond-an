\version "2.11.62"

\header {
  lsrtags = "staff-notation"
  texidoc = "New metronome marks can be created in markup mode,
but they will not change the tempo in MIDI output."

  doctitle = "Creating metronome marks in markup mode"
}

\relative c' {
  \tempo \markup {
    \concat {
      (
      \smaller \general-align #Y #DOWN \note #"16." #1
      " = "
      \smaller \general-align #Y #DOWN \note #"8" #1
      )
    }
  }
  c1
  c4 c' c,2
}
