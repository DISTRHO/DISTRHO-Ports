<Cabbage>
form size(350, 125), caption("Metalizer"), colour("black"),pluginID("meta") 
image bounds(5, 5, 340, 90), colour("black"), outline("white")
rslider bounds(15, 15, 70, 70), channel("depth"), text("Depth"), colour("lightgrey") range(0.2, 4, 0.2)
rslider bounds(95, 15, 70, 70), channel("time"), text("Time"), colour("lightgrey") range(0.0001,8,.1)
rslider bounds(170, 15, 70, 70), channel("mix"), text("Dry/Wet"), colour("lightgrey") range(0, 1, 0)
checkbox pos(260, 15) text("On/Off"), channel("on_off")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n  
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2

;Author: Giorgio Zucco (2012)
;(adapted from Metallic reverb from CsoundBook)

opcode	Metalizer,aa,aakk

asig1,asig2,kdepth,krvt	xin


klp1      =         1/8*kdepth          
klp2      =         1/13*kdepth
klp3      =         1/546*kdepth
klp4      =         1/739*kdepth
klp5      =         1/1360 *kdepth
klp6      =         1/2654*kdepth
aecho1    vcomb      asig1, krvt, klp1, 1 
aecho2    vcomb      asig2, krvt, klp2, 1
aecho1    =         asig1+aecho1
aecho2    =         asig2+aecho2
aecho3    vcomb      aecho1, krvt, klp3, 1
aecho4    vcomb      aecho1, krvt, klp4, 1
aecho5    vcomb      aecho2, krvt, klp5, 1
aecho6    vcomb      aecho2, krvt, klp6, 1
aout1     =         (aecho1+aecho3+aecho5)*.1
aout2     =         (aecho2+aecho4+aecho6)*.1


xout	aout1 ,aout2

endop

 

instr 1

kdepth chnget "depth"
ktime chnget "time"
kOnOff chnget "on_off"
kmix chnget "mix"

a1,a2	ins

if		kOnOff	=	1	then

aout1,aout2	Metalizer	a1,a2,kdepth,ktime

elseif		kOnOff	=	0	then	

aout1	=	0
aout2	=	0

endif

amix1	ntrpol	aout1,a1,1-kmix
amix2	ntrpol	aout2,a2,1-kmix

kpan1	jspline	1,1,5
kpan2	jspline 1,1,4

aleft,aright	pan2		amix1,kpan1
aleft2,aright2	pan2		amix2,kpan2

outs		(aleft+aleft2)*.5,(aright+aright2)*.5

endin




</CsInstruments>
<CsScore>

i1 0 360000
</CsScore>
</CsoundSynthesizer>