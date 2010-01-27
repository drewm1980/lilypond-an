#! /bin/sh
for notation in 6-6-tetragram avique mirck twinline twinline-2 kevin 5-line frix a-b ailler express beyreuther-untitled isomorph; do
    lilypond --png -b eps -dno-gs-load-fonts -dinclude-eps-fonts -o chords-${notation} -e '(define notation-style "'"${notation}"'")' chords-template
done
notation=traditional
lilypond --png -b eps -dno-gs-load-fonts -dinclude-eps-fonts chords-traditional
rm -f *.{eps,tex,texi,ps}
