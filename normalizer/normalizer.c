#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
//#include <complex.h>


typedef struct {
	unsigned long samples;
	int channels;
	int bytepersample;
	int freq;
  int type;

} WaveFormat;

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


//-------------------------------------------------
//7th noise shaper utility
//-------------------------------------------------
void Shaper_Coeff(double Zero1, double Zero2, double Zero3, double Pole, double * A, double * B, bool Print)
{
  //This routine calculate standard H(z) function, as
  // H(z) = 1+A1*Z^-1+A2*Z^-2+A3Z^-3+A4Z^-4..... / 1+B1Z^-1+B2Z^-2+....

  double Theta1, Theta2, Theta3;
  double s1, s2, s3;

  //for 64 oversampling... Zero1 = 1.0 means 44100/2.0=22050 
  Theta1 = ((2.0 * M_PI ) / 2.0) * Zero1;
  Theta2 = ((2.0 * M_PI ) / 2.0) * Zero2;
  Theta3 = ((2.0 * M_PI ) / 2.0) * Zero3;

  //(1-x)(1-2cosT1x +x^2)(1-2cosT2x +x^2)(1-2cosT3x +x^2)
  s1 = 2.0 * cos(Theta1);
  s2 = 2.0 * cos(Theta2);
  s3 = 2.0 * cos(Theta3);

  //printf("Theta1 = %.15f\n",Theta1);
  //printf("Theta2 = %.15f\n",Theta2);
  //printf("Theta3 = %.15f\n",Theta3);

  //printf("s1 = %.15f\n",s1);
  //printf("s2 = %.15f\n",s2);
  //printf("s3 = %.15f\n",s3);

  A[0] = 1.0;
  A[1] = -1.0 * (s1 + s2 + s3 + 1.0);
  A[2] = (s1*s2 + s2*s3 + s3*s1 + s1 + s2 + s3 + 3.0);
  A[3] = -1.0 * (2.0*s1 + 2.0*s2 + 2.0*s3 + s1*s2*s3 + s1*s2 + s2*s3 + s3*s1 + 3.0); 
  A[4] = -1.0 * A[3];
  A[5] = -1.0 * A[2];
  A[6] = -1.0 * A[1];
  A[7] = -1.0 * A[0];

  //7th order, pole shift
  //printf("\n7th order Pole shift\n");
  B[0] = 1.0;
  B[1] = -7.0 * Pole ;
  B[2] = 21.0 * Pole * Pole;
  B[3] = -35.0 * Pole * Pole * Pole;
  B[4] = 35.0 * Pole * Pole * Pole * Pole;
  B[5] = -21.0 * Pole * Pole * Pole * Pole * Pole;
  B[6] = 7.0 * Pole * Pole * Pole * Pole * Pole * Pole;
  B[7] = -1.0 * Pole * Pole * Pole * Pole * Pole * Pole * Pole;

  //printf("A: %+.8f,%+.8f,%+.8f,%+.8f,%+.8f,%+.8f,%+.8f\n",A[1],A[2],A[3],A[4],A[5],A[6],A[7]);
  //printf("B: %+.8f,%+.8f,%+.8f,%+.8f,%+.8f,%+.8f,%+.8f\n",B[1],B[2],B[3],B[4],B[5],B[6],B[7]);
  //printf("Shaper Coeff calculation Done\n");
}

//-------------------------------------------------
//7th noise shaper function
//-------------------------------------------------
void Shaper_Process_float(double * A, double * B, double * d, double * input, float * output, int Samples, int bit)
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

  for (int i = 0; i < Samples; i++){
    double Q_inner1 = a1 * d[1] + a2 * d[2] + a3 * d[3] + a4 * d[4] + a5 * d[5] + a6 * d[6] + a7 * d[7];
    double x_in = input[i] + Q_inner1;
    float y_out = 0.0;
    
    y_out = (float)x_in;

    double Q_error = x_in - (double)y_out;
    double Q_inner2 = b1 * d[1] + b2 * d[2] + b3 * d[3] + b4 * d[4] + b5 * d[5] + b6 * d[6] + b7 * d[7];

    d[7] = d[6];
    d[6] = d[5];
    d[5] = d[4];
    d[4] = d[3];
    d[3] = d[2];
    d[2] = d[1];
    d[1] = Q_error + Q_inner2;  //stored into Delay register
    d[0] = y_out;

    output[i] = (y_out);
  }
}

//-------------------------------------------------
//
//-------------------------------------------------

void Shaper_Process_int(double * A, double * B, double * d, double * input, int * output, int Samples, int bit)
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

  //printf("bit: %d\n", bit);

  long max = 4294967296/2-1;
  if (bit == 16) {
    max = 65536/2-1;
  }
  if (bit == 24) {
    max = 16777216/2-1;
  }
  if (bit == 32) {
    max = 4294967296/2-1;
  }

  //printf("max: %ld\n", max);

  for (int i = 0; i < Samples; i++){

    double in =  ( (input[i]) * (double)max);
    
    double Q_inner1 = a1 * d[1] + a2 * d[2] + a3 * d[3] + a4 * d[4] + a5 * d[5] + a6 * d[6] + a7 * d[7];
    double x_in = in + Q_inner1;
    long y_out = 0;
    
    y_out = (long)(x_in);

    

    if(y_out > max){
      y_out = max;
    }
    if(y_out < -1*max){
      y_out = -1*max;
    }

    double Q_error = x_in - ((double)y_out  );
    double Q_inner2 = b1 * d[1] + b2 * d[2] + b3 * d[3] + b4 * d[4] + b5 * d[5] + b6 * d[6] + b7 * d[7];

    //if(i % 100 == 0){
      //printf("in: %3.14lf x_in:%3.14lf qe: %3.14lf out: %3.14lf\n", in, x_in, Q_error, (y_out / (double)max ) );
    //}

    d[7] = d[6];
    d[6] = d[5];
    d[5] = d[4];
    d[4] = d[3];
    d[3] = d[2];
    d[2] = d[1];
    d[1] = Q_error + Q_inner2;  //stored into Delay register
    d[0] = ((double)y_out  );



    output[i] = (int)((y_out) );
  }
}

//-----------------------------------------------
//
//-----------------------------------------------

int loadWave(char* filename, double*** out, WaveFormat* format){
	FILE *f = fopen(filename, "rb");

	//can open?
	if (f == NULL) {
		printf("Can not open %s\n", filename);
		return -1;
	}
	//printf("finename = '%s'\n", filename);

	unsigned char s[10];

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
	//printf("[RIFF] (%lu bytes)\n", read4bytes(f));
	read4bytes(f);

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

	long len;

	//Chunk Length?
	len = read4bytes(f);
	//printf("[WAVEfmt ] (%lu bytes)\n", len);
	if (len < 16) {
		printf("Length of WAVEfmt must be 16\n");
		return -1;
	}
	unsigned int type = read2bytes(f);
	//printf("  Data type = %u (1 = PCM)\n", type);
	int channels = read2bytes(f);
	//printf("  Number of channels = %u (1 = mono, 2 = stereo)\n", channels);
	int SamplingRate = read4bytes(f);
	//printf("  Sampling rate = %luHz\n", SamplingRate);
	//printf("  Bytes per second = %lu\n", read4bytes(f));
	read4bytes(f);
	int BytesPerSample = read2bytes(f);
	//printf("  Bytes per sample = %u\n", BytesPerSample);
	int bits = read2bytes(f);
	//printf("  Bits per sample = %u\n", bits);
	if (len != 16) {
		fseek(f, len - 16, SEEK_CUR);
		fread(s, 4, 1, f);
		if (memcmp(s, "fact", 4) == 0) {

			len = read4bytes(f);
			//printf("found fact chunk: %d bytes", len);
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
		//printf("[%s] (%lu bytes)\n", s, len);
		if (memcmp(s, "data", 4) == 0) break;
		for (int i = 0; i < (int)len; i++)
		{
			//printf("%02x ", fgetc(f));
			fgetc(f);
		}
		//printf("\n");
	}
	// malloc wave size
	//long dwWaveBytes = iSamples * BytesPerSample * SamplingRate / 44100;
	unsigned char * p;
	if (type == 3) {
		//float* p;
		p = (unsigned char*)malloc(len);
		//memset(p + len, 0, dwWaveBytes);	//fill 0 at tail
		//memset(p, 0, dwWaveBytes);	//fill 0 at head
		fread(p , len, 1, f);
		fclose(f);
	}
	else {

		//add tail, when len % 512 > 0
		p = (unsigned char *)malloc(len);
		//memset(p + len, 0, dwWaveBytes);	//fill 0 at tail
		fread(p, len, 1, f);	//p has wave format data, 16bit
		fclose(f);
	}

	unsigned long pCounter = 0;
	unsigned long count = 0;

	unsigned long samples = len / BytesPerSample;

	double** wave;
	wave = (double**)malloc(sizeof(double*)*channels);
	for(int i=0;i<channels;i++){
		wave[i] = (double*)malloc(sizeof(double)*samples);
	}

	// wave to array

	while(pCounter < len){
		for (int c = 0; c < channels; c++) {
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
					H = *(p + pCounter +  + 0 + c*BytesPerSample / channels); //read actual data
					H2 = *(p + pCounter + 1 + c*BytesPerSample / channels);
				}
				if (BytesPerSample == 6) {
					L = 0x00;
					M = *(p + pCounter  + 0 + c*BytesPerSample / channels); //read actual data
					H = *(p + pCounter  + 1 + c*BytesPerSample / channels);
					H2 = *(p + pCounter + 2 + c*BytesPerSample / channels);
				}
				if (BytesPerSample == 8) {
					L = *(p + pCounter  + 0 + c*BytesPerSample / channels); //read actual data
					M = *(p + pCounter  + 1 + c*BytesPerSample / channels);
					H = *(p + pCounter  + 2 + c*BytesPerSample / channels);
					H2 = *(p + pCounter + 3 + c*BytesPerSample / channels);
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
						signal = *((float*)(p + pCounter  + c*BytesPerSample / channels));
					}
					if (BytesPerSample == 16) {
						signal = *((double*)(p + pCounter  + c*BytesPerSample / channels));
					}
				}
			}

			wave[c][count] = signal;
		}

		pCounter += BytesPerSample;
		count++;
	}

	format->samples = samples;
	format->bytepersample = BytesPerSample;
	format->freq = SamplingRate;
	format->channels = channels;
  format->type = type;

	*out = wave;

	return 0;
}

//-----------------------------------------------
//
//-----------------------------------------------

int writeWave(char* filename, double*** in, unsigned long size, int type, int outputbit, int outsampling, int channels, double* zero){
  char* writefilename = filename;
 
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
  unsigned long datasize = size * (outputbit/8)* channels;
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
    write2bytes(wf, (unsigned short)type);
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

  // noise shaper init
  //double zero[4] = { 0.969, 0.737, 0.424, 0.85 };
  double A[8];
  double B[8];
  double D[8] = { 0.0 };
  Shaper_Coeff(zero[0], zero[1], zero[2], 0.85, A, B, true);

  float* Shaper_out;
  int*   Shaper_int;

  if(outputbit == 32 && type == 3){
    Shaper_out = (float*)malloc( sizeof(float)*size*channels);
  }
  if(type == 1){
    Shaper_int = (int*)malloc( sizeof(int)*size*channels);
  }

  if(outputbit == 32 && type == 3){
    
    for(int c=0;c<channels;c++){
      //void Shaper_Process_int(double * A, double * B, double * d, double * input, int * output, int Samples, int bit)
      Shaper_Process_float(A, B, D, (*in)[c], &Shaper_out[c*size], size, outputbit);
    }
  }

  if(type == 1){

    for(int c=0;c<channels;c++){
      //Shaper_Process_int(A, B, D, (*in)[c], &Shaper_int[c*size], size, outputbit);
    }
  }

  //printf("end noise shaper\n");

  // end noise shaper

  unsigned long pCounter = 0;
  unsigned long WriteBufferPointer = 0;
  unsigned char* WriteBuffer;
  //32bit float output
  float * Floatdata;
  double * Doubledata;

  if(type == 1){
    WriteBuffer = (unsigned char*)malloc(sizeof(unsigned char)*size*channels*4);
    memset(WriteBuffer, 0, sizeof(unsigned char)*size*channels *4);
  }
  if(type == 3 && outputbit == 32){
    Floatdata = (float*)malloc(size * channels * sizeof(float));
    memset(Floatdata, 0, size * channels * sizeof(float));
  }
  if(type == 3 && outputbit == 64){
    Doubledata = (double*)malloc(size * channels * sizeof(double));
    memset(Doubledata, 0, size * channels * sizeof(double));
  }

  for(int j=0;j<size;j++) {
    for (int iChannel = 0; iChannel < channels; iChannel++){
        unsigned char H = 0x00;
        unsigned char H2 = 0x00;
        unsigned char M = 0x00;
        unsigned char L = 0x00;
        unsigned long PCMvalue;
        float oneResult;
        double doubleResult;

        if(outputbit == 32 && type == 3){
          oneResult = (float)Shaper_out[iChannel*size + j ];
        }
        else {
          if(outputbit == 64){
              doubleResult = (*in)[iChannel][j];
          }
        }
        
        //change to PCM value
        if(type == 1){
          //PCMvalue = Shaper_int[iChannel*size + j ];
          long max = 4294967296-1;
          PCMvalue = (long)(((*in)[iChannel][j] + 1.0)*0.5*max);

/*
          if(PCMvalue > 5000){
            printf("%d\n", PCMvalue);
          }
*/
        
          L = (PCMvalue & 0xFF);
          M = (PCMvalue >> 8) & 0xFF;
          H = (PCMvalue >> 16) & 0xFF;
          H2 = (PCMvalue >> 24) & 0xFF;
          //invert H2 sign bit, XOR
          H2 = H2 ^ 0x80;
          //write PCM value to file
          //printf("%d,%d,%d,%d\n", L,M,H,H2);

            if (outputbit == 32){
              //L = (PCMvalue & 0xFF);
              //M = (PCMvalue >> 8) & 0xFF;
              //H = (PCMvalue >> 16) & 0xFF;
              //H2 = (PCMvalue >> 24) & 0xFF;
              //invert H2 sign bit, XOR
              //H2 = H2 ^ 0x80;
              WriteBuffer[WriteBufferPointer] = L; WriteBufferPointer++;
              WriteBuffer[WriteBufferPointer] = M; WriteBufferPointer++;
              WriteBuffer[WriteBufferPointer] = H; WriteBufferPointer++;
              WriteBuffer[WriteBufferPointer] = H2; WriteBufferPointer++;

            } 
            if (outputbit == 24){
             //L = (PCMvalue & 0xFF);
              //M = (PCMvalue >> 8) & 0xFF;
              //H = (PCMvalue >> 16) & 0xFF;
              //invert H2 sign bit, XOR
              //H = H ^ 0x80;
              WriteBuffer[WriteBufferPointer] = M; WriteBufferPointer++;
              WriteBuffer[WriteBufferPointer] = H; WriteBufferPointer++;
              WriteBuffer[WriteBufferPointer] = H2; WriteBufferPointer++;
            } 
            if (outputbit == 16){
              //L = (PCMvalue & 0xFF);
              //M = (PCMvalue >> 8) & 0xFF;
              //invert H2 sign bit, XOR
              //M = M ^ 0x80;
              WriteBuffer[WriteBufferPointer] = H; WriteBufferPointer++;
              WriteBuffer[WriteBufferPointer] = H2; WriteBufferPointer++;
            }   
          
        }
        if( type == 3){
            if(outputbit == 32){
              Floatdata[WriteBufferPointer] = oneResult;
            }
            else {
              Doubledata[WriteBufferPointer] = doubleResult;
            }
            WriteBufferPointer++;
        }
    }
  }

  if(type == 3){
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

  fclose(wf);

  if(outputbit == 32 && type == 3){
    free(Shaper_out);
    free(Floatdata);
  }
  if(outputbit == 64 && type == 3){
    free(Doubledata);
  }
  if(type == 1){
    free(Shaper_int);
    free(WriteBuffer);
  }
}

//------------------------------------------
//
//------------------------------------------

int normalize(double*** in, double db, int size, int channels){
  double maxValue = 0.0;

  for(int i=0;i<channels;i++){
    for(int j=0;j<size;j++){
      if( fabs( (*in)[i][j]) > maxValue){
        maxValue = fabs( (*in)[i][j] );
      }
    }
  }

  if(maxValue == 0.0){
    return -1;
  }

  double level = pow(10, db / 20);

  printf("%lf\n", level);

  double multiplier = level / maxValue;

  printf("%lf\n", multiplier);

  for(int i=0;i<channels;i++){
    for(int j=0;j<size;j++){
        (*in)[i][j] *= multiplier;
        if((*in)[i][j] > 1.0){
          (*in)[i][j] = 1.0;
          printf("clipped\n");
        }
    }
  }
}

//-----------------------------------------------
// main routine
//-----------------------------------------------
int main(int argc, char * argv[]){
	char* infile = 0;
  char* outfile = 0;
	int checkFreq = 0;
  int outputbit = -1;
  int outputtype = 3;
  double outputlevel = 0;
  //double zero_float[4] = { 0.969, 0.737, 0.424, 0.85 };
  double zero_float[4] = { 0.939, 0.724, 0.392, 0.85 };
  double zero_int[4] = { 0.966, 0.736, 0.425, 0.85 };
  double* zero = zero_float;

	for (int i = 1; i<argc; i++) {
		if (argv[i][0] != '-') {
			if (infile == 0) {
				infile = argv[i];
        continue;
			}
      if (outfile == 0) {
        outfile = argv[i];
        continue;
      }

      // db 
      outputlevel = atof(argv[i]);

			continue;
		}

		if ( !strcmp(argv[i], "--checkfreq")) {
			checkFreq = 1;
			continue;
		}

    // outputbit setting
    if(!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bit")){
      if(i+1 != argc){
        outputbit = atoi(argv[i+1]);

        if(outputbit == 16 || outputbit == 24){
          outputtype = 1;
          zero = zero_int;
        }

        if( !( outputbit == 16 || outputbit == 24 || outputbit == 32 || outputbit == 64) ){
          printf("outputbit must be 16 24 32 64 .\n");
          return -1;
        }
        i++;
      }
      continue;
    }

    // outputbit setting
    if(!strcmp(argv[i], "-t") || !strcmp(argv[i], "--type")){
      if(i+1 != argc){
        if(!strcmp(argv[i+1], "float") ){
          outputtype = 3;
          i++;
          continue;
        }
        if(!strcmp(argv[i+1], "int") ){
          outputtype = 1;
          zero = zero_int;
          i++;
          continue;
        }
        
          printf("outputtype is float or int .\n");
          return -1;
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

    // if argv = -0.1 : set db
    if(argv[i][1] != '\0'){
      outputlevel = atof(argv[i]);
      continue;
    }

		// usage
		printf("usage: checkhfa infile [--checkfreq]\n");
		return -1;
	}

	if (infile == 0 ) {
		// usage
		printf("usage: checkhfa infile [--checkfreq]\n");
		return -1;
	}

	double** wave;
	WaveFormat format;

	int ret = loadWave(infile, &wave, &format);

	if(ret != 0){
		return -1;
	}
	// freq check mode
	if(checkFreq == 1){
		printf("%d\n", format.freq);
		return 0;
	}
	printf("samples: %d\n", format.samples);

	int size = format.samples;
	int freq = format.freq;
  int channels = format.channels;
  int type = format.type;

  if(outputbit == -1){
    outputbit = format.bytepersample*4;
    printf("outputbit: %d\n", outputbit);
  }

  
  normalize(&wave, outputlevel , size, channels); 

  printf("%s\n", outfile);

  //int writeWave(char* filename, double*** in, unsigned long size, int type, int outputbit, int outsampling, int channels)
  writeWave(outfile, &wave, size, outputtype, outputbit, freq, channels, zero);

	return 0;
}