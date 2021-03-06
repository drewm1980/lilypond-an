%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.64"

\header {
  lsrtags = "vocal-music, template"

  texidoces = "
He aquí una partitura vocal estándar para cuatro voces SATB. Con
grupos mayores, suele ser útil incluir una sección que aparezca en
todas las partes.  Por ejemplo, el compás y la armadura casi siempre
son los mismos para todas. Como en la plantilla \"Himno\", las cuatro
voces se reagrupan en sólo dos pentagramas.

"
  doctitlees = "Plantilla de conjunto vocal"
  
  texidocde = "
Dieses Beispiel ist für vierstimmigen Gesang (SATB). Bei größeren 
Stücken ist es oft sinnvoll, eine allgemeine Variable zu bestimmen, 
die in allen Stimmen eingefügt wird. Taktart und Vorzeichen etwa 
sind fast immer gleich in allen Stimmen.
"

  texidoc = "
Here is a standard four-part SATB vocal score. With larger ensembles,
it is often useful to include a section which is included in all parts.
For example, the time signature and key signature are almost always the
same for all parts. Like in the \"Hymn\" template, the four voices are
regrouped on only two staves.

"
  doctitle = "Vocal ensemble template"
} % begin verbatim

global = {
  \key c \major
  \time 4/4
}

sopMusic = \relative c'' {
  c4 c c8[( b)] c4
}
sopWords = \lyricmode {
  hi hi hi hi
}

altoMusic = \relative c' {
  e4 f d e
}
altoWords = \lyricmode {
  ha ha ha ha
}

tenorMusic = \relative c' {
  g4 a f g
}
tenorWords = \lyricmode {
  hu hu hu hu
}

bassMusic = \relative c {
  c4 c g c
}
bassWords = \lyricmode {
  ho ho ho ho
}

\score {
  \new ChoirStaff <<
    \new Lyrics = sopranos { s1 }
    \new Staff = women <<
      \new Voice = "sopranos" {
        \voiceOne
        << \global \sopMusic >>
      }
      \new Voice = "altos" {
        \voiceTwo
        << \global \altoMusic >>
      }
    >>
    \new Lyrics = "altos" { s1 }
    \new Lyrics = "tenors" { s1 }
    \new Staff = men <<
      \clef bass
      \new Voice = "tenors" {
        \voiceOne
        << \global \tenorMusic >>
      }
      \new Voice = "basses" {
        \voiceTwo << \global \bassMusic >>
      }
    >>
    \new Lyrics = basses { s1 }    
    \context Lyrics = sopranos \lyricsto sopranos \sopWords
    \context Lyrics = altos \lyricsto altos \altoWords
    \context Lyrics = tenors \lyricsto tenors \tenorWords
    \context Lyrics = basses \lyricsto basses \bassWords
  >>  
  \layout {
    \context {
      % a little smaller so lyrics
      % can be closer to the staff
      \Staff
      \override VerticalAxisGroup #'minimum-Y-extent = #'(-3 . 3)
    }
  }
}
