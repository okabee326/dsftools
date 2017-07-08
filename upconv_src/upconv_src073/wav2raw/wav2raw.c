/****************************************************************************/
/* wav2raw (C) 2011-2012 By 59414d41										*/
/* wavファイルを64bitのrawファイルへ変換する								*/
/*																			*/
/****************************************************************************/

/*--- Info -----------------------------------------------------------------
 * wav2raw 入力ファイル名 出力ファイル名
 *
 * 関連ファイル
 * xxx.param - パラメーターファイル
 * xxx.files - 出力したファイル名
 *
 */

/*--- Log ------------------------------------------------------------------
 * Ver 0.10 <09/07/15> - upconvから分離
 * Ver 0.21 <09/10/26> - ちょっと修正
 * Ver 0.30 <09/11/01> - パラメータファイルの採用
 * Ver 0.31 <09/11/16> - エラー情報をファイルへ出力するようにした
 * Ver 0.50 <10/11/02> - 処理修正
 * Ver 0.70 <11/07/24> - コンパイラをmingwに変更
 *						 大きなファイルに対応
 *						 split処理をやめたことによる修正
 * Ver 0.80 <12/02/11> - fileioを使用するように修正
 *						 マルチチャンネルに対応
 */

#define STR_COPYRIGHT	"wav2raw (c) 2011-2012 Ver 0.80 By 59414d41\n\n"
#define STR_USAGE		"wav2raw infile outfile\n"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "./../upconv/fileio.h"
#include "PLG_AudioIO.h"

#if 0
#define	PRINT_LOG(s)	do {																	\
							FILE *log;															\
							log = fopen("/tmp/wav2raw.log","a");									\
							if (log) {															\
								fprintf(log,"%s [%d] %s\n",__FUNCTION__,__LINE__,s);			\
								fclose(log);													\
							}																	\
						} while (0)
#else
#define	PRINT_LOG(s)	//
#endif

// サンプルを処理するデータ型
//#define SSIZE	int64_t

typedef long long int SSIZE;


typedef struct {
	SSIZE	minR1;
	SSIZE	maxR1;
	SSIZE	minR2;
	SSIZE	maxR2;
	SSIZE	minR3;
	SSIZE	maxR3;
	SSIZE	minR4;
	SSIZE	maxR4;
	SSIZE	minR5;
	SSIZE	maxR5;
	SSIZE	minR6;
	SSIZE	maxR6;
	SSIZE	maxLv1;
	SSIZE	maxLv2;
	SSIZE	maxLv3;
	SSIZE	maxLv4;
	SSIZE	maxLv5;
	SSIZE	maxLv6;
	SSIZE	tmpLv1;
	SSIZE	tmpLv2;
	SSIZE	tmpLv3;
	SSIZE	tmpLv4;
	SSIZE	tmpLv5;
	SSIZE	tmpLv6;
	SSIZE	cntLv1;
	SSIZE	cntLv2;
	SSIZE	cntLv3;
	SSIZE	cntLv4;
	SSIZE	cntLv5;
	SSIZE	cntLv6;
} NORM_INFO;

NORM_INFO NormInfo;
int errLine;

/*--- Function Prototype ---------------------------------------------------*/

int nBitTo64S(int nCh,int ch,int bit,void *in,FIO *fio,DWORD nSample);

//---------------------------------------------------------------------------
// Function   : main
// Description: 引数を処理し変換関数を呼び出す
//
//
int main(int argc, char *argv[])
{
	
//	printf("%d", __WORDSIZE);

	char tmppath[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char fname2[_MAX_FNAME];
	char ext[_MAX_EXT];
	char param[512];
	char *p1,*p2;
	long temp;
	FIO  fp_r;
	FIO  fp_w1,fp_w2,fp_w3,fp_w4,fp_w5,fp_w6;
	FILE *ofp;
	FILE *fp_param;
	FILE *fp_files;
	SOUNDFMT inFmt;
	unsigned char *inBuffer;
	DWORD i,inSample,nSample;
	fio_size rs,rd;
	fio_size wr;
	fio_size max_size;
	int retCode;
	FILEINFO fileInfo;
	fio_size seekPtr;
	fio_size rd_byte;
	SSIZE max,avg;
	double persent,per;
	int err1,err2,err3,err4,err5,err6;
	int thread = 1;
	int fio;
	fio = -1;
	errLine = 0;
	if (argc == 3) {
		do {
			memset(&NormInfo,0,sizeof (NORM_INFO));
			memset(&fileInfo,0,sizeof (FILEINFO));
PRINT_LOG("");
			// param ファイル
			_splitpath(argv[2],drive,dir,fname,ext);
			_makepath(tmppath,drive,dir,fname,"param");
			//printf("%s", tmppath);
			// ファイルオープン
			fp_param = fopen(tmppath,"r");
			if (fp_param == NULL) {
				retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
				break;
			}
			
PRINT_LOG("");
			if (fgets(param,512,fp_param) == NULL) {
				retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
				break;
			}
			fclose(fp_param);
			fp_param = NULL;
			p1 = param;
			p2 = strchr(p1,(int)' ');

PRINT_LOG(param);
			for (;p1 != NULL;) {
				if (p2 != NULL) {
					*p2 = '\0';
				}

				if (sscanf(p1,"-thread:%ld",&temp) == 1) {
					if (temp >= 1 && temp <= 24) {
						thread = (int)temp;
					}
				}
				if (sscanf(p1,"-fio:%ld",&temp) == 1) {
					if (temp >= 30 && temp <= 16000) {
						fio = temp / 10;
					}
				}
				if (p2 == NULL) {
					break;
				}
				p1 = p2 + 1;
				p2 = strchr(p1,(int)' ');
			}
			// 音声ファイル情報を取得する
			retCode = PLG_InfoAudioData(argv[1],&inFmt,&inSample,&fileInfo);
			if (retCode != STATUS_SUCCESS) {
				retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
				break;
			}
			
			if (!(strcmp(inFmt.fmt,"wav") == 0 || strcmp(inFmt.fmt,"rf64") == 0) || inFmt.channel < 1 || inFmt.channel > 6) {
				retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
				break;
			}

#ifdef _OPENMP
	omp_set_num_threads(thread);
#endif
			// fioが使用するメモリバッファの数調整
			if (fio != -1) {
				switch (inFmt.channel) {
					case 1:
					case 2:
						break;
					case 3:
						fio = (fio * 2) / 3;
						break;
					case 4:
						fio = (fio * 2) / 4;
						break;
					case 5: 
						fio = (fio * 2) / 5;
						break;
					case 6: 
						fio = (fio * 2) / 6;
						break;
				}
			}

			// 入力ファイルオープン
			fio_open(&fp_r,argv[1],FIO_MODE_R);
			if (fp_r.error) {
				retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
				break;
			}
			seekPtr = (fio_size)fileInfo.dataOffset;
			fio_seek(&fp_r,seekPtr,SEEK_SET);
			if (fp_r.error) {
				retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
				break;
			}

			// 出力ファイル名の作成(files)
			_splitpath(argv[2],drive,dir,fname,ext);
			_makepath(tmppath,drive,dir,fname,"files");
			// 出力ファイルオープン(param)
			fp_files = fopen(tmppath,"a");
			if (fp_files == NULL) {
				retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
				break;
			}

			// 出力ファイル名の作成
			_makepath(tmppath,drive,dir,fname,"param");
			// 出力ファイルオープン
			fp_param = fopen(tmppath,"a");
			if (fp_param == NULL) {
				retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
				break;
			}
			fprintf(fp_param,"-is:%d -iw:%d\n",inFmt.sample,inFmt.bitsPerSample);

			max_size = inSample;
			max_size *= sizeof (SSIZE);

			// 出力ファイル名の作成(Left)
			_makepath(tmppath,drive,dir,fname,"r1");
			// 出力ファイルオープン
			
			fio_open(&fp_w1,tmppath,FIO_MODE_W);
			if (fp_w1.error) {
				retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
				break;
			}
			fprintf(fp_files,"%s\n",tmppath);
			fio_set_maxsize(&fp_w1,max_size);
			if (fio != -1) {
				fio_set_memory_limit(&fp_w1,fio);
			}
			if (inFmt.channel >= 2) {
				// 出力ファイル名の作成(Right)
				_makepath(tmppath,drive,dir,fname,"r2");
				// 出力ファイルオープン
				fio_open(&fp_w2,tmppath,FIO_MODE_W);
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fprintf(fp_files,"%s\n",tmppath);
				fio_set_maxsize(&fp_w2,max_size);
				if (fio != -1) {
					fio_set_memory_limit(&fp_w2,fio);
				}
			}
			if (inFmt.channel >= 3) {
				// 出力ファイル名の作成(3)
				_makepath(tmppath,drive,dir,fname,"r3");
				// 出力ファイルオープン
				fio_open(&fp_w3,tmppath,FIO_MODE_W);
				if (fp_w3.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fprintf(fp_files,"%s\n",tmppath);
				fio_set_maxsize(&fp_w3,max_size);
				if (fio != -1) {
					fio_set_memory_limit(&fp_w3,fio);
				}
			}
			if (inFmt.channel >= 4) {
				// 出力ファイル名の作成(4)
				_makepath(tmppath,drive,dir,fname,"r4");
				// 出力ファイルオープン
				fio_open(&fp_w4,tmppath,FIO_MODE_W);
				if (fp_w4.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fprintf(fp_files,"%s\n",tmppath);
				fio_set_maxsize(&fp_w4,max_size);
				if (fio != -1) {
					fio_set_memory_limit(&fp_w4,fio);
				}
			}
			if (inFmt.channel >= 5) {
				// 出力ファイル名の作成(5)
				_makepath(tmppath,drive,dir,fname,"r5");
				// 出力ファイルオープン
				fio_open(&fp_w5,tmppath,FIO_MODE_W);
				if (fp_w5.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fprintf(fp_files,"%s\n",tmppath);
				fio_set_maxsize(&fp_w5,max_size);
				if (fio != -1) {
					fio_set_memory_limit(&fp_w5,fio);
				}
			}
			if (inFmt.channel >= 6) {
				// 出力ファイル名の作成(6)
				_makepath(tmppath,drive,dir,fname,"r6");
				// 出力ファイルオープン
				fio_open(&fp_w6,tmppath,FIO_MODE_W);
				if (fp_w6.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				fprintf(fp_files,"%s\n",tmppath);
				fio_set_maxsize(&fp_w6,max_size);
				if (fio != -1) {
					fio_set_memory_limit(&fp_w6,fio);
				}
			}

			// 読み込みバッファサイズ計算
			if (inFmt.bitsPerSample != 20) {
				rs = (inFmt.bitsPerSample / 8) * inFmt.channel;
			} else {
				rs = (24 / 8) * inFmt.channel;
			}

			inBuffer = (unsigned char *)malloc(rs * inFmt.sample * 10);
			if (inBuffer == NULL) {
				retCode = STATUS_MEM_ALLOC_ERR;errLine = __LINE__;
				break;
			}
			// 変換と書き出し

			err1 = err2 = err3 = err4 = err5 = err6 = 0;
			per = -1;
			for (i = 0;i < inSample;i += inFmt.sample * 10) {
PRINT_LOG("");
				persent = ((double)i / inSample);
				persent *= 100;
				if (persent != per) {
//					Sleep(1);
					fprintf(stdout,"%d%%\n",(int)persent);
					fflush(stdout);
				}
				per = persent;

				if (err1 || err2 || err3 || err4 || err5 || err6) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
				nSample = inFmt.sample * 10;
				if (i + nSample > inSample) {
					nSample = inSample - i;
				}
				rd = fio_read(inBuffer,rs,nSample,&fp_r);
				if (fp_r.error) {
					retCode = STATUS_FILE_READ_ERR;errLine = __LINE__;
					break;
				}
				if (rd == 0) {
					break;
				}
				#pragma omp parallel
				{
					#pragma omp sections
					{
						#pragma omp section
						{
							if (nBitTo64S(inFmt.channel,0,inFmt.bitsPerSample,inBuffer,&fp_w1,rd)) {
								err1 = 1;
							}
						}
						#pragma omp section
						{
							if (inFmt.channel >= 2) {
								if (nBitTo64S(inFmt.channel,1,inFmt.bitsPerSample,inBuffer,&fp_w2,rd)) {
									err2 = 1;
								}
							}
						}
						#pragma omp section
						{
							if (inFmt.channel >= 3) {
								if (nBitTo64S(inFmt.channel,2,inFmt.bitsPerSample,inBuffer,&fp_w3,rd)) {
									err3 = 1;
								}
							}
						}
						#pragma omp section
						{
							if (inFmt.channel >= 4) {
								if (nBitTo64S(inFmt.channel,3,inFmt.bitsPerSample,inBuffer,&fp_w4,rd)) {
									err4 = 1;
								}
							}
						}
						#pragma omp section
						{
							if (inFmt.channel >= 5) {
								if (nBitTo64S(inFmt.channel,4,inFmt.bitsPerSample,inBuffer,&fp_w5,rd)) {
									err5 = 1;
								}
							}
						}
						#pragma omp section
						{
							if (inFmt.channel >= 6) {
								if (nBitTo64S(inFmt.channel,5,inFmt.bitsPerSample,inBuffer,&fp_w6,rd)) {
									err6 = 1;
								}
							}
						}
					}
				}
			}
PRINT_LOG("");
			fio_close(&fp_r);
			fio_close(&fp_w1);
			if (fp_w1.error) {
				retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
				break;
			}
			if (inFmt.channel >= 2) {
				fio_close(&fp_w2);
				if (fp_w2.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
			}
			if (inFmt.channel >= 3) {
				fio_close(&fp_w3);
				if (fp_w3.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
			}
			if (inFmt.channel >= 4) {
				fio_close(&fp_w4);
				if (fp_w4.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
			}
			if (inFmt.channel >= 5) {
				fio_close(&fp_w5);
				if (fp_w5.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
			}
			if (inFmt.channel >= 6) {
				fio_close(&fp_w6);
				if (fp_w6.error) {
					retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
					break;
				}
			}

PRINT_LOG("");
			if (retCode != STATUS_SUCCESS) {
				break;
			}
PRINT_LOG("");
			
			// 音の最大レベルを記録する(1)
			if (NormInfo.maxR1 < 0) {
				NormInfo.maxR1 *= -1;
			}
			if (NormInfo.minR1 < 0) {
				NormInfo.minR1 *= -1;
			}
			max = NormInfo.maxR1;
			if (max < NormInfo.minR1) {
				max = NormInfo.minR1;
			}
			if (NormInfo.cntLv1 > 0) {
				NormInfo.tmpLv1 /= NormInfo.cntLv1;
			}

			if (NormInfo.maxLv1 > 0) {
				avg = (NormInfo.maxLv1 + NormInfo.tmpLv1) / 2;
			} else {
				avg = NormInfo.tmpLv1;
			}
			avg <<= 40;
			persent = (double)max / (double)0x7FFFFFFFFFFFFF;
			wr = fprintf(fp_param,"r1=%.10lf,%llx\n",persent,avg);	// 音のレベル(%),音の平均値
			if (wr == EOF) {
				retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
				break;
			}

			// 音の最大レベルを記録する(2)
			persent = 1.00;
			avg = 0;
			if (inFmt.channel >= 2) {
				if (NormInfo.maxR2 < 0) {
					NormInfo.maxR2 *= -1;
				}
				if (NormInfo.minR2 < 0) {
					NormInfo.minR2 *= -1;
				}
				max = NormInfo.maxR2;
				if (max < NormInfo.minR2) {
					max = NormInfo.minR2;
				}

				if (NormInfo.cntLv2 > 0) {
					NormInfo.tmpLv2 /= NormInfo.cntLv2;
				}
				if (NormInfo.maxLv2 > 0) {
					avg = (NormInfo.maxLv2 + NormInfo.tmpLv2) / 2;
				} else {
					avg = NormInfo.tmpLv2;
				}
				avg <<= 40;
				persent = (double)max / (double)0x7FFFFFFFFFFFFF;
			}
			wr = fprintf(fp_param,"r2=%.10lf,%llx\n",persent,avg);
			if (wr == EOF) {
				retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
				break;
			}
			// 音の最大レベルを記録する(3)
			persent = 1.00;
			avg = 0;
			if (inFmt.channel >= 3) {
				if (NormInfo.maxR3 < 0) {
					NormInfo.maxR3 *= -1;
				}
				if (NormInfo.minR3 < 0) {
					NormInfo.minR3 *= -1;
				}
				max = NormInfo.maxR3;
				if (max < NormInfo.minR3) {
					max = NormInfo.minR3;
				}
				if (NormInfo.cntLv3 > 0) {
					NormInfo.tmpLv3 /= NormInfo.cntLv3;
				}
				if (NormInfo.maxLv3 > 0) {
					avg = (NormInfo.maxLv3 + NormInfo.tmpLv3) / 2;
				} else {
					avg = NormInfo.tmpLv3;
				}
				avg <<= 40;
				persent = (double)max / (double)0x7FFFFFFFFFFFFF;
			}
			wr = fprintf(fp_param,"r3=%.10lf,%llx\n",persent,avg);
			if (wr == EOF) {
				retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
				break;
			}
			// 音の最大レベルを記録する(4)
			persent = 1.00;
			avg = 0;
			if (inFmt.channel >= 4) {
				if (NormInfo.maxR4 < 0) {
					NormInfo.maxR4 *= -1;
				}
				if (NormInfo.minR4 < 0) {
					NormInfo.minR4 *= -1;
				}
				max = NormInfo.maxR4;
				if (max < NormInfo.minR4) {
					max = NormInfo.minR4;
				}
				if (NormInfo.cntLv4 > 0) {
					NormInfo.tmpLv4 /= NormInfo.cntLv4;
				}
				if (NormInfo.maxLv4 > 0) {
					avg = (NormInfo.maxLv4 + NormInfo.tmpLv4) / 2;
				} else {
					avg = NormInfo.tmpLv4;
				}
				avg <<= 40;
				persent = (double)max / (double)0x7FFFFFFFFFFFFF;
			}
			wr = fprintf(fp_param,"r4=%.10lf,%llx\n",persent,avg);
			if (wr == EOF) {
				retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
				break;
			}
			// 音の最大レベルを記録する(5)
			persent = 1.00;
			avg = 0;
			if (inFmt.channel >= 5) {
				if (NormInfo.maxR5 < 0) {
					NormInfo.maxR5 *= -1;
				}
				if (NormInfo.minR5 < 0) {
					NormInfo.minR5 *= -1;
				}
				max = NormInfo.maxR5;
				if (max < NormInfo.minR5) {
					max = NormInfo.minR5;
				}
				if (NormInfo.cntLv5 > 0) {
					NormInfo.tmpLv5 /= NormInfo.cntLv5;
				}
				if (NormInfo.maxLv5 > 0) {
					avg = (NormInfo.maxLv5 + NormInfo.tmpLv5) / 2;
				} else {
					avg = NormInfo.tmpLv5;
				}
				avg <<= 40;
				persent = (double)max / (double)0x7FFFFFFFFFFFFF;
			}
			wr = fprintf(fp_param,"r5=%.10lf,%llx\n",persent,avg);
			if (wr == EOF) {
				retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
				break;
			}
			// 音の最大レベルを記録する(6)
			persent = 1.00;
			if (inFmt.channel >= 6) {
				if (NormInfo.maxR6 < 0) {
					NormInfo.maxR6 *= -1;
				}
				if (NormInfo.minR6 < 0) {
					NormInfo.minR6 *= -1;
				}
				max = NormInfo.maxR6;
				if (max < NormInfo.minR6) {
					max = NormInfo.minR6;
				}
				if (NormInfo.cntLv6 > 0) {
					NormInfo.tmpLv6 /= NormInfo.cntLv6;
				}
				if (NormInfo.maxLv6 > 0) {
					avg = (NormInfo.maxLv6 + NormInfo.tmpLv6) / 2;
				} else {
					avg = NormInfo.tmpLv6;
				}
				avg <<= 40;
				persent = (double)max / (double)0x7FFFFFFFFFFFFF;
			}
			wr = fprintf(fp_param,"r6=%.10lf,%llx\n",persent,avg);
			if (wr == EOF) {
				retCode = STATUS_FILE_WRITE_ERR;errLine = __LINE__;
				break;
			}
			fclose(fp_param);
			fclose(fp_files);
			retCode = STATUS_SUCCESS;
PRINT_LOG("");
		} while (0);
	} else {
		printf(STR_COPYRIGHT);
		printf(STR_USAGE);
		exit(0);
	}
	if (retCode != STATUS_SUCCESS) {
		_splitpath(argv[2],drive,dir,fname,ext);
		_makepath(tmppath,drive,dir,fname,"err");
		ofp = fopen(tmppath,"w");
		if (ofp) {
			switch (retCode) {
				case STATUS_FILE_READ_ERR:
					fprintf(ofp,"wav2raw:[%04d] File read error.\n",errLine);
					break;
				case STATUS_FILE_WRITE_ERR:
					fprintf(ofp,"wav2raw:[%04d] File write error.\n",errLine);
					break;
				case STATUS_MEM_ALLOC_ERR:
					fprintf(ofp,"wav2raw:[%04d] Memory Allocation error.\n",errLine);
					break;
				default:
					fprintf(ofp,"wav2raw:[%04d] Other error.\n",errLine);
			}
			fclose(ofp);
		}
	}
	exit(retCode);
}

//---------------------------------------------------------------------------
// Function   : nBitTo64S
// Description: nBit のデータを64Bit 符号付データに変換する(内部表現は60Bit)
// ---
//	nCh		:チャンネル数
//	ch		:音声データのチャネル
//	bit 	:入力データのビット数
//	in		:入力データのアドレス
//	fio 	:出力FIOのアドレス
//	nSample :サンプル数
//
int nBitTo64S(int nCh,int ch,int bit,void *in,FIO *fio,DWORD nSample)
{
	DWORD i;
	SSIZE ns;
	short *p16;
	unsigned char *p24;
	float  *p32;
	double *p64;
	SSIZE out;
	SSIZE max,min;
	SSIZE maxLv,maxLv2;
	int next;
	fio_size ws;

	ns = 0;
	maxLv2 = 0;
	if (bit == 16) {
		/* 16Bit */
		next = 1 * nCh;
		p16 = (short *)in;
		if (ch >= 1) {
			p16++;
		}
		if (ch >= 2) {
			p16++;
		}
		if (ch >= 3) {
			p16++;
		}
		if (ch >= 4) {
			p16++;
		}
		if (ch >= 5) {
			p16++;
		}
	}
	if (bit == 24 || bit == 20) {
		/* 24Bit */
		next = 3 * nCh;
		p24 = (char *)in;
		if (ch >= 1) {
			p24+=3;
		}
		if (ch >= 2) {
			p24+=3;
		}
		if (ch >= 3) {
			p24+=3;
		}
		if (ch >= 4) {
			p24+=3;
		}
		if (ch >= 5) {
			p24+=3;
		}
	}
	if (bit == 32) {
		/* 32Bit */
		p32 = (float *)in;
		next = 1 * nCh;
		if (ch >= 1) {
			p32++;
		}
		if (ch >= 2) {
			p32++;
		}
		if (ch >= 3) {
			p32++;
		}
		if (ch >= 4) {
			p32++;
		}
		if (ch >= 5) {
			p32++;
		}
	}
	if (bit == 64) {
		/* 64Bit */
		p64 = (double *)in;
		next = 1 * nCh;
		if (ch >= 1) {
			p64++;
		}
		if (ch >= 2) {
			p64++;
		}
		if (ch >= 3) {
			p64++;
		}
		if (ch >= 4) {
			p64++;
		}
		if (ch >= 5) {
			p64++;
		}
	}

	if (ch == 0) {
		max = NormInfo.maxR1;
		min = NormInfo.minR1;
		maxLv = NormInfo.maxLv1;
		maxLv2 = NormInfo.tmpLv1;
		ns	   = NormInfo.cntLv1;
	} else if (ch == 1) {
		max = NormInfo.maxR2;
		min = NormInfo.minR2;
		maxLv = NormInfo.maxLv2;
		maxLv2 = NormInfo.tmpLv2;
		ns	   = NormInfo.cntLv2;
	} else if (ch == 2) {
		max = NormInfo.maxR3;
		min = NormInfo.minR3;
		maxLv = NormInfo.maxLv3;
		maxLv2 = NormInfo.tmpLv3;
		ns	   = NormInfo.cntLv3;
	} else if (ch == 3) {
		max = NormInfo.maxR4;
		min = NormInfo.minR4;
		maxLv = NormInfo.maxLv4;
		maxLv2 = NormInfo.tmpLv4;
		ns	   = NormInfo.cntLv4;
	} else if (ch == 4) {
		max = NormInfo.maxR5;
		min = NormInfo.minR5;
		maxLv = NormInfo.maxLv5;
		maxLv2 = NormInfo.tmpLv5;
		ns	   = NormInfo.cntLv5;
	} else if (ch == 5) {
		max = NormInfo.maxR6;
		min = NormInfo.minR6;
		maxLv = NormInfo.maxLv6;
		maxLv2 = NormInfo.tmpLv6;
		ns	   = NormInfo.cntLv6;
	}

	switch (bit) {
		case 16:
			for (i = 0;i < nSample;i++) {
				out = (SSIZE)*p16;
				out <<= (64 - 16);
				out >>= 8;
				
				if (max < out) {
					max = out;
				} else if (min > out) {
					min = out;
				}
				if ((out >> 40) > 0) {
					maxLv2 += (out >> 40);
					ns++;
					if (maxLv2 >= 0x1000000000000) {
						maxLv2 /= ns;
						if (maxLv > 0) {
							maxLv = (maxLv + maxLv2) / 2;
						} else {
							maxLv = maxLv2;
						}
						maxLv2 = 0;
						ns = 0;
					}
				}
				p16 += next;
				ws = fio_write(&out,sizeof (SSIZE),1,fio);
				if (fio->error || ws != 1) {
					break;
				}
			}
			break;
		case 20:
			for (i = 0;i < nSample;i++) {
				out  = (SSIZE)p24[2];
				out  = out << 8;
				out |= (SSIZE)p24[1];
				out  = out << 8;
				out |= (SSIZE)p24[0];
				out  = out << (64 - 20);
				out  = out >> 8;

				if (max < out) {
					max = out;
				} else if (min > out) {
					min = out;
				}
				if ((out >> 40) > 0) {
					maxLv2 += (out >> 40);
					ns++;
					if (maxLv2 >= 0x1000000000000) {
						maxLv2 /= ns;
						if (maxLv > 0) {
							maxLv = (maxLv + maxLv2) / 2;
						} else {
							maxLv = maxLv2;
						}
						maxLv2 = 0;
						ns = 0;
					}
				}

				p24 += next;
				ws = fio_write(&out,sizeof (SSIZE),1,fio);
				if (fio->error || ws != 1) {
					break;
				}
			}
			break;
		case 24:
			for (i = 0;i < nSample;i++) {
				out  = (SSIZE)p24[2];
				out  = out << 8;
				out |= (SSIZE)p24[1];
				out  = out << 8;
				out |= (SSIZE)p24[0];
				out  = out << (64 - 24);
				out  = out >> 8;

				if (max < out) {
					max = out;
				} else if (min > out) {
					min = out;
				}
				if ((out >> 40) > 0) {
					maxLv2 += (out >> 40);
					ns++;
					if (maxLv2 >= 0x1000000000000) {
						maxLv2 /= ns;
						if (maxLv > 0) {
							maxLv = (maxLv + maxLv2) / 2;
						} else {
							maxLv = maxLv2;
						}
						maxLv2 = 0;
						ns = 0;
					}
				}

				p24 += next;
				ws = fio_write(&out,sizeof (SSIZE),1,fio);
				if (fio->error || ws != 1) {
					break;
				}
			}
			break;
		case 32:
			for (i = 0;i < nSample;i++) {
				out = (SSIZE)(*p32 * 0x7FFFFFFFFFFFFF);
				
				if (max < out) {
					max = out;
				} else if (min > out) {
					min = out;
				}
				if ((out >> 40) > 0) {
					maxLv2 += (out >> 40);
					ns++;
					if (maxLv2 >= 0x1000000000000) {
						maxLv2 /= ns;
						if (maxLv > 0) {
							maxLv = (maxLv + maxLv2) / 2;
						} else {
							maxLv = maxLv2;
						}
						maxLv2 = 0;
						ns = 0;
					}
				}
				
				p32 += next;
				ws = fio_write(&out,sizeof (SSIZE),1,fio);
				if (fio->error || ws != 1) {
					break;
				}
			}
			break;
		case 64:
			for (i = 0;i < nSample;i++) {
				out = (SSIZE)(*p64 * 0x7FFFFFFFFFFFFF);
				
				if (max < out) {
					max = out;
				} else if (min > out) {
					min = out;
				}
				if ((out >> 40) > 0) {
					maxLv2 += (out >> 40);
					ns++;
					if (maxLv2 >= 0x1000000000000) {
						maxLv2 /= ns;
						if (maxLv > 0) {
							maxLv = (maxLv + maxLv2) / 2;
						} else {
							maxLv = maxLv2;
						}
						maxLv2 = 0;
						ns = 0;
					}
				}

				p64 += next;
				ws = fio_write(&out,sizeof (SSIZE),1,fio);
				if (fio->error || ws != 1) {
					break;
				}
			}
			break;
		default:
			break;
	}
	
	if (ch == 0) {
		NormInfo.maxR1 = max;
		NormInfo.minR1 = min;
		NormInfo.maxLv1 = maxLv;
		NormInfo.tmpLv1 = maxLv2;
		NormInfo.cntLv1 = ns;
	} else if (ch == 1) {
		NormInfo.maxR2 = max;
		NormInfo.minR2 = min;
		NormInfo.maxLv2 = maxLv;
		NormInfo.tmpLv2 = maxLv2;
		NormInfo.cntLv2 = ns;
	} else if (ch == 2) {
		NormInfo.maxR3 = max;
		NormInfo.minR3 = min;
		NormInfo.maxLv3 = maxLv;
		NormInfo.tmpLv3 = maxLv2;
		NormInfo.cntLv3 = ns;
	} else if (ch == 3) {
		NormInfo.maxR4 = max;
		NormInfo.minR4 = min;
		NormInfo.maxLv4 = maxLv;
		NormInfo.tmpLv4 = maxLv2;
		NormInfo.cntLv4 = ns;
	} else if (ch == 4) {
		NormInfo.maxR5 = max;
		NormInfo.minR5 = min;
		NormInfo.maxLv5 = maxLv;
		NormInfo.tmpLv5 = maxLv2;
		NormInfo.cntLv5 = ns;
	} else if (ch == 5) {
		NormInfo.maxR6 = max;
		NormInfo.minR6 = min;
		NormInfo.maxLv6 = maxLv;
		NormInfo.tmpLv6 = maxLv2;
		NormInfo.cntLv6 = ns;
	}
	if (fio->error || ws != 1) {
		return -1;
	}
	
	return 0;
}
