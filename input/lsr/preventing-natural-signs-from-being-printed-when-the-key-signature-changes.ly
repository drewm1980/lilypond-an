%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.64"

\header {
  lsrtags = "pitches"

doctitlees = "Evitar que se impriman becuadros cuando cambia la armadura"
texidoces = "
Cuando cambia la armadura de la tonalidad, se imprimen becuadros
automáticamente para cancelar las alteraciones de las armaduras
anteriores.  Esto se puede evitar estableciendo al valor \"falso\" la
propiedad @code{printKeyCancellation} del contexto @code{Staff}.

"

doctitlede = "Auflösungzeichen nicht setzen, wenn die Tonart wechselt"

texidocde = "
Wenn die Tonart wechselt, werden automatisch Auflösungszeichen ausgegeben,
um Versetzungszeichen der vorherigen Tonart aufzulösen.  Das kann
verhindert werden, indem die @code{printKeyCancellation}-Eigenschaft
im @code{Staff}-Kontext auf \"false\" gesetzt wird.
"

  texidoc = "
When the key signature changes, natural signs are automatically printed
to cancel any accidentals from previous key signatures.  This may be
prevented by setting to \"false\" the @code{printKeyCancellation}
property in the @code{Staff} context. 

"
  doctitle = "Preventing natural signs from being printed when the key signature changes"
} % begin verbatim

\relative c' {
  \key d \major
  a4 b cis d
  \key g \minor
  a4 bes c d
  \set Staff.printKeyCancellation = ##f
  \key d \major
  a4 b cis d
  \key g \minor
  a4 bes c d
}
