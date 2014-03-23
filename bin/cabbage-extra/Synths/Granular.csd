<Cabbage>
form caption("Granular synthesizer") size(428, 220), colour("black"), pluginID("gra1")
rslider bounds(0, 0, 80, 80) channel("random"), range(1, 2000, 200), caption("Random freq"), colour("white"), midictrl(1, 1)
rslider bounds(85, 0, 80, 80) channel("size"), range(0.005,.2, .02), caption("Grain size"), colour("white"), midictrl(1, 2)
rslider bounds(170, 0, 80, 80) channel("density"), range(1, 10, 1), caption("Density"), colour("white"), midictrl(1, 3)
rslider bounds(255, 0, 80, 80) channel("reverb"), range(0, 1, 0), caption("Reverb"), colour("white"), midictrl(1, 4)
rslider bounds(340, 0, 80, 80) channel("freq"), range(0,30, 0), caption("Flanger"), colour("white"), midictrl(1, 5)

combobox bounds(50, 90, 160, 20), channel("func1"), value(14), colour("darkgrey"), text("wt1","wt2","wt3","wt4","wt5","wt6","wt7","wt8","wt9","wt10","wt11","wt12","wt13","wt14")
combobox bounds(220, 90, 160, 20), channel("func2"), value(5), colour("darkgrey"), text("wt1","wt2","wt3","wt4","wt5","wt6","wt7","wt8","wt9","wt10","wt11","wt12","wt13","wt14")


keyboard pos(0, 130), size(420, 50)

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=NULL -M0 --midi-key-cps=5 --midi-velocity-amp=4
;-n -d -+rtmidi=NULL -M0
</CsOptions>
<CsInstruments>

;Author: Giorgio Zucco (2012)


sr = 44100
ksmps = 64
nchnls = 2
0dbfs = 1

gasig1 init 0
gasig2 init 0

giadd ftgen 14,0,8193,9,1,1,0,1.02,.28,0,.98,.14,0,4,.22,0,7,.39,0 ;bell
gihanning	ftgen	200,0,8192,-20,2	;Hanning
gibarlett	ftgen	300,0,8192,-20,3	;Bartlett (Triangle)



instr	1


gkfn1	chnget	"func1";grain waveform
gkfn2	chnget	"func2";grain waveform
;grain 
kcps	=	p5*.5	;grain freq
kfmd	chnget		"random";variazione random in Hz. 
kgdur	chnget		"size"	;durata dei grani in secondi
iovrlp	chnget		"density"	;numero di grani sovrapposti
kfreq	chnget	"freq"

iwfn1	=	200	;inviluppo a1
iwfn2	=	300	;inviluppo a2

;adsr
iatt	=	0.2
idec	=	1
isus	=	1
irel	=	.5

a1	grain2	kcps,kfmd,kgdur,iovrlp,gkfn1,iwfn1
a2	grain2	kcps*2,kfmd,kgdur,iovrlp,gkfn2,iwfn2

adel1	randh	.009,kfreq
adel2	randh	.009,kfreq
klfo	poscil	.01,.1,4	;Lfo
afl1	flanger	a1,adel1+klfo,.9
afl2	flanger	a2,adel2+klfo,.9

kadsr	mxadsr	iatt,idec,isus,irel

;aleft	clip    (a1+afl1)*.1,2,.8
;aright	clip    (a2+afl2)*.1,2,.8

;al1	= (a1+afl1)
;ar1	= (a2+afl2)
;aleft	clip    al1*.1,2,.8
;aright	clip    ar1*.1,2,.8

; Or using function syntax
aleft	= clip    ((a1+afl1)*.1,2,.8)
aright	= clip    ((a2+afl2)*.1,2,.8)

outs	(aleft)*kadsr,(aright)*kadsr

vincr	gasig1,(aleft)*kadsr
vincr	gasig2,(aright)*kadsr
endin

instr	3
klevel	chnget	"reverb"
al,ar	reverbsc	gasig1*.5,gasig2*.5,.85,16000,sr,0
kpan1	randomi	0,1,4
kpan2	randomi	0,1,8
aout1,aout2	pan2	al,kpan1
aout3,aout4	pan2	ar,1-kpan2
outs	(aout1+aout2)*klevel,(aout3+aout4)*klevel
clear	gasig1,gasig2
endin


</CsInstruments>
<CsScore>
f1	0	1024	10	1
f2 0 4096 10 1 0 .111 0 .04 0 .02 0 .012 ;Triangular1
f3 0 4096 10 1 0 .111 0 .04 0 .02 0 .012 0  .009  0  .007;Triangular 2
f5 0 4096 10 1 0 0.3 0  0.2 0  0.14 0 0.1 0 0.09 0 0.07 0 0.06 0 0.05   ;square
f4 0 4096 10 1 0  .333333 0   .2 0       .142857  0    .111111 0  .090909 0       .076923 ; Square
f6 0   4096 10 1 1 1 1 .7 .5 .3 .1 		;pulse

f7  0 4096  10  1 0 0 1;sine+4
f8  0 4096  10  1 0 0 0 1;sine+5
f9  0 4096  10  1 0 0 0 0 1;sine+6
f10  0 4096  10  1 0 0 0 0 0 1;sine+7
f11  0 4096  10  1 0 0 0 0 0 0 1;sine+8
f12  0 4096  10  1 0 0 0 0 0 0 0 1;sine+9
f13  0 4096  10  1 0 0 0 0 0 0 0 0 1;sine+10


f0	36000

i3	0	36000
</CsScore>
</CsoundSynthesizer>
