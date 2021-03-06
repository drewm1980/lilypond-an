\version "2.11.61"
\header {
  lsrtags = "rhythms,tweaks-and-overrides"
  texidoc = "
Setting the @code{'transparent} property will cause an object to be
printed in \"invisible ink\": the object is not printed, but all its
other behavior is retained.  The object still takes up space, it takes
part in collisions, and slurs, ties and beams can be attached to it.

This snippet demonstrates how to connect different voices using ties.
Normally, ties only connect two notes in the same voice.  By
introducing a tie in a different voice, and blanking the first up-stem
in that voice, the tie appears to cross voices.  To prevent the blanked stem's
flag from interfering with tie positioning, the stem is extended.
"
  doctitle = "Making an object invisible with the transparent property"
}

\relative c'' {
  \time 2/4
  << {
    \once \override Stem #'transparent = ##t
    \once \override Stem #'length = #8
    b8 ~ b\noBeam
    \once \override Stem #'transparent = ##t
    \once \override Stem #'length = #8
    g8 ~ g\noBeam
  }
  \\
  {
    b8 g g e
  } >>
}
