\version "2.11.64"

\header {
  lsrtags = "rhythms, percussion"

  texidoc = "
Though the polymetric time signature shown was not the most essential
item here, it has been included to show the beat of this piece (which
is the template of a real Balkan song!).

"
  doctitle = "Heavily customized polymetric time signatures"
}

#(define plus (markup #:vcenter "+"))
#(define ((custom-time-signature one two three four five six
           seven eight nine ten eleven num) grob)
            (grob-interpret-markup grob
              (markup #:override '(baseline-skip . 0) #:number
                (#:line (
                    (#:column (one num)) plus
                    (#:column (two num)) plus
                    (#:column (three num)) plus
                    (#:column (four num)) plus
                    (#:column (five num)) plus
                    (#:column (six num)) plus
                    (#:column (seven num)) plus
                    (#:column (eight num)) plus
                    (#:column (nine num)) plus
                    (#:column (ten num)) plus
                    (#:column (eleven num))))
                )))

melody = \relative c'' {
  \set Staff.instrumentName = #"Bb Sop."
  \key g \major
  #(set-time-signature 25 8 '(3 2 2 3 2 2 2 2 3 2 2))
  \override Staff.TimeSignature #'stencil =
    #(custom-time-signature "3" "2" "2" "3" "2" "2"
      "2" "2" "3" "2" "2" "8")
  c8 c c d4 c8 c b c b a4 g fis8 e d c b' c d e4-^ fis8 g \break
  c,4. d4 c4 d4. c4 d c2 d4. e4-^ d4
  c4. d4 c4 d4. c4 d c2 d4. e4-^ d4 \break
  c4. d4 c4 d4. c4 d c2 d4. e4-^ d4
  c4. d4 c4 d4. c4 d c2 d4. e4-^ d4 \break
}

drum = \new DrumStaff \drummode {
  \bar "|:" bd4.^\markup { "Drums" } sn4 bd \bar ":" sn4.
  bd4 sn \bar ":" bd sn bd4. sn4 bd \bar ":|"
}

{
  \melody
  \drum
}