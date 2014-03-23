<Cabbage>
form caption("John Chowning collection") size(562, 340), colour("black"),pluginID("cho1") 
rslider bounds(15, 10, 100, 100) channel("spread"), range(0,1,.5), caption("Spread"), colour("GreenYellow "), midictrl(1, 1)
rslider bounds(120, 10, 100, 100) channel("semi"), range(-24,12,-12), caption("Coarse"), colour("GreenYellow ")
rslider bounds(225, 10, 100, 100) channel("detune"), range(-2,2,0), caption("Detune"), colour("GreenYellow ")
rslider bounds(330, 10, 100, 100) channel("vibamp"), range(0,20,0), caption("Vib Amp"), colour("GreenYellow ")
rslider bounds(435, 10, 100, 100) channel("vibrate"), range(0,20,0), caption("Vib rate"), colour("GreenYellow ")

groupbox bounds(15, 120, 240, 100), text("ADSR amplitude"), plant("ADSR"){ 
rslider bounds(.0, .3, .6, .6), text("A"), colour("orange"), channel("att"), range(0.01,3, .5)
rslider bounds(.25, .3, .6, .6), text("D"), colour("orange"), channel("dec"), range(0,1, .5)
rslider bounds(.5, .3, .6, .6), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
rslider bounds(.75, .3, .6, .6), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
}

combobox bounds(280,120, 160, 20), channel("select"), value(1), text("clar","glass", "loop","perc","piano","rebell","soprano","string","trumpet","vibr")
rslider bounds(280,150, 60, 60), text("Rev level"), colour("GreenYellow "), channel("level"), range(0,1,.8)
rslider bounds(350, 150, 60, 60), text("Rev size"), colour("GreenYellow "), channel("size"), range(.1,.99,.7)
checkbox  bounds(420, 160, 160, 30),channel("onoff"), text("Reverb On/Off"), value(0)
keyboard pos(10, 240), size(540, 60)

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -M0 -b1024 
</CsOptions>
<CsInstruments>

sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

;Author: Giorgio Zucco (2012)

ga1	init	0
ga2	init	0


;Chowning opcodes
;********************************************
opcode CHOWNClar,a,kk

gitemp1 ftgen  0,0, 8192, -12, 8
gitemp2 ftgen  0,0, 8192, 9,   1, 1, 0
gitemp3 ftgen  0,0, 8192, 9,   1, 1, 90


kamp,knote  xin
;setksmps 10
;inote          cpsmidib

;iveloc         ampmidi 1

kgate          linenr    kamp,0,1,.01

kenv           linseg    0,0.1,1,2-0.3,1,0.2,0

kramp          linseg    0,2/2,4,2/2,0

k1             =         kramp * (.8 - 1/.8)/2

k2a            =         kramp * (.8 + 1/.8)/2

k2b            =         200 * k2a

k3a            table     k1*2,gitemp1,0,0,0

k3b            =         k3a * (-1/2)

a1             poscil     k2b,knote*2,gitemp3

a2             poscil     k1,knote*2,gitemp3

a3             =         kenv* exp(a2+k3b)

asig           poscil     a3,knote+a1,gitemp2

   xout asig*kgate

endop

;**************************************************
opcode CHOWNglass,a,kki

kamp,knote,ifunc xin

kgate         linenr    kamp, 0, .2, .01
imfreq        =         1.4 * 200
immax         =         10 * imfreq
kmenv         linseg    1, 5/6, 0, 5 - (5/6), 0
koenv         expseg    0.8, 5/6, 1, 5 - (5/6) , .01
amod          poscil    kmenv * immax, 1.414*knote, ifunc
aout          poscil    koenv * 1, knote + amod, ifunc
 
xout aout*kgate         

       endop


;**************************************************

opcode CHOWNLoop,a,kki                                 

gitemp1 ftgen  0,  0,  513, 5,   .7,  16,      .8,48, 1 ,    64,      .8, 128,.2, 256, .001

gitemp2 ftgen  0,  0,  513, 7,  0 ,   12,      1,  52, 0,    460,     0


kamp,kfreq,ifunc  xin
    
kgate     linenr    kamp, 0, .2, .01
i1        =         1/.2
idev1     =         0 * 110
idev2     =         (25-0) * 110
ampcar    poscil    1, i1, gitemp1
ampmod    poscil    idev2, i1,gitemp2
amod      poscil    ampmod+idev1, kfreq*1,ifunc
asig      poscil    ampcar, kfreq+amod,ifunc

xout      asig*kgate 

  endop
;**************************************************
opcode CHOWNPerc,a,kki


kamp,kcps,ifunc xin


keg       expseg    .001, .01, 1, .1, .8, 999, .001
keg       linenr    keg * kamp, .01, .333, .05
kmeg      expseg    .001, .01,12000/1000, .25, .2, 999, .001
a1        foscili   keg, kcps, 1, 1.4, kmeg, 1
         
xout a1        

       endop
;**************************************************

opcode CHOWNPiano,a,kki

kamp,kcps,ifunc xin

;gitemp100  ftgen 0,0 ,128 ,7, 0, 128 , 1

kndx	expon	1,	.3,	.01

a1	foscili	kamp,	kcps,	1,	12,	kndx,	ifunc
a2	foscili	kamp,	kcps + .1,	1,	12,	kndx,	ifunc
a3	foscili	kamp,	kcps - .1,	1,	12,	kndx,	ifunc

aosc	=	a1 + a2 + a3
kenv	mxadsr	.01,	.1,	.8,	.1
aosc	=	aosc * .5

aout =	aosc*kenv
       
xout aout         

       endop
;**************************************************
opcode CHOWNrebell,a,kki


gifc2 ftgen 0,0.0 ,512 ,7 ,1 ,512 ,1
gifc3 ftgen 0,0.0, 512, 7 ,.5 ,512, .5
gifc4 ftgen 0,0.0 ,513 ,7 ,1 ,512, 0, 1, 0
gifc5 ftgen 0,0.0, 513 ,7 ,0 ,512, 1, 1, 1
gifc6 ftgen 0,0.0 ,513, 7, 1, 256, 0, 256, 1 ,1, 1


kamp,knote,ifunc xin

kgate          linenr    kamp,0,1.5,.01
kpitch1        =         knote       
kpitch2        =         kpitch1 * 1.001     
kpitch3        =         kpitch1 * 0.998     
imodbase       =         (19 <= 5 ? 19 : 5)      
imodmax        =         (19 <= 5 ? 5 : 19)       
indxbase       =         (1 <= 2 ? 1 : 2)        
indxmax        =         (1 <= 2 ? 2 : 1)        
kmodchg        oscil1i   0.00,imodmax - imodbase,p3,gifc4       
kmod           =         imodbase + kmodchg      
kndxchg        oscil1i   0.00,indxmax - indxbase,5,gifc5       
kndx           =         indxbase + kndxchg      
afm1           foscili   1*.45,kpitch1,1,kmod,kndx,ifunc       
afm2           foscili   1*.35,kpitch2,1,kmod,kndx,ifunc        
afm3           foscili   1*.32,kpitch3,1,kmod,kndx,ifunc       
afmttl         =         afm1+afm2+afm3      
aosc1          poscil     1 *.45,kpitch1,ifunc      
aosc2          poscil     1 *.35,kpitch2,ifunc       
aosc3          poscil     1 *.32,kpitch3,ifunc      
aoscttl        =         aosc1+aosc2+aosc3       
afm            =         afmttl - aoscttl;                   
icfbase        =         (3000 <= 12000 ? 3000 : 12000)      
icfmax         =         (3000 <= 12000 ? 12000 : 3000)      
kcfchg         oscil1i   0.00,(icfmax - icfbase),5,gifc6      
kcf            =         icfbase + kcfchg        
ibwbase        =         (100 <= 400 ? 100 : 400)        
ibwmax         =         (100 <= 400 ? 400 : 100)        
kbwchg         oscil1i   0.00,ibwmax - ibwbase,5,gifc6      
kbw            =         ibwbase + kbwchg        
aflt1          reson     afm,kcf,kbw,1       
aflt2          reson     afm,kcf*.9, kbw*1.11,1       
abal           balance   aflt1+aflt2,afm       

asig           envlpx    abal,.2, gifc5, gifc5*.39, gifc5, 1                       

xout asig*kgate         

       endop


;**************************************************
opcode CHOWNSoprano,a,kki

kveloc,knote,ifunc xin

kgate          linenr    kveloc,0,2,.01
        k7     =         knote
        k2     =         7000
        k2     =         (k2/k7)+.5
        k2     =         int(k2)
        k3     =         .04
        k4     =         .25
        k5     =         5*.4
        k6     =         5
        k8     =         k5/k4
        k9     =         exp(1.5*log(7000/32767))
        k10    =         .00311*log(k7)
        k11    =         sqrt(7000/32767)
        k12    =         (1-k3)*k11
        k13    =         k4*k7
        k14    =         k9*k3
a1             linen     k12,.1,3,.08
a2             poscil     k13,k7,ifunc
a8             =         k7+a2
a4             linenr    k10,.6,3,.1
a4             poscil     a4,k6,ifunc
a5             randi     k10,16
a6             linseg    -.03,.07,.03,.03,0,3-.1,0
a6             =         a4+a5+a6+1.
a1             poscil    a1,(a8+a2)*a6,ifunc
a7             =         (a2*k8)+k2
a3             linseg    0,.07,.1,.03,1.,3-.18,1,.02,.1,.06,0
a3             =         a3*(k9*k3)
a3             poscil    a3,a7*a6,ifunc

aout    =  (a1+a3)*kgate


xout aout         

endop

;**************************************************
opcode CHOWNString,a,kki

kamp,knote,ifunc xin
;setksmps 10
iveloc         ampmidi   1

kgate          linenr    1, 0, .2, .01
ifc            =         cpspch(5.00)
ifm1           =         ifc
ifm2           =         ifc*3
ifm3           =         ifc*4
indx1          =         7.5/log(ifc)                  ;RANGE FROM CA 2 TO 1
indx2          =         15/sqrt(ifc)                  ;RANGE FROM CA 2.6 TO .5
indx3          =         1.25/sqrt(ifc)                ;RANGE FROM CA .2 TO .038
kvib           init      0               
timout    0,.75,transient

kvbctl         linen     1,.5,2-.75,.1
krnd           randi     .0075,15                      ;RANDOM DEVIATION IN VIB WIDTH        
kvib           poscil   kvbctl*.03+krnd,5.5*kvbctl,ifunc

transient:
timout    .2,2,continue

ktrans         linseg    1,.2,0,1,0     ;TRANSIENT ENVELOPE
anoise         randi     ktrans,.2*ifc         ;NOISE... 
attack         poscil    anoise,knote,ifunc
 
continue:      

amod1          poscil   ifm1*(indx1+ktrans),knote,ifunc
amod2          poscil    ifm2*(indx2+ktrans),knote*3,ifunc
amod3          poscil    ifm3*(indx3+ktrans),knote*4,ifunc
asig           poscil    kamp,(amod1+amod2+amod3)*(1+kvib),ifunc
asig           linen     asig+attack,.2,2,.2

xout  asig *kgate      
       endop


;**************************************************
opcode CHOWNTrumpet,a,kki

gitemp2 ftgen 0,0,129,9,.35,1,0
gitemp3 ftgen 0,0,129,9,.4,1,0
gitemp4 ftgen 0,0,129,9,.25,1,0
gitemp5 ftgen 0,0,129,9,.3875,1.4142,0
 
kamp,knote,ifunc xin

;setksmps 10

kgate          linenr         kamp/2, 0, .2, .01
irandev        =              .007
ifreqrand      =              125
ivibatt        =              .6
ivibdec        =              .2
ivibwth        =              .007
ivibrate       =              7
iportdev       =              .03
iportatt       =              .06
iportdec       =              .01
iampfac        =              .833
iafrmfac       =              1-iampfac
imax           =              2.66
iratio         =              1.8/imax
ifundatt       =              .03
ifunddec       =              .15
ifrmatt        =              .03
ifrmdec        =              .3
imodatt        =              .03
imoddec        =              .01

kphs           linen          ivibwth, ivibatt, 1, ivibdec
kvfac          table          kphs, gitemp4, ifunc
kvibgate       =              kvfac*ivibwth 
kport          envlpx         iportdev, iportatt, 1, iportdec, gitemp5, 1, .01
krand          randi          irandev, ifreqrand, -1
kosc           poscil          kvibgate, ivibrate, ifunc
kvib           =              (krand+1)*(kosc+1)*(kport+1)
       
kgatemod       envlpx         imax*knote, imodatt, 1, imoddec, gitemp3, 1.2, .01, .2
kgatefd        envlpx         iampfac, ifundatt, 1, ifunddec, gitemp2, 1, .01
kgatefrm       envlpx         iafrmfac, ifrmatt, 1, ifrmdec, gitemp2, 1, .01
amod           poscil         kgatemod, knote*kvib, ifunc
afund          poscil         kgatefd, knote*kvib+amod, ifunc
afrm           poscil         kgatefrm, (int(1500/knote+.5)*knote)*kvib+amod*iratio, ifunc
xout            (afund+afrm)*kgate
 endop
;**************************************************
opcode CHOWNVibr,a,kki

kamp,kifc,ifunc xin

gitemp100  ftgen 0,0 ,128 ,7, 0, 128 , 1
       
        kifm1        =         kifc

        kifm2        =         kifc*3

        kifm3        =         kifc*4

        kindx1       =         7.5/log(kifc)        ;RANGE FROM CA 2 TO 1

        kindx2       =         15/sqrt(kifc)        ;RANGE FROM CA 2.6 TO .5

        kindx3       =         1.25/sqrt(kifc)      ;RANGE FROM CA .2 TO .038

        kvib        init      0               
 

                    timout    0,0.75,transient               ;DELAYS VIBRATO FOR P8 SECONDS

        kvbctl      linen     1,.5,10,.1                    ;VIBRATO CONTROL ENVELOPE

        krnd        randi     .0075,15                      ;RANDOM DEVIATION IN VIB WIDTH        

        kvib        poscil    kvbctl*.03+krnd,5.5*kvbctl,ifunc  ;VIBRATO GENERATOR

        

transient:

                    timout    .2,10,continue                ;EXECUTE FOR .2 SECS ONLY

        ktrans      linseg    1,.2,0,1,0                    ;TRANSIENT ENVELOPE 

        anoise      randi     ktrans,.2*kifc                 ;NOISE... 

        attack      poscil     anoise,2000,ifunc                  ;...CENTERED AROUND 2KHZ
 

continue:      

        amod1       poscil    kifm1*(kindx1+ktrans),kifm1,ifunc 

        amod2       poscil    kifm2*(kindx2+ktrans),kifm2,ifunc 

        amod3       poscil    kifm3*(kindx3+ktrans),kifm3,ifunc 

        asig        poscil    kamp,(kifc+amod1+amod2+amod3)*(1+kvib),ifunc

        asig        linen     asig+attack,.26,10,.2

        imax        ampmidi   1, gitemp100

        kgate       linenr    imax, 0, 1, .01

                    aout   =    asig * kgate


xout aout         

       endop




;**************************************************

instr	1

;kselect	init	1
;channel
;kmodfreq	chnget	"mod"
ksemi1	chnget	"semi"
kdetune	chnget	"detune"
kspread chnget "spread"
iatt	chnget	"att"
idec	chnget	"dec"
isus	chnget	"sus"
irel	chnget	"rel"
klfoamp	chnget	"vibamp"
klforate	chnget	"vibrate"
kselect	chnget	"select"

;midi
imidinn notnum
iamp	ampmidi	1
kbend	pchbend 0,2  ;pitch bend
;lfo
klfo	lfo	klfoamp,klforate,2
kfreq1 =	cpsmidinn(imidinn+kbend+int(ksemi1)) ;controllo midi
kfreq2 =	cpsmidinn(imidinn+kbend+kdetune+int(ksemi1))


;CHOWNClar,CHOWNglass,CHOWNLoop,CHOWNPerc,CHOWNPiano,CHOWNrebell,CHOWNSoprano,CHOWNString,CHOWNTrumpet,CHOWNVibr
if	kselect	=	1	then
acho1	CHOWNClar	iamp,kfreq1+klfo,1	
acho2	CHOWNClar	iamp,kfreq2+klfo,1	
elseif	kselect	=	2	then
acho1	CHOWNglass	iamp,kfreq1+klfo,1	
acho2	CHOWNglass	iamp,kfreq2+klfo,1
elseif	kselect	=	3	then
acho1	CHOWNLoop	iamp,kfreq1+klfo,1	
acho2	CHOWNLoop	iamp,kfreq2+klfo,1
elseif	kselect	=	4	then
acho1	CHOWNPerc	iamp,kfreq1+klfo,1	
acho2	CHOWNPerc	iamp,kfreq2+klfo,1
elseif	kselect	=	5	then
acho1	CHOWNPiano	iamp,kfreq1+klfo,1	
acho2	CHOWNPiano	iamp,kfreq2+klfo,1
elseif	kselect	=	6	then
acho1	CHOWNrebell	iamp,kfreq1+klfo,1	
acho2	CHOWNrebell	iamp,kfreq2+klfo,1
elseif	kselect	=	7	then
acho1	CHOWNSoprano	iamp,kfreq1,1	
acho2	CHOWNSoprano	iamp,kfreq2,1
elseif	kselect	=	8	then
acho1	CHOWNString	iamp,kfreq1+klfo,1	
acho2	CHOWNString	iamp,kfreq2+klfo,1
elseif	kselect	=	9	then
acho1	CHOWNTrumpet	iamp,kfreq1+klfo,1	
acho2	CHOWNTrumpet	iamp,kfreq2+klfo,1
elseif	kselect	=	10	then
acho1	CHOWNVibr	iamp,kfreq1,1	
acho2	CHOWNVibr	iamp,kfreq2,1



endif

ktrig	changed	kselect
if	ktrig	=	1	then
reinit	play
endif
play:

;master
aL	clip	acho1,0,0dbfs
aR	clip	acho2,0,0dbfs


aoutL = ((aL * kspread) + (aR * (1 - kspread))) *.5  
aoutR = ((aL * (1-kspread)) + (aR * kspread))   *.5

kadsr	mxadsr	iatt,idec,isus,irel

outs	(aoutL)*kadsr,(aoutR)*kadsr
vincr	ga1,aoutL*kadsr
vincr	ga2,aoutR*kadsr
endin

instr	10	;reverb
ktrig	chnget	"onoff"
klevel	chnget	"level"
ksize	chnget	"size"
if	ktrig	=	1	then
	
al,ar	reverbsc	ga1,ga2,ksize,16000,sr,0
asum1	dcblock2	al
asum2	dcblock2	ar
else
asum1	=	0
asum2	=	0
endif

outs	asum1*klevel,asum2*klevel
clear	ga1,ga2
endin


</CsInstruments>
<CsScore>
f1	0	16384	10	1

;f0 3600
i1	0	36000
i10	0	36000
</CsScore>
</CsoundSynthesizer>
