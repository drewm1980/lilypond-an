\header {

  texidoc = "stem tremolo vertical distance also obeys staff-space settings. "

}

\version "2.11.51"

\layout {
  ragged-right = ##t
}

fragment = \relative c' {
  c4:64 c:64 c8:64 c:64 c:64 c:64 c:64
}


<<
  \new Staff \with {
    fontSize = #-4
    \override StaffSymbol #'staff-space = #(magstep -4)
  }
  \fragment
  \new Staff \fragment
>>
