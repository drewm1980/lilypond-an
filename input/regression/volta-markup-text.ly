\version "2.11.51"

\header {
  texidoc = "Volte using @code{repeatCommands} can have markup
text."
}

voltaAdLib = \markup { 1. 2. 3... \text \italic { ad lib. } }

\relative c'' {
  c1
  \set Score.repeatCommands = #(list (list 'volta voltaAdLib) 'start-repeat)
  c4 b d e
  \set Score.repeatCommands = #'((volta #f) (volta "4.") end-repeat)
  f1
  \set Score.repeatCommands = #'((volta #f))
}
