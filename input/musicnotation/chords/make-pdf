#! /bin/sh
for notation in 6-6-tetragram avique mirck twinline twinline-2 kevin 5-line frix a-b ailler express beyreuther-untitled isomorph; do
    lilypond -o chords-${notation} -e '(define notation-style "'"${notation}"'")' chords-template
done
notation=traditional
lilypond chords-traditional
rm -f *.{eps,tex,texi,ps}
