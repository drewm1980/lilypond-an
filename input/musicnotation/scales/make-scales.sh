#! /bin/sh
for notation in 6-6-tetragram avique mirck twinline twinline-2 kevin 5-line frix a-b ailler express beyreuther-untitled isomorph; do
    lilypond --png -b eps -dno-gs-load-fonts -dinclude-eps-fonts -o scales-${notation} -e '(define notation-style "'"${notation}"'")' scales-template
done
notation=traditional
lilypond --png -b eps -dno-gs-load-fonts -dinclude-eps-fonts scales-traditional
rm -f *.{eps,tex,texi,ps,pdf}
