<Cabbage>
form caption("Wave Terrain Synth"), size(760, 320), pluginID("wte1") 

xypad bounds(0, 0, 250, 210), channel("chanx", "chany"), text("Wave terrain X/Y"), rangex(0, 1, .2), rangey(0, 1, .4)

groupbox bounds(260, 0, 240, 100), text("ADSR"), plant("adsr"){ 
rslider bounds(.0, .3, .6, .6), text("A"), colour("orange"), channel("att"), range(0.01,3, .5)
rslider bounds(.25, .3, .6, .6), text("D"), colour("orange"), channel("dec"), range(0,1, .5)
rslider bounds(.5, .3, .6, .6), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
rslider bounds(.75, .3, .6, .6), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
}

groupbox bounds(260, 105, 240, 100), text("Tune/Moog filter"), plant("tune"){ 
rslider bounds(.0, .3, .6, .6), text("Coarse"), colour("red"), channel("det1"), range(-24,12,0)
rslider bounds(.25, .3, .6, .6), text("Detune"), colour("red"), channel("detune"), range(-1,1,0)
rslider bounds(.5, .3, .6, .6), text("cutoff"), colour("red"), channel("cutoff"), range(500,22000,4000)
rslider bounds(.74, .3, .6, .6), text("reso"), colour("red"), channel("resonance"), range(.1,.9,.5)
}

groupbox bounds(510,0, 240, 100), text("LFO/Stereo chorus"), plant("mod"){ 
rslider bounds(.0, .3, .6, .6), text("Lfo amp"), colour("red"), channel("lfoamp"), range(0,50, 0)
rslider bounds(.25, .3, .6, .6), text("Lfo rate"), colour("red"), channel("lforate"), range(0,20, 0)
rslider bounds(.5, .3, .6, .6), text("depth"), colour("red"), channel("chdepth"), range(0,10,0)
rslider bounds(.75, .3, .6, .6), text("rate"), colour("red"), channel("chrate"), range(0,10,0)
}

groupbox bounds(510,105, 240, 100), text("Reverb"), plant("rev"){ 
rslider bounds(0, .3, .6, .6), text("level"), colour("dodgerblue"), channel("level"), range(0,1,0)
rslider bounds(0.25, .3, .6, .6), text("size"), colour("dodgerblue"), channel("size"), range(.1,.99,.5)
;rslider bounds(115, 30, 60, 60), text("depth"), colour("dodgerblue"), channel("chdepth"), range(0,10,0)
;rslider bounds(165, 30, 60, 60), text("rate"), colour("dodgerblue"), channel("chrate"), range(0,10,0)
checkbox  bounds(.55, .40, .60, .30),channel("onoff"), text("On/Off"), value(0), colour("red")
}
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

;turnon don't work!
;turnon	10	;chorus
;turnon	12	;reverb

ga1	init	0
ga2	init	0
garev1	init	0
garev2	init	0

gitemp ftgen 1,0,16384,10,1,0,.6,0,.3,0,.2,0,.1

gitemp ftgen 2,0,16384,10,1,1,1,1,0.7,0.5,0.3,0.1



;*******************************************
opcode StChorus,aa,aakkkk

asigr,asigl,kdepth,kdepthl,krate,kratel  xin   ;legge i parametri in entrata

;ar,al  StChorus asigr,asigl,kdepth,kdepthl,krate,kratel


k1ch  randi       kdepth/2,krate,1
ar1   vdelay asigr,kdepth/2+k1ch,10
k2ch  randi       kdepth/2,krate*0.9,.2
ar2   vdelay asigr,kdepth/2+k2ch,10
k3ch  randi       kdepth/2,krate*1.1,.2
ar3   vdelay asigr,kdepth/2+k3ch,10
k4ch  randi       kdepth/2,krate*1.3,.1
ar4   vdelay asigr,kdepth/2+k4ch,10

k1chl  randi       kdepthl/2,kratel,1
ar1l   vdelay asigl,kdepthl/2+k1chl,10
k2chl  randi       kdepthl/2,kratel*0.9,.2
ar2l   vdelay asigl,kdepthl/2+k2chl,10
k3chl  randi       kdepthl/2,kratel*1.1,.2
ar3l   vdelay asigl,kdepthl/2+k3chl,10
k4chl  randi       kdepthl/2,kratel*1.3,.1
ar4l   vdelay asigl,kdepthl/2+k4chl,10


aL   =    (ar1+ar2+ar3+ar4)/4
aR  =    (ar1l+ar2l+ar3l+ar4l)/4
xout aL,aR           ; write output

       endop
;***********************************************



instr 1


iatt	chnget	"att"
idec	chnget	"dec"
isus	chnget	"sus"
irel	chnget	"rel"
ksemi	chnget	"det1"
kdet	chnget	"detune"
klfoamp	chnget	"lfoamp"
klforate	chnget	"lforate"
kcutoff	chnget	"cutoff"
kreso	chnget	"resonance"
kx1	chnget "chanx"
ky1	chnget "chany"	
kx	portk kx1,.05	;remove noise
ky	portk ky1,.05	
;midi

imidinn notnum
iamp	ampmidi	1
kbend	pchbend 0,2  ;pitch bend
kfreq1 =	cpsmidinn(imidinn+kbend+int(ksemi)) ;controllo midi
kfreq2 =	cpsmidinn(imidinn+kbend+int(ksemi)+kdet) 


;lfo
klfo	poscil	klfoamp,klforate,1

ifunc1	=	1
ifunc2	=	2


a1b	wterrain	iamp,(kfreq1+klfo),.1+kx,.1+ky,-kx,.1+ky,ifunc1,ifunc2
a2b	wterrain	iamp,(kfreq2+klfo),.1+kx,.1+ky,-kx,.1+ky,ifunc1,ifunc2


kadsr	mxadsr	iatt,idec,isus,irel

aout	=	a1b+a2b	;global out

;abulp1	butterlp	aout,kcutoff*kadsrfilt

afilt	moogladder	aout,kcutoff,kreso
amaster	balance 	afilt,aout

aclip	clip	amaster,0,0dbfs

kpan	jspline	1,0,8
al,ar	pan2	aclip,kpan	
outs	(al*kadsr),(ar*kadsr)
vincr	ga1,(al*kadsr)
vincr	ga2,(ar*kadsr)
vincr	garev1,(al*kadsr)
vincr	garev2,(ar*kadsr)

endin

;stereo chorus
instr	10


k1	chnget	"chdepth"
k2	chnget	"chrate"
kfxdepth	portk	k1,.01
kfxrate	portk	k2,.01

acho1,acho2	StChorus	ga1,ga2,kfxdepth,kfxdepth*.8,kfxrate*.8,kfxrate*.5

outs	acho1,acho2	
clear	ga1,ga2
endin


instr	12	;reverb
ktrig	chnget	"onoff"
klevel	chnget	"level"
ksize	chnget	"size"
if	ktrig	=	1	then
	
al,ar	reverbsc	garev1,garev2,ksize,16000,sr,0
asum1	dcblock2	al
asum2	dcblock2	ar
else
asum1	=	0
asum2	=	0
endif

outs	asum1*klevel,asum2*klevel
clear	garev1,garev2
endin


</CsInstruments>
<CsScore> 


;f0 36000
i1	0	36000
i10	0	36000
i12	0	36000
</CsScore>
</CsoundSynthesizer>