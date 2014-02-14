<Cabbage>
form caption("HiperSaw synthesizer") size(552, 330), colour("black"),pluginID("hip1") 
rslider bounds(15, 10, 100, 100) channel("detune"), range(0,1,.5), caption("Depth"), colour("orange"), midictrl(1, 1)
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
rslider bounds(0.05, .30, .60, .60), text("Lfo Amp"), colour("orange"), channel("lfoamp"), range(0,100, 0)
rslider bounds(.5, .30, .60, .60), text("Lfo Rate"), colour("orange"), channel("lforate"), range(0,20, 0)
;rslider bounds(115, 30, 60, 60), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
;rslider bounds(165, 30, 60, 60), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
}
groupbox bounds(410, 120, 130, 100), text("LFO filter"), plant("lfofilter"){ 
rslider bounds(0.05, .30, .60, .60), text("Lfo Amp"), colour("orange"), channel("lfoamp2"), range(0,1000, 0)
rslider bounds(.5, .30, .60, .60), text("Lfo Rate"), colour("orange"), channel("lforate2"), range(0,10, 0)
;rslider bounds(115, 30, 60, 60), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
;rslider bounds(165, 30, 60, 60), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
}

keyboard pos(1, 240), size(550, 60)
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

;UDO Hipersaw
opcode	Hipersaw2,a,kkki

kamp,kcps,kdetune,ifunc	xin             

krnd1	poscil	kdetune,.1,ifunc
krnd2	poscil	kdetune,.2,ifunc
krnd3	poscil	kdetune,.3,ifunc
krnd4	poscil	kdetune,.4,ifunc
krnd5	poscil	kdetune,.5,ifunc
krnd6	poscil	kdetune,.6,ifunc
krnd7	poscil	kdetune,.7,ifunc
krnd8	poscil	kdetune,.8,ifunc

a1	vco2	kamp,kcps,0
a2	vco2	kamp,kcps+kdetune,0
a3	vco2	kamp,kcps+(kdetune+krnd1)*.1,0
a4	vco2	kamp,kcps+(kdetune+krnd2)*.1,0
a5	vco2	kamp,kcps+(kdetune+krnd3)*.1,0
a6	vco2	kamp,kcps+(kdetune+krnd4)*.1,0
a7	vco2	kamp,kcps+(kdetune+krnd5)*.1,0
a8	vco2	kamp,kcps+(kdetune+krnd6)*.1,0
a9	vco2	kamp,kcps+(kdetune+krnd7)*.1,0
a10	vco2	kamp,kcps+(kdetune+krnd8)*.1,0


asum	sum	a1,a2,a3,a4,a5,a6,a7,a8,a9,a10


xout	asum*.3                 ; write output

endop


instr	1

;channel
kdrive	chnget	"detune"
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
kfreq1 =	cpsmidinn(imidinn+kbend+int(ksemi1)) ;controllo midi
kfreq2 = cpsmidinn(imidinn+kbend+int(ksemi1)) ;controllo midi + detune

;lfo
klfo	poscil	klfoamp,klforate,1	
;hipersaw waveform
asig1	Hipersaw2	iamp,(kfreq1+klfo),kdrive*8,1
asig2	Hipersaw2	iamp,(kfreq2+klfo),(1-kdrive*8),1

;filter	
klfofilter	lfo	klfoamp2,klforate2,3
aout1	moogladder	asig1,kcut+klfofilter,kreso
aout2	moogladder	asig2,kcut+klfofilter,kreso
;aout1	balance	amoog1,asig1
;aout2	balance	amoog2,asig2

;master
aL	clip	aout1,0,.9
aR	clip	aout2,0,.9


aoutL = ((aL * kspread) + (aR * (1 - kspread))) *.5  
aoutR = ((aL * (1-kspread)) + (aR * kspread))   *.5

kadsr	mxadsr	iatt,idec,isus,irel

outs	aoutL*kadsr,aoutR*kadsr

endin



</CsInstruments>
<CsScore>
f1	0	4096	10	1
i1	0	36000
</CsScore>
</CsoundSynthesizer>
