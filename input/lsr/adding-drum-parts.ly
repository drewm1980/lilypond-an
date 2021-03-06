%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.64"

\header {
  lsrtags = "rhythms, percussion"

  texidoces = "
Mediante la utilización de las potentes herramientas preconfiguradas
como la función @code{\\drummode} y el contexto @code{DrumStaff}, la
introducción de partes para percusión es muy fácil: las percusiones se
sitúan en sus propias posiciones de pentagrama (con una clave
especial) y tienen las cabezas correspondientes al instrumento.  Es
posible añadir un símbolo adicional a la percusión o reducir el número
de líneas.

"
  doctitlees = "Escritura de partes de percusión"

  texidoc = "
Using the powerful pre-configured tools such as the @code{\\drummode}
function and the @code{DrumStaff} context, inputting drum parts is
quite easy: drums are placed at their own staff positions (with a
special clef symbol) and have note heads according to the drum.
Attaching an extra symbol to the drum or restricting the number of
lines is possible. 

"
  doctitle = "Adding drum parts"
} % begin verbatim

drh = \drummode { cymc4.^"crash" hhc16^"h.h." hh hhc8 hho hhc8 hh16 hh hhc4 r4 r2 }
drl = \drummode { bd4 sn8 bd bd4 << bd ss >>  bd8 tommh tommh bd toml toml bd tomfh16 tomfh }
timb = \drummode { timh4 ssh timl8 ssh r timh r4 ssh8 timl r4 cb8 cb }

\score {
  <<
    \new DrumStaff \with {
      drumStyleTable = #timbales-style
      \override StaffSymbol #'line-count = #2
      \override BarLine #'bar-size = #2
    } <<
      \set Staff.instrumentName = #"timbales"
      \timb
    >>
    \new DrumStaff <<
      \set Staff.instrumentName = #"drums"
      \new DrumVoice { \stemUp \drh }
      \new DrumVoice { \stemDown \drl }
    >>
  >>
  \layout { }
  \midi {
    \context {
      \Score
      tempoWholesPerMinute = #(ly:make-moment 120 4)
    }
  }
}
