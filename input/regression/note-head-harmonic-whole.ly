\header {

  texidoc = "A harmonic note head must be centered if the base note
  is a whole note."

}


\version "2.11.51"

\paper {
  ragged-right = ##t
}

\relative c' {
  <e a\harmonic>1
  <e'' a\harmonic>1
}

