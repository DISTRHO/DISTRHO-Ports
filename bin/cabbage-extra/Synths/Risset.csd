<Cabbage>
form caption("Jean Claude Risset collection") size(552, 330), colour("black"),pluginID("jea1") 
rslider bounds(15, 10, 100, 100) channel("spread"), range(0,1,.5), caption("Spread"), colour("tomato"), midictrl(1, 1)
rslider bounds(120, 10, 100, 100) channel("semi"), range(-24,12,-12), caption("Coarse"), colour("tomato")
rslider bounds(225, 10, 100, 100) channel("detune"), range(-2,2,0), caption("Detune"), colour("tomato")
rslider bounds(330, 10, 100, 100) channel("vibamp"), range(0,20,0), caption("Vib Amp"), colour("tomato")
rslider bounds(435, 10, 100, 100) channel("vibrate"), range(0,20,0), caption("Vib rate"), colour("tomato")

groupbox bounds(15, 120, 240, 100), text("ADSR amplitude"), plant("ADSR"){ 
rslider bounds(.0, .3, .6, .6), text("A"), colour("orange"), channel("att"), range(0.01,3, .5)
rslider bounds(.25, .3, .6, .6), text("D"), colour("orange"), channel("dec"), range(0,1, .5)
rslider bounds(.5, .3, .6, .6), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
rslider bounds(.75, .3, .6, .6), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
}
;RissClar RissDrum RissFlute RissHarmon RissNoise RissOctave RissRing
combobox bounds(280,120, 160, 20), channel("select"), value(1), text("clar","snare", "flute","harmon","noise","octave","ring")
rslider bounds(280,150, 60, 60), text("Rev level"), colour("tomato"), channel("level"), range(0,1,.8)
rslider bounds(350, 150, 60, 60), text("Rev size"), colour("tomato"), channel("size"), range(.1,.99,.7)
checkbox  bounds(420,150, 160, 30),channel("onoff"), text("Reverb On/Off"), value(0)

keyboard pos(1, 240), size(550, 60)

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -M0 -b1024  -m0d
</CsOptions>
<CsInstruments>

sr = 44100
ksmps = 16
nchnls = 2
0dbfs = 1

;Author: Giorgio Zucco (2012)

ga1	init	0
ga2	init	0




;Risset opcodes 

;**************************************************
opcode RissClar,a,kk

gisine1  ftgen 0 ,0 ,512, 7, -1, 200, -.5, 112, .5 ,200, 1
gisine3  ftgen 0 ,  0, 512, 9, 1, 1, 0

kveloc,knote  xin


kgate         linenr         kveloc,0,.2,.01
a1            linen          255,.085,.75,100
a1            poscil         a1,knote,gisine3
a1            table          a1+256,gisine1
xout           a1*kgate
endop

;**************************************************
opcode RissDrum,a,kk

gitemp1  ftgen 0 , 0 ,512, 9 ,1, 1, 0
gitemp2  ftgen 0 , 0, 512, 5 ,4096 ,512, 1
gitemp3  ftgen 0 , 0, 512, 9 ,10 ,1 ,0, 16, 1.5, 0 ,22, 2 ,0, 23 ,1.5, 0
gitemp4  ftgen 0 , 0, 512, 9, 1, 1, 0
gitemp5  ftgen 0 , 0, 512, 5, 256, 512, 1

kveloc,knote  xin


kgate     linenr    kveloc/2, 0, .2, .01
i1        =         6000*.3
i2        =         500*.1
i3        =         1
i4        =         6000*.8
i5        =         500
a1     randi     6000,4000
a1     poscil     a1,i3,gitemp2
a1     poscil     a1,3000,gitemp1
a2     poscil     i1,i3,gitemp2
a2     poscil     a2,knote*.01,gitemp3
a3     poscil     i4,i3,gitemp5
a3     poscil     a3,knote,gitemp4
aout = (a1+a2+a3)*.0001
xout       aout*kgate
 
endop

;**************************************************
opcode RissGliss,a,ik

gitemp1  ftgen 0 ,0,	8192,	10,	1

iamp,kfreq  xin

irise = .5
kenv1 expseg .01, irise, iamp*1200,1000-irise,1
k1 randi kfreq*.05, 1, .2/(1000)
k2 randi kfreq*.25, .1, .3/(1000)
a1 poscil kenv1, kfreq+k1+k2, gitemp1
kenv11 expseg .01, irise, iamp*.83*1200,1000-irise,1
k11 randi kfreq*.05*1.154, 1, .2/(1000)
k12 randi kfreq*.25*1.154, .1, .3/(1000)
a11 poscil kenv11, kfreq*1.154+k11+k12, gitemp1
kenv21 expseg .01, irise, iamp*.33*1200,1000-irise,1
k21 randi kfreq*.051*.417, 1, .2/(1000)
k22 randi kfreq*.25*1.417, .1, .3/(1000)
a21 poscil kenv21, kfreq*1.417+k21+k22, gitemp1
kenv31 expseg .01, irise, iamp*.67*1200,1000-irise,1
k31 randi kfreq*.05*1.604, 1, .2/(1000)
k32 randi kfreq*.25*1.604, .1, .3/(1000)
a31 poscil kenv31, kfreq*1.604+k31+k32, gitemp1
kenv41 expseg .01, irise, iamp*1200,1000-irise,1
k41 randi kfreq*.05*2.521, 1, .2/(1000)
k42 randi kfreq*.25*2.521, .1, .3/(1000)
a41 poscil kenv41, kfreq*2.521+k41+k42, gitemp1
kenv51 expseg .01, irise, iamp*.33*1200,1000-irise,1
k51 randi kfreq*.05*2.792, 1, .2/(1000)
k52 randi kfreq*.25*2.792, .1, .3/(1000)
a51 poscil kenv51, kfreq*2.792+k51+k52, gitemp1
kenv61 expseg .01, irise, iamp*.33*1200,1000-irise,1
k61 randi kfreq*.05*3.383, 1, .2/(1000)
k62 randi kfreq*.25*3.383, .1, .3/(1000)
a61 poscil kenv61, kfreq*3.383+k61+k62,gitemp1
a1=(a1+a11+a21+a31+a41+a51+a61)*.01

;kenv71 expseg .01, irise, 1, 100*.5, 1, 100*.5-irise, 1

;kdeclick linsegr  0,.1,1,.3,.5,0.001,.01
kdeclick  linsegr  0.01, 0.01, 1, 0.33*1, 0

xout ((a1)*kdeclick)*.02       ; write output

       endop


;**************************************************
opcode RissFlute,a,kk

gitemp1  ftgen 0 ,0, 512, 9 ,1 ,.26,0
gitemp2  ftgen 0 , 0, 512, 10, 1, .4, .2, .1, .1, .05     

kamp,kfreq  xin


k1        randi     kamp*.01,5
k1        =         k1 + kamp
k2        poscil    k1,1/.24,gitemp1
k2        =         k2 + .74
a1        poscil    k2,kfreq,gitemp2
kdeclick  linsegr  0.1, 0.01, 1, 0.33*1, 0
      xout      a1*kdeclick
          endop

;**************************************************
opcode RissHarmon,a,kk
gisine  ftgen 0 , 0, 1024, 10, 1 ,0, 0 ,0 , .7, .7, .7, .7 ,.7, .7 

kveloc,knote  xin

kgate          linenr    kveloc,1,2,.01
i1             =         .03
i2             =         2*.03
i3             =         3*.03
i4             =         4*.03
ampenv         linenr    9,.01,1,.01      
a1             poscil    ampenv,knote,gisine       
a2             poscil    ampenv,knote+i1,gisine    
a3             poscil    ampenv,knote+i2,gisine    
a4             poscil    ampenv,knote+i3,gisine 
a5             poscil    ampenv,knote+i4,gisine 
a6             poscil    ampenv,knote-i1,gisine 
a7             poscil    ampenv,knote-i2,gisine 
a8             poscil    ampenv,knote-i3,gisine 
a9             poscil    ampenv,knote-i4,gisine 
aout =      ((a1+a2+a3+a4+a5+a6+a7+a8+a9)*kgate)*.05
xout aout
endop


;**************************************************
opcode RissNoise,a,ik


gitemp1  ftgen 0 ,0,	8192,	10,	1
gitemp2  ftgen 0 ,0,	8192,	7,	-1,	8192,	1
gitemp3  ftgen 0 ,0,	8192,	7,	1,	4096,	1,	0,	-1,	4096,	-1
gitemp4  ftgen 0 ,0,	8192,	21,	1

iveloc,knote  xin

kenv	linsegr	0,.01,iveloc, 1, iveloc,2,iveloc*.2,.25, 0
aosc	poscil	kenv, 440, gitemp4
kenv	expon	2, 1, .01
aosc2	butterbp	aosc, knote, knote * kenv
aosc2	butterbp	aosc2, knote, knote * kenv
aosc	balance	aosc2, aosc
aosc	=	aosc * .7
xout	aosc

endop

;**************************************************
opcode RissOctave,a,kk


gitemp2  ftgen 0 ,0, 128, 8, 0, .5, 0, 63.5, 1, 63.5, 0, .5 ,0
gisine  ftgen 0 ,0, 4096,10,1

kamp,kfreq  xin

krate = .1


iphsoff   =         (65/5)*.01

ka1       =         kamp
iph1      =         0
kamp1     poscil    ka1,krate,gitemp2,iph1
kfreq1    =         kfreq
asig1     poscil    kamp1,kfreq1,gisine

ka2       =         kamp
iph2      =         iph1+iphsoff
kamp2     poscil    ka2,krate,gitemp2,iph2
kfreq2    =         kfreq*2
asig2     poscil    kamp2,kfreq2,gisine

ka3       =         kamp
iph3      =         iph2+iphsoff
kamp3     poscil    ka3,krate,gitemp2,iph3
kfreq3    =         kfreq*3
asig3     poscil    kamp3,kfreq3,gisine

ka4       =         kamp
iph4      =         iph3+iphsoff
kamp4     poscil    ka4,krate,gitemp2,iph4
kfreq4    =         kfreq*4
asig4     poscil    kamp4,kfreq4,gisine

ka5       =         kamp
iph5      =         iph4+iphsoff
kamp5     poscil    ka5,krate,gitemp2,iph5
kfreq5    =         kfreq*5
asig5     poscil    kamp5,kfreq5,gisine

add       =         asig1+asig2+asig3+asig4+asig5
;kenv      linseg    0,p3/2,1,p3/2,0
aout      =         add
kdeclick  linsegr  0.01, 0.01, 1, 0.33*1, 0
xout      (aout*.5)*kdeclick
   endop



;**************************************************
opcode RissRing,a,kk


gitemp2  ftgen 0 ,0, 512, 7, 0, 43, 1, 171, 1, 84, -1, 171, -1, 43, 0
gitemp3  ftgen 0 ,0, 512, 9, 1, 1, 0


kveloc,knote  xin
kgate     linenr         kveloc/2, 0, .2, .01
a1        expseg         .2,2.3,1,1-.01,.001
a1        poscil         a1,knote,gitemp3
a2        poscil         10000,1500,gitemp2
aout = (a1*a2)*.001
xout       aout*kgate

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



if	kselect	=	1	then
ariss1	RissClar	iamp,kfreq1+klfo			
ariss2	RissClar	iamp,kfreq2+klfo
elseif	kselect	=	2	then
ariss1	RissDrum	iamp,kfreq1+klfo			
ariss2	RissDrum	iamp,kfreq2+klfo
elseif	kselect	=	3	then
ariss1	RissFlute	iamp,kfreq1+klfo			
ariss2	RissFlute	iamp,kfreq2+klfo
elseif	kselect	=	4	then
ariss1	RissHarmon	iamp,kfreq1+klfo			
ariss2	RissHarmon	iamp,kfreq2+klfo
elseif	kselect	=	5	then
ariss1	RissNoise	iamp,kfreq1+klfo			
ariss2	RissNoise	iamp,kfreq2+klfo
elseif	kselect	=	6	then
ariss1	RissOctave	iamp,kfreq1+klfo			
ariss2	RissOctave	iamp,kfreq2+klfo
elseif	kselect	=	7	then
ariss1	RissRing	iamp,kfreq1+klfo			
ariss2	RissRing	iamp,kfreq2+klfo
endif

ktrig	changed	kselect
if	ktrig	=	1	then
reinit	play
endif
play:

;master
aL	clip	ariss1,0,0dbfs
aR	clip	ariss2,0,0dbfs


aoutL = ((aL * kspread) + (aR * (1 - kspread))) *.5  
aoutR = ((aL * (1-kspread)) + (aR * kspread))   *.5

kadsr	mxadsr	iatt,idec,isus,irel

outs	(aoutL*kadsr),(aoutR*kadsr)
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
