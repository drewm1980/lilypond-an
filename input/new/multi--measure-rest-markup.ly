\version "2.11.61"

\header {
  lsrtags = "rhythms, text"
  texidoc = "Markups attached to a multi-measure rest will be
centered above or below it.  Long markups attached to multi-measure
rests do not cause the measure to expand.  To expand a multi-measure
rest to fit the markup, use a spacer rest with an attached markup
before the multi-measure rest.

Note that the spacer rest causes a bar line to be inserted.  Text attached
to a spacer rest in this way is left-aligned to the position where
the note would be placed in the measure, but if the measure length is
determined by the length of the text, the text will appear to be
centered."
  doctitle = "Multi-measure rest markup"
}

\relative c' {
  \compressFullBarRests
  \textLengthOn
  s1*0^\markup { [MAJOR GENERAL] }
  R1*19
  s1*0_\markup { \italic { Cue: ... it is yours } }
  s1*0^\markup { A }
  R1*30^\markup { [MABEL] }
  \textLengthOff
  c4^\markup { CHORUS } d f c
}
