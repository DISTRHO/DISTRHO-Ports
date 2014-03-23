Convolution Reverb
------------------
You are encouraged to experiment with different impulse files.
You can find some here: http://www.openairlib.net/
The one I reference in this example can be found here: http://www.iainmccurdy.org/CsoundRealtimeExamples/SourceMaterials/Stairwell.wav
NOTE THAT ZERO LATENCY (or close to zero) CONV0LUTION WILL NOT BE POSSIBLE.

Instructions
------------
'Size Ratio' compresses the duration of the impulse file (provided that 'compressed' is chosen for 'resize' mode).
'Curve' defines the shape of an envelope applied to compressed impulse files using GEN16
	zero		=	straight line decaying
	positive vals. 	=	increasingly convex decaying
	negative vals	=	increasingly concave decaying
Increase 'iplen' (must be a power of 2) if experiencing performance problems, or reduce it to reduce latency.
'Delay OS' fine tunes the delay applied to the dry signal. Normally this should be zero but adjust it to modify how the dry signal lines up with the convoluted signal.

<Cabbage>
form caption("Convolution Reverb") size(505, 90), pluginID("Conv")
image bounds(0, 0, 505, 90), colour(135, 30, 30,220), shape("rounded"), outline(255,255,150), line(4) 
button  bounds( 10,  10, 90, 20),       text("forward","backward"),                            channel("FwdBwd"), value(0), fontcolour(250,250,250)
label   bounds( 30,  30, 60, 12),       text("direction"), fontcolour(250,250,250)
button  bounds( 10,  50, 90, 20),       text("normal","compressed"),                           channel("resize"), value(0), fontcolour(250,250,250)
label   bounds( 35,  70, 60, 12),       text("resize"), fontcolour(250,250,250)
rslider bounds(105, 11, 70, 70),        text("Size Ratio"),     colour(135, 30, 30), trackercolour(255,255,150), fontcolour(250,250,250),  channel("CompRat"),     range(0, 1.00, 1)
rslider bounds(170, 11, 70, 70),        text("Curve"),          colour(135, 30, 30), trackercolour(255,255,150), fontcolour(250,250,250),  channel("Curve"),       range(-8.00, 8.00, 0)
rslider bounds(235, 11, 70, 70),        text("In Skip"),        colour(135, 30, 30), trackercolour(255,255,150), fontcolour(250,250,250),  channel("skipsamples"), range(0, 1.00, 0)
rslider bounds(300, 11, 70, 70),        text("Del.OS."),        colour(135, 30, 30), trackercolour(255,255,150), fontcolour(250,250,250),  channel("DelayOS"),     range(-1.00, 1.00, 0)
rslider bounds(365, 11, 70, 70),        text("Mix"),            colour(135, 30, 30), trackercolour(255,255,150), fontcolour(250,250,250),  channel("mix"),         range(0, 1.00, 0.25)
rslider bounds(430, 11, 70, 70),        text("Level"),          colour(135, 30, 30), trackercolour(255,255,150), fontcolour(250,250,250),  channel("level"),       range(0, 1.00, 0.4)
}
</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-d -n
</CsOptions>

<CsInstruments>

sr 		= 	44100	;SAMPLE RATE
ksmps 		= 	32	;NUMBER OF AUDIO SAMPLES IN EACH CONTROL CYCLE
nchnls 		= 	2	;NUMBER OF CHANNELS (2=STEREO)
0dbfs		=	1

;Author: Iain McCurdy (2012)

; IMPULSE RESPONSES STORED AS STEREO GEN01 FUNCTION TABLES
giImpulse     ftgen   1, 0, 262144, 1, "stairwell.wav", 0, 0, 0	;stereo file, duration 1.176s

; reverse function table UDO
opcode	tab_reverse,i,i
ifn             xin
iTabLen         =               nsamp(ifn)
iTableRev       ftgen           ifn+100,0,-iTabLen,-2, 0
icount          =               0
loop:
ival            table           iTabLen-icount-1, ifn
                tableiw         ival,icount,iTableRev
                loop_lt         icount,1,iTabLen,loop
                xout   	        iTableRev
endop

; compress function table UDO
opcode	tab_compress,i,iii
ifn,iCompRat,iCurve    xin
iTabLen         =               nsamp(ifn)
iTabLenComp     =               int(nsamp(ifn)*iCompRat)
iTableComp      ftgen           ifn+200,0,-iTabLenComp,-2, 0
iAmpScaleTab	ftgen		ifn+300,0,-iTabLenComp,-16, 1,iTabLenComp,iCurve,0
icount          =               0
loop:
ival            table           icount, ifn
iAmpScale	table		icount, iAmpScaleTab
                tableiw         ival*iAmpScale,icount,iTableComp
                loop_lt         icount,1,iTabLenComp,loop
                xout   	        iTableComp
endop

; compress reverse function table UDO
opcode	tab_compress_rev,i,iii
ifn,iCompRat,iCurve    xin
iTabLen         =               nsamp(ifn)
iTabLenComp     =               int(nsamp(ifn)*iCompRat)
iTableComp      ftgen           ifn+400,0,-iTabLenComp,-2, 0
iAmpScaleTab	ftgen		ifn+500,0,-iTabLenComp,-16, 1,iTabLenComp,iCurve,0
icount          =               0
loop:
ival            table           icount, ifn
iAmpScale	table		icount, iAmpScaleTab
                tableiw         ival*iAmpScale, iTabLenComp-icount-1,iTableComp
                loop_lt         icount,1,iTabLenComp,loop
                xout   	        iTableComp
endop

instr	1	;CONVOLUTION REVERB INSTRUMENT
	kFwdBwd		chnget	"FwdBwd"
	kresize		chnget	"resize"
	kmix		chnget	"mix"
	klevel		chnget	"level"
	kCompRat	chnget	"CompRat"
	kCurve		chnget	"Curve"
	kskipsamples	chnget	"skipsamples"
	kDelayOS	chnget	"DelayOS"
	kCompRat       init	1 			;IF THIS IS LEFT UNINITIALISED A CRASH WILL OCCUR! 
	
	ainL,ainR	ins				;READ STEREO AUDIO INPUT
	;ainL,ainR	diskin2	"808loop.wav",1,0,1	;USE A SOUND FILE FOR TESTING
	ainMix		sum	ainL,ainR
	
	;CREATE REVERSED TABLES
	irev	tab_reverse	giImpulse

	kSwitch	changed		kskipsamples,kFwdBwd,kDelayOS,kCompRat,kCurve	;GENERATE A MOMENTARY '1' PULSE IN OUTPUT 'kSwitch' IF ANY OF THE SCANNED INPUT VARIABLES CHANGE. (OUTPUT 'kSwitch' IS NORMALLY ZERO)
	if	kSwitch=1	then			;IF I-RATE VARIABLE IS CHANGED...
		reinit	UPDATE				;BEGIN A REINITIALISATION PASS IN ORDER TO EFFECT THIS CHANGE. BEGIN THIS PASS AT LABEL ENTITLED 'UPDATE' AND CONTINUE UNTIL rireturn OPCODE 
	endif						;END OF CONDITIONAL BRANCHING
	UPDATE:						;LABEL
	;CREATE COMPRESSED TABLES
	icomp	tab_compress	giImpulse,i(kCompRat),i(kCurve)

	;CREATE COMPRESSED REVERSED TABLES
	icomprev	tab_compress_rev	giImpulse,i(kCompRat),i(kCurve)

	iplen		=	1024				;BUFFER LENGTH (INCREASE IF EXPERIENCING PERFORMANCE PROBLEMS, REDUCE IN ORDER TO REDUCE LATENCY)
	itab		=	giImpulse			;DERIVE FUNCTION TABLE NUMBER OF CHOSEN TABLE FOR IMPULSE FILE
	iirlen		=	nsamp(itab)*0.5			;DERIVE THE LENGTH OF THE IMPULSE RESPONSE IN SAMPLES. DIVIDE BY 2 AS TABLE IS STEREO.
	iskipsamples	=	nsamp(itab)*0.5*i(kskipsamples)	;DERIVE INSKIP INTO IMPULSE FILE. DIVIDE BY 2 (MULTIPLY BY 0.5) AS ALL IMPULSE FILES ARE STEREO
	
	;FORWARDS REVERB
	if kFwdBwd==0&&kresize==0 then
	 aL,aR	ftconv	ainMix, itab, iplen,iskipsamples, iirlen		;CONVOLUTE INPUT SOUND
	 adelL	delay	ainL, abs((iplen/sr)+i(kDelayOS)) 	;DELAY THE INPUT SOUND ACCORDING TO THE BUFFER SIZE
	 adelR	delay	ainR, abs((iplen/sr)+i(kDelayOS)) 	;DELAY THE INPUT SOUND ACCORDING TO THE BUFFER SIZE
        
        ;BACKWARDS REVERB
        elseif kFwdBwd==1&&kresize==0 then
	 aL,aR	ftconv	ainMix, irev, iplen, iskipsamples, iirlen				;CONVOLUTE INPUT SOUND
	 adelL	delay	ainL,abs((iplen/sr)+(iirlen/sr)-(iskipsamples/sr)+i(kDelayOS))	;DELAY THE INPUT SOUND ACCORDING TO THE BUFFER SIZE AND THE DURATION OF THE IMPULSE FILE
	 adelR	delay	ainR,abs((iplen/sr)+(iirlen/sr)-(iskipsamples/sr)+i(kDelayOS))	;DELAY THE INPUT SOUND ACCORDING TO THE BUFFER SIZE AND THE DURATION OF THE IMPULSE FILE

	elseif kFwdBwd==0&&kresize==1 then
	 aL,aR	ftconv	ainMix, icomp, iplen,iskipsamples, iirlen*i(kCompRat)		;CONVOLUTE INPUT SOUND
	 adelL	delay	ainL, abs((iplen/sr)+i(kDelayOS)) 				;DELAY THE INPUT SOUND ACCORDING TO THE BUFFER SIZE
	 adelR	delay	ainR, abs((iplen/sr)+i(kDelayOS)) 				;DELAY THE INPUT SOUND ACCORDING TO THE BUFFER SIZE

	elseif kFwdBwd==1&&kresize==1 then
	 aL,aR	ftconv	ainMix, icomprev, iplen, iskipsamples, iirlen*i(kCompRat)		;CONVOLUTE INPUT SOUND
	 adelL	delay	ainL,abs((iplen/sr)+((iirlen*i(kCompRat))/sr)-(iskipsamples/sr)+i(kDelayOS))	;DELAY THE INPUT SOUND ACCORDING TO THE BUFFER SIZE AND THE DURATION OF THE IMPULSE FILE
	 adelR	delay	ainR,abs((iplen/sr)+((iirlen*i(kCompRat))/sr)-(iskipsamples/sr)+i(kDelayOS))	;DELAY THE INPUT SOUND ACCORDING TO THE BUFFER SIZE AND THE DURATION OF THE IMPULSE FILE
	endif

	; CREATE A DRY/WET MIX
	aMixL	ntrpol	adelL,aL*0.1,kmix
	aMixR	ntrpol	adelR,aR*0.1,kmix
        	outs	aMixL*klevel,aMixR*klevel
endin
		
</CsInstruments>

<CsScore>
i 1 0 3600	;INSTRUMENT 2 (CONVOLUTION INSTRUMENT) RUNS FOR 1 HOUR (AND KEEPS REAL-TIME PERFORMANCE GOING) 
</CsScore>

</CsoundSynthesizer>
