\header {
  texidoc = "
	In packed mode, pack notes as tight as possible.  This makes
	sense mostly in combination with raggedright mode: the notes
	are then printed at minimum distance.  This is mostly useful
	for ancient notation, but may also be useful for some flavours
	of contemporary music.  If not in raggedright mode, lily will
	pack as much bars of music as possible into a line, but the
	line will then be stretched to fill the whole linewidth.
"
  }

\version "2.11.40"

\layout {
  ragged-right = ##t
}


\relative {
  \override Score.SpacingSpanner #'packed-spacing = ##t
  c2 d4 f8[ g]
}