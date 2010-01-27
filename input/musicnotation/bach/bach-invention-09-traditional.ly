\include "bach-invention-09-common.ly"
#(use-modules (guile-user))
% sample notation
#(define notation-style "traditional")
% or run from command line as
% lilypond -o scales-twinline -e '(define notation-style "twinline")' scales-template


\header {
  subtitle          = #notation-style
}

\score {
   \context GrandStaff <<
      \context Staff = "one" <<
         \voiceOne
         \global
      >>
      \context Staff = "two" <<
         \voiceTwo
         \global
      >>
   >>
   
   \layout{ }

  \midi {
    \context {
      \Score
      tempoWholesPerMinute = #(ly:make-moment 94 4)
      }
    }


}
