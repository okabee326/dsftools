#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
//#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <omp.h>

#include <time.h>

//For FIR processing
#define DEFTAPS	512
#define K_alpha	10.0
//data to keep FIR history, for each channel. (2,3 is for reverse test)
int h_idata[2][DEFTAPS * 2];

int oversampling = 64;
//#define oversampling 64

//Shaper routine internal signal history
int sec_num = 8;
double D[2][4*8];	//Filter history, for L/R
double D2[2][8];

void write2bytes(FILE *f, unsigned short d)
{
	unsigned char buf[2];
	buf[0] = (unsigned char)(d & 0xFF);
	d = d >> 8;
	buf[1] = (unsigned char)(d & 0xFF);
	int ret = fwrite (buf , 1 , sizeof(buf) , f);
	if (ret != 2)
       fprintf(stderr, "Write error\n");
	return;
}
void write4bytes(FILE *f, unsigned long d)
{
	unsigned char buf[4];
	buf[0] = (unsigned char)(d & 0xFF);
	d = d >> 8;
	buf[1] = (unsigned char)(d & 0xFF);
	d = d >> 8;
	buf[2] = (unsigned char)(d & 0xFF);
	d = d >> 8;
	buf[3] = (unsigned char)(d & 0xFF);
	int ret = fwrite (buf , 1 , sizeof(buf) , f);
	if (ret != 4)
       fprintf(stderr, "Write error\n");
	return;
}

unsigned long read4bytes(FILE *f)
{
    unsigned char buf[4];

    if (fread(buf, 4, 1, f) != 1) {
        fprintf(stderr, "Read error\n");
        exit(1);
    }
    return ((256LU * buf[3] + buf[2]) * 256LU + buf[1] ) * 256LU + buf[0]  ;
}

unsigned read2bytes(FILE *f)
{
    unsigned char buf[2];

    if (fread(buf, 2, 1, f) != 1) {
        fprintf(stderr, "Read error\n");
        exit(1);
    }
    return 256U * buf[1] + buf[0] ;
}

//FIR utility
//Bessel Func, for Kaiser window
float Bessel(float alpha)
{
	float delta = (float)1e-12;
	float BesselValue;
	float Term, k, F;
                                          
	k = 0.0f;
	BesselValue = 0.0f;
	F = 1.0f;
	Term = 0.5f;
	while( (Term < -delta) || ( delta < Term) )
	{
		k = k + 1.0f;						// k = step 1,2,3,4,5
		F = F * ((alpha / 2.0f)/k);			// f(k+1) = f(k)*(c/k),f(0)=1. c is alpha/2. c/k -> 0
		Term = F*F;							// Current term value
		BesselValue = BesselValue + Term;	// Sum(f(k)^2)
	}
	return BesselValue;
}

//Generate Kaiser window, then apply to FIR coefficients
void DoKaiser(float * FIRCoeff, float K_Alpha)
{
	int i;
	float Numer, Denom;
	float center,Kg,Kd;
	float	Kwindow[DEFTAPS];		//Kaiser Window Parameter

	Denom = Bessel(K_Alpha);	

	center = (float)(DEFTAPS - 1)/2;
	for (i = 0; i < DEFTAPS; i++)
	{
		Kg = (float)(i*1.0 - center) / center;		//Regulated Distance, -1 to 1
		Kd = (float)(K_Alpha * sqrt(1.0 - Kg*Kg));	//kaiser Distance
		Numer = Bessel(Kd);
		Kwindow[i] = Numer / Denom;
	}

	for(i = 0; i < DEFTAPS; i++) 
	{
		FIRCoeff[i] = FIRCoeff[i] * Kwindow[i];
	}
}

//Generate Frequency Responce.
void CreateFres(long SampleFreq, int fFreqLow, int fFreqHi, float * FreqResR)
{
	// gain = pow(10.0, desired_dB / 20.0);
	// There are folding at Fs/2. _____L~~~~~H_____|_____H'~~~L'____. |=Fs/2

	int i;
	int taps_half = DEFTAPS / 2;
	double stopgain = pow(10.0, -200.0/20.0);
	for (i = 0; i < taps_half ; i++)
	{
		float CurrentFreq = (float)(SampleFreq * (i + 1)/DEFTAPS);
		if (CurrentFreq < fFreqLow)
		{
			//Stop Area 1
			FreqResR[i] = (float)stopgain;
		}
		else if (CurrentFreq < fFreqHi) 
		{
			//Pass Area 1
			FreqResR[i] = 1.0f;
		}
		else if (CurrentFreq <= SampleFreq / 2)
		{
			//Stop Area 2
			FreqResR[i] = (float)stopgain;
		}
	}
	for (i = taps_half; i < DEFTAPS; i++)
	{
		//Mirror Freq Responce
		FreqResR[i] = FreqResR[DEFTAPS - i - 1];
	}
	//debug. use scilab, -->M = fscanfMat("c:\temp\FreqRes.txt");plot(M);
	FILE * f;
	f = fopen("FreqRes.txt", "w");
	for (int i = 0 ; i < DEFTAPS; i++)
	{
		fprintf(f, "%3.5lf\n", FreqResR[i]);
	}
	fclose(f);
}

//make FIR coeff, from Frequency Response.
void CalcCoeffs(float * FreqResR, float * FIRCoeff)
{
	float costable[DEFTAPS];
	float Tempcoeff_r[DEFTAPS];
//#pragma omp parallel for
	for (int i = 0; i < DEFTAPS; i++)
	{
		costable[i] = (float)cos( 2.0 * M_PI * i / DEFTAPS);
	}

	for (int i = 0; i < DEFTAPS; i++) {
		Tempcoeff_r[i] = 0.0;
		float Coeff = 0.0;
		for (int j = 0; j < DEFTAPS; j++) {
			Coeff += FreqResR[j]*costable[(i * j)%DEFTAPS];
		}
		Tempcoeff_r[i] = Coeff / DEFTAPS;
	}

	for (int i = 0; i < DEFTAPS / 2 ; i++) 
	{
		FIRCoeff[i] = (float)Tempcoeff_r[DEFTAPS / 2 - i];
	}
	for (int i = DEFTAPS / 2 ; i < DEFTAPS; i++) 
	{
		FIRCoeff[i] = (float)Tempcoeff_r[i-(DEFTAPS / 2)];
	}

	//debug. use scilab, -->M = fscanfMat("c:\temp\Coeffs.txt");plot(M);
	FILE * f;
	f = fopen("Coeffs.txt", "w");
	for (int i = 0 ; i < DEFTAPS; i++)
	{
		fprintf(f, "%3.15lf\n", FIRCoeff[i]);
	}
	fclose(f);

}

//FIR main Routine
void ProcessFIR(int * input, float * output, int Samples, float * coeffs, int channel, int mode)
{
	//mode: 0= normal, 1=reverse
	int* in  = input;
	float* out = output;	
	float* FIRCoeff = coeffs;
	//add input to input buffer
	for (int i = 0; i < Samples; i++){
		h_idata[channel][DEFTAPS + i] = in[i];
	}


	//This is special routine for DSD to wav. (DSD data is 1 or -1)
#pragma omp parallel for
	for (int i = 0; i < Samples; i++) {
		float dOut = 0.0;
		for (int j = 0; j < DEFTAPS; j++)
		{
			if (h_idata[channel][i + j] > 0) {
				dOut += FIRCoeff[j];
			}else {
				dOut -= FIRCoeff[j];
			}
		}
		out[i] = dOut;
	}	

	//slide input buffer
	int temp_f = 0.0f;
	for (int i = 0; i < DEFTAPS; i++) {
		temp_f =  h_idata[channel][i + Samples];
		h_idata[channel][i] = temp_f;
	}

}
/*
float sinc(float x)
{
	float y;

	if(x == 0) return 1;

	else { 
		y = sin(x)/x ;
		return y;
	}
}
*/
//Low pass filter
// http://vstcpp.wpblog.jp/?page_id=523
// http://aidiary.hatenablog.com/entry/20120103/1325594723

void makeCoeff(double* omega, double* alpha, double* a, double* b, int samplerate, double freq, double q){
	// ƒtƒBƒ‹ƒ^ŒW”‚ðŒvŽZ‚·‚é
	// biquad

	*omega = 2.0 * M_PI * freq / samplerate;
	*alpha = sin(*omega) / (2.0 * q);
 
	a[0] =  1.0 + *alpha;
	a[1] = -2.0 * cos(*omega);
	a[2] =  1.0 - *alpha;
	b[0] = (1.0 - cos(*omega)) / 2.0;
	b[1] =  1.0 - cos(*omega);
	b[2] = (1.0 - cos(*omega)) / 2.0;
	
/*	double fc = tan(freq * M_PI / (double)samplerate) / (2 * M_PI );
	
	// butterworth
	double denom = 1.0 + (2.0 * sqrt(2) * M_PI * fc) + 4 * M_PI*M_PI * fc*fc;
	b[0] = 1.0 / denom;
    b[1] = -2.0 / denom;
    b[2] = 1.0 / denom;
    a[0] = 1.0;
    a[1] = (8.0 * M_PI*M_PI * fc*fc - 2) / denom;
    a[2] = (1.0 - (2.0 * sqrt(2) * M_PI * fc) + 4 * M_PI*M_PI * fc*fc) / denom;
*/
}

//makeHighpass
void makeHighpass(double* omega, double* alpha, double* a, double* b, int samplerate, double freq, double q){
	// ƒtƒBƒ‹ƒ^ŒW”‚ðŒvŽZ‚·‚é
	// biquad

	*omega = 2.0 * M_PI * freq / samplerate;
	*alpha = sin(*omega) / (2.0 * q);
 
	a[0] =  1.0 + *alpha;
	a[1] = -2.0 * cos(*omega);
	a[2] =  1.0 - *alpha;
	b[0] = (1.0 + cos(*omega)) / 2.0;
	b[1] =  -(1.0 + cos(*omega) );
	b[2] = (1.0 + cos(*omega)) / 2.0;
	
}

// float input[]  c“ü—ÍM†‚ÌŠi”[‚³‚ê‚½ƒoƒbƒtƒ@B
// flaot output[] cƒtƒBƒ‹ƒ^ˆ—‚µ‚½’l‚ð‘‚«o‚·o—ÍM†‚Ìƒoƒbƒtƒ@B
// int   size     c“ü—ÍM†Eo—ÍM†‚Ìƒoƒbƒtƒ@‚ÌƒTƒCƒYB
// float samplerate c ƒTƒ“ƒvƒŠƒ“ƒOŽü”g”B
// float freq c ƒJƒbƒgƒIƒtŽü”g”B
// float q    c ƒtƒBƒ‹ƒ^‚ÌQ’lB
void lowpass(double* input, double* output, int size, double* a, double* b, int channel, int section, unsigned long *counter)
{
/*	
	printf("a0: %lf\n", a[0]);
	printf("a1: %lf\n", a[1]);
	printf("a2: %lf\n", a[2]);
	printf("b0: %lf\n", b[0]);
	printf("b1: %lf\n", b[1]);
	printf("b2: %lf\n", b[2]);
*/
//printf("%lu %lu\n", input, output);
	double b0a0 = b[0]/a[0];
	double b1a0 = b[1]/a[0];
	double b2a0 = b[2]/a[0];
	double a1a0 = a[1]/a[0];
	double a2a0 = a[2]/a[0];

	double buf[size];
	if(section == 0 ){
		for(int i=0;i<size;i++){
			if(input[i] > 0){
				buf[i] = 1.0;
		  	}
		  	else {
				buf[i] = -1.0;
		  	}
		}
	}
	else {
		for(int i=0;i<size;i++){
			buf[i] = input[i];
		}
	}
	
	volatile double dummy = *(buf + size -8);
	
	double* d0 = &D[channel][section*4];
	double* d1 = &D[channel][section*4+1];
	double* d2 = &D[channel][section*4+2];
	double* d3 = &D[channel][section*4+3];

	// ƒtƒBƒ‹ƒ^‚ð“K—p
	for(int i = 0; i < size; i++)
	{
		double in = buf[i];
		double out = 0.0;
		
		
		// “ü—ÍM†‚ÉƒtƒBƒ‹ƒ^‚ð“K—p‚µAo—ÍM†‚Æ‚µ‚Ä‘‚«o‚·B
		/*out = (b0a0 * in + b1a0 * D[channel][0+section*4]  + b2a0 * D[channel][1+section*4]
		                             - a1a0 * D[channel][2+section*4] - a2a0 * D[channel][3+section*4]);
		 */
		 out = b0a0 * in + b1a0 * (*d0)  + b2a0 * (*d1)
		                             - a1a0 * (*d2) - a2a0 * (*d3) ;
		 
 
		*d1  = *d0;       // 2‚Â‘O‚Ì“ü—ÍM†‚ðXV
		*d0  = in;  // 1‚Â‘O‚Ì“ü—ÍM†‚ðXV
 
		*d3 = *d2;      // 2‚Â‘O‚Ìo—ÍM†‚ðXV
		*d2 = out; // 1‚Â‘O‚Ìo—ÍM†‚ðXV

 		output[i] = out;
	}
}
void lowpass_f(float* input, float* output, int size, double* a, double* b, int channel, int section, unsigned long *counter)
{
/*	
	printf("a0: %lf\n", a[0]);
	printf("a1: %lf\n", a[1]);
	printf("a2: %lf\n", a[2]);
	printf("b0: %lf\n", b[0]);
	printf("b1: %lf\n", b[1]);
	printf("b2: %lf\n", b[2]);
*/
	// ƒtƒBƒ‹ƒ^‚ð“K—p
	for(int i = 0; i < size; i++)
	{
		float in = 0.0;
		float out = 0.0;
		
		if(section==0){
		  if(input[i] > 0){
			in = 1.0f;
		  }
		  else {
			in = -1.0f;
		  }

		}
		else {
			in = (input[i]);
		}
		// “ü—ÍM†‚ÉƒtƒBƒ‹ƒ^‚ð“K—p‚µAo—ÍM†‚Æ‚µ‚Ä‘‚«o‚·B
		out = (b[0]/a[0] * in + b[1]/a[0] * D[channel][0+section*4]  + b[2]/a[0] * D[channel][1+section*4]
		                             - a[1]/a[0] * D[channel][2+section*4] - a[2]/a[0] * D[channel][3+section*4]);
 

 
		D[channel][1+section*4]  = D[channel][0+section*4];       // 2‚Â‘O‚Ì“ü—ÍM†‚ðXV
		D[channel][0+section*4]  = in;  // 1‚Â‘O‚Ì“ü—ÍM†‚ðXV
 
		D[channel][3+section*4] = D[channel][2+section*4];      // 2‚Â‘O‚Ìo—ÍM†‚ðXV
		D[channel][2+section*4] = out; // 1‚Â‘O‚Ìo—ÍM†‚ðXV
	/*	
		if(out < 0.001 && out > -0.001){
 			out = 0.0f;
 		}
	*/	
		//if(section==0){
 			
 		//	out = (out - 0.5) * 2;
		//}
 		output[i] = out;
	}
}

//7th noise shaper utility
void Shaper_Coeff(double Zero1, double Zero2, double Zero3, double Pole, double * A, double * B, bool Print)
{
	//This routine calculate standard H(z) function, as
	// H(z) = 1+A1*Z^-1+A2*Z^-2+A3Z^-3+A4Z^-4..... / 1+B1Z^-1+B2Z^-2+....

	double Theta1, Theta2, Theta3;
	double s1, s2, s3;

	//for 64 oversampling... Zero1 = 1.0 means 44100/2.0=22050 
	Theta1 = ((2.0 * M_PI / oversampling) / 2.0) * Zero1;
	Theta2 = ((2.0 * M_PI / oversampling) / 2.0) * Zero2;
	Theta3 = ((2.0 * M_PI / oversampling) / 2.0) * Zero3;

	//(1-x)(1-2cosT1x +x^2)(1-2cosT2x +x^2)(1-2cosT3x +x^2)
	s1 = 2.0 * cos(Theta1);
	s2 = 2.0 * cos(Theta2);
	s3 = 2.0 * cos(Theta3);

	printf("Theta1 = %.15f\n",Theta1);
	printf("Theta2 = %.15f\n",Theta2);
	printf("Theta3 = %.15f\n",Theta3);

	printf("s1 = %.15f\n",s1);
	printf("s2 = %.15f\n",s2);
	printf("s3 = %.15f\n",s3);

	A[0] = 1.0;
	A[1] = -1.0 * (s1 + s2 + s3 + 1.0);
	A[2] = (s1*s2 + s2*s3 + s3*s1 + s1 + s2 + s3 + 3.0);
	A[3] = -1.0 * (2.0*s1 + 2.0*s2 + 2.0*s3 + s1*s2*s3 + s1*s2 + s2*s3 + s3*s1 + 3.0); 
	A[4] = -1.0 * A[3];
	A[5] = -1.0 * A[2];
	A[6] = -1.0 * A[1];
	A[7] = -1.0 * A[0];

	//7th order, pole shift
	printf("\n7th order Pole shift\n");
	B[0] = 1.0;
	B[1] = -7.0 * Pole ;
	B[2] = 21.0 * Pole * Pole;
	B[3] = -35.0 * Pole * Pole * Pole;
	B[4] = 35.0 * Pole * Pole * Pole * Pole;
	B[5] = -21.0 * Pole * Pole * Pole * Pole * Pole;
	B[6] = 7.0 * Pole * Pole * Pole * Pole * Pole * Pole;
	B[7] = -1.0 * Pole * Pole * Pole * Pole * Pole * Pole * Pole;

	printf("A: %+.8f,%+.8f,%+.8f,%+.8f,%+.8f,%+.8f,%+.8f\n",A[1],A[2],A[3],A[4],A[5],A[6],A[7]);
	printf("B: %+.8f,%+.8f,%+.8f,%+.8f,%+.8f,%+.8f,%+.8f\n",B[1],B[2],B[3],B[4],B[5],B[6],B[7]);

	if (Print){
		//Scilab command file for display.
		FILE * f;
		if ( (f = fopen("/tmp/7thNoiseShaper.sce", "w")) != NULL ) {
			fprintf(f, "z=%%z;\n");
			fprintf(f, "H1=(1-z^(-1))*(1-%.8f*z^(-1)+z^(-2))*(1-%.8f*z^(-1)+z^(-2))*(1-%.8f*z^(-1)+z^(-2));\n",s1,s2,s3);
			fprintf(f, "H=(1%+.15f*z^(-1)%+.15f*z^(-2)%+.15f*z^(-3)%+.15f*z^(-4)%+.15f*z^(-5)%+.15f*z^(-6)%+.15f*z^(-7));\n",A[1],A[2],A[3],A[4],A[5],A[6],A[7]);
			fprintf(f, "D=(1%+.15f*z^(-1)%+.15f*z^(-2)%+.15f*z^(-3)%+.15f*z^(-4)%+.15f*z^(-5)%+.15f*z^(-6)%+.15f*z^(-7));\n",B[1],B[2],B[3],B[4],B[5],B[6],B[7]);
			fprintf(f, "sys=syslin(\"d\",H1/D);\n");
			fprintf(f, "clf();f1=scf(0);plzr(sys);f1=scf(1);gainplot(sys,1e-3,1e-1,0.001);\n");
			fprintf(f, "f1=scf(0);\n");
			fclose(f);		
		}
	}

	printf("Shaper Coeff calculation Done\n");
}

//7th noise shaper function
void Shaper_Process(double * A, double * B, double * d, double * input, float * output, int Samples, int step)
{
	//This routine uses http://en.wikipedia.org/wiki/Digital_filter Direct form II type filter.
	//coefficients are converted as below.

	//for noise shaper IIR,
	double a1, a2, a3, a4, a5, a6, a7;
	double b1, b2, b3, b4, b5, b6, b7;
	//modify IIR method to Direct Form II

	b1 = -1.0 * B[1];
	b2 = -1.0 * B[2];
	b3 = -1.0 * B[3];
	b4 = -1.0 * B[4];
	b5 = -1.0 * B[5];
	b6 = -1.0 * B[6];
	b7 = -1.0 * B[7];
	a1 = -1.0 * A[1] - b1;
	a2 = -1.0 * A[2] - b2;
	a3 = -1.0 * A[3] - b3;
	a4 = -1.0 * A[4] - b4;
	a5 = -1.0 * A[5] - b5;
	a6 = -1.0 * A[6] - b6;
	a7 = -1.0 * A[7] - b7;
	//input: -1.0 to 1.0
	//output: -1 or +1

	//double Q_inner1;	//upper side
	//double Q_inner2;	//lower side
	//double Q_error;
	//double x_in;
	//double y_out;
	

//#pragma omp parallel for
	for (int i = 0; i < Samples; i++){
		//output[i] = (float)input[i];
		//continue;
		
		double Q_inner1 = a1 * d[1] + a2 * d[2] + a3 * d[3] + a4 * d[4] + a5 * d[5] + a6 * d[6] + a7 * d[7];
		double x_in = input[i] + Q_inner1;
		float y_out = 0.0;
		
		y_out = (float)x_in;
		
		
		
		//*counter++;
		double Q_error = x_in - (double)y_out;
		double Q_inner2 = b1 * d[1] + b2 * d[2] + b3 * d[3] + b4 * d[4] + b5 * d[5] + b6 * d[6] + b7 * d[7];

		d[7] = d[6];
		d[6] = d[5];
		d[5] = d[4];
		d[4] = d[3];
		d[3] = d[2];
		d[2] = d[1];
		d[1] = Q_error + Q_inner2;	//stored into Delay register
		d[0] = y_out;

		output[i] = (y_out);
	}
}


int main(int argc, char * argv[])
{
	// arg = input DSF file, sampling rate(44100,88200,176400), bit (16/24)
	// if file only, outputs 88200/24bit.
	char* infile = 0;
	char* outfile = 0;
	int samplingrate = 88200;
	int outsampling = 88200;
	int outputbit = 32;
	int cutoff = samplingrate *2;
	int useNoiseShaper = 1;
	//double zero[4] = { 0.969, 0.7568, 0.4347, 0.85 };
	//double zero[4] = { 0.96902, 0.75702, 0.43501, 0.85 };
	//double zero[4] = { 0.969, 0.757, 0.435, 0.85 };
	//double zero[4] = { 0.969, 0.685, 0.414, 0.85 };
	double zero[4] = { 0.969, 0.737, 0.424, 0.85 };
	//double zero[4] = { 0.969, 0.70116, 0.43335, 0.85 };
	//double zero[4] = { 0.96900592, 0.75700447, 0.43500243, 0.85 };
	
	for(int i=1;i<argc;i++){
		if( argv[i][0] != '-' ){
			if(infile == 0){
				infile = argv[i];
			}
			else {
				outfile = argv[i];
			}
			continue;
		}
		// samplingrate setting
		if(!strcmp(argv[i], "-s") || !strcmp(argv[i], "--sampling")){
			if(i+1 != argc){
				samplingrate = atoi(argv[i+1]);
				// if samplingrate is 48k,96k... outsampling=48k and samplingrate=44k
				outsampling = samplingrate;
				cutoff = samplingrate * 2;
				if(samplingrate % 44100 != 0){
					if(samplingrate % 48000 == 0){
						samplingrate = 44100 * (samplingrate / 48000);
						cutoff = samplingrate * 2;
					}
					else {
						printf("sampling rate is 44100 88200 176400hz  .\n");
						return -1;
					}
				}
				i++;
			}
			continue;
		}
		if(!strcmp(argv[i], "-n") || !strcmp(argv[i], "--ns")){
			useNoiseShaper = 0;

			continue;
		}

		// outputbit setting
		if(!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bit")){
			if(i+1 != argc){
				outputbit = atoi(argv[i+1]);
				if( !( outputbit == 16 || outputbit == 24 || outputbit == 32 || outputbit == 64) ){
					printf("outputbit must be 16 24 32 64 .\n");
					return -1;
				}
				i++;
			}
			continue;
		}
		if(!strcmp(argv[i], "-l") || !strcmp(argv[i], "--lowpass")){
			if(i+1 != argc){
				 cutoff = atoi(argv[i+1]);
				if( ( cutoff < 1) ){
					printf("lowpass filter cutoff freq must be over 0 .\n");
					return -1;
				}
				i++;
			}
			continue;
		}
		if(!strcmp(argv[i], "-z") || !strcmp(argv[i], "--zero")){
			if(i+4 < argc){
				for(int j=0;j<4;j++){
					zero[j] = atof(argv[i+j+1]);
				}
				
				i+=4;
			}
			else {
				
					printf("Zero Point option needs 4 parameters.\n");
					return -1;
				
			}
			continue;
		}
		
		// usage
		printf("usage: dsf2wav infile outfile [-s sampling] [-b bit] [-n] [-z zero1 zero2 zero3 zero4]\n");
		return -1;
	}
	
	if(infile == 0 || outfile == 0){
		// usage
		printf("usage: dsf2wav infile outfile [-s sampling] [-n] [-b bit]  [-z zero1 zero2 zero3 zero4]\n");
		return -1;
	}

	char *filename = infile;
	FILE *f;
    int channels, BlockBytes;
	unsigned long len;
	long DSDrate;
	long DSDsamplecount;
	unsigned char s[10];
	f = fopen(filename, "rb");

/*
	if (argc > 2) {
		sscanf(argv[2], "%d", &samplingrate);
	}
	if (argc == 4) {
		sscanf(argv[3], "%d", &outputbit);
	}
*/

    if (f == NULL) {
        printf("Can not open %s\n", filename);
        return -1;
    }
    printf("finename = '%s'\n", filename);
    if (fread(s, 4, 1, f) != 1) {
        printf("Read error\n");
        fclose(f);
        return -1;
    }
    if (memcmp(s, "DSD ", 4) != 0) {
        printf("Not a DSD format\n");
        fclose(f);
        return -1;
    }
    printf("[DSD ] (%lu bytes)\n", read4bytes(f));
    printf("0x00000000 %lu\n", read4bytes(f));

	printf("File Size  %lu\n", read4bytes(f));
    printf("0x00000000 %lu\n", read4bytes(f));

	printf("ID3 Tag    %lu\n", read4bytes(f));
    printf("0x00000000 %lu\n", read4bytes(f));

    if (fread(s, 4, 1, f) != 1) {
        printf("Read error\n");
        fclose(f);
        return -1;
    }
    if (memcmp(s, "fmt ", 4) != 0) {
        printf("fmt not found\n");
        fclose(f);
        return -1;
    }

	len = read4bytes(f);
    printf("[fmt ] (%lu bytes)\n", len);
    if (len != 52) {
        printf("Length of fmt must be 52\n");
        return -1;
    }
	printf("0x00000000 %lu\n", read4bytes(f));

    printf("Data type = %u\n", read4bytes(f));
	printf("0x00000000 %lu\n", read4bytes(f));

    printf("DSD type? = %u\n", read4bytes(f));

	channels = read4bytes(f);
    printf("channels?     = %u\n", channels);
	DSDrate = read4bytes(f);	//DSD64:2.8M
    printf("Sampling rate = %luHz\n", DSDrate);

    oversampling = DSDrate / 2822400 * 64;

    printf("Sampling bit  = %lu\n", read4bytes(f));
	DSDsamplecount = read4bytes(f);
    printf("Sampling count= %lu\n", DSDsamplecount);
	printf("0x00000000 %lu\n", read4bytes(f));
	BlockBytes = read4bytes(f);
    printf("Block Bytes   = %lu\n", BlockBytes);
	printf("0x00000000 %lu\n", read4bytes(f));
    if (fread(s, 4, 1, f) != 1) {
        printf("Read error\n");
        fclose(f);
        return -1;
    }
    if (memcmp(s, "data", 4) != 0) {
        printf("data not found\n");
        fclose(f);
        return -1;
    }
	len = read4bytes(f);
	printf("Data Bytes   = (%lu bytes)\n", len);
	printf("0x00000000 %lu\n", read4bytes(f));

	printf("Reading File into Memory.....\n");
	//get memory for "len" bytes
	unsigned char * p;
	p = (unsigned char *)malloc(len + BlockBytes * channels);
	memset(p, 0x69, len + BlockBytes * channels);	//note, DSD 0 is not 0x00.

	fread(p, len, 1,f);	//p has DSD, 4096L 4096R 4096L ...
	fclose(f);

	char* writefilename = outfile;
	//strcpy(writefilename, filename);
	//strcat(writefilename, ".to.WAV");
	FILE * wf;
	wf = fopen(writefilename, "wb");
    if (wf == NULL) {
        printf("Can not open to write %s\n", writefilename);
        return -1;
    }
	//make wave header
	fputc('R', wf);
	fputc('I', wf);
	fputc('F', wf);
	fputc('F', wf);
	//long datasize = (DSDsamplecount) / DSDrate * samplingrate * channels * outputbit/8;
	unsigned long datasize = (ceil((double)len / (double)BlockBytes) -2) * BlockBytes*8 / (DSDrate/ samplingrate) * (outputbit/8)* channels / 2;
	//printf("ceil(%lu): %lu\n",(unsigned long)len, (unsigned long)ceil(len / (double)BlockBytes));
	//printf("datasize: %lu\n", datasize);
	unsigned long filesize = datasize + 8 + 28;
	write4bytes(wf, (unsigned long)filesize);
	fputc('W', wf);
	fputc('A', wf);
	fputc('V', wf);
	fputc('E', wf);
	fputc('f', wf);
	fputc('m', wf);
	fputc('t', wf);
	fputc(' ', wf);
	write4bytes(wf, (unsigned long)16);
	if(outputbit == 32 || outputbit == 64){
		write2bytes(wf, (unsigned short)3);
	}
	else {
		write2bytes(wf, (unsigned short)1);
	}
	write2bytes(wf, (unsigned short)channels);
	write4bytes(wf, (unsigned long)outsampling);
	write4bytes(wf, (unsigned long)outsampling*channels* outputbit/8);
	write2bytes(wf, (unsigned short)channels* outputbit/8);
	write2bytes(wf, (unsigned short)outputbit);
	fputc('d', wf);
	fputc('a', wf);
	fputc('t', wf);
	fputc('a', wf);
	write4bytes(wf, (unsigned long)datasize);

	//Prepare FIR parameter

	float FreqResR[DEFTAPS];
	float FIRCoeff[DEFTAPS];
	memset(h_idata, 0, sizeof(int)*DEFTAPS*4);	//FIR history buffer

	long * DSDdata_int;	//one block DSD data, float, 1 / -1.0
	DSDdata_int = (long *)malloc(sizeof(long)*BlockBytes*8* channels); //1 byte DSD has 8 data
	memset(DSDdata_int, 0, sizeof(long)*BlockBytes*8 * channels);
	
	double * LPFdata;	//LPFed data. this is 2.8MHz float PCM, contains both channel in DSD64
	LPFdata = (double *)malloc(sizeof(double)*(BlockBytes * 8 * channels* sec_num));
	memset(LPFdata, 0, sizeof(double)*BlockBytes * 8 * channels* sec_num);
	
	float * Shaper_out;	//float64bit -> 32bit noise shaper output
	Shaper_out = (float *)malloc(sizeof(float)*(BlockBytes * 8 * channels* sec_num));
	memset(Shaper_out, 0, sizeof(float)*BlockBytes * 8 * channels* sec_num);

	//integer output
	unsigned char * WriteBuffer;
	WriteBuffer = (unsigned char * )malloc(BlockBytes * 8 * channels * 4);
	memset(WriteBuffer, 0, BlockBytes * 8 * channels * 4);
	
	//32bit float output
	float * Floatdata = (float*)malloc(BlockBytes * 8 * channels * sizeof(float));
	memset(Floatdata, 0, BlockBytes * 8 * channels * sizeof(float));
	
	double * Doubledata = (double*)malloc(BlockBytes * 8 * channels * sizeof(double));
	memset(Doubledata, 0, BlockBytes * 8 * channels * sizeof(double));

	CreateFres(44100 * oversampling, 0, samplingrate, FreqResR);	//22050 LPF
	CalcCoeffs(FreqResR, FIRCoeff);
	DoKaiser(FIRCoeff, K_alpha);
	
	//Shaper Coefficient Calcuration
	double A[8*sec_num];
	double B[8*sec_num];
	double A2[8];
	double B2[8];
	
	Shaper_Coeff(zero[0], zero[1], zero[2], 0.85, A2, B2, true);
	
	//initialize history buffer for shaper
	for(int i = 0; i < channels; i++){
		for (int j = 0; j < 4*sec_num; j++){
			D[i][j] = 0.0;
		}
	}
	for(int i = 0; i < channels; i++){
		for (int j = 0; j < 8; j++){
			D2[i][j] = 0.0;
		}
	}
	
	//void makeCoeff(double* omega, double* alpha, double* a, double* b, int samplerate, double freq, double q)
	double omega = 0;
	double alpha = 0;
	int count=0;
	for(count=0;count<sec_num-1;count++){
	makeCoeff(&omega, &alpha, &A[count*3], &B[count*3], samplingrate*oversampling, samplingrate*2, 1.0/sqrt(2) );
	}
	//count++;
	//makeCoeff(&omega, &alpha, &A[4*3], &B[4*3], samplingrate*oversampling, samplingrate, 1.0/sqrt(2) );
	//makeCoeff(&omega, &alpha, &A[5*3], &B[5*3], samplingrate*oversampling, samplingrate, 1.0/sqrt(2) );
	//makeCoeff(&omega, &alpha, &A[6*3], &B[6*3], samplingrate*oversampling, samplingrate, 1.0/sqrt(2) );
	//makeCoeff(&omega, &alpha, &A[8*3], &B[8*3], samplingrate*oversampling, samplingrate, 1.0/sqrt(2) );
	makeCoeff(&omega, &alpha, &A[(sec_num-1)*3], &B[(sec_num-1)*3], samplingrate*oversampling, samplingrate*2, 1.0/sqrt(2) );
	//count++;
	//makeHighpass(&omega, &alpha, &A[(sec_num-1)*3], &B[(sec_num-1)*3], samplingrate*oversampling, 1, 1.0/sqrt(2) );
	//makeHighpass(&omega, &alpha, &A[(sec_num-1)*3], &B[(sec_num-1)*3], samplingrate*oversampling, 5, 1.0/sqrt(2) );
	//count++;
	//makeHighpass(&omega, &alpha, &A[count*3], &B[count*3], samplingrate*oversampling, 5, 1.0/sqrt(2) );
	//makeHighpass(&omega, &alpha, &A[5*3], &B[11*3], samplingrate*oversampling, 5, 1.0/sqrt(2) );

	//makeCoeff(&omega, &alpha, &A[1*3], &B[1*3], samplingrate*oversampling, samplingrate*2, 1.0/sqrt(2) );
	//makeCoeff(&omega, &alpha, &A[2*3], &B[2*3], samplingrate*oversampling, samplingrate*2, 1.0/sqrt(2) );
	//makeCoeff(&omega, &alpha, &A[3*3], &B[3*3], samplingrate*oversampling, samplingrate*2, 1.0/sqrt(2) );
	//makeCoeff(&omega, &alpha, &A[4*3], &B[4*3], samplingrate*oversampling, samplingrate*2, 1.0/sqrt(2) );
	//makeCoeff(&omega, &alpha, &A[5*3], &B[5*3], samplingrate*oversampling, samplingrate*2, 1.0/sqrt(2) );
	//makeCoeff(&omega, &alpha, &A[6*3], &B[6*3], samplingrate*oversampling, samplingrate*2, 1.0/sqrt(2) );
	//makeCoeff(&omega, &alpha, &A[7*3], &B[7*3], samplingrate*oversampling, samplingrate*2, 1.0/sqrt(2) );

/*	
	for(int count=2;count<4;count++){
	makeCoeff(&omega, &alpha, &A[count*3], &B[count*3], samplingrate*oversampling, samplingrate*(4-count)/2, 1.0/sqrt(2) );
	}
*/	


	unsigned long pCounter = 0;	//Counter for data buffer.

	printf("conversion start\n");
	double start,end;
	start = omp_get_wtime();


	
	int WriteBufferPointer = 0;
	
	while (pCounter < len ) {
		WriteBufferPointer = 0;
		//loop for channel
		
		#pragma omp parallel for num_threads(2)
		for (int iChannel = 0; iChannel < channels; iChannel++){
			unsigned char DSD = 0x00;
			unsigned char oneSample = 0x00;
			
			//read DSD data, convert to 1.0 / -1.0 float
			unsigned long shaper_counter = 0;
			for (int j = 0; j < BlockBytes; j++){
				DSD = *(p + pCounter + iChannel*BlockBytes + j);	//read one byte
				for (int k = 0; k < 8; k++) {
					oneSample = (DSD>>k) & 0x01;
					if (oneSample == 1){
						DSDdata_int[j*8 + k + BlockBytes*8*iChannel] = 1;
					}else{
						DSDdata_int[j*8 + k + BlockBytes*8*iChannel] = -1;
					}
				}
			}
			//apply LPF
			//int LPF_x = 1;
			for (int x = 0; x < oversampling; x++)	//block*8 is too large for FIR
			{
				//ProcessFIR(&DSDdata_int[x*BlockBytes*8/oversampling], &LPFdata[iChannel*(BlockBytes*8) + x*BlockBytes*8/oversampling],  BlockBytes*8/oversampling, FIRCoeff, iChannel, 1);
				//void lowpass(float* input, float* output, int size, float *omega, float *alpha, float* a, float* b, int channel);
				lowpass((double*)&DSDdata_int[x*BlockBytes*8/oversampling + BlockBytes*8*iChannel], &LPFdata[(iChannel*(BlockBytes*8) + x*BlockBytes*8/oversampling)+ (channels*(BlockBytes*8)*0)],BlockBytes*8/oversampling, A, B, iChannel, 0, &pCounter);
				//Shaper_Process(A2, B2, &D2[iChannel][0], &LPFdata[(iChannel*(BlockBytes*8) + x*BlockBytes*8/oversampling)+ (channels*(BlockBytes*8)*0)], &Shaper_out[(iChannel*(BlockBytes*8) + x*BlockBytes*8/oversampling)+ (channels*(BlockBytes*8)*0)], BlockBytes*8/oversampling, DSDrate / samplingrate);
			}
			for(int y = 1;y < sec_num;y++){
			  for (int x = 0; x < oversampling; x++){	//block*8 is too large for FIR
				lowpass(&LPFdata[(iChannel*(BlockBytes*8) + x*BlockBytes*8/oversampling)+ (channels*(BlockBytes*8)*(y-1))], &LPFdata[(iChannel*(BlockBytes*8) + x*BlockBytes*8/oversampling)+ (channels*(BlockBytes*8)*y)],BlockBytes*8/oversampling, &A[y*3], &B[y*3], iChannel, y, &pCounter);
				//Shaper_Process(A2, B2, &D2[iChannel][0], &LPFdata[(iChannel*(BlockBytes*8) + x*BlockBytes*8/oversampling)+ (channels*(BlockBytes*8)*y)], &Shaper_out[(iChannel*(BlockBytes*8) + x*BlockBytes*8/oversampling)+ (channels*(BlockBytes*8)*y)], BlockBytes*8/oversampling, DSDrate / samplingrate);			  	
			  }
			}
			
			if(outputbit == 32 && useNoiseShaper == 1){
			   for (int x = 0; x < oversampling; x++){	//block*8 is too large for FIR
			 	 Shaper_Process(A2, B2, &D2[iChannel][0], &LPFdata[(iChannel*(BlockBytes*8) + x*BlockBytes*8/oversampling)+ (channels*(BlockBytes*8)*(sec_num-1) )], &Shaper_out[(iChannel*(BlockBytes*8) + x*BlockBytes*8/oversampling)+ (channels*(BlockBytes*8)*0)], BlockBytes*8/oversampling, DSDrate / samplingrate);	
			   }
			}
			
		}
		/*
		for(int dc=0;dc<1024;dc++){
			printf("%16.14f\n",LPFdata[dc]);
		}
		*/
		//now LPFData has [ channel 0 LPFed... ][ channel 1 LPFed.... ] data.
		//WAV is LRLRLRLR.. so loop goes length / then channel
		int DecimationStep = DSDrate / samplingrate ;

		
		for (int j = 0; j < BlockBytes*8; j = j + DecimationStep){
			for (int iChannel = 0; iChannel < channels; iChannel++){
				unsigned char H = 0x00;
				unsigned char M = 0x00;
				unsigned char L = 0x00;
				unsigned long PCMvalue;
				float oneResult;
				double doubleResult;
				//read float
				//oneResult = (float) LPFdata[iChannel*(BlockBytes*8)+ j];
				//oneResult = (float)LPFdata[iChannel*(BlockBytes*8) + j + (channels*(BlockBytes*8)*(sec_num-1))];
				if(outputbit == 32 && useNoiseShaper == 1){

				  oneResult = (float)Shaper_out[iChannel*(BlockBytes*8) + j + (channels*(BlockBytes*8)*(0))];
				  //oneResult = (float)LPFdata[iChannel*(BlockBytes*8) + j + (channels*(BlockBytes*8)*(sec_num-1))];
				}
				else {
				  if(outputbit == 64){
				  	  doubleResult = LPFdata[iChannel*(BlockBytes*8) + j + (channels*(BlockBytes*8)*(sec_num-1))];
				  }
				  else{
				      oneResult = (float)LPFdata[iChannel*(BlockBytes*8) + j + (channels*(BlockBytes*8)*(sec_num-1))];
				  }
				}
				//oneResult = Shaper_out[iChannel*(BlockBytes*8) + j];
				//note, DSD does x0.5 when converting WAV to DSD
				if(outputbit == 64){
					doubleResult = doubleResult * 2.0;
					oneResult = doubleResult;
				}
				else {
					oneResult = oneResult * 2.0f;
				}
				
				//change to PCM value
				PCMvalue = (unsigned long)(((oneResult + 1.0f)/2.0f) * 256 * 256 * 256);
				L = (PCMvalue & 0xFF);
				M = (PCMvalue >> 8) & 0xFF;
				H = (PCMvalue >> 16) & 0xFF;
				//invert H sign bit, XOR
				H = H ^ 0x80;
				//write PCM value to file
				if( outputbit == 32 || outputbit == 64){
					if(outputbit == 32){
						Floatdata[WriteBufferPointer] = oneResult;
					}
					else {
						Doubledata[WriteBufferPointer] = doubleResult;
					}
					WriteBufferPointer++;
				}
				else {
					if (outputbit == 24){
						WriteBuffer[WriteBufferPointer] = L; WriteBufferPointer++;
						WriteBuffer[WriteBufferPointer] = M; WriteBufferPointer++;
						WriteBuffer[WriteBufferPointer] = H; WriteBufferPointer++;
					} else {	//use 16bit
						WriteBuffer[WriteBufferPointer] = M; WriteBufferPointer++;
						WriteBuffer[WriteBufferPointer] = H; WriteBufferPointer++;
					}		
				}	
			}
		}
		// Å‰‚ÆÅŒã‚Í‘‚©‚È‚¢
		if(pCounter != 0 || pCounter + BlockBytes * 2 < len){
			if(outputbit == 32 || outputbit == 64){
				if(outputbit == 64){
					fwrite(Doubledata, 1, WriteBufferPointer*sizeof(double), wf);
				}
				else {
					fwrite(Floatdata, 1, WriteBufferPointer*sizeof(float), wf);
				}
			}
			else {
				fwrite(WriteBuffer, 1, WriteBufferPointer, wf);
			}
		}
		pCounter += BlockBytes * 2;
		printf("processed %d\r", pCounter);
	}

	printf("\n");
	fclose(wf);
	
	free(p);
	free(DSDdata_int);
	free(LPFdata);
	free(Shaper_out);
	free(WriteBuffer);
		free(Doubledata);
		free(Floatdata);
	
	end = omp_get_wtime();
	printf("%.4fSeconds, %d Bytes, %.2f KBytes/Sec \n",(double)(end-start), len, (double)(len/1000) / (double)(end-start) );

	printf("Finished DSD to WAV conversion.\n");

	return 0;

}
