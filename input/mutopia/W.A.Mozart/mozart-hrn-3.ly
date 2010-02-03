\header{
  title = "Konzert Nr. 3 Es dur"
  subtitle = "für Horn und Orchester"
  composer = "Wolfgang Amadeus Mozart (1756-1791)"
  enteredby = "HWN"
  opus = "KV 447"

  copyright = "public domain"
  instrument = "Horn in F"
  editor = "Henri Kling"
  mutopiatitle = "Horn Concerto 3"
  mutopiacomposer = "W.A.Mozart"
  mutopiaopus = "KV447"
  style = "classical"
  maintainer = "hanwen@xs4all.nl"
  maintainerEmail = "hanwen@xs4all.nl"
  maintainerWeb = "http://www.xs4all.nl/~hanwen/"	
  lastupdated = "2002/May/21"
  source = "Edition Breitkopf 2563"
  footer = "Mutopia-2002/05/21-25"

  tagline = \markup { \smaller
      \column {
	   \fill-line { \footer "" }
	   \fill-line { { "This music is part of the Mutopia project,"
			  \typewriter { "http://mutopiaproject.org/" }
			 } }
	   \fill-line { #(ly:export (string-append "It has been typeset and placed in the public "
			  "domain by "  maintainer  "."))  }
	   \fill-line { #(ly:export (string-append "Unrestricted modification and redistribution"
			  " is permitted and encouraged---copy this music"
			  " and share it!")) }
	   }
       }
}
%{

This is the Mozart 3 for horn.  It's from an Edition Breitkopf EB
2563, edited by Henri Kling. Henri Kling (1842-1918) was a horn
virtuoso that taught in Geneva. 

%}

\version "2.11.61"

\include "mozart-hrn3-defs.ily"
\include "mozart-hrn3-allegro.ily"
\include "mozart-hrn3-romanze.ily"
\include "mozart-hrn3-rondo.ily"

\paper {
    between-system-space = 20 \mm
}


\book {
    \score {
	{ \transpose c' bes \allegro }
	\layout { }
	\header { piece = "Allegro" opus = "" }	
	
  \midi {
    \context {
      \Score
      tempoWholesPerMinute = #(ly:make-moment 90 4)
      }
    }


    }

    \score {
	{ \transpose c' bes \romanze }
	\header { piece = "Romanze" opus = "" }	
	
  \midi {
    \context {
      \Score
      tempoWholesPerMinute = #(ly:make-moment 70 4)
      }
    }


	\layout {}
    }

    \score
    {
	{ \transpose c' bes \rondo }
	\header { piece = "Rondo" opus = "" }
	
  \midi {
    \context {
      \Score
      tempoWholesPerMinute = #(ly:make-moment 100 4)
      }
    }


	\layout { }
    }
}

%% Local Variables:
%% coding: utf-8
%% End:
