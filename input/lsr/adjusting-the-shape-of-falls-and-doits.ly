%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.64"

\header {
  lsrtags = "expressive-marks"

  texidoces = "
Puede ser necesario trucar la propiedad
@code{shortest-duration-space} para poder ajustar el tamaño de las
caídas y subidas de tono («falls» y «doits»).

"
  doctitlees = "Ajustar la forma de las subidas y caídas de tono"
  
%% Translation of GIT committish :<6ce7f350682dfa99af97929be1dec6b9f1cbc01a>
texidocde = "
Die @code{shortest-duration-space}-Eigenschaft kann verändert werden, um
das Aussehen von unbestimmten Glissandi anzupassen.

"
  doctitlede = "Das Aussehen von unbestimmten Glissandi anpassen"

  texidoc = "
The @code{shortest-duration-space} property may have to be tweaked to
adjust the shape of falls and doits.

"
  doctitle = "Adjusting the shape of falls and doits"
} % begin verbatim

\relative c'' {
  \override Score.SpacingSpanner #'shortest-duration-space = #4.0
  c2-\bendAfter #+5
  c2-\bendAfter #-3
  c2-\bendAfter #+8
  c2-\bendAfter #-6
}
