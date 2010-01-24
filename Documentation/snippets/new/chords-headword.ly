\version "2.12.0"
#(set-global-staff-size 15)
\paper{
  ragged-right=##f
  line-width=15\cm
  indent=0\cm
}

\header {
  lsrtags = "headwords"
  texidoc = ""
  doctitle = "headword"
}


\layout {
  \context { \Score
    \override PaperColumn #'keep-inside-line = ##t
    \override NonMusicalPaperColumn #'keep-inside-line = ##t
  }
}

theChords = \chordmode {
  \time 2/2
  f1 | c2 f2 | f1 | c2 f2| %\break
  f2 bes2 | f1 | c2:7 f | c1 | \break
}

verseOne = \lyricmode{
  \set stanza = "1. "
  Fair is the sun - shine,
  Fair - er the moon - light
  And all the stars __ _  in heav'n a -- bove;
}

verseTwo = \lyricmode{
  \set stanza = "2. "
  Fair are the mead - ows,
  Fair - er the wood - land,
  Robed in the flow -- ers of bloom -- ing spring;
}

Soprano = {
  \time 2/2
  \key f \major
  \stemUp
  f'2 f'4 f' | g'4 e' f'2 | a'4. a'8 a'4 a' | bes'4 g' a'2 |
c''2 f''4 d'' |  c''2  bes'4  a' | bes'2 a' | g'1 |
}

Alto = {
  \key f \major
  c'2 c'4 c' | d'4 c' c'2 | f'4. f'8 f'4 fis' | g'4 e' f'2 |
  f'2 f'4 f' |  f'2  g'4  f' | e'2 f' | e'1 |
}

Tenor = {
  \key f \major
  \stemDown
  a2 a4 a | bes4 g a2  | c'4. c'8 d'4 d' | d'4 c' c'2 |
  a2 d'4 bes | a2 c'4 c' | c'2 c'  | c'1 |
}

Bass = {
  \key f \major
  f2 f4 f | bes,4 c  f2 | f4. e8 d4 c | bes,4 c f2 |
  f2 bes,4 d | f2 e4 f | g2 f | c1 |
}


\score {
  <<
    \new ChordNames { \theChords }
    \context Staff = upper {
      \context Voice = sop {
        <<
          \Soprano
          \Alto
        >>
      }
    }
    \context Lyrics="LyrOne" \lyricsto "sop" {\verseOne}
    \context Lyrics="LyrTwo" \lyricsto "sop" {\verseTwo}
    \context Staff = lower {
      \new Voice {
        \clef bass
        #(set-accidental-style 'modern-cautionary)
        <<
          \Tenor
          \Bass
        >>
      }
    }
  >>

\layout {
  %between-system-space = 1\mm
  indent = 0
  \context {
    \Score
    \remove "Bar_number_engraver"
  }
  \context { \Staff
    \override VerticalAxisGroup #'minimum-Y-extent = #'(-1 . 1)
  }
  }
}
\paper {  }
