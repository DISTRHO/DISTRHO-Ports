<Cabbage>
form caption("Scantable synthesizer") size(552, 340), colour("black"),pluginID("sca1") 
rslider bounds(15, 10, 100, 100) channel("init"), range(50,59,50), caption("Initial"), colour("white"), midictrl(1, 1)
rslider bounds(120, 10, 100, 100) channel("semi"), range(-24,12,-12), caption("Coarse"), colour("white")
rslider bounds(225, 10, 100, 100) channel("detune"), range(-2,2,0), caption("detune"), colour("white")
rslider bounds(330, 10, 100, 100) channel("cutoff"), range(60,22000,4500), caption("Lowpass"), colour("white")
rslider bounds(435, 10, 100, 100) channel("spread"), range(0, 1, .5), caption("Stereo image"), colour("white")

groupbox bounds(15, 120, 240, 100), text("ADSR amplitude"), plant("ADSR"){ 
rslider bounds(.0, .3, .6, .6), text("A"), colour("orange"), channel("att"), range(0.01,3, .5)
rslider bounds(.25, .3, .6, .6), text("D"), colour("orange"), channel("dec"), range(0,1, .5)
rslider bounds(.5, .3, .6, .6), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
rslider bounds(.75, .3, .6, .6), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
}

groupbox bounds(270, 120, 130, 100), text("LFO"), plant("lfo"){ 
rslider bounds(.04, .3, .6, .6), text("Lfo Amp"), colour("white"), channel("lfoamp"), range(0,20, 0)
rslider bounds(.5, .3, .6, .6), text("Lfo Rate"), colour("white"), channel("lforate"), range(.1,20, 0)
}

groupbox bounds(410, 120, 130, 100), text("LFO filter"), plant("lfofilter"){ 
rslider bounds(0.04, .3, .6, .6), text("Lfo Amp"), colour("white"), channel("lfoamp2"), range(0,2000, 0)
rslider bounds(0.5, .3, .6, .6), text("Lfo Rate"), colour("white"), channel("lforate2"), range(0,20, 0)
}

keyboard pos(15, 240), size(520, 60)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -M0 -b1024  -m0d
</CsOptions>
<CsInstruments>
; Initialize the global variables. 
sr = 44100
ksmps = 64
nchnls = 2
0dbfs = 1

;Author: Giorgio Zucco (2012)

;scantable

gitemp	ftgen 2000, 0, 128, -7, 1, 128, 1

gitemp	ftgen  3000, 0, 128, -7, 0, 64, 100, 64, 0

gitemp	ftgen  4000, 0, 128, -7, 1, 128, 1

gitemp	ftgen  5000, 0, 128, -7, 0, 128, 0

gitemp	ftgen  29, 0, 4096, 10, 1
;************************************************


instr	1

;channel
kcondfn	init	50
ksemi1	chnget	"semi"
kdetune	chnget	"detune"
kcut	chnget	"cutoff"
kreso	chnget	"reso"
kspread chnget "spread"
iatt	chnget	"att"
idec	chnget	"dec"
isus	chnget	"sus"
irel	chnget	"rel"
klfoamp	chnget	"lfoamp"
klforate	chnget	"lforate"
klfoamp2	chnget	"lfoamp2"	;lfo x filter
klforate2	chnget	"lforate2"
;midi
imidinn notnum
iamp	ampmidi	1
kbend	pchbend 0,2  ;pitch bend
;lfo
klfo	lfo	klfoamp,klforate,2
kfreq1 =	cpsmidinn(imidinn+kbend+int(ksemi1))+klfo ;controllo midi
kfreq2 =	cpsmidinn(imidinn+kbend+kdetune+int(ksemi1))+klfo

kcondfn	chnget	"init"
ktrig	changed	kcondfn
if	ktrig	=	1	then
reinit	play
endif
play:		

asig1 scantable  iamp,kfreq1,i(kcondfn),2000,3000,4000,5000
asig2 scantable  iamp,kfreq2,i(kcondfn),2000,3000,4000,5000

;filter	
klfofilter	lfo	klfoamp2,klforate2,3
afilt1	clfilt	asig1,kcut+klfofilter,0,8
afilt2	clfilt	asig2,kcut+klfofilter,0,8
;master
aL	clip	afilt1,0,0dbfs
aR	clip	afilt2,0,0dbfs


aoutL = ((aL * kspread) + (aR * (1 - kspread))) *.5  
aoutR = ((aL * (1-kspread)) + (aR * kspread))   *.5

kadsr	madsr	iatt,idec,isus,irel

outs	aoutL*kadsr,aoutR*kadsr

endin



</CsInstruments>
<CsScore>
;Initial Condition/State
f50    0 128 7 0 30 0 2 1 2 0 30 0 30 0 2 -1 2 0 30 0
f51    0 128 7 0 30 0 2 1 2 0 30 0
f52    0 128 10 1 0 1 0 1
f53    0 128 10 1 .9 .8 .7 .6 .5 .4 .3 .2
f54    0 128 10 1 0 2 1 0 0 1 0 2 1
f55    0 128 10 1 1 0 1 2 1 0 1 2 1 0 
f56  0 128 7 0 128 0
f57  0 128 7 0 64 1 64 0
f58  0 128 10 1 1 0 1 0 1 0 1 0 1 0 0 1 0 1 0 1 0 1
f59  0 128 7 0 32 1 32 0 32 1 32 0

;f0 3600
i1	0	36000
</CsScore>
</CsoundSynthesizer>
