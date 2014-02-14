<Cabbage>
form caption("Additive synthesizer") size(562, 340), colour("black"),pluginID("add1") 
rslider bounds(15, 10, 100, 100) channel("speed"), range(1,10,2), caption("Speed"), colour("lime ")
rslider bounds(120, 10, 100, 100) channel("semi"), range(-12,12,0), caption("Coarse"), colour("lime ")
rslider bounds(225, 10, 100, 100) channel("partials"), range(3,8,4), caption("Partials"), colour("lime ")
rslider bounds(330, 10, 100, 100) caption("Vib Amp"), colour("lime "), channel("lfoamp"), range(0,20, 0)
rslider bounds(435, 10, 100, 100) caption("Vib Rate"), colour("lime "), channel("lforate"), range(0,20, 0)

groupbox bounds(15, 120, 240, 100), text("ADSR amplitude"), plant("ADSR"){ 
rslider bounds(0, .3, .6, .6), text("A"), colour("lime"), channel("att"), range(0.01,3, .5)
rslider bounds(.25, .3, .6, .6), text("D"), colour("lime "), channel("dec"), range(0,1, .5)
rslider bounds(.5, .3, .6, .6), text("S"), colour("lime "), channel("sus"), range(0,1,.8)
rslider bounds(.75, .3, .6, .6), text("R"), colour("lime "), channel("rel"), range(0.01,3, .2)
}
groupbox bounds(285, 120, 250, 100), text("Filter/Reverb"), plant("filtrev"){ 
rslider bounds(0, .3, .6, .6), text("cutoff"), colour("lime "), channel("cutoff"), range(200,8000,1000)
rslider bounds(.25, .3, .6, .6),text("level"), colour("lime "), channel("level"), range(0,1,.7)
rslider bounds(.5, .3, .6, .6), text("size"), colour("lime "), channel("size"), range(.1,.99,.8)
checkbox  bounds(.75, .40, .60, .30),channel("onoff"), text("ON"), value(0)
}

keyboard pos(10, 240), size(540, 60)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -M0 -b1024  -m0d
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

;Author: Giorgio Zucco (2012)
garev	init	0


instr	1

;channel
kspeed	chnget	"speed"
ksemi1	chnget	"semi"
ktable	chnget	"partials"
iatt	chnget	"att"
idec	chnget	"dec"
isus	chnget	"sus"
irel	chnget	"rel"
klfoamp	chnget	"lfoamp"
klforate	chnget	"lforate"
kcutoff	chnget	"cutoff"

;midi
imidinn notnum
iamp	ampmidi	1
kbend	pchbend 0,2  ;pitch bend
kbasefreq =	cpsmidinn(imidinn+kbend+int(ksemi1)) ;controllo midi

;vibrato
klfo	vibr	klfoamp,klforate,1

;additive
kpartial	init	3
iamptab	=	2	;tabella ampiezze 
kpartial	=	int(ktable)	;tabella parziali

ktrig	changed	kpartial
if	ktrig	=	1	then
reinit	play
endif
play:

k1	randomh	1,7,kspeed	;indice della tabella letto in modo casuale
k2	randomh	1,7,kspeed-.9	;simile ad un arpeggio di armonici con rate indipendente
k3	randomh	1,7,kspeed-.8
k4	randomh	1,7,kspeed-.7
k5	randomh	1,7,kspeed-.6
k6	randomh	1,7,kspeed-.5
k7	randomh	1,7,kspeed-.4

;tabella delle parziali
kfreqtable1	table	k1,i(kpartial)
kfreqtable2	table	k2,i(kpartial)
kfreqtable3	table	k3,i(kpartial)
kfreqtable4	table	k4,i(kpartial)
kfreqtable5	table	k5,i(kpartial)
kfreqtable6	table	k6,i(kpartial)
kfreqtable7	table	k7,i(kpartial)

;tabella ampiezze
iamptable1	table	1,iamptab
iamptable2	table	2,iamptab
iamptable3	table	3,iamptab
iamptable4	table	4,iamptab
iamptable5	table	5,iamptab
iamptable6	table	6,iamptab
iamptable7	table	7,iamptab

;otto oscillatori 
a1	poscil	iamp*2,(kbasefreq+klfo),1	;frequenza fondamentale
a2	poscil	iamp*iamptable1,((kbasefreq+klfo)+klfo)*kfreqtable1,1
a3	poscil	iamp*iamptable2,(kbasefreq+klfo)*kfreqtable2,1
a4	poscil	iamp*iamptable3,(kbasefreq+klfo)*kfreqtable3,1
a5	poscil	iamp*iamptable4,(kbasefreq+klfo)*kfreqtable4,1
a6	poscil	iamp*iamptable5,(kbasefreq+klfo)*kfreqtable5,1
a7	poscil	iamp*iamptable6,(kbasefreq+klfo)*kfreqtable6,1
a8	poscil	iamp*iamptable7,(kbasefreq+klfo)*kfreqtable7,1


asum	sum	a1,a2,a3,a4,a5,a6,a7,a8 ;somma degli oscillatori

afilt	clfilt	asum/8,kcutoff,0,4
aout	clip	afilt,0,0dbfs
kenv	mxadsr	iatt,idec,isus,irel

outs	aout*kenv,aout*kenv ;uscita con inviluppo
vincr	garev,aout*kenv

endin

instr	3	;reverb
ktrig	chnget	"onoff"
klevel	chnget	"level"
ksize	chnget	"size"
if	ktrig	=	1	then
	
al,ar	reverbsc	garev,garev,ksize,16000,sr,0
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
clear	garev
endin


</CsInstruments>
<CsScore>
f1	0	16384	10	1
f2	0	-7	-2	.9	.8	.7	.6	.5	.4	.3	.2	;ampiezze parziali

f3	0	-7	-2	1	1.1	1.2	1.3	1.4	1.5	1.6	1.7	;parziali
f4	0	-7	-2	1	2	3	4	5	6	7	8
f5	0	-7	-2	1	1.2	1.4	1.6	1.8	2	2.4	2.6	
f6	0	-7	-2	1	2	4	5	7	8	10	11	
f7	0	-7	-2	1	1.3	1.5	1.7	1.9	2.11	3.13	4.15	
f8	0	-7	-2	1	1	3	5	7	9	11	13

i1	0	36000
i3	0	36000
</CsScore>
</CsoundSynthesizer>