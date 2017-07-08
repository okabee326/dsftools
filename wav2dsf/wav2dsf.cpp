// wav2dsf.cpp.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//
//#include "stdafx.h"
//#include <unistd.h>

//WAV to DSF converter (now supports 44.1/16 or 24 Stereo)

#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <memory.h>
#include <immintrin.h>
//for execution time
#include <time.h>
#include <omp.h>

//For FIR processing
int DEFTAPS = 32768;
double K_alpha = 10.0;

//data to keep FIR history, for each channel. (2,3 is for reverse test)
double** h_idata;//[4][DEFTAPS * 2];
double* h_idata_base;

//Processing, per DSD 4096 bytes = 32768 sample / 2.8MHz = 512 samples of 44100. (x64 oversample)
int oversampling = 256;
#define iSamples 512

//Shaper routine internal signal history
double D[2][7];	//Filter history, for L/R

				// How to debug long Signal array. Scilab is easy to prot. change XXXXXX to the array you want to see.
				//	// use scilab, -->  M = fscanfMat("c:\temp\XXXXXXXX.txt");plot(M);
				//	FILE * f1;
				//	_wfopen_s(&f1, L"C:\\Temp\\XXXXXXXX.txt", L"w");
				//	for (int x = 0 ; x < oversampling*iSamples; x++)
				//	{
				//		fprintf(f1, "%3.15lf\n", XXXXXXXX[x]);
				//	}
				//	fclose(f1);


				//Wave Utility
unsigned long read4bytes(FILE *f)
{
	unsigned char buf[4];

	if (fread(buf, 4, 1, f) != 1) {
		fprintf(stderr, "Read error\n");
		exit(1);
	}
	return ((256LU * buf[3] + buf[2]) * 256LU + buf[1]) * 256LU + buf[0];
}

unsigned read2bytes(FILE *f)
{
	unsigned char buf[2];

	if (fread(buf, 2, 1, f) != 1) {
		fprintf(stderr, "Read error\n");
		exit(1);
	}
	return 256U * buf[1] + buf[0];
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
	int ret = fwrite(buf, 1, sizeof(buf), f);
	if (ret != 4)
		fprintf(stderr, "Write error\n");
	return;
}


//FIR utility
//Bessel Func, for Kaiser window
double Bessel(double alpha)
{
	double delta = 1e-12;
	double BesselValue;
	double Term, k, F;

	k = 0.0;
	BesselValue = 0.0;
	F = 1.0;
	Term = 0.5;
	while ((Term < -delta) || (delta < Term)) {
		k = k + 1.0;						// k = step 1,2,3,4,5
		F = F * ((alpha / 2.0) / k);			// f(k+1) = f(k)*(c/k),f(0)=1. c is alpha/2. c/k -> 0
		Term = F*F;							// Current term value
		BesselValue = BesselValue + Term;	// Sum(f(k)^2)
	}
	return BesselValue;
}

//Generate Kaiser window, then apply to FIR coefficients
void DoKaiser(double * FIRCoeff, double K_Alpha)
{
	int i;
	double  Denom;

	//double	Kwindow[DEFTAPS];		//Kaiser Window Parameter
	double*	Kwindow = (double*)malloc(sizeof(double)* DEFTAPS);

	Denom = Bessel(K_Alpha);

	double center = (double)(DEFTAPS - 1) / 2;

	//#pragma omp parallel
	//{
	//#pragma omp for
	for (i = 0; i < DEFTAPS; i++) {
		double Kg, Kd, Numer;
		Kg = (double)(i*1.0 - center) / center;		//Regulated Distance, -1 to 1
		Kd = (double)(K_Alpha * sqrt(1.0 - Kg*Kg));	//kaiser Distance
		Numer = Bessel(Kd);
		Kwindow[i] = Numer / Denom;
	}
	//#pragma omp for
	for (i = 0; i < DEFTAPS; i++) {
		FIRCoeff[i] = FIRCoeff[i] * Kwindow[i];
	}
	//}

	free(Kwindow);
}

//Generate Frequency Responce.
void CreateFres(long SampleFreq, int fFreqLow, int fFreqHi, double * FreqResR)
{
	// gain = pow(10.0, desired_dB / 20.0);
	// There are folding at Fs/2. _____L~~~~~H_____|_____H'~~~L'____. |=Fs/2

	int i;
	int taps_half = DEFTAPS / 2;
	double stopgain = pow(10.0, -200.0 / 20.0);
	//#pragma omp parallel
	//{
	//	#pragma omp for
	for (i = 0; i < taps_half; i++) {
		double CurrentFreq = (double)(  (double)SampleFreq * (i + 1) / (double)DEFTAPS);
		if (CurrentFreq < fFreqLow) {
			//Stop Area 1
			FreqResR[i] = (double)stopgain;
		}
		else if (CurrentFreq < fFreqHi) {
			//Pass Area 1
			FreqResR[i] = 1.0;
		}
		else if (CurrentFreq <= SampleFreq / 2) {
			//Stop Area 2
			FreqResR[i] = (double)stopgain;
		}
	}
	//#pragma omp for
	for (i = taps_half; i < DEFTAPS; i++) {
		//Mirror Freq Responce
		FreqResR[i] = FreqResR[DEFTAPS - i - 1];
	}
	//}
	/*
	FILE *f;
	f = fopen("freq.txt", "w");

	for (int j = 0; j<DEFTAPS; j++) {
		fprintf(f, "[%d] %3.8lf\n", j, FreqResR[j]);
	}
	fclose(f);
	*/
}

//make FIR coeff, from Frequency Response.
void CalcCoeffs(double * FreqResR, double * FIRCoeff)
{
	//double costable[DEFTAPS];
	//double Tempcoeff_r[DEFTAPS];
	double* costable = (double*)malloc(sizeof(double) * DEFTAPS);
	double* Tempcoeff_r = (double*)malloc(sizeof(double) * DEFTAPS);

#pragma omp parallel for
	for (int i = 0; i < DEFTAPS; i++) {
		costable[i] = (double)cos(2.0 * M_PI * i / DEFTAPS);
	}
	int c = 0;
#pragma omp parallel for
	for (long i = 0; i < DEFTAPS; i++) {
		Tempcoeff_r[i] = 0.0;
		double Coeff;
		Coeff = 0.0;
		for (long j = 0; j < DEFTAPS; j++) {
			long index = (i * j) % DEFTAPS;
			Coeff += FreqResR[j] * costable[index];
		}
		Tempcoeff_r[i] = Coeff / (double)DEFTAPS;
	}
	for (int i = 0; i < DEFTAPS / 2; i++) {
		FIRCoeff[i] = (double)Tempcoeff_r[DEFTAPS / 2 - i];
	}
	for (int i = DEFTAPS / 2; i < DEFTAPS; i++) {
		FIRCoeff[i] = (double)Tempcoeff_r[i - (DEFTAPS / 2)];
	}
/*
	printf("FIRCoeff[16384]: %3.14lf\n", FIRCoeff[16384]);

	FILE *f;
	f = fopen("Tempcoeff_r.txt", "w");

	for (int j = 0; j<DEFTAPS; j++) {
		fprintf(f, "[%d] %3.8lf\n", j, Tempcoeff_r[j]);
	}
	fclose(f);
*/

	free(costable);
	free(Tempcoeff_r);
}


void CalcFres(double * FIRCoeff)
{

	int i, k;
	double	omega;		//angular frequency
	double	Resp_Real;	//Frequency Responce, Real
	double	Resp_Image;	//Frequency Responce, imaginary
	double	AmpABS2;	//square of amplitude characteristic
	int TAPS_HALF = DEFTAPS / 2;

	//double Gain[DEFTAPS / 2];
	double* Gain = (double*)malloc(sizeof(double)*DEFTAPS / 2);

	for (i = 0; i < TAPS_HALF; i++)
	{
		//omega = 0 to 2Pi (rad)
		omega = (i / (DEFTAPS / 360.0) * 2.0 * M_PI) / 360.0;

		//initialize
		Resp_Real = 0.00;
		Resp_Image = 0.00;

		//accumulate for all TAP of FIR
		for (k = 0; k < DEFTAPS; k++)
		{
			Resp_Real = Resp_Real + FIRCoeff[k] * cos(k * omega);
			Resp_Image = Resp_Image + FIRCoeff[k] * sin(k * omega);
		}
		//calculate square of amplitude characteristic
		AmpABS2 = Resp_Real * Resp_Real + Resp_Image * Resp_Image;

		//get dB value for amplitude characteristic
		Gain[i] = 10 * log10(AmpABS2);
	}

	//debug
	/*
	FILE * f;
	f = fopen("/tmp/FreqGain.txt", "w");
	for (int i = 0 ; i < DEFTAPS/2; i++)
	{
	fprintf(f, "%3.8lf\n", Gain[i]);
	}
	fclose(f);
	*/

	free(Gain);
}

/*
void OptCoeff(double* in, double** out, int step)
in:  double[DEFTAPS]
out: double[step][DEFTAPS/step]
step: oversampling / (SamplingRate / 44100)
*/
void OptCoeff(double* in, double** out, int step) {
	if (step < 1) {
		return;
	}

	for (int i = 0; i<step; i++) {
		int jStart;
		if (i % step == 0) {
			jStart = 0;
		}
		else {
			jStart = step - (i % step);
		}
		for (int j = 0; j<DEFTAPS / step; j++) {
			out[i][j] = in[jStart + step*j];
		}
	}
	
	FILE * f;
	f = fopen("optcoeff.txt", "w");
	for (int i = 0 ; i < step; i++)
	{
	for(int j=0;j<DEFTAPS / step;j++){
	fprintf(f, "[%d][%d] %3.8lf\n",i, j, out[i][j]);
	}
	}
	fclose(f);

	f = fopen("coeff.txt", "w");

	for(int j=0;j<DEFTAPS;j++){
	fprintf(f, "[%d] %3.8lf\n", j, in[j]);
	}
	fclose(f);
	
}

/*
void ProcessFIR_SIMD(
input: non-oversampled wave
output: oversampled wave
samples: input samples
coeffs: optimised coeffs
channel: channel
skips : oversampling / (SamplingRate / 44100)
)
*/
void ProcessFIR_SIMD(double * input, double * output, int Samples, double ** coeffs, int channel, int skips) {
	double* in = input;
	double* out = output;
	double** FIRCoeff = coeffs;

	int taps = DEFTAPS / skips; // 32768taps, 44100hz/x256 : 128

	for (size_t i = 0; i < Samples; i++) {
		h_idata[channel][taps + i] = in[i];
	}

	double gain_temp = (double)(skips / 2);

#pragma omp parallel for
	for (int i = 0; i < Samples; i++) {
		//printf("%d\n",i);
#ifdef _MSC_VER
		double __declspec(align(32)) dOuts[4];
		//double __declspec(align(32)) in_buf[taps];
		
#else
		double __attribute__((aligned(32))) dOuts[4];
		//double __attribute__((aligned(32))) in_buf[taps];
#endif
		double* in_buf = (double*)_mm_malloc(sizeof(double) * taps, 32);

		//__m256d* vx;
		//__m256d* vz =  (__m256d*)(dOuts);
		double dOut;
		dOut = 0.0;

		//after j = 0 sliding in_buf
		// in many cores, no assurance to in_buf data.
		memcpy(in_buf, &h_idata[channel][i], sizeof(double)*taps);



		//AVX	
#ifdef __AVX__	
		dOut = 0.0;
		for (int k = 0; k<(taps); k += 4) {
			__m256d x = _mm256_load_pd(&in_buf[k]);
			__m256d y = _mm256_load_pd(&FIRCoeff[0][k]);
			__m256d z = _mm256_load_pd(&dOuts[k]);

			z = _mm256_mul_pd(x, y);
			/*
			dOut += (double)z[0];
			dOut += (double)z[1];
			dOut += (double)z[2];
			dOut += (double)z[3];
			*/
			_mm256_store_pd(dOuts, z);
			dOut += dOuts[0];
			dOut += dOuts[1];
			dOut += dOuts[2];
			dOut += dOuts[3];
		}

#else
		//Normal
		dOut = 0.0;
		for (int k = 0; k<taps; k++) {
			dOut += in_buf[k] * FIRCoeff[0][k];
		}
#endif

		out[i*skips + 0] = dOut * gain_temp;
/*
		if (out[i*skips + 0] > 1.0) {
			for (int k = 0; k < taps; k++) {
				printf(" dOut[%d]: %3.14lf\n", k, FIRCoeff[0][k]);
			}
		}
*/
		//sliding h_idata
		memcpy(in_buf, &h_idata[channel][i + 1], sizeof(double)*taps);

		for (int j = 1; j<skips; j++) {
			dOut = 0.0;

			//AVX
#ifdef __AVX__	
			for (int k = 0; k<(taps); k += 4) {
				__m256d x = _mm256_load_pd(&in_buf[k]);
				__m256d y = _mm256_load_pd(&FIRCoeff[j][k]);
				__m256d z = _mm256_load_pd(&dOuts[k]);

				z = _mm256_mul_pd(x, y);

				/*
				dOut += (double)z[0];
				dOut += (double)z[1];
				dOut += (double)z[2];
				dOut += (double)z[3];
				*/
				_mm256_store_pd(dOuts, z);
				dOut += dOuts[0];
				dOut += dOuts[1];
				dOut += dOuts[2];
				dOut += dOuts[3];
			}
#else

			//Normal
			for (int k = 0; k<(taps); k++) {
				dOut += in_buf[k] * FIRCoeff[j][k];
			}
#endif

			out[i*skips + j] = dOut * gain_temp;

		}

		_mm_free(in_buf);

	}

	// taps < Samples under DEFTAPS=512*oversampling
	//if( DEFTAPS <= Samples * oversampling){
	//	memcpy(h_idata[channel], &h_idata[channel][Samples-taps-1], sizeof(double)*taps);
	//}
	//	else {
	for (int i = 0; i < taps; i++) {
		h_idata[channel][i] = h_idata[channel][i + Samples];
	}
	//	}
}


//FIR main Routine
void ProcessFIR(double * input, double * output, int Samples, double * coeffs, int channel, int skips, int * jStarts)
{
	//mode: 0= normal, 1=reverse

	double* in = input;
	double* out = output;
	double* FIRCoeff = coeffs;

	//__builtin_assume_aligned(h_idata[channel], 32);
	//__builtin_assume_aligned(FIRCoeff, 32);

	//add input to input buffer
	for (size_t i = 0; i < Samples; i++) {
		h_idata[channel][DEFTAPS + i] = in[i];
	}

	//float dOut = 0.0;

	//if (skips > 0) {//FAST mode, for LPF oversampled signal (almost 0)

	// optimise point
	double register gain_temp = (double)(skips / 4);
	/*double register __attribute__((aligned(32))) gain_d[4] = {
	gain_temp, gain_temp, gain_temp, gain_temp
	};*/

#pragma omp parallel for
	for (int i = 0; i < Samples; i++) {
		//double register __attribute__((aligned(32))) dOuts[4];
		//double register __attribute__((aligned(32))) in_buf[4];
		//double register __attribute__((aligned(32))) coeff_buf[4];

		//_mm_prefetch(h_idata + DEFTAPS, _MM_HINT_T2);
		//_mm_prefetch(FIRCoeff + DEFTAPS, _MM_HINT_T2);

		double register dOut = 0.0;

		//input buffer is oversampled, so 0, 64, 128,... has data but others are 0.
		// Normal

		for (int j = jStarts[i]; j < DEFTAPS; j = j + skips) {
			//normal method
			dOut += h_idata[channel][i + j] * FIRCoeff[j];
		}
		//63 of 0 was inserted, so 1.0 -> 1.0/64. shift the level back
		out[i] = dOut * gain_temp; // 4 is gain

								   /*
								   //AVX
								   for (int j = jStarts[i]; j < DEFTAPS; j += 4*skips) {
								   in_buf[0] = h_idata[channel][i + j];
								   in_buf[1] = h_idata[channel][i + j + skips];
								   in_buf[2] = h_idata[channel][i + j + skips * 2];
								   in_buf[3] = h_idata[channel][i + j + skips * 3];
								   coeff_buf[0] = FIRCoeff[j];
								   coeff_buf[1] = FIRCoeff[j + skips];
								   coeff_buf[2] = FIRCoeff[j + skips * 2];
								   coeff_buf[3] = FIRCoeff[j + skips * 3];

								   __m256d vx =  _mm256_load_pd(in_buf);
								   __m256d vy =  _mm256_load_pd(coeff_buf);
								   __m256d vz =  _mm256_load_pd(dOuts);

								   vz = _mm256_mul_pd(vx, vy);

								   //vx =  _mm256_load_pd(dOuts);


								   //vz = _mm256_mul_pd(vz, v_gain);

								   _mm256_store_pd(dOuts, vz);


								   //out[i] += dOuts[0]*(double)skips*0.25 + dOuts[1]*(double)skips*0.25 +
								   // 			dOuts[2]*(double)skips*0.25 + dOuts[3]*(double)skips*0.25;
								   dOut += dOuts[0] + dOuts[1] + dOuts[2] + dOuts[3];

								   }
								   out[i] = dOut * gain_temp;
								   // end AVX
								   */
								   /*
								   static const size_t double_size = 4;

								   for (int j = 0; j < DEFTAPS/skips; j+=double_size) {
								   __m256d vx =  _mm256_load_pd(&h_idata[channel][i] + j);
								   __m256d vy =  _mm256_load_pd(FIRCoeff + j);
								   __m256d vz =  _mm256_load_pd(dOuts);

								   vz = _mm256_mul_pd(vx, vy);
								   for(int k=0;k<4;k++){
								   dOut += vz[k];
								   }
								   }
								   _mm_free(dOuts);
								   */
								   // end AVX
								   /*
								   dOut = dOut * (double)skips / 4.0; // 4 is gain

								   for(int j=0;j<skips;j++){
								   out[i*skips+j] = dOut;
								   }
								   */

	}
	//	}else{		

	//AVX
	/*		static const size_t double_size = 4;
	const size_t end = DEFTAPS / double_size;


	for (int i = 0; i < Samples; i++) {
	double* dOuts = (double*)_mm_malloc(sizeof(double)*4, 32);



	//printf("vx: %lu vy: %lu vz: %lu\n", vx,vy,vz);

	//#pragma omp parallel for
	for (size_t j = 0; j < DEFTAPS; j+=double_size){
	__m256d vx =  _mm256_load_pd(&h_idata[channel][i] + j);
	__m256d vy =  _mm256_load_pd(FIRCoeff + j);
	__m256d vz =  _mm256_load_pd(dOuts);
	vz = _mm256_mul_pd(vx, vy);
	//printf("%d\n", j);
	for (int k = 0; k < 4; k++){
	out[i] += dOuts[k];
	//printf("%d: %lf, %lf\n", j, in[i], dOuts[0]);
	}
	}


	//out[i] = dOut;

	_mm_free(dOuts);
	}


	*/
	//	}
	/*
	for (int j = 0; j < DEFTAPS; j++){
	dOut += h_idata[channel][i + j] * FIRCoeff[j];
	}
	*/

	//slide input buffer
	//float temp_f = 0.0f;

	for (int i = 0; i < DEFTAPS; i++) {
		h_idata[channel][i] = h_idata[channel][i + Samples];
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

	printf("Theta1 = %.15f\n", Theta1);
	printf("Theta2 = %.15f\n", Theta2);
	printf("Theta3 = %.15f\n", Theta3);

	printf("s1 = %.15f\n", s1);
	printf("s2 = %.15f\n", s2);
	printf("s3 = %.15f\n", s3);

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
	B[1] = -7.0 * Pole;
	B[2] = 21.0 * Pole * Pole;
	B[3] = -35.0 * Pole * Pole * Pole;
	B[4] = 35.0 * Pole * Pole * Pole * Pole;
	B[5] = -21.0 * Pole * Pole * Pole * Pole * Pole;
	B[6] = 7.0 * Pole * Pole * Pole * Pole * Pole * Pole;
	B[7] = -1.0 * Pole * Pole * Pole * Pole * Pole * Pole * Pole;

	printf("A: %+.8f,%+.8f,%+.8f,%+.8f,%+.8f,%+.8f,%+.8f\n", A[1], A[2], A[3], A[4], A[5], A[6], A[7]);
	printf("B: %+.8f,%+.8f,%+.8f,%+.8f,%+.8f,%+.8f,%+.8f\n", B[1], B[2], B[3], B[4], B[5], B[6], B[7]);
	
	/*
	//Scilab command file for display.
	FILE * f;
	if ( (f = fopen("7thNoiseShaper.sce", "w")) != NULL ) {
	fprintf(f, "z=%%z;\n");
	fprintf(f, "H1=(1-z^(-1))*(1-%.8f*z^(-1)+z^(-2))*(1-%.8f*z^(-1)+z^(-2))*(1-%.8f*z^(-1)+z^(-2));\n",s1,s2,s3);
	fprintf(f, "H=(1%+.15f*z^(-1)%+.15f*z^(-2)%+.15f*z^(-3)%+.15f*z^(-4)%+.15f*z^(-5)%+.15f*z^(-6)%+.15f*z^(-7));\n",A[1],A[2],A[3],A[4],A[5],A[6],A[7]);
	fprintf(f, "D=(1%+.15f*z^(-1)%+.15f*z^(-2)%+.15f*z^(-3)%+.15f*z^(-4)%+.15f*z^(-5)%+.15f*z^(-6)%+.15f*z^(-7));\n",B[1],B[2],B[3],B[4],B[5],B[6],B[7]);
	fprintf(f, "sys=syslin(\"d\",H1/D);\n");
	fprintf(f, "clf();f1=scf(0);plzr(sys);f1=scf(1);gainplot(sys,1e-3,1e-1,0.001);\n");
	fprintf(f, "f1=scf(0);\n");
	fclose(f);
	}
	*/
	
	printf("Shaper Coeff calculation Done\n");
}

//7th noise shaper function
void Shaper_Process(double * A, double * B, double * d, double * input, double * output, int Samples)
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
	for (int i = 0; i < Samples; i++) {
		double Q_inner1 = a1 * d[1] + a2 * d[2] + a3 * d[3] + a4 * d[4] + a5 * d[5] + a6 * d[6] + a7 * d[7];
		double x_in = input[i] + Q_inner1;
		double y_out;
		if (x_in >= 0.0) {
			y_out = 1.0;
		}
		else {
			y_out = -1.0;
		}
		double Q_error = x_in - y_out;
		double Q_inner2 = b1 * d[1] + b2 * d[2] + b3 * d[3] + b4 * d[4] + b5 * d[5] + b6 * d[6] + b7 * d[7];

		d[7] = d[6];
		d[6] = d[5];
		d[5] = d[4];
		d[4] = d[3];
		d[3] = d[2];
		d[2] = d[1];
		d[1] = Q_error + Q_inner2;	//stored into Delay register

		output[i] = y_out;
	}
}


int main(int argc, char * argv[])
{
	/*if ((argc > 3) || (argc<2)){
	printf("usage: Wave2DSD infile outfile, or Wave2DSD infile\n");
	return -1;
	}
	*/

	char* infile = 0;
	char* outfile = 0;

	// default coeff
	//Shaper_Coeff(0.97, 0.77, 0.43, 0.85, A, B, true);
	// wav2dsff p:2 coeff
	//Shaper_Coeff(0.95, 0.74, 0.42, 0.85, A, B, true);
	//double zero[4] = { 0.95, 0.74, 0.42, 0.85 };
	//double zero[4] = { 0.969, 0.757, 0.435, 0.85 };
	//double zero[4] = { 0.969, 0.766, 0.422, 0.85 };
	double zero[4] = { 0.957, 0.763, 0.419, 0.85 };
	int cutoff = -1;

	for (int i = 1; i<argc; i++) {
		if (argv[i][0] != '-') {
			if (infile == 0) {
				infile = argv[i];
			}
			else {
				outfile = argv[i];
			}
			continue;
		}
		// k_alpha setting
		if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--alpha")) {
			if (i + 1 != argc) {
				K_alpha = atof(argv[i + 1]);
				if (K_alpha <= 0.0f) {
					printf("K_alpha is must be over 0.\n");
					return -1;
				}
				i++;
			}
			continue;
		}
		// oversampling rate setting
		if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--oversampling")) {
			if (i + 1 != argc) {
				oversampling = atoi(argv[i + 1]);
				if (oversampling % 2 != 0) {
					printf("oversampling value is not even.\n");
					return -1;
				}
				i++;
			}
			continue;
		}
		// taps setting
		if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--taps")) {
			if (i + 1 != argc) {
				DEFTAPS = atoi(argv[i + 1]);
				if (DEFTAPS % 2 != 0 && DEFTAPS >= 512) {
					printf("TAPS value must be even and over 512.\n");
					return -1;
				}
				i++;
			}
			continue;
		}
		if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--lowpass")) {
			if (i + 1 != argc) {
				cutoff = atoi(argv[i + 1]);
				if ((cutoff < 1)) {
					printf("lowpass filter cutoff freq must be over 0 .\n");
					return -1;
				}
				i++;
			}
			continue;
		}
		// zero point setting
		if (!strcmp(argv[i], "-z") || !strcmp(argv[i], "--zero")) {
			if (i + 4 < argc) {
				for (int j = 0; j<4; j++) {
					zero[j] = atof(argv[i + j + 1]);
				}

				i += 4;
			}
			else {

				printf("Zero Point option needs 4 parameters.\n");
				return -1;

			}
			continue;
		}
		// zero point coeff select from collection
		// thanks to wav2dsff.exe
		if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--coeff")) {
			if (i + 1 != argc) {
				int option = atoi(argv[i + 1]);
				if (option <= 0 || option > 5) {
					printf("coeff number is integer from 1 to 5.\n");
					return -1;
				}
				else {
					switch (option) {
					case 1:
						zero[0] = 0.94; zero[1] = 0.73; zero[2] = 0.41;
						break;
					case 3:
						zero[0] = 0.96; zero[1] = 0.76; zero[2] = 0.43;
						break;
					case 4:
						zero[0] = 0.97; zero[1] = 0.77; zero[2] = 0.43;
						break;
					case 5:
						zero[0] = 0.98; zero[1] = 0.78; zero[2] = 0.44;
						break;
					default:
						zero[0] = 0.95; zero[1] = 0.74; zero[2] = 0.42;
						break;
					}
				}
				i++;
			}
			continue;
		}
		// usage
		printf("usage: wav2dsf infile outfile [-a alpha] [-t taps] [-v oversampling] [-c coeff number] [-z zero1 zero2 zero3 zero4]\n");
		return -1;
	}

	if (infile == 0 || outfile == 0) {
		// usage
		printf("usage: wav2dsf infile outfile [-a alpha] [-t taps] [-v oversampling] [-c coeff number] [-z zero1 zero2 zero3 zero4]\n");
		return -1;
	}

	printf("infile: %s\n", infile);
	printf("outfile: %s\n", outfile);
	printf("taps: %d\n", DEFTAPS);
	printf("alpha: %lf\n", K_alpha);



	//Wave File processing
	char *filename = infile;
	FILE *f;
	int channels, bits;
	unsigned long len;
	unsigned char s[10];
	f = fopen(filename, "rb");

	//can open?
	if (f == NULL) {
		printf("Can not open %s\n", filename);
		return -1;
	}
	printf("finename = '%s'\n", filename);

	//Prepare output file.
	char *writefilename;
	/*if (argc == 3) {
	writefilename = argv[2];
	}
	if (argc == 2) {
	//append extention ".DSF".
	writefilename = strcat(filename, ".DSF");
	}
	*/
	writefilename = outfile;

	//RIFF?
	if (fread(s, 4, 1, f) != 1) {
		printf("Read error\n");
		fclose(f);
		return -1;
	}
	if (memcmp(s, "RIFF", 4) != 0) {
		printf("Not a RIFF format\n");
		fclose(f);
		return -1;
	}
	printf("[RIFF] (%lu bytes)\n", read4bytes(f));

	//WAVEfmt?
	if (fread(s, 8, 1, f) != 1) {
		printf("Read error\n");
		fclose(f);
		return -1;
	}
	if (memcmp(s, "WAVEfmt ", 8) != 0) {
		printf("Not a WAVEfmt format\n");
		fclose(f);
		return -1;
	}
	//Chunk Length?
	len = read4bytes(f);
	printf("[WAVEfmt ] (%lu bytes)\n", len);
	if (len < 16) {
		printf("Length of WAVEfmt must be 16\n");
		return -1;
	}

	//Format info
	unsigned int type = read2bytes(f);
	printf("  Data type = %u (1 = PCM)\n", type);
	channels = read2bytes(f);
	printf("  Number of channels = %u (1 = mono, 2 = stereo)\n", channels);
	int SamplingRate = read4bytes(f);
	printf("  Sampling rate = %luHz\n", SamplingRate);
	if (SamplingRate == 48000) {
		SamplingRate = 44100;
	}
	if (SamplingRate == 96000) {
		SamplingRate = 88200;
	}
	if (SamplingRate == 192000) {
		SamplingRate = 176400;
	}

	if (cutoff == -1) {
		cutoff = SamplingRate / 2;
	}

	printf("  Bytes per second = %lu\n", read4bytes(f));
	int BytesPerSample = read2bytes(f);
	printf("  Bytes per sample = %u\n", BytesPerSample);
	bits = read2bytes(f);
	printf("  Bits per sample = %u\n", bits);

	if (len != 16) {
		fseek(f, len - 16, SEEK_CUR);
		fread(s, 4, 1, f);
		if (memcmp(s, "fact", 4) == 0) {

			len = read4bytes(f);
			printf("found fact chunk: %d bytes", len);
			//return -1;
			fseek(f, len, SEEK_CUR);
		}
		else {
			fseek(f, -4L, SEEK_CUR);
		}
	}

	//read until 'data' chunk
	while (fread(s, 4, 1, f) == 1) {
		len = read4bytes(f);
		s[4] = 0;
		printf("[%s] (%lu bytes)\n", s, len);
		if (memcmp(s, "data", 4) == 0) break;
		for (int i = 0; i < (int)len; i++)
		{
			printf("%02x ", fgetc(f));
		}
		printf("\n");
	}


	//Bytes count, processed at once. high samplingrate, more samples processed.
	long dwWaveBytes = iSamples * BytesPerSample * SamplingRate / 44100;	// L,R, 16bit/24bit
	long pre_zerofillsize = len % dwWaveBytes;
	//get memory for "len" bytes
	unsigned char * p;
	if (type == 3) {
		//float* p;
		p = (unsigned char*)malloc(len + dwWaveBytes + pre_zerofillsize);
		memset(p + len, 0, dwWaveBytes);	//fill 0 at tail
		memset(p, 0, dwWaveBytes);	//fill 0 at head
		fread(p + pre_zerofillsize, len, 1, f);
		fclose(f);
	}
	else {

		//add tail, when len % 512 > 0
		p = (unsigned char *)malloc(len + dwWaveBytes);
		memset(p + len, 0, dwWaveBytes);	//fill 0 at tail
		fread(p, len, 1, f);	//p has wave format data, 16bit
		fclose(f);
	}


	//Prepare output file.
	FILE * wf;
	wf = fopen(writefilename, "wb");
	//DSD header
	fputc('D', wf);
	fputc('S', wf);
	fputc('D', wf);
	fputc(' ', wf);
	write4bytes(wf, (unsigned long)0x0000001C); //chunk size 28

	unsigned long totalfilesize;
	totalfilesize = 28 + 52 + 12;	//for header

									//this is how many samples will be processed
	unsigned long long total_samples = len / BytesPerSample; //BytesPerSmaple = 2 or 3 * channels

														//512 samples is one convert block. for 44100, 512->32768->4096 88200, 512*2->32768->4096.
	unsigned long convert_blocks2 = (unsigned long)ceil((double)total_samples / (SamplingRate / 44100) / iSamples) * (oversampling / 64);
	printf("convert_blocks: %lu\n", convert_blocks2);

	//PCM 512 samples -> x64, 32768 / 2.8M / 1bit DSD = 4096 bytes.
	unsigned long long dsd_data_length = 4096 * convert_blocks2 * channels;

	totalfilesize += dsd_data_length;
	printf("totalfilesize: %lu\n", totalfilesize);

	unsigned long long dsd_total_samples = 4096 * convert_blocks2 * 8; //... this number can be 64bit for large file

	unsigned long higher = totalfilesize / 4294967296;
	unsigned long lower = totalfilesize % 4294967296;

	write4bytes(wf, (unsigned long)higher); //4 bytes 0
	write4bytes(wf, (unsigned long)lower);
	write4bytes(wf, (unsigned long)0x00000000); //4 bytes 0
	write4bytes(wf, (unsigned long)0x00000000); //no ID3 tag
	write4bytes(wf, (unsigned long)0x00000000); //4 bytes 0

												//fmt chunk
	fputc('f', wf);
	fputc('m', wf);
	fputc('t', wf);
	fputc(' ', wf);
	write4bytes(wf, (unsigned long)0x00000034); //chunk size 52
	write4bytes(wf, (unsigned long)0x00000000); //4 bytes 0
	write4bytes(wf, (unsigned long)0x00000001); //some ID?
	write4bytes(wf, (unsigned long)0x00000000); //4 bytes 0
	write4bytes(wf, (unsigned long)0x00000002); //stereo?
	write4bytes(wf, (unsigned long)0x00000002); //channels
	if (oversampling == 64) {
		write4bytes(wf, (unsigned long)0x002B1100); //2822400 sampling
	}
	if (oversampling == 128) {
		write4bytes(wf, (unsigned long)0x00562200); //2822400 sampling*2
	}
	if (oversampling == 256) {
		write4bytes(wf, (unsigned long)0x00AC4400); //2822400 sampling*4
	}
	write4bytes(wf, (unsigned long)0x00000001); //1 bit?
	write4bytes(wf, (unsigned long)dsd_total_samples);
	write4bytes(wf, (unsigned long)0x00000000); //4 bytes 0
	write4bytes(wf, (unsigned long)4096 * oversampling / 64); //4096
	write4bytes(wf, (unsigned long)0x00000000); //4 bytes 0

												//data chunk
	fputc('d', wf);
	fputc('a', wf);
	fputc('t', wf);
	fputc('a', wf);
	write4bytes(wf, (unsigned long)(dsd_data_length + 12));
	write4bytes(wf, (unsigned long)0x00000000); //4 bytes 0

	printf("dsd_data_size: %lu\n", dsd_data_length + 12);

	//debug fclose(wf);

	//variable for FIR Speedup
	int Skips = oversampling * 44100 / SamplingRate;

	//h_idata	
	//float h_idata[4][DEFTAPS * 2];
	h_idata = (double**)_mm_malloc(sizeof(double*) * 2, 32);
	h_idata_base = (double*)_mm_malloc(sizeof(double) * 4 * (DEFTAPS), 32);

	for (int i = 0; i<2; i++) {
		h_idata[i] = &h_idata_base[ i*(DEFTAPS) * 2 ];
		//memset(h_idata[i], 0, sizeof(double)*(DEFTAPS) * 2);
		for (int j = 0; j<DEFTAPS*2; j++) {
			h_idata[i][j] = 0.0;
		}
	}

	//Prepare FIR parameter
	//double FreqResR[DEFTAPS];
	double* FreqResR = (double*)malloc(sizeof(double)*DEFTAPS);
	memset(FreqResR, 0, sizeof(double)*DEFTAPS);
	//double FIRCoeff[DEFTAPS];
	double* FIRCoeff = (double*)_mm_malloc(sizeof(double)*DEFTAPS, 32);
	memset(FIRCoeff, 0, sizeof(double)*DEFTAPS);
	//memset(h_idata, 0, sizeof(float)*DEFTAPS*4);

	CreateFres(44100 * oversampling, 0, cutoff, FreqResR);
	CalcCoeffs(FreqResR, FIRCoeff);
	DoKaiser(FIRCoeff, K_alpha);

	

	double** FIRCoeff_opt = (double**)_mm_malloc(sizeof(double*)* Skips, 32);
	double*  OptCoeff_base = (double*)_mm_malloc(sizeof(double)* (DEFTAPS / Skips)*Skips, 32);
	for (int i = 0; i<Skips; i++) {
		FIRCoeff_opt[i] = OptCoeff_base + (DEFTAPS / Skips)*i;
		//memset(FIRCoeff_opt[i], 0, sizeof(double)*(DEFTAPS / Skips));	//initialized to 0
		for (int j = 0; j<(DEFTAPS / Skips); j++) {
			FIRCoeff_opt[i][j] = 0.0;
		}
	}

	OptCoeff(FIRCoeff, FIRCoeff_opt, Skips);

	//CalcFres(FIRCoeff);

	//Shaper Coefficient Calcuration
	double A[8];
	double B[8];
	// for Zero shift, 1.0 = 22050Hz. Pole Shift should be 0.85
	// Scilab can not track(up to 0.7) but works.
	//orig Shaper_Coeff(0.95, 0.74, 0.42, 0.85, A, B, true);
	//Shaper_Coeff(0.97, 0.77, 0.43, 0.85, A, B, true);
	Shaper_Coeff(zero[0], zero[1], zero[2], zero[3], A, B, true);

	//initialize history buffer for shaper
	for (int i = 0; i < channels; i++) {
		for (int j = 0; j < 7; j++) {
			D[i][j] = 0.0;
		}
	}

	//Prepare buffers
	//double FIR_in[channels][iSamples * oversampling]; //oversampled data
	//memset(FIR_in, 0, sizeof(double)*(channels * iSamples * oversampling));	//initialized to 0
	double** FIR_in = (double**)_mm_malloc(sizeof(double*)* channels, 32);
	double*  FIR_in_base = (double*)_mm_malloc(sizeof(double)* iSamples*oversampling*channels, 32);

	for (int i = 0; i<channels; i++) {
		FIR_in[i] = FIR_in_base + iSamples*oversampling*i;
		//memset(FIR_in[i], 0, sizeof(double)*(iSamples * oversampling));	//initialized to 0
		for (int j = 0; j<iSamples * oversampling; j++) {
			FIR_in[i][j] = 0.0;
		}
	}

	//FIR_in[1][5000] = 0.0;


	//double FIR_out[channels][iSamples * oversampling]; //FIR result
	//float* FIR_out = (float*)_mm_malloc(sizeof(float)* iSamples*oversampling,32);
	//memset(FIR_out, 0, sizeof(double)*(channels * iSamples * oversampling));	//initialized to 0
	double** FIR_out = (double**)_mm_malloc(sizeof(double*)* channels, 32);
	double*  FIR_out_base = (double*)_mm_malloc(sizeof(double)* iSamples*oversampling*channels, 32);
	for (int i = 0; i<channels; i++) {
		FIR_out[i] = FIR_out_base + iSamples*oversampling*i;
		//memset(FIR_out[i], 0, sizeof(double)*(iSamples * oversampling));	//initialized to 0
		for (int j = 0; j<iSamples * oversampling; j++) {
			FIR_out[i][j] = 0.0;
		}
	}

	/*
	double Shaper_out[channels][iSamples * oversampling];	//contains 0/1
	memset(Shaper_out, 0, sizeof(double)*(channels * iSamples * oversampling));	//initialized to 0
	*/
	double** Shaper_out = (double**)_mm_malloc(sizeof(double*)* channels, 32);
	double*  Shaper_out_base = (double*)_mm_malloc(sizeof(double)* iSamples * oversampling*channels, 32);
	for (int i = 0; i<channels; i++) {
		Shaper_out[i] = Shaper_out_base + iSamples*oversampling*i;
		//memset(FIR_out[i], 0, sizeof(double)*(iSamples * oversampling));	//initialized to 0
		for (int j = 0; j<iSamples * oversampling; j++) {
			Shaper_out[i][j] = 0.0;
		}
	}


/*
	unsigned char DSD_out[channels][iSamples * oversampling / 8];	//4096 bytes summarized
	memset(DSD_out, 0, (channels * iSamples * oversampling / 8));	//initialized to 0
	*/
	unsigned char** DSD_out = (unsigned char**)_mm_malloc(sizeof(unsigned char*)* channels, 32);
	unsigned char*  DSD_out_base = (unsigned char*)_mm_malloc(sizeof(double)* channels * iSamples * oversampling / 8, 32);
	for (int i = 0; i<channels; i++) {
		DSD_out[i] = DSD_out_base + iSamples * oversampling / 8 *i;
		//memset(FIR_out[i], 0, sizeof(double)*(iSamples * oversampling));	//initialized to 0
		for (int j = 0; j<iSamples * oversampling / 8; j++) {
			DSD_out[i][j] = 0;
		}
	}


	unsigned long pCounter = 0;	//wave file read counter
								/*
								int jStarts[iSamples];

								for(int i=0;i<iSamples;i++){
								if (i % Skips == 0) {
								jStarts[i] = 0;
								} else {
								jStarts[i] = Skips - (i % Skips);
								}
								}
								*/
	printf("Start Conversion...\n");
	double start, end;
	start = omp_get_wtime();

	while (pCounter < len + pre_zerofillsize) {
		int count = 0;
		int l = (len / dwWaveBytes) + 1;
		//for(count=0;count<l;count++) {
		//Loop for channels. stereo, then 0 , 1
		//pCounter = count * dwWaveBytes;
		//	#pragma omp for
		for (int currentChannel = 0; currentChannel < channels; currentChannel++) {
			//loop for Sample Block. 512 sample. it will be converted to 4096 Bytes DSD block.
			for (int j = 0; j < iSamples * SamplingRate / 44100; j++) {	//when 88200, take x2 samples
				unsigned char H = 0x00;
				unsigned char M = 0x00;
				unsigned char L = 0x00;
				unsigned char H2 = 0x00;
				unsigned long PCMvalue;
				double signal;
				if (type == 1) {
					if (BytesPerSample == 4) {
						L = 0x00;
						M = 0x00;
						H = *(p + pCounter + j * 4 + 0 + currentChannel*BytesPerSample / channels); //read actual data
						H2 = *(p + pCounter + j * 4 + 1 + currentChannel*BytesPerSample / channels);
					}
					if (BytesPerSample == 6) {
						L = 0x00;
						M = *(p + pCounter + j * 6 + 0 + currentChannel*BytesPerSample / channels); //read actual data
						H = *(p + pCounter + j * 6 + 1 + currentChannel*BytesPerSample / channels);
						H2 = *(p + pCounter + j * 6 + 2 + currentChannel*BytesPerSample / channels);
					}
					if (BytesPerSample == 8) {

						L = *(p + pCounter + j * 8 + 0 + currentChannel*BytesPerSample / channels); //read actual data
						M = *(p + pCounter + j * 8 + 1 + currentChannel*BytesPerSample / channels);
						H = *(p + pCounter + j * 8 + 2 + currentChannel*BytesPerSample / channels);
						H2 = *(p + pCounter + j * 8 + 3 + currentChannel*BytesPerSample / channels);
					}
					PCMvalue = H2 * 256L * 256L * 256L + H * 256L * 256L + M * 256L + L;
					if (PCMvalue < 0x80000000) {
						signal = (PCMvalue / 4294967296.0);
						signal = signal * 2.0;					//PCM Positive, 0 to 0.5 -> 0.0 to 1.0
					}
					else {
						signal = (PCMvalue / 4294967296.0);	//PCM Negative, 0.5 to 1.0
						signal = signal - 1.0;				// -> -0.5 to 0.0
						signal = signal * 2.0;				// -> -1.0 to 0.0
					}
				}
				else {
					if (type == 3) {
						if (BytesPerSample == 8) {
							signal = *((float*)(p + pCounter + j * 8 + 0 + currentChannel*BytesPerSample / channels));
						}
						if (BytesPerSample == 16) {
							signal = *((double*)(p + pCounter + j * 16 + 0 + currentChannel*BytesPerSample / channels));
						}
					}
				}

				signal = signal * 0.50f; //normalize, -6dB
										 //original
										 //FIR_in[currentChannel][j * Skips] = signal;	//oversample, others are 0
										 //input is not oversampled.
				FIR_in[currentChannel][j] = signal;
			}
			//do FIR (LPF) for FIR_in, to FIR_out
			//original length of FIR_in, out is 512 * 64.
			/*
			for (int k = 0; k < oversampling; k++){
			double* _in = &FIR_in[currentChannel][k*iSamples];
			double* _out = &FIR_out[currentChannel][k*iSamples];

			ProcessFIR(_in, _out, iSamples, FIRCoeff, currentChannel, Skips, jStarts);
			//ProcessFIR(FIR_in[currentChannel], FIR_out[currentChannel], iSamples * SamplingRate / 44100, FIRCoeff, currentChannel, Skips, jStarts, oversampling * 44100 / SamplingRate);
			}
			*/
			//void ProcessFIR_SIMD(double * input, double * output, int Samples, double ** coeffs, int channel, int skips)
			ProcessFIR_SIMD(FIR_in[currentChannel], FIR_out[currentChannel], iSamples* (SamplingRate / 44100), FIRCoeff_opt, currentChannel, Skips);
			/*
			if(pCounter / dwWaveBytes == 128){
			FILE * f1;
			f1 = fopen("h_idata.txt", "w");
			for (int x = 0 ; x < iSamples+DEFTAPS/Skips; x++)
			{
			fprintf(f1, "[%d]%3.15lf\n", x, h_idata[currentChannel][x]);
			}
			fclose(f1);
			}
			*/
		}
#pragma omp parallel for
		for (int currentChannel = 0; currentChannel < channels; currentChannel++) {
			//do 7th noise shaper for FIR_out, to Shaper_out
			Shaper_Process(A, B, &D[currentChannel][0], FIR_out[currentChannel], Shaper_out[currentChannel], iSamples * oversampling);

			//debug
			/*
			if ((currentChannel == 1) && (pCounter == dwWaveBytes * 56)) {
				FILE * f1;
				f1 = fopen("FIR_in.txt", "w");
				for (int x = 0; x < iSamples; x++)
				{
					fprintf(f1, "%3.15lf\n", FIR_in[1][x]);
				}
				fclose(f1);
				FILE * f2;
				f2 = fopen("FIR_out.txt", "w");
				for (int x = 0; x < oversampling*iSamples; x++)
				{
					fprintf(f2, "%3.15lf\n", FIR_out[1][x]);
				}
				fclose(f2);
				FILE * f3;
				f3 = fopen("Shaper_out.txt", "w");
				for (int x = 0; x < oversampling*iSamples; x++)
				{
					fprintf(f3, "%3.15lf\n", Shaper_out[1][x]);
				}
				fclose(f3);
				FILE * f4;
				f4 = fopen("h_idata.txt", "w");
				for (int x = 0; x < 640; x++)
				{
					fprintf(f4, "%3.15lf\n", h_idata[1][x]);
				}
				fclose(f4);
				

			}
			*/
						
			//printf("d0:%3.15lf\n", D[0][5]);
			
			//Bit to Byte loading, for DSD_out
			for (int m = 0; m < iSamples * oversampling / 8; m++) {
				DSD_out[currentChannel][m] = 0x00;
				for (int n = 0; n < 8; n++) {
					if (Shaper_out[currentChannel][m * 8 + n] > 0.0) {
						DSD_out[currentChannel][m] |= (0x01) << n;

					}
				}
			}
		}
		//write file.
		for (int currentChannel = 0; currentChannel < channels; currentChannel++) {
		  int ret;

		  //ret = fwrite (DSD_out[currentChannel] , 1 , sizeof(DSD_out[currentChannel]) , wf);
		  ret = fwrite(DSD_out[currentChannel], 1, sizeof(unsigned char)*iSamples * oversampling / 8, wf);

		  if (ret != sizeof(unsigned char)*iSamples * oversampling / 8 ) {
			fprintf(stderr, "DSD result Write error\n");
			return -1;
		  }
		}
		//} // end parallel
		pCounter += dwWaveBytes;
		count = pCounter / dwWaveBytes;

		printf("[%d / %d ] \r", (int)(count), l);

		//usleep(1);
	}
	fclose(wf);

	printf("\nOK.\n");
	_mm_free(FIR_in_base);
	_mm_free(FIR_out_base);
	_mm_free(FIR_in);
	_mm_free(FIR_out);

	_mm_free(Shaper_out_base);
	_mm_free(Shaper_out);

	free(FreqResR);

	//_mm_free(OptCoeff_base);
	//_mm_free(FIRCoeff_opt);

	//_mm_free(FIRCoeff);
	//file write
	/*
	for(int i=0;i<len;i+=dwWaveBytes){
	int ret = fwrite(DSD_memory[i], 1, sizeof(DSD_memory[i]), wf );

	if(ret != sizeof(DSD_memory[i]) ){
	fprintf(stderr, "DSD write error\n");
	break;
	}
	}
	*/
	printf("End Conversion.\n");
	end = omp_get_wtime();
	printf("%.4fSeconds, %d Bytes, %.2f KBytes/Sec \n", (double)(end - start), len, (double)(len / 1000) / (double)(end - start));

	//_mm_free(h_idata_base);
	//printf("\nOK\n");
	//free(h_idata);


	return 0;
}
