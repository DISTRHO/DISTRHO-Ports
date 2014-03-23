<Cabbage>
form caption("Percussive flute synthesizer"), size(760, 320), pluginID("per1") 

xypad bounds(0, 0, 250, 210), channel("chanx", "chany"), text("Modulation"), rangex(1,50,6.72), rangey(0,1,0.43)

groupbox bounds(260, 0, 240, 100), text("ADSR"), plant("adsr"){ 
rslider bounds(.0, .3, .6, .6), text("A"), colour("orange"), channel("att"), range(0.01,3, .5)
rslider bounds(.25, .3, .6, .6), text("D"), colour("orange"), channel("dec"), range(0,1, .5)
rslider bounds(.5, .3, .6, .6), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
rslider bounds(.75, .3, .6, .6), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
}

groupbox bounds(260, 105, 240, 100), text("Tune"), plant("tune"){ 
rslider bounds(.0, .3, .6, .6), text("Semi"), colour("white "), channel("semi"), range(-24,12,0)
rslider bounds(.25, .3, .6, .6), text("Detune"), colour("white "), channel("detune"), range(-1,1,0)
rslider bounds(.5, .3, .6, .6), text("Cutoff"), colour("white "), channel("cutoff"), range(200,12000,3000)
rslider bounds(.75, .3, .6, .6), text("Spread"), colour("white "), channel("spread"), range(0,1,.5)
}

groupbox bounds(510,0, 240, 100), text("LFO/Stereo chorus"), plant("mod"){ 
rslider bounds(.0, .3, .6, .6), text("Lfo amp"), colour("white "), channel("lfoamp"), range(0,5, 0)
rslider bounds(.25, .3, .6, .6), text("Lfo rate"), colour("white "), channel("lforate"), range(0,10, 0)
rslider bounds(.5, .3, .6, .6), text("depth"), colour("white "), channel("chdepth"), range(0,10,0)
rslider bounds(.75, .3, .6, .6), text("rate"), colour("white "), channel("chrate"), range(0,10,0)
}

groupbox bounds(510,105, 240, 100), text("Reverb"), plant("rev"){ 
rslider bounds(0, .3, .6, .6), text("level"), colour("dodgerblue"), channel("level"), range(0,1,0)
rslider bounds(0.25, .3, .6, .6), text("size"), colour("dodgerblue"), channel("size"), range(.1,.99,.5)
checkbox  bounds(.55, .40, .60, .30),channel("onoff"), text("On/Off"), value(0)
}

keyboard bounds(0, 220, 750, 70)

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d -m0d -+rtmidi=NULL -M0 
</CsOptions>
<CsInstruments>
sr = 44100 
ksmps = 32 
nchnls = 2 
0dbfs  = 1 

;Author: Giorgio Zucco (2012)


;turnon	10	;chorus
;turnon	12	;reverb

ga1	init	0
ga2	init	0
garev1	init	0
garev2	init	0


;*******************************************
opcode StChorus,aa,aakkkk

asigr,asigl,kdepth,kdepthl,krate,kratel  xin   ;legge i parametri in entrata

;ar,al  StChorus asigr,asigl,kdepth,kdepthl,krate,kratel


k1ch  randi       kdepth/2,krate,1
ar1   vdelay3 asigr,kdepth/2+k1ch,10
k2ch  randi       kdepth/2,krate*0.9,.2
ar2   vdelay3 asigr,kdepth/2+k2ch,10
k3ch  randi       kdepth/2,krate*1.1,.2
ar3   vdelay3 asigr,kdepth/2+k3ch,10
k4ch  randi       kdepth/2,krate*1.3,.1
ar4   vdelay3 asigr,kdepth/2+k4ch,10

k1chl  randi       kdepthl/2,kratel,1
ar1l   vdelay3 asigl,kdepthl/2+k1chl,10
k2chl  randi       kdepthl/2,kratel*0.9,.2
ar2l   vdelay3 asigl,kdepthl/2+k2chl,10
k3chl  randi       kdepthl/2,kratel*1.1,.2
ar3l   vdelay3 asigl,kdepthl/2+k3chl,10
k4chl  randi       kdepthl/2,kratel*1.3,.1
ar4l   vdelay3 asigl,kdepthl/2+k4chl,10


aL   =    (ar1+ar2+ar3+ar4)/2
aR  =    (ar1l+ar2l+ar3l+ar4l)/2
xout aL,aR           ; write output

       endop
;***********************************************

instr 1


iatt	chnget	"att"
idec	chnget	"dec"
isus	chnget	"sus"
irel	chnget	"rel"
kdet	chnget	"detune"
ksemi	chnget	"semi"
kcut	chnget	"cutoff"
kspread	chnget	"spread"
klfoamp	chnget	"lfoamp"
klforate	chnget	"lforate"
kindex	chnget	"fmamp"
kfmrate	chnget	"fmrate"
kcutoff	chnget	"cutoff"
kreso	chnget	"resonance"
kc1	chnget "chanx"
kc2	chnget "chany"	
;midi
imidinn notnum
iamp	ampmidi	1
kbend	pchbend 0,2  ;pitch bend
kfreq1 =	cpsmidinn(imidinn+kbend+int(ksemi)) ;controllo midi
kfreq2 =	cpsmidinn(imidinn+kbend+int(ksemi)+kdet) 

ifn1 = 1
ifn2 = 1
ifn3 = 1
ifn4 = 1
ivfn = 1

asig1	fmpercfl	iamp, kfreq1,kc1,kc2,klfoamp,klforate,ifn1,ifn2,ifn3,ifn4,ivfn
asig2	fmpercfl	iamp, kfreq2,kc1,kc2,klfoamp,klforate,ifn1,ifn2,ifn3,ifn4,ivfn

afl1	clfilt	asig1,kcutoff,0,4
afl2	clfilt	asig2,kcutoff,0,4

;master
al	=	((afl1 * kspread) + (afl2 * (1 - kspread))) *.2
ar	=	((afl1 * (1-kspread)) + (afl2 * kspread))  *.2 

aoutL	clip	al,0,0dbfs
aoutR	clip	ar,0,0dbfs

kadsr	mxadsr	iatt,idec,isus,irel
	
outs	(aoutL*kadsr),(aoutR*kadsr)

vincr	ga1,(aoutL*kadsr)
vincr	ga2,(aoutR*kadsr)
vincr	garev1,(aoutL*kadsr)
vincr	garev2,(aoutR*kadsr)

endin

;stereo chorus
instr	10


k1	chnget	"chdepth"
k2	chnget	"chrate"
kfxdepth	portk	k1,.01
kfxrate	portk	k2,.01

acho1,acho2	StChorus	ga1,ga2,kfxdepth,kfxdepth*.8,kfxrate,kfxrate*.8

outs	acho1,acho2	
clear	ga1,ga2
endin


instr	12	;reverb
ktrig	chnget	"onoff"
klevel	chnget	"level"
ksize	chnget	"size"
if	ktrig	=	1	then
	
al,ar	reverbsc	garev1,garev2,ksize,16000,sr,0
adc1	dcblock2	al
adc2	dcblock2	ar
kpan1	randomi	0,1,4
kpan2	randomi	0,1,8
aout1,aout2	pan2	adc1,kpan1
aout3,aout4	pan2	adc2,1-kpan2
asum1	=	(aout1+aout2)*klevel
asum2	=	(aout3+aout4)*klevel	
else
asum1	=	0
asum2	=	0
endif

outs	asum1,asum2
clear	garev1,garev2
endin


</CsInstruments>
<CsScore> 
;  sine wave.
f 1 0 32768 10 1

i1	0	36000
i10	0	36000
i12	0	36000
</CsScore>
</CsoundSynthesizer>