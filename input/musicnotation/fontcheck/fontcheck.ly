\version "2.10"
#(use-modules (guile-user))
\include "english.ly"
\include "../chromatic.ly"
% sample notation
%#(define notation-style "6-6-tetragram")
% or run from command line as
% lilypond -o scales-twinline -e '(define notation-style "twinline")' scales-template

\header {
  title = "Scales with alternative notation"
  subtitle          = #notation-style
}


% For png, use
% lilypond --png -b eps -dno-gs-load-fonts -dinclude-eps-fonts *&

\paper{
  indent=0\mm
  line-width=120\mm
  oddFooterMarkup=##f
  oddHeaderMarkup=##f
  bookTitleMarkup = ##f
  scoreTitleMarkup = ##f
}


scalesAll = \relative {
  c^\markup{"C"} cs^\markup{"C#"} d^\markup{"D"} ds^\markup{"D#"} e^\markup{"E"}
  f^\markup{"F"} fs^\markup{"F#"} g^\markup{"G"} gs^\markup{"G#"} 
  a^\markup{"A"} as^\markup{"A#"} b^\markup{"B"}
}

\new AlternativeStaff
{
  \setAltStaff #notation-style #-2 #1
  \time 4/4
  \scalesAll
}
