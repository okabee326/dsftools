#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include <complex.h>
#include <fftw3.h>


typedef struct {
	unsigned long samples;
	int channels;
	int bytepersample;
	int freq;

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
			//fseek(f, -4L, SEEK_CUR);
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
/*
		if(count % (10*SamplingRate) == 0){
			printf("[%d]: %3.14lf\n", count, out[0][count]);
		}
*/
		pCounter += BytesPerSample;
		count++;
	}

	format->samples = samples;
	format->bytepersample = BytesPerSample;
	format->freq = SamplingRate;
	format->channels = channels;

	*out = wave;

	return 0;
}

int main(int argc, char * argv[]){
	char* infile = 0;
	int checkFreq = 0;

	for (int i = 1; i<argc; i++) {
		if (argv[i][0] != '-') {
			if (infile == 0) {
				infile = argv[i];
			}
			continue;
		}

		if ( !strcmp(argv[i], "--checkfreq")) {
			checkFreq = 1;
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
	//printf("samples: %d\n", format.samples);

	int size = format.samples;
	int freq = format.freq;



	if(freq > size){
		return -1;
	}

	fftw_complex *fft_in, *fft_out;
  	fftw_plan p;
  	int i;

  	//printf("%3.14lf\n", wave[0][0]);

  	fft_in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * freq);
  	fft_out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * freq);

  	int target = 21330;
  	int target2 = 22160;
  	int target3 = 22450;
  	int target4 = 22830;
  	double target_avg = 0.0;
  	double target2_avg = 0.0;
  	double target3_avg = 0.0;
  	double target4_avg = 0.0;
  	
  	for(int i=0;i<size;i+=freq*10) 
  	{
  		for(int j=0;j<freq;j++){
  			fft_in[j][0] = wave[0][i+j];
  			fft_in[j][1] = 0.0;
  		}

  		p = fftw_plan_dft_1d(freq, fft_in, fft_out,FFTW_FORWARD,FFTW_ESTIMATE);
  		/* process with FFTW */
  		fftw_execute(p);

  		double total_m = 0.0;
  		
  		for(int j=target-8; j<target+8; j++) {
  			double re = fft_out[j][0];         // 複素数の実数部
        	double im = fft_out[j][1];         // 複素数の虚数部
        	double mag = sqrt(re*re + im*im);      // 大きさを計算
        	total_m += mag;
        }

        target_avg = fmax(target_avg, total_m / 16.0);

		total_m = 0.0;
        
        for(int j=target2-8; j<target2+8; j++) {
  			double re = fft_out[j][0];         // 複素数の実数部
        	double im = fft_out[j][1];         // 複素数の虚数部
        	double mag = sqrt(re*re + im*im);      // 大きさを計算
        	total_m += mag;
        }
        target2_avg = fmax(target2_avg, total_m / 16.0);

        total_m = 0.0;
        
        for(int j=target3-8; j<target3+8; j++) {
  			double re = fft_out[j][0];         // 複素数の実数部
        	double im = fft_out[j][1];         // 複素数の虚数部
        	double mag = sqrt(re*re + im*im);      // 大きさを計算
        	total_m += mag;
        }
        target3_avg = fmax(target3_avg, total_m / 16.0);

        total_m = 0.0;
        
        for(int j=target4-8; j<target4+8; j++) {
  			double re = fft_out[j][0];         // 複素数の実数部
        	double im = fft_out[j][1];         // 複素数の虚数部
        	double mag = sqrt(re*re + im*im);      // 大きさを計算
        	total_m += mag;
        }
        target4_avg = fmax(target4_avg, total_m / 16.0);



  		fftw_destroy_plan(p);
  		
	}

	//printf("%d: target_avg: %3.14lf\n", target, target_avg);
    //printf("%d: target2_avg: %3.14lf\n", target2, target2_avg);
    //printf("%d: target3_avg: %3.14lf\n", target3, target3_avg);
    //printf("%d: target4_avg: %3.14lf\n", target4, target4_avg);

    int outputfreq = target;

    if(target2_avg > 1.0){
    	outputfreq = target2;
    }
    if(target3_avg > 1.0){
    	outputfreq = target3;
    }
    if(target4_avg > 1.0){
    	outputfreq = target4;
    }

    printf("%d\n", outputfreq);

  	fftw_free(fft_in);
  	fftw_free(fft_out);

	return 0;
}