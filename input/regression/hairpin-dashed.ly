\version "2.11.51"

\header {
  texidoc ="Hairpin crescendi may be dashed. "

}


\relative c' {
  \override Hairpin  #'style = #'dashed-line
  f2\< g c1 d4\> b a gis\! }     

