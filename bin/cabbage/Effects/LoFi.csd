<Cabbage>
form size(230, 120), caption("Lo Fi"), pluginID("lofi")
image pos(-40,-10), size(300,300), colour(0,  130,230,0), shape("ellipse"), outline(200, 30,100, 80), line(70)
image pos(  0,  0), size(300,300), colour(200, 30, 30,0), shape("ellipse"), outline(  0,130, 30, 80), line(70)
image pos( 40, 10), size(300,300), colour(250,  0, 30,0), shape("ellipse"), outline(  0,  0, 30, 50), line(70)
image pos( 80, 20), size(300,300), colour(  0,  0,255,0), shape("ellipse"), outline(  0,  0,255, 50), line(70)
image pos(120, 30), size(300,300), colour(  0,255,255,0), shape("ellipse"), outline(  0,255,255, 50), line(70)
image pos(160, 40), size(300,300), colour(255,255,  0,0), shape("ellipse"), outline(255,255,  0, 50), line(70)
image pos(0, 0), size(230, 120), colour("Chocolate"), shape("rounded"), outline("white"), line(5)
rslider bounds(  5, 31, 80, 80), text("Bits"), channel("bits"), range(1, 16, 16), fontcolour("White"),colour("orange"), trackercolour("brown")
rslider bounds( 75,  7, 80, 80), text("Foldover"), channel("fold"), range(1, 1024, 0, 0.25), fontcolour("White"),colour("orange"), trackercolour("brown")
rslider bounds(145, 31, 80, 80), text("Level"), channel("level"), range(0, 1.00, 1), fontcolour("White"),colour("orange"), trackercolour("brown")
}
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

;Iain McCurdy, 2012
;http://iainmccurdy.org/csound.html


opcode	LoFi,a,akk
	ain,kbits,kfold	xin									;READ IN INPUT ARGUMENTS
	kvalues		pow		2, kbits					;RAISES 2 TO THE POWER OF kbitdepth. THE OUTPUT VALUE REPRESENTS THE NUMBER OF POSSIBLE VALUES AT THAT PARTICULAR BIT DEPTH
	aout		=	(int((ain/0dbfs)*kvalues))/kvalues	;BIT DEPTH REDUCE AUDIO SIGNAL
	aout		fold 	aout, kfold							;APPLY SAMPLING RATE FOLDOVER
		xout	aout									;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr 1
kbits chnget "bits"
kfold chnget "fold"
klevel chnget "level"
a1,a2	ins
kporttime	linseg	0,0.001,0.01
kfold	portk	kfold,kporttime
a1	LoFi	a1,kbits*0.6,kfold
a2	LoFi	a2,kbits*0.6,kfold
a1	=	a1 * klevel
a2	=	a2 * klevel
	outs	a1,a2
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>