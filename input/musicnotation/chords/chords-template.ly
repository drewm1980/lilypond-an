\version "2.10"
\include "english.ly"
#(set-global-staff-size 20)
#(use-modules (guile-user))
\include "../chromatic.ly"
% sample notation
%#(define notation-style "twinline")
%#(define notation-style "6-6-tetragram")
%#(define notation-style "kevin")

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


\header {
  title = "Chords with alternative notation"
  subtitle          = #notation-style
}


simplechords =
{
  \chordmode {
    \time 4/4
    a4 b c' d'
    \break
    a:m b:m c':m d':m
    \break
    a:6- b:6- c':6- d':6-
  }
}

harmonies =
\relative c, {
  \chordmode {
    \time 4/4
    c4 c:m c:7 c:dim7
    \break
    g4 g:m g:7 g:dim7
    \break
    d4 d:m d:7 d:dim7
    \break
    a4 a:m a:7 a:dim7
    \break
    e4 e:m e:7 e:dim7
    \break
    b4 b:m b:7 b:dim7
    \break
    fs4 fs:m fs:7 fs:dim7
    \break
    ds4 ds:m ds:7 ds:dim7
  }
}


\score {
  <<
    \context ChordNames {
      \set chordChanges = ##t
%      \harmonies
      \simplechords
    }
    \new AlternativeStaff
    {
      \context Voice = singer
      \setAltStaff #notation-style #0 #1
%      \harmonies
      \simplechords
    }

>>
}

