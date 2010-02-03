#! /bin/sh
source ../notations.sh
for notation in ${NOTATIONS}; do
    lilypond --png -b eps -dno-gs-load-fonts -dinclude-eps-fonts -o scales-${notation} -e '(define notation-style "'"${notation}"'")' scales-template
done
lilypond --png -b eps -dno-gs-load-fonts -dinclude-eps-fonts scales-traditional
rm -f *.{eps,tex,texi,ps,pdf}
