\header
{
  texidoc = "Tie directions can be set with @code{_} and @code{^}.
This makes correction in complex chords easier."

}
\version "2.11.51"
\paper
{
  ragged-right= ##t
}

\relative c' {
  <f^~ d'>(  <e f c'>)
}

