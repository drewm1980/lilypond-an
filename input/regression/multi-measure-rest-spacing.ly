\header {

  texidoc = "By setting texts starting with a multi-measure rest, an 
extra spacing column is created. This should not cause problems."
}

  \layout {
    ragged-right = ##t
  }

\version "2.11.51"


<<
  \set Score.skipBars = ##t
  \new Staff \new Voice { 
    <<  { R1*40 }  { s1*0_"bla" }>> 
  }
>>

 
