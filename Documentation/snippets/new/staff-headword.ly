\version "2.12.0"
\include "catalan.ly"
#(set-global-staff-size 15)
\paper{
  ragged-right=##t
  line-width=17\cm
  indent=0\cm
}


\header {
  lsrtags = "headwords"
  texidoc = ""
  doctitle = "headword"
}

\header {
	mutopiatitle = "Napolitan Dance (Swan Lake)"
 	mutopiacomposer = "TchaikovskyPI"
 	mutopiainstrument = "Piano, Trumpet, Percussion"
 	source = "New arrangement"
 	style = "Classical"
 	copyright = "Public Domain"
 	maintainer = "Laurence Sardain"
 	lastupdated = "2006/Dec/21"
%	title = "Le Lac des Cygnes"
%	subtitle = "Danse Napolitaine"
%	composer = "Piotr Ilitch Tchaïkovski"
%	arranger = "arr. Laurence Sardain"
%       footer = "Mutopia-2006/12/22-896"
}

trompette = \relative do'' {
  \clef treble
  \key mib \major
  \time 2/4
  R2^\markup { \italic Comodo } |
  r8 \once \override TextScript #'padding = #2.0
  sib16-.^\markup {\dynamic p \italic grazioso} do-. mib( re)-. do-. sib-. |
  re8-. r8 re4->( |
  re8) do16-. re-. mib( re) do-. re-. |
  do8-. r8 sib4-> |
}

tambourin = \relative do' {
  \time 2/4
  r8 do16 do do8 do |
  r8 do16 do do8 do |
  r8 do r do |
  r8 do16 do do8 do |
  r8 do r do |
}

tambourinMidi = \drummode {
  \time 2/4
  r8 tamb16 tamb tamb8 tamb |
  r8 tamb16 tamb tamb8 tamb |
  r8 tamb r tamb |
  r8 tamb16 tamb tamb8 tamb |
  r8 tamb r tamb |
}

upper = \relative do' {
  \clef treble
  \key mib \major
  \time 2/4
  r8\p <sol sib mib>16-. <sol sib mib>-. <sol sib mib>8-. <sol sib mib>-. |
  r8 <sol sib mib>16-. <sol sib mib>-. <sol sib mib>8-. <sol sib mib>-. |
  r8 <lab sib re>16-. <lab sib re>-. <lab sib re>8-. <lab sib re>-. |
  r8 <lab sib re>16-. <lab sib re>-. <lab sib re>8-. <lab sib re>-. |
  r8 <sol sib mib>16-. <sol sib mib>-. <sol sib mib>8-. <sol sib mib>-. |
}

lower = \relative do {
  \clef bass
  \key mib \major
  \time 2/4
  mib4-. r4 |
  sib-. r |
  fa'-. r |
  sib, r |
  mib4-. r4 |
}


\score {
  <<
    \context Staff = "trumpet" <<
      \set Staff.instrumentName = \markup { "Trumpet" \concat{ B \teeny \raise #0.4 \flat } }
      \transpose sib do'
      \trompette
    >>
    \context RhythmicStaff = "tambourin" <<
      \set Staff.instrumentName = "Tambourine"
      \tambourin
    >>
    \context PianoStaff = "prima" <<
      \set PianoStaff.instrumentName = "Piano  "
      \context Staff = "uppera" \upper
      \context Staff = "lowera" \lower
    >>
  >>
  \layout { }
}
\score {
  <<
    \context Staff = "trumpet" {
      \set Staff.midiInstrument = "trumpet"
      \trompette
    }
    \context DrumStaff = "tambourin" {
      \tambourinMidi
    }
    \context Staff = "piano" <<
      \upper
      \lower
    >>
  >>
  \midi {
    \context {
      \Score
      tempoWholesPerMinute = #(ly:make-moment 72 4)
    }
  }
}

