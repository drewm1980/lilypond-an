%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
%% Tags: rhythms, percussion
\version "2.11.35"

\header { texidoc = "
LilyPond makes drums input quite easy, with powerful pre-configured
tools such as the \\drummode function and the DrumStaff context:  drums
are placed to their own staff positions (with a special clef symbol)
and have note heads according to the drum. You can easily attach an
extra symbol to the drum, and restrict the number of lines.
" }
% begin verbatim
drh = \drummode { cymc4.^"crash" hhc16^"h.h." hh \repeat "unfold" 5 {hhc8 hho hhc8 hh16 hh} hhc4 r4 r2 }
drl = \drummode {\repeat "unfold" 3 {bd4 sn8 bd bd4 << bd ss >> } bd8 tommh tommh bd toml toml bd tomfh16 tomfh }
timb = \drummode { \repeat "unfold" 2 {timh4 ssh timl8 ssh r timh r4 ssh8 timl r4 cb8 cb} }

\score {
  \repeat "volta" 2 {
    <<
      \new DrumStaff \with {
	drumStyleTable = #timbales-style
	\override StaffSymbol #'line-count = #2
	\override BarLine #'bar-size = #2
      } <<
	\set Staff.instrumentName = "timbales"
	\timb
      >>
      \new DrumStaff <<
	\set Staff.instrumentName = "drums"
	\new DrumVoice {\stemUp \drh }
	\new DrumVoice {\stemDown \drl }
      >>
    >>
  }
  \layout {}

  \midi {
    \context {
      \Score
      tempoWholesPerMinute = #(ly:make-moment 120 4)
      }
    }


}
