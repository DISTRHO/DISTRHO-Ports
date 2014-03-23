<Cabbage>
form caption("Scanned synthesizer") size(552, 330), colour("black"),pluginID("sca1") 
rslider bounds(15, 10, 100, 100) channel("speed"), range(0.01,.2,.01), caption("Speed"), colour("orange"), midictrl(1, 1)
rslider bounds(120, 10, 100, 100) channel("semi"), range(-12,12,-12), caption("Coarse"), colour("orange")
rslider bounds(225, 10, 100, 100) channel("cutoff"), range(60,22000,12000), caption("Cutoff"), colour("orange")
rslider bounds(330, 10, 100, 100) channel("reso"), range(.1,.99,.2), caption("Resonance"), colour("orange")
rslider bounds(435, 10, 100, 100) channel("spread"), range(0, 1, .5), caption("Stereo image"), colour("orange")

groupbox bounds(15, 120, 240, 100), text("ADSR amplitude"), plant("ADSR"){ 
rslider bounds(.0, .3, .6, .6), text("A"), colour("orange"), channel("att"), range(0.01,3, .5)
rslider bounds(.25, .3, .6, .6), text("D"), colour("orange"), channel("dec"), range(0,1, .5)
rslider bounds(.5, .3, .6, .6), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
rslider bounds(.75, .3, .6, .6), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
}
groupbox bounds(270, 120, 130, 100), text("LFO"), plant("lfo"){ 
rslider bounds(.04, .3, .6, .6), text("Lfo Amp"), colour("orange"), channel("lfoamp"), range(0,4, 0)
rslider bounds(.5, .3, .6, .6), text("Lfo Rate"), colour("orange"), channel("lforate"), range(.1,10, 0)
;rslider bounds(115, 30, 60, 60), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
;rslider bounds(165, 30, 60, 60), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
}
groupbox bounds(410, 120, 130, 100), text("LFO filter"), plant("lfofilter"){ 
rslider bounds(.04, .3, .6, .6), text("Lfo Amp"), colour("orange"), channel("lfoamp2"), range(0,1000, 0)
rslider bounds(.5, .3, .6, .6), text("Lfo Rate"), colour("orange"), channel("lforate2"), range(0,10, 0)
;rslider bounds(115, 30, 60, 60), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
;rslider bounds(165, 30, 60, 60), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
}

keyboard pos(15, 240), size(530, 60)
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




instr	1

;channel
krate	chnget	"speed"
ksemi1	chnget	"semi"
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
kfreq =	cpsmidinn(imidinn+kbend+int(ksemi1))+klfo ;controllo midi

;scanned
ktrig	changed	krate
if	ktrig	=	1	then
reinit	play
endif
play:

;irate	=	i(krate)
ifnvel	=	6
ifnmass	=	2
ifnstif	=	3
ifnctr	=	4
ifndamp	=	5
kmass	=	2
kstif	=	.1
kcentr	=	.1
kdamp	=	-.01
ileft	=	.5
iright	=	.5
kpos	=	0
ky	=	0
ain	=	0
idisp	=	1
id	=	2

scanu	1,.01+i(krate),ifnvel,ifnmass,ifnstif,ifnctr,ifndamp,kmass,kstif, kcentr,kdamp,ileft,iright,kpos,ky,ain,idisp,id

asig1	scans	iamp,kfreq,8,id
asig2	scans	iamp,kfreq,7,id
;filter	
klfofilter	lfo	klfoamp2,klforate2,3
amoog1	moogladder	asig1,kcut+klfofilter,kreso
amoog2	moogladder	asig2,kcut+klfofilter,kreso
aout1	balance	amoog1,asig1
aout2	balance	amoog2,asig2

;master
aL	clip	aout1,0,.9
aR	clip	aout2,0,.9


aoutL = ((aL * kspread) + (aR * (1 - kspread))) *.5  
aoutR = ((aL * (1-kspread)) + (aR * kspread))   *.5

kadsr	madsr	iatt,idec,isus,irel

outs	aoutL*kadsr,aoutR*kadsr

endin



</CsInstruments>
<CsScore>
f1 0 128 10 1
; Initial condition
;f1 0 128 7 0 64 1 64 0
; Masses
f2 0 128 -7  1 128 .8
; Spring matrices
f3 0 16384 -23 "string-128"
; Centering force
f4  0 128 -7 0 64  6  64 0
; Damping
f5 0 128 -7 1 128 .0001
; Initial velocity
f6 0 128 -7 .02 128 0
; Trajectories
f8 0	128	-5	.001	32	64	64	96	32
f7  0 128 -7 0 128 128   

;f0 3600
i1	0	36000
</CsScore>
</CsoundSynthesizer>
