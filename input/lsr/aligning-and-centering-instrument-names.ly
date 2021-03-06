%% Do not edit this file; it is auto-generated from input/new
%% This file is in the public domain.
\version "2.11.64"

\header {
  texidoces = "
Los nombres de instrumento se imprimen generalmente a la izquierda de
los pentagramas.  Para alinear los nombres de varios instrumentos
distintos, sitúelos dentro de un bloque @code{\\markup} y utilice una
de las siguientes posiblidades:

*
    Nombres de instrumento alineados por la derecha: es el
    comportamiento predeterminado
  
*
    Nombres de instrumento centrados: la utilización de la instrucción
    @code{\\hcenter-in #n} sitúa los nombres de instrumento dentro de
    un rectángulo separado, donde @code{n} es la anchura del
    rectángulo
  
* 
    Nombres de instrumento alineados por la izquierda: los nombres se
    imprimen en la parte superior de un rectángulo vacío, utilizando
    la instrucción @code{\\combine} con un objeto @code{\\hspace #n}.

"
  doctitlees = "Alinear y centrar los nombres de instrumento"

  lsrtags = "text, paper-and-layout, titles"
  texidoc = "The horizontal alignment of instrument names is tweaked
by changing the @code{Staff.InstrumentName #'self-alignment-X} property.
The @code{\\layout} variables @code{indent} and @code{short-indent}
define the space in which the instrument names are aligned before the
first and the following systems, respectively."
  doctitle = "Aligning and centering instrument names"
} % begin verbatim


\paper {
  left-margin = 3\cm
}

\score {
  \new StaffGroup <<
    \new Staff {
      \override Staff.InstrumentName #'self-alignment-X = #LEFT
      \set Staff.instrumentName = \markup \left-column {
        "Left aligned"
        "instrument name"
      }
      \set Staff.shortInstrumentName = #"Left"
      c''1
      \break
      c''1
    }
    \new Staff {
      \override Staff.InstrumentName #'self-alignment-X = #CENTER
      \set Staff.instrumentName = \markup \center-column {
        Centered
        "instrument name"
      }
      \set Staff.shortInstrumentName = #"Centered"
      g'1
      g'1
    }
    \new Staff {
      \override Staff.InstrumentName #'self-alignment-X = #RIGHT
      \set Staff.instrumentName = \markup \right-column {
        "Right aligned"
        "instrument name"
      }
      \set Staff.shortInstrumentName = #"Right"
      e'1
      e'1
    }
  >>
  \layout {
    ragged-right = ##t
    indent = 4\cm
    short-indent = 2\cm
  }
}
