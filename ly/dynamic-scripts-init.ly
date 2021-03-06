\version "2.10.0"

%
% declare the standard dynamic identifiers.
%

#(define (make-dynamic-script str)
   (make-music 'AbsoluteDynamicEvent
              'text str))
ppppp = #(make-dynamic-script "ppppp")
pppp = #(make-dynamic-script "pppp")
ppp = #(make-dynamic-script "ppp")
pp = #(make-dynamic-script "pp")
p = #(make-dynamic-script "p")
mp = #(make-dynamic-script "mp")
mf = #(make-dynamic-script "mf")

%% f is pitch.
"f" = #(make-dynamic-script "f")
ff = #(make-dynamic-script "ff")
fff = #(make-dynamic-script "fff")
ffff = #(make-dynamic-script "ffff")
fp = #(make-dynamic-script "fp")
sf = #(make-dynamic-script "sf")
sfp = #(make-dynamic-script "sfp")
sff = #(make-dynamic-script "sff")
sfz = #(make-dynamic-script "sfz")
fz = #(make-dynamic-script "fz")
sp = #(make-dynamic-script "sp")
spp = #(make-dynamic-script "spp")
rfz = #(make-dynamic-script "rfz")

