\version "2.11.62"

\header {
  lsrtags = "staff-notation"
  texidoc = "To change the tempo in MIDI output without printing
anything, make the metronome mark invisible:"

  doctitle = "Changing the tempo without a metronome mark"
}

\score {
  \new Staff \relative c' {
    \tempo 4 = 160
    c4 e g b
    c4 b d c
    \set Score.tempoHideNote = ##t
    \tempo 4 = 96
    d,4 fis a cis
    d4 cis e d
  }
  \layout { }
  \midi { }
}
