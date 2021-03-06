\version "2.11.62"

\header {
  lsrtags = "editorial-annotations, vocal-music"

  texidoc = "
This example shows how to put crosses on stems. Mark the beginning
of a spoken section with the @code{\\speakOn} keyword, and end it
with the @code{\\speakOff} keyword.  Remember to end cross sections
before entering any rest: this function also adds crosses to the
invisible stems of rests.
"
  doctitle = "Marking notes of spoken parts with a cross on the stem"
}

speakOn = {
  \override Stem #'stencil = #(lambda (grob)
  (ly:stencil-combine-at-edge
    (ly:stem::print grob)
    Y
    (- (ly:grob-property grob 'direction))
    (grob-interpret-markup grob
      (markup #:hspace -1.025 #:fontsize -4
        #:musicglyph "noteheads.s2cross"))
    -2.3 0))
}

speakOff = {
  \revert Stem #'stencil
}

\score {
  \new Staff {
    \relative c'' {
      a4 b a c
      \speakOn
      g4 f r g
      b4 r d e
      \speakOff
      c4 a g f
    }
  }
}
