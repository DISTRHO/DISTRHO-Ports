<Cabbage>
form caption("Inharmonic Synth"), size(445, 320), pluginID("InSy")
image pos(0, 0), size(445, 290), colour("black"), shape("rounded"), outline("brown"), line(4)
combobox caption("Instrument"), channel("Instr"),  pos(275, 10), size(160, 90), value(4), text("Bass Guitar", "Dahina", "Banyan", "Xylophone", "Tibetan Bowl 180mm", "Spinel Sphere", "Pot Lid", "Red Cedar Wood Plate", "Tubular Bell", "Redwood Wood Plate", "Douglas Fir Wood Plate", "Uniform Wooden Bar", "Uniform Aluminium Bar", "Vibraphone 1", "Vibraphone 2", "Chalandi Plates", "Tibetan Bowl 152mm", "Tibetan Bowl 140mm", "Wine Glass", "Small Handbell", "Albert Clock Bell", "Wood Block")

groupbox bounds(10, 10, 260, 90), text("Filter Envelope"){
rslider bounds( 10, 35,60,60), text("Amount"), colour("cornflowerblue"), channel("FAmt"), range(0, 1, 0.7)
rslider bounds( 60, 35,60,60), text("Dec."), colour("cornflowerblue"), channel("FDec"), range(0, 1, 0.45)
rslider bounds(110, 35,60,60), text("Sus."), colour("cornflowerblue"), channel("FSus"), range(0, 1, 0)
rslider bounds(160, 35,60,60), text("Rel."), colour("cornflowerblue"), channel("FRel"), range(0, 1, 0.45)
rslider bounds(210, 35,60,60), text("Shape"), channel("FShp"), colour("cornflowerblue"), range(-50, 50, -16)
}

groupbox bounds(10, 105, 210, 90), text("Amplitude Envelope"){
rslider bounds( 10,130,60,60), text("Att."), channel("AAtt"), colour("red"), range(0, 1, 0)
rslider bounds( 60,130,60,60), text("Dec."), channel("ADec"), colour("red"),range(0, 1, 0.45)
rslider bounds(110,130,60,60), text("Sus"), channel("ASus"), colour("red"),range(0, 1, 0)
rslider bounds(160,130,60,60), text("Rel"), channel("ARel"), colour("red"),range(0, 1, 0.45)
}

groupbox bounds(225, 105, 210, 90), text("Chorus"){
checkbox channel("ChoOnOff"), pos(235,135), size(40, 43), value(1)
label    bounds(235,178, 40, 11), text("On/Off"), colour("black")
rslider  bounds(275,130,60,60), text("Mix"), channel("ChoMix"), range(0, 1, 0.5)
rslider  bounds(325,130,60,60), text("Depth"), channel("ChoDep"), range(0, 0.1, 0.01,0.5, 0.001)
rslider  bounds(375,130,60,60), text("Rate"), channel("ChoRte"), range(0, 20, 0.96, 0.5)
}

keyboard pos(10, 200), size(425, 80)
image bounds(5, 295, 240, 22), colour(75, 85, 90, 100), plant("credit"){
label bounds(0.03, 0.15, .9, .7), text("Author: Iain McCurdy |2012|"), colour("white")
</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-dm0 -n -+rtmidi=null -M0
</CsOptions>

<CsInstruments>

sr 		= 	44100
ksmps 		= 	64
nchnls 		= 	2
0dbfs		=	1	;MAXIMUM AMPLITUDE
massign	3,2

;Author: Iain McCurdy (2012)

;FUNCTION TABLES STORING MODAL FREQUENCY RATIOS===================================================================================================================================
;plucked string
girtos1		ftgen	0,0,-20, -2, 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
;dahina
girtos2		ftgen	0,0,-6,-2, 1, 2.89, 4.95, 6.99, 8.01, 9.02
;banyan
girtos3		ftgen	0,0,-6,-2, 1, 2.0, 3.01, 4.01, 4.69, 5.63
;xylophone
girtos4		ftgen	0,0,-6,-2, 1, 3.932, 9.538, 16.688, 24.566, 31.147
;tibetan bowl (180mm)
girtos5		ftgen	0,0,-7,-2, 1, 2.77828, 5.18099, 8.16289, 11.66063, 15.63801, 19.99
;spinel sphere with diameter of 3.6675mm
girtos6		ftgen	0,0,-24,-2, 1, 1.026513174725, 1.4224916858532, 1.4478690202098, 1.4661959580455, 1.499452545408, 1.7891839345101, 1.8768994627782, 1.9645945254541, 1.9786543873113, 2.0334612432847, 2.1452852391916, 2.1561524686621, 2.2533435661294, 2.2905090816065, 2.3331798413917, 2.4567715528268, 2.4925556408289, 2.5661806088514, 2.6055768738808, 2.6692760296751, 2.7140956766436, 2.7543617293425, 2.7710411870043 
;pot lid
girtos7		ftgen	0,0,-6,-2, 1, 3.2, 6.23, 6.27, 9.92, 14.15
;red cedar wood plate
girtos8		ftgen	0,0,-4,-2, 1, 1.47, 2.09, 2.56
;tubular bell
girtos9		ftgen	0,0,-10,-2, 272/437, 538/437, 874/437, 1281/437, 1755/437, 2264/437, 2813/437, 3389/437, 4822/437, 5255/437
;redwood wood plate
girtos10	ftgen	0,0,-4,-2, 1, 1.47, 2.11, 2.57
;douglas fir wood plate
girtos11	ftgen	0,0,-4,-2, 1, 1.42, 2.11, 2.47
;uniform wooden bar
girtos12	ftgen	0,0,-6,-2, 1, 2.572, 4.644, 6.984, 9.723, 12
;uniform aluminum bar
girtos13	ftgen	0,0,-6,-2, 1, 2.756, 5.423, 8.988, 13.448, 18.680
;vibraphone 1
girtos14	ftgen	0,0,-6,-2, 1, 3.984, 10.668, 17.979, 23.679, 33.642
;vibraphone 2
girtos15	ftgen	0,0,-6,-2, 1, 3.997, 9.469, 15.566, 20.863, 29.440
;Chalandi plates
girtos16	ftgen	0,0,-5,-2, 1, 1.72581, 5.80645, 7.41935, 13.91935
;tibetan bowl (152 mm)
girtos17	ftgen	0,0,-7,-2, 1, 2.66242, 4.83757, 7.51592, 10.64012, 14.21019, 18.14027
;tibetan bowl (140 mm)	
girtos18	ftgen	0,0,-5,-2, 1, 2.76515, 5.12121, 7.80681, 10.78409
;wine glass
girtos19	ftgen	0,0,-5,-2, 1, 2.32, 4.25, 6.63, 9.38
;small handbell
girtos20	ftgen	0,0,-22,-2, 1, 1.0019054878049, 1.7936737804878, 1.8009908536585, 2.5201981707317, 2.5224085365854, 2.9907012195122, 2.9940548780488, 3.7855182926829, 3.8061737804878, 4.5689024390244, 4.5754573170732, 5.0296493902439, 5.0455030487805, 6.0759908536585, 5.9094512195122, 6.4124237804878, 6.4430640243902, 7.0826219512195, 7.0923780487805, 7.3188262195122, 7.5551829268293 
;albert clock bell belfast
girtos21	ftgen	0,0,-22,-2, 2.043260,1.482916,1.000000,3.328848,4.761811,1.477056,0.612007,2.661295,1.002793,4.023776,0.254139,2.043916,4.032463,2.659438,4.775560,5.500494,3.331014,0.809697,2.391301, 0.254098,1.901476,2.366563    ;,0.614968,2.046543,1.814887,3.130744,2.484426,0.558874,0.801697,0.070870,3.617036,2.782656
;wood block
girtos22	ftgen	0,0,4,-2,	915/915,1540/915,1863/915,3112/915
;=================================================================================================================================================================================

;=================================================================================================================================================================================
;GEN09 FUNCTION TABLE VERSIONS OF THE MODAL FREQUENCY TABLES
;THESE AREN'T ACTUALLY NEEDED AS THE TABLES ARE CALCULATED IN INSTRUMENT 2 BUT IF INSTRUMENT 2 IS OMITTED THESE TABLES WILL BE NEEDED
;NOTE THAT PARTIAL NUMBER HAVE BEEN MULTIPLIED BY giRtosScale AND THAT THE FREQUENCY OF ANY OSCILLATOR THAT USES THESE TABLES WILL HAVE TO BE DIVIDED BY THE SAME NUMBER 
giwave1		ftgen	0, 0, 262144, 9, 1000,1.000,0,	 2000,1.000,0,	 3000,1.000,0,	 4000,1.000,0,	 5000,1.000,0,	 6000,1.000,0,	 7000,1.000,0,	 8000,1.000,0,	 9000,1.000,0,	 10000,1.000,0,	 11000,1.000,0,	 12000,1.000,0,	 13000,1.000,0,	 14000,1.000,0,	 15000,1.000,0,	 16000,1.000,0,	 17000,1.000,0,	 18000,1.000,0,	 19000,1.000,0,	 20000,1.000,0
giwave2		ftgen	0, 0, 262144, 9, 1000,1.000,0,	 2890,1.000,0,	 4950,1.000,0,	 6990,1.000,0,	 8010,1.000,0,	 9020,1.000,0
giwave3		ftgen	0, 0, 262144, 9, 1000,1.000,0,	 2000,1.000,0,	 3010,1.000,0,	 4010,1.000,0,	 4690,1.000,0,	 5630,1.000,0	 
giwave4		ftgen	0, 0, 262144, 9, 1000,1.000,0,	 3932,1.000,0,	 9538,1.000,0,	 16688,1.000,0,	 24566,1.000,0,	 31147,1.000,0
giwave5		ftgen	0, 0, 262144, 9, 1000,1.000,0,	 2778,0.500,0,	 5180,0.250,0,	 8162,0.125,0,	 11660,0.062,0,	 15638,0.031,0,	 19990,0.016,0
giwave6		ftgen	0, 0, 262144, 9, 1000,1.000,0,	 1026,1.000,0,	 1422,1.000,0,	 1447,1.000,0,	 1466,1.000,0,	 1499,1.000,0,	 1789,1.000,0,	 1876,1.000,0,	 1964,1.000,0,	 1978,1.000,0,	 2033,1.000,0,	 2145,1.000,0,	 2156,1.000,0,	 2253,1.000,0,	 2290,1.000,0,	 2333,1.000,0,	 2456,1.000,0,	 2492,1.000,0,	 2566,1.000,0,	 2605,1.000,0,	 2669,1.000,0,	 2714,1.000,0
giwave7		ftgen	0, 0, 262144, 9, 1000,1.000,0,	 3200,1.000,0,	 6230,1.000,0,	 6270,1.000,0,	 9920,1.000,0,	 14150,1.000,0
giwave8		ftgen	0, 0, 262144, 9, 1000,1.000,0,	 1470,1.000,0,	 2090,1.000,0,	 2560,1.000,0
giwave9		ftgen	0, 0, 262144, 9, 622,1.000,0,	 1231,1.000,0,	 2000,1.000,0,	 2931,1.000,0,	 4016,1.000,0,	 5180,1.000,0,	 6437,1.000,0,	 7755,1.000,0,	 11034,1.000,0,	 12025,1.000,0
giwave10	ftgen	0, 0, 262144, 9, 1000,1.000,0,	 1470,1.000,0,	 2110,1.000,0,	 2570,1.000,0
giwave11	ftgen	0, 0, 262144, 9, 1000,1.000,0,	 1420,1.000,0,	 2110,1.000,0,	 2470,1.000,0
giwave12	ftgen	0, 0, 262144, 9, 1000,1.000,0,	 2572,1.000,0,	 4644,1.000,0,	 6984,1.000,0,	 9723,1.000,0,	 12000,1.000,0
giwave13	ftgen	0, 0, 262144, 9, 1000,1.000,0,	 2756,1.000,0,	 5423,1.000,0,	 8988,1.000,0,	 13448,1.000,0,	 18680,1.000,0
giwave14	ftgen	0, 0, 262144, 9, 1000,1.000,0,	 3984,1.000,0,	 10668,1.000,0,	 17979,1.000,0,	 23679,1.000,0,	 33642,1.000,0
giwave15	ftgen	0, 0, 262144, 9, 1000,1.000,0,	 3997,1.000,0,	 9469,1.000,0,	 15566,1.000,0,	 20863,1.000,0,	 29440,1.000,0
giwave16	ftgen	0, 0, 262144, 9, 1000,1.000,0,	 1725,1.000,0,	 5806,1.000,0,	 7419,1.000,0,	 13919,1.000,0
giwave17	ftgen	0, 0, 262144, 9, 1000,1.000,0,	 2662,1.000,0,	 4837,1.000,0,	 7515,1.000,0,	 10640,1.000,0,	 14210,1.000,0,	 18140,1.000,0
giwave18	ftgen	0, 0, 262144, 9, 1000,1.000,0,	 2765,1.000,0,	 5121,1.000,0,	 7806,1.000,0,	 10784,1.000,0
giwave19	ftgen	0, 0, 262144, 9, 1000,1.000,0,	 2320,1.000,0,	 4250,1.000,0,	 6630,1.000,0,	 9380,1.000,0
giwave20	ftgen	0, 0, 262144, 9, 1000,1.000,0,	 1001,0.833,0,	 1793,0.694,0,	 1800,0.579,0,	 2520,0.482,0,	 2522,0.402,0,	 2990,0.335,0,	 2994,0.279,0,	 3785,0.233,0,	 3806,0.194,0,	 4568,0.162,0,	 4575,0.135,0,	 5029,0.112,0,	 5045,0.093,0,	 6075,0.078,0,	 5909,0.065,0,	 6412,0.054,0,	 6443,0.045,0,	 7082,0.038,0,	 7092,0.031,0,	 7318,0.026,0,	 7555,0.022,0
giwave21	ftgen	0, 0, 262144, 9, 1000,1.000,0,	 1001,0.833,0,	 1793,0.694,0,	 1800,0.579,0,	 2520,0.482,0,	 2522,0.402,0,	 2990,0.335,0,	 2994,0.279,0,	 3785,0.233,0,	 3806,0.194,0,	 4568,0.162,0,	 4575,0.135,0,	 5029,0.112,0,	 5045,0.093,0,	 6075,0.078,0,	 5909,0.065,0,	 6412,0.054,0,	 6443,0.045,0,	 7082,0.038,0,	 7092,0.031,0,	 7318,0.026,0,	 7555,0.022,0
giwave22	ftgen	0, 0, 262144, 9, 1000,1.000,0,	 1001,0.833,0,	 1793,0.694,0,	 1800,0.579,0,	 2520,0.482,0,	 2522,0.402,0,	 2990,0.335,0,	 2994,0.279,0,	 3785,0.233,0,	 3806,0.194,0,	 4568,0.162,0,	 4575,0.135,0,	 5029,0.112,0,	 5045,0.093,0,	 6075,0.078,0,	 5909,0.065,0,	 6412,0.054,0,	 6443,0.045,0,	 7082,0.038,0,	 7092,0.031,0,	 7318,0.026,0,	 7555,0.022,0
;=================================================================================================================================================================================

gisine		ftgen	0,0,4096,10,1				;A SINE WAVE
giRtosScale	=	100					;SCALING FACTOR APPLIED TO RATIOS WRITTEN TO GEN09 FUNCTION TABLE
								;FREQUENCY OF AUDIO OSCILLATOR WILL BE DIVIDED BY THIS VALUE
								;TOO LOW A VALUE HERE WILL RESULT IN QUANTISATION OF PARTIAL FREQUENCIES
								;TOO HIGH A VALUE WILL RESULT IN HIGHER PARTIALS BEING POORLY REPRESENTED IN THE TABLE, LEADING TO ALIASING 
gilfoshape	ftgen	0, 0, 4096, 19, 0.5, 1, 180, 1	;U-SHAPE PARABOLA (USED BY THE CHORUS EFFECT)
gidurscal	ftgen	0, 0, 128, -16, 10, 128, -4, 0.1	;A FUNCTION USED TO RESCALE ENVELOPE DURATIONS ACCORDING TO NOTE PLAYED
givelscal	ftgen	0, 0, 128, -16, 0, 128, -4, 1		;A FUNCTION USED TO REMAP MIDI VELOCITY VALUES - THE CURVATURE OF THIS FUNCTION CAN BE CHANGED TO TAKE ACCOUNT OF THE VARYING VELOCITY RESPONSE OF DIFFERENT MIDI KEYBOARDS
gasend		init	0

instr	CreateGEN09Tables	;DERIVE GEN09 FUNCTION TABLE WAVEFORMS FROM THE GEN02 FUNCTION TABLES OF MODAL FREQUENCY RATIOS
;A MACRO IS DEFINED THAT READS A SINGLE MODAL FERQUENCY RATIO FROM A TABLE 
#define	PARTIAL(PartNum)
	#
	if $PartNum<=inratios then				;IF COUNTER IS WITHIN THE LIMITS OF THE NUMBER OF DEFINED MODAL FREQUENCY RATIOS...
	 irto$PartNum	table	$PartNum-1,girtos$WaveNum	;...READ A VALUE FROM THE TABLE
	 irto$PartNum	=	round(irto$PartNum*giRtosScale)	;MULTIPLY RATIO BY giRtosScale THEN INTEGERISE BYT ROUND TO THE NEAREST WHOLE NUMBER
	 iamp$PartNum	=	iamp/(iampscal^($PartNum-1))	;DERIVE AMPLITUDE VALUE FOR THIS PARTIAL
	else							;IF COUNTER IS BEYOND THE LIMITS OF THE NUMBER OF DEFINED MODAL FREQUENCY RATIOS...
	 irto$PartNum	=	0				;SET RATIO TO ZERO
	 iamp$PartNum	=	0				;SET PARTIAL STRENGTH TO ZERO
	endif
	#

;A MACRO IS DEFINED THAT CREATES A GEN09 TABLE FROM MODAL FREQUENCY DATA 
#define	WAVEFORM(WaveNum'AmpScal)
	#
	inratios	=	ftlen(girtos$WaveNum)
	iampscal	=	$AmpScal			;AMPLITUDE SCALING 
	iamp		=	1				;PARTIAL STRENGTH OF FIRST PARTIAL
	$PARTIAL(1)						;MACRO EXPANDED FOR EACH PARTIAL...
	$PARTIAL(2)
	$PARTIAL(3)
	$PARTIAL(4)
	$PARTIAL(5)
	$PARTIAL(6)
	$PARTIAL(7)
	$PARTIAL(8)
	$PARTIAL(9)
	$PARTIAL(10)
	$PARTIAL(11)
	$PARTIAL(12)                          
	$PARTIAL(13)
	$PARTIAL(14)
	$PARTIAL(15)
	$PARTIAL(16)
	$PARTIAL(17)
	$PARTIAL(18)                                                                                                                                                         
	$PARTIAL(19)
	$PARTIAL(20)
	$PARTIAL(21)
	$PARTIAL(22)                                                                                                                                                            
	;GENERATE A GEN09 FUNCTION TABLE (ALL PHASES ARE SET TO ZERO)
	giwave$WaveNum	ftgen	0,0,131072,9, irto1,iamp1,0, irto2,iamp2,0, irto3,iamp3,0, irto4,iamp4,0, irto5,iamp5,0, irto6,iamp6,0, irto7,iamp7,0, irto8,iamp8,0, irto9,iamp9,0, irto10,iamp10,0, irto11,iamp11,0, irto12,iamp12,0, irto13,iamp13,0, irto14,iamp14,0, irto15,iamp15,0, irto16,iamp16,0, irto17,iamp17,0, irto18,iamp18,0, irto19,iamp19,0, irto20,iamp20,0, irto21,iamp21,0, irto22,iamp22,0 	

	;UNCOMMENT THE FOLLOWING LINE IF YOU WISH TO WRITE GENERATED TABLES TO A TEXT FILE FOR LATER USE
	;fprints "GEN09InharmonicTables.txt", "giwave$WaveNum%tftgen%t0, 0, 131072s, 9, %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0,%t %d,%5.3f,0%n", irto1,iamp1, irto2,iamp2, irto3,iamp3, irto4,iamp4, irto5,iamp5, irto6,iamp6, irto7,iamp7, irto8,iamp8, irto9,iamp9, irto10,iamp10, irto11,iamp11, irto12,iamp12, irto13,iamp13, irto14,iamp14, irto15,iamp15, irto16,iamp16, irto17,iamp17, irto18,iamp18, irto19,iamp19, irto20,iamp20, irto21,iamp21, irto22,iamp22
	#	
	;EXPAND MACRO FOR EACH WAVEFORM TO BE CREATED
	;ARG1 = A COUNTER, SHOULD CORRESPOND TO NUMBER APPENDED TO 'girtos' FOR THE TABLE HANDLE OF THE REQUIRED MODAL FREQUENCIES TABLE
	;ARG2 = PARTIAL STRENGTHS SCALING: 1=FLAT >1=HIGHER PARTIALS ARE INCREASINGLY ATTENUATED
	$WAVEFORM(1' 1.5)
	$WAVEFORM(2' 2)                                                                                  
	$WAVEFORM(3' 1.5)
	$WAVEFORM(4' 2)
	$WAVEFORM(5' 2)
	$WAVEFORM(6' 1)
	$WAVEFORM(7' 1.2)
	$WAVEFORM(8' 1.5)
	$WAVEFORM(9' 1)          
	$WAVEFORM(10' 1)
	$WAVEFORM(11' 1)
	$WAVEFORM(12' 1.5)                                       
	$WAVEFORM(13' 1)
	$WAVEFORM(14' 2)
	$WAVEFORM(15' 2)    
	$WAVEFORM(16' 1.5)
	$WAVEFORM(17' 2)
	$WAVEFORM(18' 2)
	$WAVEFORM(19' 1)
	$WAVEFORM(20' 1.2)
	$WAVEFORM(21' 1.1)
	$WAVEFORM(22' 1.1)
endin

instr	2	;SOUND GENERATING INSTRUMENT
	iInstr	chnget	"Instr"
	iAAtt	chnget	"AAtt"
	iADec	chnget	"ADec"
	iASus	chnget	"ASus"
	iARel	chnget	"ARel"
	iFAmt	chnget	"FAmt"
	iFDec	chnget	"FDec"
	iFSus	chnget	"FSus"
	iFRel	chnget	"FRel"
	iFShp	chnget	"FShp"
	iAAtt	=	(iAAtt^3)*(15-0.001)+0.001
	iADec	=	(iADec^3)*(15-0.001)+0.001
	iARel	=	(iARel^3)*(15-0.001)+0.001
	iFDec	=	(iFDec^3)*(15-0.001)+0.001
	iFRel	=	(iFRel^3)*(15-0.001)+0.001

	icps	cpsmidi						;READ CPS VALUE IN FROM MIDI KEYBOARD
	inum	notnum						;READ MIDI NOTE NUMBER
	ivel	veloc	0,1					;READ MIDI VELOCITY
	ivel	table	ivel,givelscal,1			;REMAP MIDI VELOCITY - SEE TABLE givelscal ABOVE
	iscale	table	inum,gidurscal				;DURATIONAL RESCALING ACCORDING TO NOTE PLAYED - HIGHER NOTE WILL DECAY QUICKER IN REFLECTION OF REAL-WORLD CHARACTERISTICS

	aenv	expsegr		0.001,iAAtt,1,iADec*iscale,iASus+0.0001,iARel*iscale,0.0001	;AMPLITUDE ENVELOPE WITH MIDI SENSING RELEASE SEGEMENT
	asig	poscil		0.1*aenv*ivel,icps/giRtosScale,giwave1+iInstr-1				;AUDIO OSCILLATOR poscil3 USED FOR IMPROVED FIDELITY OVER OTHER OSCILLATORS. FREQUENCY IS SCALED DOWN ACCORIND TO THE VALUE OF giRtosScale DEFINED IN THE HEADER

	;UNFORTUNATELY transegr DOESN'T SEEM TO WORK SO IN THE MEANTIME THIS RATHER PROTRACTED ALTERNATIVE WILL ACT AS A SUBSTITUTE 
	iFlev1	limit		cpsoct((iFAmt*10*ivel)+4),icps,sr/2	;DERIVE STARTING FILTER VALUE IN CPS FROM FLTK KNOB (RANGE 0 TO 1). LIMIT IT BETWEEN FUNDEMENTAL FREQ. AND NYQUIST.
	iFsus	=		icps+cpsoct((iFSus*10)+4)			;DERIVE FILTER SUSTAIN VALUE IN CPS FROM FLTK KNOB VALUE (RANGE 0 - 1)
	iFend	=		icps						;FINAL FILTER ENVELOPE VALUE IS ALWAYS FUNDEMENTAL FREQUENCY
	
	krel	release						;SENSE WHEN A NOTE HAS BEEN RELEASED. 1=NOTE_RELEAESD 0=NOTE_BEING_HELD
	if krel==0 then						;IF NOTE IS BEGIN HELD...
	  kcf	transeg		iFlev1,iFDec*iscale,iFShp,iFsus	;CREATE ADS (ATTACK-DECAY-SUSTAIN) PART OF ENVELOPE
	else							;OTHERWISE (WE ARE IN THE RELEASE STAGE)
	  ktrig	changed		krel				;INSTIGATE A SHORT REINITIALISATION TO DERIVE AN I-TIME VALUE OF THE CURRENT AMPLITUDE VALUE FOR THE STARTING POINT OF THE RELEASE ENVELOPE 
	  if	ktrig==1 then					;
	    reinit	StartRel
	  endif
	  StartRel:
	  kcf	transeg		i(kcf),iFRel*iscale,iFShp,iFend		;RELEASE STAGE OF THE ENVELOPE. 'i(kcf)' AS THE STARTING VALUE ENSURES THAT IT ALWAYS PICKS UP FROM WHERE THE 'ADS' PART OF THE ENVELOPE LEFT OFF.
	  rireturn
	endif
	
	;asig	butlp		asig, kcf
	asig	clfilt		asig, kcf, 0, 2					;LOW PASS FILTER THE SOUND (SUBTRACTIVE SYNTHESIS)
	gasend	=	gasend+asig

endin

instr	3
	kChoOnOff	chnget	"ChoOnOff"
	gkChoMix	chnget	"ChoMix"
	gkChoRte	chnget	"ChoRte"
	gkChoDep	chnget	"ChoDep"

	if kChoOnOff==0 kgoto SKIP_CHORUS
	kporttime	linseg	0,0.001,1
	kporttime	=	kporttime/gkChoRte
	kdlt1	randomi	ksmps/sr,gkChoDep,gkChoRte,1
	kdlt1	portk	kdlt1,kporttime
	adlt1	interp	kdlt1
	acho1	vdelay	gasend,adlt1*1000,1*1000
	
	kdlt2	randomi	ksmps/sr,gkChoDep,gkChoRte,1
	kdlt2	portk	kdlt2,kporttime
	adlt2	interp	kdlt2
	acho2	vdelay	gasend,adlt2*1000,1*1000
	
	kpan1	randomi	0,1,gkChoRte,1
	kpan2	randomi	0,1,gkChoRte,1
	a1L,a1R	pan2	acho1,kpan1
	a2L,a2R	pan2	acho2,kpan2
	achoL	=	a1L+a2L
	achoR	=	a1R+a2R
		outs	achoL*gkChoMix, achoR*gkChoMix
	SKIP_CHORUS:

		outs		gasend, gasend	;SEND UN-CHORUSSED AUDIO TO THE OUTPUTS
		clear	gasend
endin

</CsInstruments>

<CsScore>
i "CreateGEN09Tables" 0 0
i 3 0 [60*60*24*7]
f 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>