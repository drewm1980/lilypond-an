%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.64"

\header {
  lsrtags = "template"

  texidoces = "
Esta plantilla simple prepara un pentagrama con notas, adecuado para
un instrumento solista o un fragmento melódico. Córtelo y péguelo en
un archivo, escriba las notas y ¡ya está!

"
  doctitlees = "Plantilla de un solo pentagrama, con notas únicamente"
  
  texidocde = "
Das erste Beispiel zeigt ein Notensystem mit Noten, passend für ein 
Soloinstrument oder ein Melodiefragment. Kopieren Sie es und fügen 
Sie es in Ihre Datei ein, schreiben Sie die Noten hinzu, und Sie haben 
eine vollständige Notationsdatei.
"

  texidoc = "
This very simple template gives you a staff with notes, suitable for a
solo instrument or a melodic fragment. Cut and paste this into a file,
add notes, and you're finished! 

"
  doctitle = "Single staff template with only notes"
} % begin verbatim

melody = \relative c' {
  \clef treble
  \key c \major
  \time 4/4
  
  a4 b c d
}

\score {
  \new Staff \melody
  \layout { }
  \midi { }
}
