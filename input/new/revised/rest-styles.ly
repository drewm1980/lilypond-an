%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.

\version "2.11.62"

\header {
  lsrtags = "rhythms, ancient-notation, tweaks-and-overrides"


  doctitlees = "Estilos de silencios"
  texidoces = "
Los silencios se pueden imprimir en distintos estilos.
"


  doctitlede = "Pausenstile"
  texidocde = "
Pausen können in verschiedenen Stilen dargestellt werden.
"


  doctitle = "Rest styles"
  texidoc = "
Rests may be used in various styles.
"
} % begin verbatim
\layout {
  indent = 0.0
  \context {
    \Staff
    \remove "Time_signature_engraver"
  }
}

\relative c {
  \set Score.timing = ##f
  \override Staff.Rest #'style = #'mensural
  r\maxima^\markup \typewriter { mensural }
  r\longa r\breve r1 r2 r4 r8 r16
  \bar ""
  
  \override Staff.Rest #'style = #'neomensural
  r\maxima^\markup \typewriter { neomensural }
  r\longa r\breve r1 r2 r4 r8 r16
  \bar ""
  
  \override Staff.Rest #'style = #'classical
  r\maxima^\markup \typewriter { classical }
  r\longa r\breve r1 r2 r4 r8 r16 r32 r64 r128
  \bar ""
  
  \override Staff.Rest #'style = #'default
  r\maxima^\markup \typewriter { default }
  r\longa r\breve r1 r2 r4 r8 r16 r32 r64 r128
}
