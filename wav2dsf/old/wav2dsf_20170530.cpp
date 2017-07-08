//WAV to DSF converter (now supports 44.1/16 or 24 Stereo)

#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <immintrin.h>
//for execution time
#include <time.h>
#include <omp.h>

//For FIR processing
#define DEFTAPS		65536
#define K_alpha	10.0f

//data to keep FIR history, for each channel. (2,3 is for reverse test)
float h_idata[4][DEFTAPS * 2];

//Processing, per DSD 4096 bytes = 32768 sample / 2.8MHz = 512 samples of 44100. (x64 oversample)
#define iSamples 512
#define oversampling 256

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
	while( (Term < -delta) || ( delta < Term) ){
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
	float  Denom;
	
	float	Kwindow[DEFTAPS];		//Kaiser Window Parameter

	Denom = Bessel(K_Alpha);	

	float center = (float)(DEFTAPS - 1)/2;
	
  //#pragma omp parallel
  //{
  ////#pragma omp for
	for (i = 0; i < DEFTAPS; i++){
		float Kg,Kd, Numer;
		Kg = (float)(i*1.0 - center) / center;		//Regulated Distance, -1 to 1
		Kd = (float)(K_Alpha * sqrt(1.0 - Kg*Kg));	//kaiser Distance
		Numer = Bessel(Kd);
		Kwindow[i] = Numer / Denom;
	}
  //#pragma omp for
	for(i = 0; i < DEFTAPS; i++) {
		FIRCoeff[i] = FIRCoeff[i] * Kwindow[i];
	}
//}
}

//Generate Frequency Responce.
void CreateFres(long SampleFreq, int fFreqLow, int fFreqHi, float * FreqResR)
{
	// gain = pow(10.0, desired_dB / 20.0);
	// There are folding at Fs/2. _____L~~~~~H_____|_____H'~~~L'____. |=Fs/2

	int i;
	int taps_half = DEFTAPS / 2;
	double stopgain = pow(10.0, -200.0/20.0);
//#pragma omp parallel for
	for (i = 0; i < taps_half ; i++){
		float CurrentFreq = (float)(SampleFreq * (i + 1)/DEFTAPS);
		if (CurrentFreq < fFreqLow){
			//Stop Area 1
			FreqResR[i] = (float)stopgain;
		}
		else if (CurrentFreq < fFreqHi) {
			//Pass Area 1
			FreqResR[i] = 1.0f;
		}
		else if (CurrentFreq <= SampleFreq / 2){
			//Stop Area 2
			FreqResR[i] = (float)stopgain;
		}
	}
	for (i = taps_half; i < DEFTAPS; i++){
		//Mirror Freq Responce
		FreqResR[i] = FreqResR[DEFTAPS - i - 1];
	}
}

//make FIR coeff, from Frequency Response.
void CalcCoeffs(float * FreqResR, float * FIRCoeff)
{
	double costable[DEFTAPS];
	double Tempcoeff_r[DEFTAPS];
	
	#pragma omp parallel for
	for (int i = 0; i < DEFTAPS; i++){
		costable[i] = (double)cos( 2.0 * M_PI * i / DEFTAPS);
	}
	int c = 0;
	for (int i = 0; i < DEFTAPS; i++) {
		if(i == 32768){
			c = 0;
		}
		Tempcoeff_r[i] = 0.0;
		double Coeff = 0.0;
		for (int j = 0; j < DEFTAPS; j++) {
			Coeff += FreqResR[j]*costable[(c * j)%DEFTAPS];
		}
		Tempcoeff_r[i] = Coeff / DEFTAPS;
		c++;
	}
	for (int i = 0; i < DEFTAPS / 2 ; i++) {
		FIRCoeff[i] = (float)Tempcoeff_r[DEFTAPS / 2 - i];
	}
	for (int i = DEFTAPS / 2 ; i < DEFTAPS; i++) {
		FIRCoeff[i] = (float)Tempcoeff_r[i-(DEFTAPS / 2)];
	}
}


void CalcFres(float * FIRCoeff)
{

	int i, k;
	double	omega;		//angular frequency
	double	Resp_Real;	//Frequency Responce, Real
	double	Resp_Image;	//Frequency Responce, imaginary
	double	AmpABS2;	//square of amplitude characteristic
	int TAPS_HALF = DEFTAPS / 2;

	double Gain[DEFTAPS / 2];

	for(i = 0; i < TAPS_HALF; i++) 
	{
		//omega = 0 to 2Pi (rad)
		omega = (i / (DEFTAPS/360.0) * 2.0 * M_PI  ) / 360.0;

		//initialize
		Resp_Real = 0.00; 
		Resp_Image = 0.00;
		
		//accumulate for all TAP of FIR
		for(k =	0; k < DEFTAPS; k++) 
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
	FILE * f;
	f = fopen("/tmp/FreqGain.txt", "w");
	for (int i = 0 ; i < DEFTAPS/2; i++)
	{
		fprintf(f, "%3.8lf\n", Gain[i]);
	}
	fclose(f);
}

//FIR main Routine
void ProcessFIR(float * input, float * output, int Samples, float * coeffs, int channel, int skips, int * jStarts)
{
	//mode: 0= normal, 1=reverse
	float* in  = input;
	float* out = output;	
	float* FIRCoeff = coeffs;

	//temp h_idata	
	//float h_idata[4][DEFTAPS * 2];
	//float temp_idata[DEFTAPS * 2 ];
	//memset(temp_data, 0, sizeof(float)*DEFTAPS*2);

	//add input to input buffer
	for (int i = 0; i < Samples; i++){
		h_idata[channel][DEFTAPS + i] = in[i];
		//temp_idata[DEFTAPS + i] = in[i];
	}

	//float dOut = 0.0;

	//if (skips > 0) {//FAST mode, for LPF oversampled signal (almost 0)
// optimise point		
#pragma omp parallel for
		for (int i = 0; i < Samples; i++) {
			float dOut = 0.0;
			//input buffer is oversampled, so 0, 64, 128,... has data but others are 0.
			/*
			int jStart;
			if (i % skips == 0) {
				jStart = 0;
			} else {
				jStart = skips - (i % skips);
			}
			*/
			for (int j = jStarts[i]; j < DEFTAPS; j = j + skips) {
				dOut += h_idata[channel][i + j] * FIRCoeff[j];
			}


			//63 of 0 was inserted, so 1.0 -> 1.0/64. shift the level back
			out[i] = dOut * (float)skips / (4); // 4 is gain
		}
//	}else{		
//original
/*
#pragma omp parallel for
		for (int i = 0; i < Samples; i++) {
			float dOut = 0.0;
		
			for (int j = 0; j < DEFTAPS; j++){
				dOut += h_idata[channel][i + j] * FIRCoeff[j];
			}
			out[i] = dOut;
		}	
*/
//	}

	//slide input buffer
	//float temp_f = 0.0f;
	for (int i = 0; i < DEFTAPS; i++) {
		/*temp_f =  h_idata[channel][i + Samples];
		h_idata[channel][i] = temp_f;*/
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
void Shaper_Process(double * A, double * B, double * D, float * input, float * output, int Samples)
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
		double Q_inner1 = a1 * D[1] + a2 * D[2] + a3 * D[3] + a4 * D[4] + a5 * D[5] + a6 * D[6] + a7 * D[7];
		double x_in = input[i] + Q_inner1;
		double y_out;
		if (x_in >= 0.0){
			y_out = 1.0;
		}else{
			y_out = -1.0;
		}
		double Q_error = x_in - y_out;
		double Q_inner2 = b1 * D[1] + b2 * D[2] + b3 * D[3] + b4 * D[4] + b5 * D[5] + b6 * D[6] + b7 * D[7];

		D[7] = D[6];
		D[6] = D[5];
		D[5] = D[4];
		D[4] = D[3];
		D[3] = D[2];
		D[2] = D[1];
		D[1] = Q_error + Q_inner2;	//stored into Delay register

		output[i] = (float)y_out;
	}
}


int main(int argc, char * argv[])
{
	if ((argc > 3) || (argc<2)){
		printf("usage: Wave2DSD infile outfile, or Wave2DSD infile\n");
		return -1;
	}
	//Wave File processing
	char *filename = argv[1];
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
	if (argc == 3) {
		writefilename = argv[2];
	}
	if (argc == 2) {
		//append extention ".DSF".
		writefilename = strcat(filename, ".DSF");
	}

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
	if (len != 16) {
		printf("Length of WAVEfmt must be 16\n");
		return -1;
	}

	//Format info
	printf("  Data type = %u (1 = PCM)\n", read2bytes(f));
	channels = read2bytes(f);
	printf("  Number of channels = %u (1 = mono, 2 = stereo)\n", channels);
	int SamplingRate = read4bytes(f);
	printf("  Sampling rate = %luHz\n", SamplingRate);
	printf("  Bytes per second = %lu\n", read4bytes(f));
	int BytesPerSample = read2bytes(f);
	printf("  Bytes per sample = %u\n", BytesPerSample);
	bits = read2bytes(f);
	printf("  Bits per sample = %u\n", bits);

	//read until 'data' chunk
	while (fread(s, 4, 1, f) == 1) {
		len = read4bytes(f);
		s[4] = 0;
		printf("[%s] (%lu bytes)\n", s, len);
		if (memcmp(s, "data", 4) == 0) break;
		for (int i = 0; i < (int)len; i++)
		{	printf("%02x ", fgetc(f)); }
		printf("\n");
	}

	//Bytes count, processed at once. high samplingrate, more samples processed.
	long dwWaveBytes = iSamples * BytesPerSample * SamplingRate / 44100;	// L,R, 16bit/24bit

	//get memory for "len" bytes
	unsigned char * p;
	//add tail, when len % 512 > 0
	p = (unsigned char *)malloc(len + dwWaveBytes);
	memset(p+len, 0, dwWaveBytes);	//fill 0 at tail
	fread(p, len, 1,f);	//p has wave format data, 16bit
	fclose(f);

	//Prepare output file.
	FILE * wf;
	wf = fopen(writefilename, "wb");
	//DSD header
	fputc('D', wf);
	fputc('S', wf);
	fputc('D', wf);
	fputc(' ', wf);
	write4bytes(wf, (unsigned long)0x0000001C); //chunk size 28
	write4bytes(wf, (unsigned long)0x00000000); //4 bytes 0
	unsigned long totalfilesize;
	totalfilesize = 28 + 52 + 12;	//for header

	//this is how many samples will be processed
	unsigned long total_samples = len / BytesPerSample; //BytesPerSmaple = 2 or 3 * channels

	//512 samples is one convert block. for 44100, 512->32768->4096 88200, 512*2->32768->4096.
	unsigned long convert_blocks2 = (unsigned long)ceil((double) total_samples * 44100 / SamplingRate / iSamples * oversampling / 64);

	//PCM 512 samples -> x64, 32768 / 2.8M / 1bit DSD = 4096 bytes.
	unsigned long dsd_data_length = 4096 * convert_blocks2 * channels ;

	totalfilesize += dsd_data_length;

	unsigned long dsd_total_samples = 4096 * convert_blocks2 * 8; //... this number can be 64bit for large file

	write4bytes(wf, (unsigned long)totalfilesize);
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
	if(oversampling == 64){
	write4bytes(wf, (unsigned long)0x002B1100); //2822400 sampling
	}
	if(oversampling == 128){
	write4bytes(wf, (unsigned long)0x00562200); //2822400 sampling*2
	}
	if(oversampling == 256){
	write4bytes(wf, (unsigned long)0x00AC4400); //2822400 sampling*4
	}
	write4bytes(wf, (unsigned long)0x00000001); //1 bit?
	write4bytes(wf, (unsigned long)dsd_total_samples);
	write4bytes(wf, (unsigned long)0x00000000); //4 bytes 0
	write4bytes(wf, (unsigned long)4096*oversampling/64); //4096
	write4bytes(wf, (unsigned long)0x00000000); //4 bytes 0

	//data chunk
	fputc('d', wf);
	fputc('a', wf);
	fputc('t', wf);
	fputc('a', wf);
	write4bytes(wf, (unsigned long)dsd_data_length);
	write4bytes(wf, (unsigned long)0x00000000); //4 bytes 0

	//debug fclose(wf);

	//Prepare FIR parameter
	float FreqResR[DEFTAPS];
	float FIRCoeff[DEFTAPS];
	memset(h_idata, 0, sizeof(float)*DEFTAPS*4);

	CreateFres(44100 * oversampling, 0, 22500, FreqResR);
	CalcCoeffs(FreqResR, FIRCoeff);
	DoKaiser(FIRCoeff, K_alpha);
	//CalcFres(FIRCoeff);

	//Shaper Coefficient Calcuration
	double A[8];
	double B[8];
	// for Zero shift, 1.0 = 22050Hz. Pole Shift should be 0.85
	// Scilab can not track(up to 0.7) but works.
	//orig Shaper_Coeff(0.95, 0.74, 0.42, 0.85, A, B, true);
	//test
	//Shaper_Coeff(0.97, 0.77, 0.43, 0.85, A, B, true);
	Shaper_Coeff(0.95, 0.74, 0.42, 0.85, A, B, true);

	//initialize history buffer for shaper
	for(int i = 0; i < channels; i++){
		for (int j = 0; j < 7; j++){
			D[i][j] = 0.0f;
		}
	}

	//Prepare buffers
	float FIR_in[channels][iSamples * oversampling]; //oversampled data
	//float* FIR_in = (float*)_mm_malloc(sizeof(float)* iSamples*oversampling,32);
	memset(FIR_in, 0, sizeof(float)*(channels * iSamples * oversampling));	//initialized to 0

	float FIR_out[channels][iSamples * oversampling]; //FIR result
	//float* FIR_out = (float*)_mm_malloc(sizeof(float)* iSamples*oversampling,32);
	memset(FIR_out, 0, sizeof(float)*(channels * iSamples * oversampling));	//initialized to 0

	float Shaper_out[channels][iSamples * oversampling];	//contains 0/1
	memset(Shaper_out, 0, sizeof(float)*(channels * iSamples * oversampling));	//initialized to 0

	float Reverse_FIR[channels][iSamples * oversampling];
	memset(Reverse_FIR, 0, sizeof(float)*( channels * iSamples * oversampling));	//initialized to 0

	unsigned char DSD_out[channels][iSamples * oversampling / 8];	//4096 bytes summarized
	memset(DSD_out, 0, (channels * iSamples * oversampling / 8));	//initialized to 0

//DSD File On Memory
	//unsigned char DSD_memory[(len/dwWaveBytes)+1][iSamples*oversampling / 8];
	//memset(DSD_memory, 0 , ((len/dwWaveBytes)+1) * iSamples*oversampling / 8);

	unsigned long pCounter = 0;	//wave file read counter
	//unsigned char H = 0x00;
	//unsigned char M = 0x00;
	//unsigned char L = 0x00;
	//unsigned long PCMvalue;
	//float signal;	//normalized value for single data
	
	//variable for FIR Speedup
	int Skips = oversampling * 44100 / SamplingRate;
	
	int jStarts[iSamples];
	
	for(int i=0;i<iSamples;i++){
		if (i % Skips == 0) {
				jStarts[i] = 0;
		} else {
				jStarts[i] = Skips - (i % Skips);
		}
	}

	printf("Start Conversion...\n");
	double start,end;
	start = omp_get_wtime();

	while (pCounter < len ) {
	int count = 0;
	int l= (len / dwWaveBytes)+1;
	//for(count=0;count<l;count++) {
		//Loop for channels. stereo, then 0 , 1
		//pCounter = count * dwWaveBytes;
//	#pragma omp for
		for (int currentChannel = 0; currentChannel < channels; currentChannel++) {
			//loop for Sample Block. 512 sample. it will be converted to 4096 Bytes DSD block.
			for (int j = 0; j < iSamples * SamplingRate / 44100 ; j++) {	//when 88200, take x2 samples
				unsigned char H = 0x00;
				unsigned char M = 0x00;
				unsigned char L = 0x00;
				unsigned long PCMvalue;
				float signal;
			
				if (BytesPerSample == 4) {
					L = 0x00;
					M = *(p + pCounter + j*4 + 0 + currentChannel*BytesPerSample/channels); //read actual data
					H = *(p + pCounter + j*4 + 1 + currentChannel*BytesPerSample/channels);		
				}
				if (BytesPerSample == 6) {
					L = *(p + pCounter + j*6 + 0 + currentChannel*BytesPerSample/channels); //read actual data
					M = *(p + pCounter + j*6 + 1 + currentChannel*BytesPerSample/channels); 
					H = *(p + pCounter + j*6 + 2 + currentChannel*BytesPerSample/channels);
		
				}
				PCMvalue = H * 256L * 256L + M * 256L + L;
				if (PCMvalue < 0x800000) {
					signal = (PCMvalue / 16777215.0f);
					signal = signal * 2.0f;					//PCM Positive, 0 to 0.5 -> 0.0 to 1.0
				} else {
					signal = (PCMvalue / 16777215.0f);	//PCM Negative, 0.5 to 1.0
					signal = signal - 1.0f;				// -> -0.5 to 0.0
					signal = signal * 2.0f;				// -> -1.0 to 0.0
				}
				signal = signal * 0.50f; //normalize, -6dB
				FIR_in[currentChannel][j * Skips] = signal;	//oversample, others are 0
			}		
			//do FIR (LPF) for FIR_in, to FIR_out
			//original length of FIR_in, out is 512 * 64.
//#pragma omp parallel for
			for (int k = 0; k < oversampling; k++){
				float* _in = &FIR_in[currentChannel][k*iSamples];
				float* _out = &FIR_out[currentChannel][k*iSamples];
				//float h_idata[4][DEFTAPS * 2];
				ProcessFIR(_in, _out, iSamples, FIRCoeff, currentChannel, Skips, jStarts);
			}
		}
#pragma omp parallel for
		for (int currentChannel = 0; currentChannel < channels; currentChannel++) {
			//do 7th noise shaper for FIR_out, to Shaper_out
			Shaper_Process(A, B, &D[currentChannel][0], FIR_out[currentChannel], Shaper_out[currentChannel], iSamples * oversampling); 

			//debug
/*			if ((currentChannel ==1) && (pCounter == dwWaveBytes * 400)) {
				FILE * f1;
				f1 = fopen("/tmp/FIR_in.txt", "w");
				for (int x = 0 ; x < oversampling*iSamples; x++)
				{
					fprintf(f1, "%3.15lf\n", FIR_in[x]);
				}
				fclose(f1);
				FILE * f2;
				f2 = fopen("/tmp/FIR_out.txt", "w");
				for (int x = 0 ; x < oversampling*iSamples; x++)
				{
					fprintf(f2, "%3.15lf\n", FIR_out[x]);
				}
				fclose(f2);			
				FILE * f3;
				f3 = fopen("/tmp/Shaper_out.txt", "w");
				for (int x = 0 ; x < oversampling*iSamples; x++)
				{
					fprintf(f3, "%3.15lf\n", Shaper_out[x]);
				}
				fclose(f3);				
			}
*/
			//Bit to Byte loading, for DSD_out
			for (int m = 0; m <  iSamples * oversampling / 8; m++){
				DSD_out[currentChannel][m] = 0x00;
				for (int n = 0; n < 8; n++){
					if (Shaper_out[currentChannel][m*8 + n] > 0.0){
						DSD_out[currentChannel][m] |= (0x01) << n ;
					}
				}
			}
		}
			//write file.
		//for (int currentChannel = 0; currentChannel < channels; currentChannel++) {
			int ret;
			
			//ret = fwrite (DSD_out[currentChannel] , 1 , sizeof(DSD_out[currentChannel]) , wf);
			ret = fwrite (DSD_out , 1 , sizeof(DSD_out) , wf);
			
			if (ret !=  sizeof(DSD_out)){
				fprintf(stderr, "DSD result Write error\n");
				//return -1;
			}
			

			// write on memory
			//memcpy( DSD_memory[count], DSD_out, sizeof(DSD_out) );
		
		//}
//} // end parallel
		pCounter += dwWaveBytes;
		count = pCounter / dwWaveBytes;
		printf("[%d / %d ]\r", (int)(count), l );
		//usleep(1);
	}


	//_mm_free(FIR_in);
	//_mm_free(FIR_out);
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
	printf("%.4fSeconds, %d Bytes, %.2f KBytes/Sec \n",(double)(end-start), len, (double)(len/1000) / (double)(end-start) );
	fclose(wf);
	return 0;
}
