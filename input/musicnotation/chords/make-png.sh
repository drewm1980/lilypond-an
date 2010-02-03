#! /bin/sh
source ../notations.sh
for notation in ${NOTATIONS_TO_TEST}; do
    lilypond --png -b eps -dno-gs-load-fonts -dinclude-eps-fonts -o chords-${notation} -e '(define notation-style "'"${notation}"'")' chords-template
done
lilypond --png -b eps -dno-gs-load-fonts -dinclude-eps-fonts chords-traditional
rm -f *.{eps,tex,texi,ps}
