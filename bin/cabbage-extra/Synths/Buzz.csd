<Cabbage>
form caption("Buzz synthesizer") size(562, 340), colour("black"),pluginID("buz1") 
rslider bounds(15, 10, 100, 100) channel("detune"), range(-1,1,0), caption("Detune"), colour("SteelBlue "), midictrl(1, 1)
rslider bounds(120, 10, 100, 100) channel("semi"), range(-24,12,-24), caption("Coarse"), colour("SteelBlue ")
rslider bounds(225, 10, 100, 100) channel("cutoff"), range(60,2000,400), caption("Cutoff"), colour("SteelBlue ")
rslider bounds(330, 10, 100, 100) channel("harm"), range(1,20,1), caption("Harmonic"), colour("SteelBlue ")
rslider bounds(435, 10, 100, 100) channel("spread"), range(0, 1, .5), caption("Stereo image"), colour("SteelBlue ")

groupbox bounds(15, 120, 240, 100), text("ADSR amplitude"), plant("ADSR"){ 
rslider bounds(.0, .3, .6, .6), text("A"), colour("orange"), channel("att"), range(0.01,3, .5)
rslider bounds(.25, .3, .6, .6), text("D"), colour("orange"), channel("dec"), range(0,1, .5)
rslider bounds(.5, .3, .6, .6), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
rslider bounds(.75, .3, .6, .6), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
}
groupbox bounds(270, 120, 130, 100), text("LFO"), plant("lfo"){ 
rslider bounds(.04, .3, .6, .6), text("Lfo Amp"), colour("SteelBlue "), channel("lfoamp"), range(0,100, 0)
rslider bounds(.5, .3, .6, .6), text("Lfo Rate"), colour("SteelBlue "), channel("lforate"), range(0,20, 0)
}
groupbox bounds(410, 120, 130, 100), text("LFO filter"), plant("lfofilter"){ 
rslider bounds(.04, .3, .6, .6), text("Lfo Amp"), colour("SteelBlue "), channel("lfoamp2"), range(0,1000, 0)
rslider bounds(.5, .3, .6, .6), text("Lfo Rate"), colour("SteelBlue "), channel("lforate2"), range(0,10, 0)
}
keyboard pos(10,240), size(540,60)

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -M0 -b1024  -m0d
</CsOptions>
<CsInstruments>
; Initialize the global variables. 
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

;Author: Giorgio Zucco (2012)


instr	1

;channel
kdetune	chnget	"detune"
ksemi	chnget	"semi"
kcut	chnget	"cutoff"
kharm	chnget	"harm"
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
kfreq1 = cpsmidinn(imidinn+kbend+int(ksemi)) 
kfreq2 = cpsmidinn(imidinn+kbend+int(ksemi)+kdetune) 
;lfo
klfo	poscil	klfoamp,klforate,1	
asig1	buzz	iamp,(kfreq1+klfo),int(kharm),1
asig2	buzz	iamp,(kfreq2+klfo),int(kharm),1
;filter	
klfofilter	lfo	klfoamp2,klforate2,3
aout1	fofilter	asig1,kcut+klfofilter,0.007,0.04
aout2	fofilter	asig2,kcut+klfofilter,0.007,0.04
aout1x	balance	aout1,asig1
aout2x	balance	aout2,asig2

;master
aL	clip	aout1x,0,0dbfs
aR	clip	aout2x,0,0dbfs


aoutL = ((aL * kspread) + (aR * (1 - kspread))) *.5
aoutR = ((aL * (1-kspread)) + (aR * kspread))  *.5 

kadsr	mxadsr	iatt,idec,isus,irel

outs	aoutL*kadsr,aoutR*kadsr

endin



</CsInstruments>
<CsScore>
f1	0	16384	10	1
f2	0	1024	10	1
i1	0	36000
</CsScore>
</CsoundSynthesizer>
