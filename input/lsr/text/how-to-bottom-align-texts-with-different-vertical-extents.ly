%%  Do not edit this file; it is auto-generated from LSR!
\version "2.11.23"

\header { texidoc = "
Some letters imply smaller vertical extents than the others; if you
have, below your staff, two markups with different vertical extents
(e.g. one text with letters \"t,h,l and/or UPPER CASE\" and one text
with \"a, c, e, n or m\"), LilyPond will align them to the top by
default, thus making it look a bit messy. Therefore, you need to add
invisible ascender letters using the \transparent command to make it
right.
" }

\new Staff {
  \override TextScript #'staff-padding = #4
  \override TextScript #'self-alignment-X = #center
  \time 2/4
  c'4_\markup { \transparent "A" "WRONG" \transparent "A" }
  c'4_\markup { "case" }
  c'4_\markup { \transparent "A" "RIGHT" \transparent "A" }
  c'4_\markup { \transparent "A" "case" \transparent "A" }
}