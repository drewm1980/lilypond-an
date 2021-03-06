%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.64"

\header {
  lsrtags = "expressive-marks"

  texidoces = "
Las abreviaturas se encuentran definidas dentro del archivo
@code{ly/script-init.ly}, donde las variables @code{dashHat},
@code{dashPlus}, @code{dashDash}, @code{dashBar},
@code{dashLarger}, @code{dashDot} y @code{dashUnderscore} reciben
valores predeterminados.  Se pueden modificar estos valores
predeterminados para las abreviaturas. Por ejemplo, para asociar
la abreviatura @code{-+} (@code{dashPlus}) con el símbolo del
semitrino en lugar del símboloo predeterminado +, asigne el valor
@code{trill} a la variable @code{dashPlus}:

"
  doctitlees = "Modificar los valores predeterminados para la notación abreviada de las articulaciones"

%% Translation of GIT committish :<6ce7f350682dfa99af97929be1dec6b9f1cbc01a>  
 texidocde = "
Die Abkürzungen sind in der Datei @samp{ly/script-init.ly} definiert, wo
den Variablen @code{dashHat}, @code{dashPlus}, @code{dashDash},
@code{dashBar}, @code{dashLarger}, @code{dashDot} und
@code{dashUnderscore} Standardwerte zugewiesen werden.  Diese Standardwerte
können verändert werden.  Um zum Beispiel die Abkürzung
@code{-+} (@code{dashPlus}) mit dem Triller anstatt mit dem +-Symbol zu
assoziieren, muss der Wert @code{trill} der Variable
@code{dashPlus} zugewiesen werden:

"
  doctitlede = "Die Standardwerte für Arkkikulationsabkürzungen verändern"

  texidoc = "
The shorthands are defined in @samp{ly/script-init.ly}, where the
variables @code{dashHat}, @code{dashPlus}, @code{dashDash},
@code{dashBar}, @code{dashLarger}, @code{dashDot}, and
@code{dashUnderscore} are assigned default values.  The default values
for the shorthands can be modified. For example, to associate the
@code{-+} (@code{dashPlus}) shorthand with the trill symbol instead of
the default + symbol, assign the value @code{trill} to the variable
@code{dashPlus}: 

"
  doctitle = "Modifying default values for articulation shorthand notation"
} % begin verbatim

\relative c'' { c1-+ }
dashPlus = "trill"
\relative c'' { c1-+ }
