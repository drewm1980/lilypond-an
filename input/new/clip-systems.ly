\version "2.11.61"
\header {
  lsrtags = "paper-and-layout"  % a new tag like "Mixing text and music" or
% "Special output" might be more adequate -jm
  texidoc = "
This code shows how to clip (extract) snippets from a full score.

This file needs to be run separately with @code{-dclip-systems}; the
snippets page may not adequately show the results.

The result will be files named
@file{@var{base}-from-@var{start}-to-@var{end}[-@var{count}].eps}.

@itemize
@item
If system starts and ends are included, they include extents of the
System grob, e.g., instrument names.

@item
Grace notes at the end point of the region are not included.

@item
Regions can span multiple systems.  In this case, multiple EPS files
are generated.

@end itemize
"
  doctitle = "Clip systems"
}

#(ly:set-option 'clip-systems)
#(set! output-count 1)

origScore = \score {
  \relative c' {
    \set Staff.instrumentName = #"bla"
    c1
    d1
    \grace c16 e1
    \key d \major
    f1 \break
    \clef bass
    g,1
    fis1
  }
}

\book {
  \score {
    \origScore
    \layout {
      % Each clip-region is a (START . END) pair
      % where both are rhythmic-locations.
      
      % (make-rhythmic-locations BAR-NUMBER NUM DEN)
      % means NUM/DEN whole-notes into bar numbered BAR-NUMBER

      clip-regions = #(list
      (cons
       (make-rhythmic-location 2 0 1)
       (make-rhythmic-location 4 0 1))
      
      (cons
       (make-rhythmic-location 0 0 1)
       (make-rhythmic-location 4 0 1))

      (cons
       (make-rhythmic-location 0 0 1)
       (make-rhythmic-location 6 0 1))
      )
    }
  }
}

#(set! output-count 0)
#(ly:set-option 'clip-systems #f)

\book {
  \score { \origScore }
  \markup { \bold \fontsize #6 clips }
  \score {
    \lyrics {
      \markup { from-2.0.1-to-4.0.1-clip.eps }
      \markup {
        \epsfile #X #30.0 #(format #f "~a-1-from-2.0.1-to-4.0.1-clip.eps"
                            (ly:parser-output-name parser)) }
    }
  }
}
