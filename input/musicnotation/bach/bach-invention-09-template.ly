\include "bach-invention-09-common.ly"
#(use-modules (guile-user))
\include "chromatic.ly"
% sample notation
%#(define notation-style "6-6-tetragram")
%#(define notation-style "twinline")
% or run from command line as
% lilypond -o scales-twinline -e '(define notation-style "twinline")' scales-template
#(define lower-octave -2)
#(define upper-octave 1)


\header {
  subtitle          = #notation-style
}

\score {
  \new StaffGroup <<
    \new AlternativeStaff
    {
      <<
      \global
      <<
	{
	  \setAltStaff #notation-style #lower-octave #upper-octave
	  \voiceOne
	} \\
	{
	  \setAltStaff #notation-style #lower-octave #upper-octave
	  \voiceTwoNoClef
	}
      >>
    >>
    }

  >>
  
% midi is not currently working with this template
%{
  \layout{ }
  \midi {
    \context {
      \Score
      tempoWholesPerMinute = #(ly:make-moment 94 4)
    }
  }
%}
}
