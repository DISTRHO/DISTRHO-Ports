<Cabbage>
form caption("Vectorial Synth"), size(760, 320), pluginID("vec1") 

xypad bounds(5, 0, 250, 205), channel("chanx", "chany"), text("Vectorial mixing"), rangex(0, 1, .5), rangey(0, 1, .5)

groupbox bounds(260, 0, 240, 100), text("ADSR"), plant("adsr"){ 
rslider bounds(.0, .3, .6, .6), text("A"), colour("orange"), channel("att"), range(0.01,3, .5)
rslider bounds(.25, .3, .6, .6), text("D"), colour("orange"), channel("dec"), range(0,1, .5)
rslider bounds(.5, .3, .6, .6), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
rslider bounds(.75, .3, .6, .6), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
}

groupbox bounds(260, 105, 240, 100), text("Tune"), plant("tune"){ 
rslider bounds(.0, .3, .6, .6), text("Detune1"), colour("dodgerblue"), channel("det1"), range(-24,12,0)
rslider bounds(.25, .3, .6, .6), text("Detune2"), colour("dodgerblue"), channel("det2"), range(-24,12,-5)
rslider bounds(.5, .3, .6, .6), text("Detune3"), colour("dodgerblue"), channel("det3"), range(-24,12,0)
rslider bounds(.75, .3, .6, .6), text("Detune4"), colour("dodgerblue"), channel("det4"), range(-24,12,-12)
}

groupbox bounds(505,0, 240, 100), text("LFO/Stereo chorus"), plant("mod"){ 
rslider bounds(.0, .3, .6, .6), text("Lfo amp"), colour("dodgerblue"), channel("lfoamp"), range(0,50, 0)
rslider bounds(.25, .3, .6, .6), text("Lfo rate"), colour("dodgerblue"), channel("lforate"), range(0,20, 0)
rslider bounds(.5, .3, .6, .6), text("depth"), colour("dodgerblue"), channel("chdepth"), range(0,10,0)
rslider bounds(.75, .3, .6, .6), text("rate"), colour("dodgerblue"), channel("chrate"), range(0,10,0)
}

groupbox bounds(505,105, 240, 100), text("Reverb"), plant("rev"){ 
rslider bounds(0, .3, .6, .6), text("level"), colour("dodgerblue"), channel("level"), range(0,1,0)
rslider bounds(0.25, .3, .6, .6), text("size"), colour("dodgerblue"), channel("size"), range(.1,.99,.5)
;rslider bounds(115, 30, 60, 60), text("depth"), colour("dodgerblue"), channel("chdepth"), range(0,10,0)
;rslider bounds(165, 30, 60, 60), text("rate"), colour("dodgerblue"), channel("chrate"), range(0,10,0)
checkbox  bounds(.55, .40, .60, .30),channel("onoff"), text("On/Off"), value(0), colour("blue")
}

keyboard bounds(5, 220, 750, 70)

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

gifn	ftgen	1,0,16384,10,1
gifn	ftgen	2,0,16384,10,1,0,1
gifn	ftgen	3,0,16384,10,1,0,0,1
gifn	ftgen	4,0,16384,10,1,0,0,0,1
gifn	ftgen	5,0,16384,10,1,0,0,0,0,1
gifn	ftgen	6,0,16384,10,1,0,1,0,0,0,1
gifn	ftgen	7,0,16384,10,1,0,0,1,0,0,0,1
gifn	ftgen	8,0,16384,10,1,0,0,0,1,0,0,0,1
gifn	ftgen	9,0,16384,10,1,0,0,0,0,1,0,0,1,1
gifn	ftgen	10,0,16384,10,1,0,0,0,0,0,1,1,0,0,1
gifn	ftgen	11,0,16384,10,1,0,0,0,0,1,1,0,0,0,0,1
gifn	ftgen	12,0,16384,10,1,0,0,0,1,1,0,0,0,0,0,0,1
gifn	ftgen	13,0,16384,10,1,0,0,0,1,1,0,0,0,0,0,0,0,1
gifn	ftgen	14,0,16384,10,1,0,0,1,0,0,1,0,0,0,0,0,0,0,1
gifn	ftgen	15,0,16384,10,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1
gifn	ftgen	16,0,16384,10,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1
gibell ftgen 17,0,16384,9,1,1,0,1.02,.28,0,.98,.14,0,4,.22,0,7,.39,0 ;bell
gichord ftgen 18,0,16384,9, 1,1,0,1.25,.08,0,1.49,.28,0,2,.52,0,2.52,.74,0 ;chord
gitemp ftgen 19 , 0 ,  16384,    10  ,  1 ,  1 ,  0  ,  5  ,  0 ,   3,    0 ,  1
gitemp ftgen 20,  0, 16384, 10, 1, 1, 0, 0, 1 ,0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1
gitemp ftgen 21,  0 ,16384 ,10, 1, 1, 0, 1, 0, 1, 0 ,1, 0, 1, 0, 1, 0, 1, 0, 1
gitemp	ftgen 22 , 0 ,16384, 10 ,1 ,0, .2 ,.2 ,.03, .12, .22, .11 ,.022, .0101, .0167

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
kdet1	chnget	"det1"
kdet2	chnget	"det2"
kdet3	chnget	"det3"
kdet4	chnget	"det4"
klfoamp	chnget	"lfoamp"
klforate	chnget	"lforate"
kindex	chnget	"fmamp"
kfmrate	chnget	"fmrate"
kcutoff	chnget	"cutoff"
kreso	chnget	"resonance"
kx	chnget "chanx"
ky	chnget "chany"	
;midi

imidinn notnum
iamp	ampmidi	1
kbend	pchbend 0,2  ;pitch bend
kfreq1 =	cpsmidinn(imidinn+kbend+int(kdet1)) ;controllo midi
kfreq2 =	cpsmidinn(imidinn+kbend+int(kdet2)) 
kfreq3 =	cpsmidinn(imidinn+kbend+int(kdet3)) 
kfreq4 =	cpsmidinn(imidinn+kbend+int(kdet4)) 

;lfo
klfo	poscil	klfoamp,klforate,1

kfreq1	=	kfreq1+klfo
kfreq2	=	kfreq2+klfo
kfreq3	=	kfreq3+klfo
kfreq4	=	kfreq4+klfo

ifunc1	=	rnd(15)
ifunc2	=	rnd(15)
ifunc3	=	rnd(15)
ifunc4	=	rnd(15)


;4 osc
ain1	poscil	iamp,kfreq1,ifunc1+1
ain2	poscil	iamp,kfreq2,ifunc2+1
ain3	poscil	iamp,kfreq3,ifunc3+1
ain4	poscil	iamp,kfreq4,ifunc4+1


;vectorial mixing
k1	=	(1-kx)*(1-ky)	
k2	=	kx*(1-ky)
k3	=	(1-kx)*ky
k4	=	kx*ky

k1port	portk	k1,.01
k2port	portk	k2,.01
k3port	portk	k3,.01
k4port	portk	k4,.01

kadsr	mxadsr	iatt,idec,isus,irel

aout	=	(ain1*k1port + ain2*k2port + ain3*k3port + ain4*k4port)*.3	;global out

;abulp1	butterlp	aout,kcutoff*kadsrfilt

;aring	poscil	aout,kcutoff,1
;amaster	balance 	afilt1,aout

aclip	clip	aout,0,0dbfs

kpan	jspline	1,0,4
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


;f0 36000
i1	0	36000
i10	0	36000
i12	0	36000
</CsScore>
</CsoundSynthesizer>