\version "2.11.51"

\header {
  texidoc = "Text that can spread over pages is entered with the
@code{\\markuplines} command."
}

#(set-default-paper-size "a6")

#(define-markup-list-command (paragraph layout props args) (markup-list?)
  (interpret-markup-list layout props 
   (make-justified-lines-markup-list (cons (make-hspace-markup 2) args))))

%% Candide, Voltaire
\markuplines \override-lines #'(baseline-skip . 2.5) {
  \paragraph {
    Il y avait en Westphalie, dans le château de M. le baron de
    Thunder-ten-tronckh, un jeune garçon à qui la nature avait donné
    les mœurs les plus douces.  Sa physionomie annonçait son âme.
    Il avait le jugement assez droit, avec l'esprit le plus simple ;
    c'est, je crois, pour cette raison qu'on le nommait Candide.  Les
    anciens domestiques de la maison soupçonnaient qu'il était fils
    de la sœur de monsieur le baron et d'un bon et honnête
    gentilhomme du voisinage, que cette demoiselle ne voulut jamais
    épouser parce qu'il n'avait pu prouver que soixante et onze
    quartiers, et que le reste de son arbre généalogique avait été
    perdu par l'injure du temps.
  }
  \paragraph {
    Monsieur le baron était un des plus puissants seigneurs de la
    Westphalie, car son château avait une porte et des fenêtres.  Sa
    grande salle même était ornée d'une tapisserie.  Tous les chiens
    de ses basses-cours composaient une meute dans le besoin ; ses
    palefreniers étaient ses piqueurs; le vicaire du village était
    son grand-aumônier.  Ils l'appelaient tous monseigneur, et ils
    riaient quand il faisait des contes.
  }
}
