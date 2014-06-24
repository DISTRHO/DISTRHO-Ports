#pragma once
//MusicDsp 
// T.Rochebois
//still indev
class Decimator17
{
private:
	float R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16,R17;
public:
	Decimator17()
	{
		R1=R2=R3=R4=R5=R6=R7=R8=R9=R10=R11=R12=R13=R14=R15=R16=R17;
	}
	float Calc(const float x0,const float x1)
	{
		//float h17x0 = h17*x0;
		//float h15x0 = h15 * x0;
		//float h13x0 = h13 * x0;


		//float h11x0=  h11 *x0;


	//	float h9x0=h9*x0;
	//	float h7x0=h7*x0;
	//	float h5x0=h5*x0;
	//	float h3x0=h3*x0;
	//	float h1x0=h1*x0;
		//float R18=R7+h17x0;
		//R17 = R16 + h15x0;
	//	R9=R8+h7x0;
	//	R8=R7+h5x0;
	//	R7=R6+h3x0;
	//	R6=R5+h1x0;
	//	R5=R4+h1x0+h0*x1;
		//R4=R3+h3x0;
	//	R3=R2+h5x0;
	//	R2=R1+h7x0;
	//	R1=h9x0;
		return R10;
	}
};
class Decimator9
{
private:
	const float h0,h1,h3,h5,h7,h9;
	float R1,R2,R3,R4,R5,R6,R7,R8,R9;
public:
	Decimator9() : h0(8192/16384.0f),h1(5042/16384.0f),h3(-1277/16384.0f),h5(429/16384.0f),h7(-116/16384.0f),h9(18/16384.0f)
	{
		R1=R2=R3=R4=R5=R6=R7=R8=R9=0.0f;
	}
	inline float Calc(const float x0,const float x1)
	{
		float h9x0=h9*x0;
		float h7x0=h7*x0;
		float h5x0=h5*x0;
		float h3x0=h3*x0;
		float h1x0=h1*x0;
		float R10=R9+h9x0;
		R9=R8+h7x0;
		R8=R7+h5x0;
		R7=R6+h3x0;
		R6=R5+h1x0;
		R5=R4+h1x0+h0*x1;
		R4=R3+h3x0;
		R3=R2+h5x0;
		R2=R1+h7x0;
		R1=h9x0;
		return R10;
	}
};
