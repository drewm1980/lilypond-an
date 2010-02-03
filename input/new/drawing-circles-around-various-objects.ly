\version "2.11.61"
\header {
  lsrtags = "tweaks-and-overrides"
  texidoc = "
The @code{\circle} markup command draws circles around various objects,
for example fingering indications. For other objects, specific tweaks
may be required: this example demonstrates two strategies for rehearsal
marks and measure numbers.
"
  doctitle = "Drawing circles around various objects"
}

\relative c' {
  c1
  \set Score.markFormatter =
    #(lambda (mark context)
             (make-circle-markup (format-mark-numbers mark context)))
  \mark \default
  c2 d^\markup {
    \override #'(thickness . 3) {
      \circle \finger 2
    }
  }
  \override Score.BarNumber #'break-visibility = #all-visible
  \override Score.BarNumber #'stencil =
    #(make-stencil-circler 0.1 0.25 ly:text-interface::print)
}
