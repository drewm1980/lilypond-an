\header{
  texidoc = "Combinations of rotation and color do work."
}

\version "2.11.51"
\relative c'{ 
  \override Hairpin #'rotation = #'(20 -1 0)
  \override Hairpin #'color = #(x11-color 'LimeGreen)
  g4\< b d f'\!
} 
