\version "2.11.58"

\header {
  texidoc = "
Dots on harmonic note heads can be shown by setting the property
@code{harmonicDots}.
"
}

\relative c'' {
  r4 <bes es\harmonic>2.
  \set harmonicDots = ##t
  r4 <bes es\harmonic>2.
}
