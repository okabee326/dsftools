//---------------------------------------------------------------------------
/****************************************************************************/
/* dsf2raw (C) 2011 By 59414d41												*/
/*																			*/
/*																			*/
/****************************************************************************/

/*--- Log ------------------------------------------------------------------
 * Ver 0.01 <11/11/20> - DSFファイル対応
 */

#define STR_COPYRIGHT	"dsf2raw.exe (c) 2011 Ver 0.02 By 59414d41\n\n"
#define STR_USAGE		"dsf2raw.exe dsffile rawfile\n"

#define STATUS_SUCCESS			(0)		/* 正常終了 */
#define STATUS_INVALID_PARAM	(-1)	/* パラメーターエラー */
#define STATUS_MEM_ALLOC_ERR	(-4)	/* メモリー確保エラー */
#define STATUS_FILE_READ_ERR	(-5)	/* ファイルリードエラー */
#define STATUS_FILE_WRITE_ERR	(-6)	/* ファイルライトエラー */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include "fftw3.h"
#include "./../upconv/fileio.h"

#if 0
#define	PRINT_LOG(s)	do {																	\
							FILE *log;															\
							clock_t cc;															\
							double ts;															\
							log = fopen("d:\\dsf.log","a");										\
							if (log) {															\
								cc = clock();													\
								ts = (double)cc / CLOCKS_PER_SEC;								\
								fprintf(log,"%.8f %s [%d] %s\n",ts,__FUNCTION__,__LINE__,s);	\
								fclose(log);													\
							}																	\
						} while (0)
#else
#define	PRINT_LOG(s)	//
#endif

#ifdef _OPENMP
#include <omp.h>
#endif
#ifndef M_PI
#define M_PI		(3.14159265358979323846)
#endif

#ifndef DWORD
#define DWORD		unsigned long
#endif

#ifndef TRUE
#define TRUE		(1)
#endif

#ifndef FALSE
#define FALSE		(0)
#endif

// サンプルを処理するデータ型
#define SSIZE	signed long long
#define UI64	unsigned long long

//
// DSF ファイルフォーマット仕様書を参照
#pragma pack(push, 1)
typedef struct {
	char	id[4];
	UI64	chunk_size;
	UI64	file_size;
	UI64	ptr_meta;
} DSF;

typedef struct {
	char	id[4];
	UI64	chunk_size;
	DWORD	fmt_version;
	DWORD	fmt_id;
	DWORD	channel_type;
	DWORD	channel_count;
	DWORD	sampling;
	DWORD	sample_bit_count;
	UI64	sample_count;
	DWORD	block_size;
	DWORD	reserved;
} DSF_FMT;

typedef struct {
	char	id[4];
	UI64	chunk_size;
} DSF_DATA;
#pragma pack(pop)

typedef struct {
	UI64	sampling;
	UI64	data_offset;
	UI64	n_sample;
	int		channel;
	int		err;
	int		errLine;
	int		decode;
	long	thread;
	long	inSampleR;
	long	outSampleR;
	long	lfc;
	long	hfc;
	long	src_flag;
	long	dsf_mode;
	long	fio;
	SSIZE	l_min,l_max;
	SSIZE	r_min,r_max;
	SSIZE	l_maxLv,r_maxLv;
	SSIZE	l_tmpLv,r_tmpLv;
	SSIZE	l_cntLv,r_cntLv;
} PARAM_INFO;

/*--- Function Prototype ---------------------------------------------------*/
void dsf_encode(char *in_file,char *out_file,PARAM_INFO *param);
void dsf_decode(char *in_file,char *out_file,PARAM_INFO *param);
void fftFilter(int lr,SSIZE inSample,SSIZE outSample,FIO *fp_r,FIO *fp_w,PARAM_INFO *param);
void fftFilterSub(SSIZE *pIn,SSIZE *pOut,fftw_complex *fftw_in,fftw_complex *fftw_out,fftw_plan fftw_p,fftw_plan fftw_ip,PARAM_INFO *param,int id);
void onebit2nbit(SSIZE offset,SSIZE n,SSIZE *buffer,FIO *fp_r,PARAM_INFO *param);
void deinterleave(UI64 inByte,FIO *fp_r,FIO *fp_w1,FIO *fp_w2,PARAM_INFO *param);
void ana_abe(SSIZE start,UI64 nSample,SSIZE *i_buffer,SSIZE *o_buffer,PARAM_INFO *param);
inline void copyToFFTW(fftw_complex *fftw,SSIZE *buf,long size);
inline void windowFFTW(fftw_complex *fftw,long size);
inline void cutFFTW(fftw_complex *fftw,long index,long size);
void *al_malloc(long size);
void *al_free(void *ptr);

//---------------------------------------------------------------------------
// Function   : main
// Description: 引数を処理し変換関数を呼び出す
//
//
int main(int argc, char *argv[])
{
	PARAM_INFO param;
	FILE *fp;
	char infile[_MAX_PATH];
	char outfile[_MAX_PATH];
	char tmppath[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char *p1,*p2;
	char pparam[512];
	int retCode;
	long temp;

	do {
		infile[0] = '\0';
		outfile[0] = '\0';
		retCode = STATUS_SUCCESS;
		memset(&param,0,sizeof (PARAM_INFO));
		param.l_min = 0;
		param.l_max = 0;
		param.r_min = 0;
		param.r_max = 0;
		param.thread = 1;
		param.n_sample = 0;
		param.dsf_mode = 0;
		param.fio = -1;

		if (argc < 3) {
			break;
		}

		strcpy(infile,argv[1]);
		strcpy(outfile,argv[2]);

		_splitpath(argv[2],drive,dir,fname,ext);
		_makepath(tmppath,drive,dir,fname,"param");
		// ファイルオープン
		fp = fopen(tmppath,"r");
		if (fp == NULL) {
			retCode = STATUS_FILE_READ_ERR;
			break;
		}

		if (fgets(pparam,512,fp) == NULL) {
			retCode = STATUS_FILE_READ_ERR;
			break;
		}
		fclose(fp);
		p1 = pparam;
		p2 = strchr(p1,(int)' ');

		for (;p1 != NULL;) {
			if (p2 != NULL) {
				*p2 = '\0';
			}

			if (sscanf(p1,"-thread:%ld",&temp) == 1) {
				if (temp >= 1 && temp <= 24) {
					param.thread = (int)temp;
				}
			}

			if (sscanf(p1,"-dsf:%ld",&temp) == 1) {
				if (temp >= 0 && temp <= 2) {
					param.dsf_mode = (int)temp;
				}
			}

			if (sscanf(p1,"-fio:%ld",&temp) == 1) {
				if (temp >= 30 && temp <= 4000) {
					param.fio = temp / 10;
				}
			}

			if (p2 == NULL) {
				break;
			}
			p1 = p2 + 1;
			p2 = strchr(p1,(int)' ');
		}

		param.decode = 1;
		dsf_decode(infile,outfile,&param);

	} while (0);

	if (param.err) {
		_splitpath(argv[2],drive,dir,fname,ext);
		_makepath(tmppath,drive,dir,fname,"err");
		fp = fopen(tmppath,"w");
		if (fp) {
			switch (param.err) {
				case STATUS_FILE_READ_ERR:
					fprintf(fp,"dsf2raw:[%04d] File read error.\n",param.errLine);
					break;
				case STATUS_FILE_WRITE_ERR:
					fprintf(fp,"dsf2raw:[%04d] File write error.\n",param.errLine);
					break;
				case STATUS_MEM_ALLOC_ERR:
					fprintf(fp,"dsf2raw:[%04d] Memory Allocation error.\n",param.errLine);
					break;
				default:
					fprintf(fp,"raw2wav:Other error.\n");
			}
			fclose(fp);
		}
	}
	return 0;
}

//---------------------------------------------------------------------------
// Function   : dsf_encode
// Description: DSF エンコード処理
// ---
// WAV ファイルを DSF ファイルへエンコードする
//
void dsf_encode(char *in_file,char *out_file,PARAM_INFO *param)
{
	// 未サポート
	param->err = 1;param->errLine = __LINE__;
}
//---------------------------------------------------------------------------
// Function   : dsf_decode
// Description: DSF デコード処理
// ---
// DSF ファイルを WAV ファイルへエンコードする
//
void dsf_decode(char *in_file,char *out_file,PARAM_INFO *param)
{
	char tmppath[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char tmpfile[_MAX_PATH];
	FIO fp_r,fp_w1,fp_w2;
	FILE *fp;
	DSF dsf;
	DSF_FMT dsf_fmt;
	DSF_DATA dsf_data;
	fio_size rs;
	fio_size ws;
	fio_size file_size;
	UI64 remain;
	char dbs[100];
	DWORD inSample,outSample;
	double dd;
	int lr_flag;
	int i,j,n;
	SSIZE avg;

#ifdef _OPENMP
	int nCpu;
	nCpu = param->thread;
	omp_set_num_threads(nCpu);
#endif

	do {
		memset(&fp_r,0,sizeof (FIO));
		memset(&fp_w1,0,sizeof (FIO));
		memset(&fp_w2,0,sizeof (FIO));
		
		fio_open(&fp_r,in_file,FIO_MODE_R);
		if (fp_r.error) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}

		rs = fio_read(&dsf,sizeof (DSF),1,&fp_r);
		if (fp_r.error || rs != 1) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		
		if (memcmp(dsf.id,"DSD ",4)) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf.chunk_size < 28) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf.file_size < (28 + 52 + 12)) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		fio_seek(&fp_r,dsf.chunk_size,SEEK_SET);
		rs = fio_read(&dsf_fmt,sizeof (DSF_FMT),1,&fp_r);
		if (fp_r.error || rs != 1) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		
		if (memcmp(dsf_fmt.id,"fmt ",4)) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf_fmt.chunk_size < 52) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf_fmt.fmt_version != 1) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf_fmt.fmt_id != 0) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (!(dsf_fmt.channel_type == 1 || dsf_fmt.channel_type == 2)) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (!(dsf_fmt.channel_count == 1 || dsf_fmt.channel_count == 2)) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (!(dsf_fmt.sampling == 2822400 || dsf_fmt.sampling == 2822400*2)) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf_fmt.sample_bit_count != 1) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf_fmt.sample_count < 1) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		if (dsf_fmt.block_size != 4096) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		fio_seek(&fp_r,dsf.chunk_size + dsf_fmt.chunk_size,SEEK_SET);
		rs = fio_read(&dsf_data,sizeof (DSF_DATA),1,&fp_r);
		if (fp_r.error || rs != 1) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}

		if (dsf_data.chunk_size <= 12) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		param->channel = dsf_fmt.channel_count;
		param->data_offset = fio_tell(&fp_r);

		sprintf(dbs,"DATA SIZE:%lld",dsf_data.chunk_size - 12);
		PRINT_LOG(dbs);
		sprintf(dbs,"sample count:%lld",dsf_fmt.sample_count);
		PRINT_LOG(dbs);

		// 出力ファイル(Left)
		_splitpath(out_file,drive,dir,fname,ext);
		_makepath(tmpfile,drive,dir,fname,"r1.tmp");
		fio_open(&fp_w1,tmpfile,FIO_MODE_W);
		if (fp_w1.error) {
			PRINT_LOG("");
			param->err = STATUS_FILE_WRITE_ERR;
			break;
		}
		if (param->channel == 1) {
			fio_set_maxsize(&fp_w1,dsf_data.chunk_size - 12);
			fio_set_memory_limit(&fp_w1,param->fio);
		}
		if (param->channel == 2) {
			// 出力ファイル(Right)
			_splitpath(out_file,drive,dir,fname,ext);
			_makepath(tmpfile,drive,dir,fname,"r2.tmp");
			fio_open(&fp_w2,tmpfile,FIO_MODE_W);
			if (fp_w2.error) {
				PRINT_LOG("");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
			fio_set_maxsize(&fp_w1,(dsf_data.chunk_size - 12) / 2);
			fio_set_maxsize(&fp_w2,(dsf_data.chunk_size - 12) / 2);
			fio_set_memory_limit(&fp_w1,param->fio);
			fio_set_memory_limit(&fp_w2,param->fio);
		}

		// 4096 ごとにインターリーブされているブロックをtmpファイルへ出力する(ステレオの場合)
		deinterleave(dsf_data.chunk_size - 12,&fp_r,&fp_w1,&fp_w2,param);
		if (param->err) {
			PRINT_LOG("");
			break;
		}
		fio_close(&fp_r);
		// 読み込みで開く
		fio_setmode_r(&fp_w1,&fp_r,NULL);
		if (fp_w1.error) {
			PRINT_LOG("");
			param->err = STATUS_FILE_WRITE_ERR;
			break;
		}
		if (fp_r.error) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}

		// 出力ファイル(Left)
		_splitpath(out_file,drive,dir,fname,ext);
		_makepath(tmpfile,drive,dir,fname,"r1");
		fio_open(&fp_w1,tmpfile,FIO_MODE_W);
		if (fp_w1.error) {
			PRINT_LOG("");
			param->err = STATUS_FILE_WRITE_ERR;
			break;
		}

		param->inSampleR  = dsf_fmt.sampling;
//		param->outSampleR  = dsf_fmt.sampling;
		param->outSampleR = 192000;

		remain = dsf_fmt.sample_count;
		remain *= 192000;
		remain /= dsf_fmt.sampling;
		remain *= sizeof (SSIZE);

		// 出力ファイルサイズの最大値を指定
		if (param->outSampleR == 192000) {
			fio_set_maxsize(&fp_w1,remain);
			fio_set_memory_limit(&fp_w1,param->fio);
		}
		remain = dsf_fmt.sample_count;
		if (param->outSampleR == 192000) {
			remain *= 192000;
			remain /= dsf_fmt.sampling;
		}
		sprintf(dbs,"remain:%lld",remain);
		PRINT_LOG(dbs);
		param->n_sample = dsf_fmt.sample_count;

		// Left
		fftFilter(0,dsf_fmt.sample_count,remain,&fp_r,&fp_w1,param);
		if (param->err) {
			PRINT_LOG("");
			break;
		}
		fio_close(&fp_r);
		fio_close(&fp_w1);
		if (param->channel == 2) {
			// 読み込みで開く
			fio_setmode_r(&fp_w2,&fp_r,NULL);
			if (fp_w2.error) {
				PRINT_LOG("");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
			if (fp_r.error) {
				PRINT_LOG("");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}

			// 出力ファイル(Right)
			_splitpath(out_file,drive,dir,fname,ext);
			_makepath(tmpfile,drive,dir,fname,"r2");
			fio_open(&fp_w2,tmpfile,FIO_MODE_W);
			if (fp_w2.error) {
				PRINT_LOG("");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
			remain = dsf_fmt.sample_count;
			remain *= 192000;
			remain /= dsf_fmt.sampling;
			remain *= sizeof (SSIZE);

			if (param->outSampleR == 192000) {
				// 出力ファイルサイズの最大値を指定
				fio_set_maxsize(&fp_w2,remain);
				fio_set_memory_limit(&fp_w2,param->fio);
			}
			remain = dsf_fmt.sample_count;
			if (param->outSampleR == 192000) {
				remain *= 192000;
				remain /= dsf_fmt.sampling;
			}
			// Right
			fftFilter(1,dsf_fmt.sample_count,remain,&fp_r,&fp_w2,param);
			if (param->err) {
				PRINT_LOG("");
				break;
			}
			fio_close(&fp_r);
			fio_close(&fp_w2);
		}

		// 出力ファイル名の作成
		_splitpath(out_file,drive,dir,fname,ext);
		_makepath(tmpfile,drive,dir,fname,"param");
		// 出力ファイルオープン
		fp = fopen(tmpfile,"a");
		if (fp == NULL) {
			param->err = STATUS_FILE_WRITE_ERR;
			break;
		}
		fprintf(fp,"-is:%d -iw:%d\n",192000,24);
		
		dd = param->l_min;
		if (dd < 0) {
			dd *= -1;
		}
		if (dd < param->l_max) {
			dd = param->l_max;
		}
		
		dd /= (double)0x7FFFFFFFFFFFFF;

		if (param->l_cntLv > 0) {
			param->l_tmpLv /= param->l_cntLv;
		}

		if (param->l_maxLv > 0) {
			avg = (param->l_maxLv + param->l_tmpLv) / 2;
		} else {
			avg = param->l_tmpLv;
		}
		avg <<= 40;

		fprintf(fp,"r1=%.10lf,%llx\n",dd,avg);
//		fprintf(fp,"r1=%lf\n",1.00);
		sprintf(dbs,"r1=%lf",dd);
		PRINT_LOG(dbs);

		if (param->channel == 1) {
			fprintf(fp,"r2=%.10lf,%llx\n",1.0,0);
		} else {
			dd = param->r_min;
			if (dd < 0) {
				dd *= -1;
			}
			if (dd < param->r_max) {
				dd = param->r_max;
			}
			dd /= (double)0x7FFFFFFFFFFFFF;

			if (param->r_cntLv > 0) {
				param->r_tmpLv /= param->r_cntLv;
			}

			if (param->r_maxLv > 0) {
				avg = (param->r_maxLv + param->r_tmpLv) / 2;
			} else {
				avg = param->r_tmpLv;
			}
			avg <<= 40;

			fprintf(fp,"r2=%.10lf,%llx\n",dd,avg);
//			fprintf(fp,"r2=%lf\n",1.00);
			sprintf(dbs,"r2=%lf",dd);
			PRINT_LOG(dbs);
		}
		fprintf(fp,"r3=%lf,%llx\n",1.0,0);
		fprintf(fp,"r4=%lf,%llx\n",1.0,0);
		fprintf(fp,"r5=%lf,%llx\n",1.0,0);
		fprintf(fp,"r6=%lf,%llx\n",1.0,0);
		fclose(fp);

	} while (0);
}
//---------------------------------------------------------------------------
// Function   : deinterleave
// Description: 4096ごとに記録されているチャンネルごとの1ブロックを結合する
// ---
//	fp_r		:入力ファイル用構造体
//	fp_w1		:出力ファイル用構造体(Left)
//	fp_w2		:出力ファイル用構造体(Right)
//	param		:変換パラメータ
//
void deinterleave(UI64 inByte,FIO *fp_r,FIO *fp_w1,FIO *fp_w2,PARAM_INFO *param)
{
	unsigned char *bit_buffer;
	fio_size rs,ws;
	int lr;

	bit_buffer	 = (unsigned char *)malloc(4096);
	if (bit_buffer == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;param->errLine = __LINE__;
		return;
	}

	fio_seek(fp_r,param->data_offset,SEEK_SET);
	if (fp_r->error) {
		param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
		return;
	}

	lr = 0;
	while (inByte > 0) {
		rs = fio_read(bit_buffer,1,4096,fp_r);
		if (fp_r->error || rs != 4096) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;param->errLine = __LINE__;
			break;
		}
		if (lr == 0) {
			ws = fio_write(bit_buffer,1,4096,fp_w1);
			if (fp_w1->error || ws != rs) {
				PRINT_LOG("");
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
		} else {
			ws = fio_write(bit_buffer,1,4096,fp_w2);
			if (fp_w2->error || ws != rs) {
				PRINT_LOG("");
				param->err = STATUS_FILE_WRITE_ERR;param->errLine = __LINE__;
				break;
			}
		}
		lr++;
		if (lr == 2) {
			lr = 0;
		}
		if (param->channel == 1) {
			lr = 0;
		}
		inByte -= 4096;
	}
	free(bit_buffer);
}
//---------------------------------------------------------------------------
// Function   : fftFilter
// Description: FFT によるフィルタ処理
// ---
//	lr			:Left/Right フラグ
//	inSample	:入力データのサンプル数(ch毎)
//	outSample	:出力データのサンプル数(ch毎)
//	fp_r		:入力ファイル用構造体
//	fp_w		:出力ファイル用構造体
//	param		:変換パラメータ
//
void fftFilter(int lr,SSIZE inSample,SSIZE outSample,FIO *fp_r,FIO *fp_w,PARAM_INFO *param)
{
	SSIZE *mem0,*mem1;
	SSIZE *mem2,*mem3,*mem4;
	SSIZE *mem5,*mem6,*mem7;
	SSIZE *mem8;
	char *inPtr;
	long wkMemSize;
	long inSampleR,outSampleR;
	long fftSizeIn,fftSizeOut,i,j,n;
	long cutOff;
	long hfc;
	long wr;
	long wkSampleR;
	long pwCnt;
	double persent,per;
	double nx;
	double *pwBase,basePw;
	SSIZE *pIn[6],*pOut[6];
	SSIZE startInSample,inSampLen,outSampLen,nSample;
	UI64 outRemain;
	fftw_complex *fftw_io[7];
	fftw_plan fftw_p[7],fftw_ip[7];
	SSIZE membyte;
	SSIZE delay;
	SSIZE sigma;
	SSIZE a,b;
	SSIZE min,max;
	char s[50];
	SSIZE maxLv,maxLv2;
	SSIZE ns;

	fftw_io[0] = NULL;
	fftw_io[1] = NULL;
	fftw_io[2] = NULL;
	fftw_io[3] = NULL;
	fftw_io[4] = NULL;
	fftw_io[5] = NULL;
	fftw_io[6] = NULL;

	fio_rewind(fp_r);

	inSampleR = param->inSampleR;
	outSampleR = param->outSampleR;
	fftSizeIn = inSampleR / 2;
	fftSizeOut = outSampleR / 2;
	if (param->inSampleR == 2822400 * 2) {
		fftSizeIn = inSampleR / 4;
		fftSizeOut = outSampleR / 4;
	}

	wkMemSize = fftSizeIn;
	wkMemSize *= 3;

	if (lr == 0) {
		min = param->l_min;
		max = param->l_max;
		maxLv  = param->l_maxLv;
		maxLv2 = param->l_tmpLv;
		ns	   = param->l_cntLv;
	} else {
		min = param->r_min;
		max = param->r_max;
		maxLv  = param->r_maxLv;
		maxLv2 = param->r_tmpLv;
		ns	   = param->r_cntLv;
	}
	
	// 入力用(1)
	mem1 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem1 == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 出力用(1)
	mem2 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem2 == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 出力用(1)
	mem3 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem3 == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 出力用(1)
	mem4 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem4 == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 出力用(2)
	mem5 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem5 == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 出力用(2)
	mem6 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem6 == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 出力用(2)
	mem7 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem7 == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 出力用(3)
	mem8 = (SSIZE *)al_malloc(wkMemSize * sizeof (SSIZE));
	if (mem8 == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// 1
	fftw_io[0] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_io[0] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 2
	fftw_io[1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_io[1] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 3
	fftw_io[2] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_io[2] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 4
	fftw_io[3] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_io[3] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 5
	fftw_io[4] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_io[4] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 6
	fftw_io[5] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * wkMemSize);
	if (fftw_io[5] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 7
	fftw_io[6] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 192000 * 2);
	if (fftw_io[6] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// 2822400 → 192000 用のプラン(1)
	fftw_p[0] = fftw_plan_dft_1d(fftSizeIn,fftw_io[0],fftw_io[0],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[0] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_ip[0] = fftw_plan_dft_1d(fftSizeOut,fftw_io[0],fftw_io[0],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[0] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 2822400 → 192000 用のプラン(2)
	fftw_p[1] = fftw_plan_dft_1d(fftSizeIn,fftw_io[1],fftw_io[1],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[1] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_ip[1] = fftw_plan_dft_1d(fftSizeOut,fftw_io[1],fftw_io[1],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[1] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 2822400 → 192000 用のプラン(3)
	fftw_p[2] = fftw_plan_dft_1d(fftSizeIn,fftw_io[2],fftw_io[2],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[2] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_ip[2] = fftw_plan_dft_1d(fftSizeOut,fftw_io[2],fftw_io[2],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[2] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 2822400 → 192000 用のプラン(4)
	fftw_p[3] = fftw_plan_dft_1d(fftSizeIn,fftw_io[3],fftw_io[3],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[3] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_ip[3] = fftw_plan_dft_1d(fftSizeOut,fftw_io[3],fftw_io[3],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[3] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 2822400 → 192000 用のプラン(5)
	fftw_p[4] = fftw_plan_dft_1d(fftSizeIn,fftw_io[4],fftw_io[4],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[4] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_ip[4] = fftw_plan_dft_1d(fftSizeOut,fftw_io[4],fftw_io[4],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[4] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	// 2822400 → 192000 用のプラン(6)
	fftw_p[5] = fftw_plan_dft_1d(fftSizeIn,fftw_io[5],fftw_io[5],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[5] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_ip[5] = fftw_plan_dft_1d(fftSizeOut,fftw_io[5],fftw_io[5],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[5] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	// 192000 → 192000 用のプラン
	fftw_p[6] = fftw_plan_dft_1d(fftSizeOut,fftw_io[6],fftw_io[6],FFTW_FORWARD,FFTW_ESTIMATE);
	if (fftw_p[6] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}
	fftw_ip[6] = fftw_plan_dft_1d(fftSizeOut,fftw_io[6],fftw_io[6],FFTW_BACKWARD,FFTW_ESTIMATE);
	if (fftw_ip[6] == NULL) {
		PRINT_LOG("ERROR");
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}


	outRemain = outSample;

	delay = 0;
	per = -1;
	sprintf(s,"%lld,%lld",inSample,outSample);
	PRINT_LOG(s);
	for (startInSample = ((fftSizeIn + (fftSizeIn / 2)) * -1);startInSample < inSample + ((fftSizeIn * 3) + fftSizeIn / 2);startInSample += fftSizeIn * 2) {
		if (startInSample >= 0 && startInSample < inSample) {
			persent = ((double)startInSample / inSample);
			persent *= 100;
			if (persent != per) {
				Sleep(1);
				if (param->channel == 2) {
					persent /= 2;
					if (lr == 1) {
						persent += 50;
					}
				}
				fprintf(stdout,"%d%%\n",(int)persent);
				fflush(stdout);
			}
			per = persent;
		}

		// 読み込んだデータをmem1に入れる
		memset(mem1,0,wkMemSize * sizeof (SSIZE));
		onebit2nbit(startInSample,wkMemSize,mem1,fp_r,param);
		if (param->err) {
			break;
		}

		// 1
		memset(mem2,0,wkMemSize * sizeof (SSIZE));
		memset(mem3,0,wkMemSize * sizeof (SSIZE));
		memset(mem4,0,wkMemSize * sizeof (SSIZE));
		// 2
		memset(mem5,0,wkMemSize * sizeof (SSIZE));
		memset(mem6,0,wkMemSize * sizeof (SSIZE));
		memset(mem7,0,wkMemSize * sizeof (SSIZE));

		memset(mem8,0,wkMemSize * sizeof (SSIZE));

		// DSD -> 192000 へデシメーション(1)
		param->lfc = 2;
		if (param->dsf_mode != 1) {
			param->hfc = 96000;
		} else {
			param->hfc = 24000;
		}
		param->src_flag = 1;

PRINT_LOG("1");
		// スレッド1,2,3の組とスレッド4,5,6の組でfftする
		pIn[0]	= &mem1[((fftSizeIn / 2) * 0)];
		pOut[0] = &mem2[((fftSizeOut / 2) * 0)];
		pIn[1]	= &mem1[((fftSizeIn / 2) * 1)];
		pOut[1] = &mem3[((fftSizeOut / 2) * 1)];
		pIn[2]	= &mem1[((fftSizeIn / 2) * 2)];
		pOut[2] = &mem4[((fftSizeOut / 2) * 2)];

		pIn[3]	= &mem1[((fftSizeIn / 2) * 2)];
		pOut[3] = &mem5[((fftSizeOut / 2) * 2)];
		pIn[4]	= &mem1[((fftSizeIn / 2) * 3)];
		pOut[4] = &mem6[((fftSizeOut / 2) * 3)];
		pIn[5]	= &mem1[((fftSizeIn / 2) * 4)];
		pOut[5] = &mem7[((fftSizeOut / 2) * 4)];

		#pragma omp parallel
		{
			#pragma omp sections
			{
				#pragma omp section
				{
					// 1
					fftFilterSub(pIn[0],pOut[0],fftw_io[0],fftw_io[0],fftw_p[0],fftw_ip[0],param,0);
				}
				#pragma omp section
				{
					// 2
					fftFilterSub(pIn[1],pOut[1],fftw_io[1],fftw_io[1],fftw_p[1],fftw_ip[1],param,1);
				}
				#pragma omp section
				{
					// 3
					fftFilterSub(pIn[2],pOut[2],fftw_io[2],fftw_io[2],fftw_p[2],fftw_ip[2],param,2);
				}
				#pragma omp section
				{
					// 4
					fftFilterSub(pIn[3],pOut[3],fftw_io[3],fftw_io[3],fftw_p[3],fftw_ip[3],param,0);
				}
				#pragma omp section
				{
					// 5
					fftFilterSub(pIn[4],pOut[4],fftw_io[4],fftw_io[4],fftw_p[4],fftw_ip[4],param,1);
				}
				#pragma omp section
				{
					// 6
					fftFilterSub(pIn[5],pOut[5],fftw_io[5],fftw_io[5],fftw_p[5],fftw_ip[5],param,2);
				}
			}
		}
		if (param->dsf_mode != 1) {
			memset(mem1,0,fftSizeOut * 3 * sizeof (SSIZE));
			#pragma omp parallel for
			for (i = fftSizeOut / 2;i < fftSizeOut + fftSizeOut / 2;i++) {
				mem1[i] = mem2[i] + mem3[i] + mem4[i];
			}
			#pragma omp parallel for
			for (i = fftSizeOut + fftSizeOut / 2;i < fftSizeOut * 2 + fftSizeOut / 2;i++) {
				mem1[i] = mem5[i] + mem6[i] + mem7[i];
			}
		} else {
			memset(mem8,0,fftSizeOut * 3 * sizeof (SSIZE));
			#pragma omp parallel for
			for (i = fftSizeOut / 2;i < fftSizeOut + fftSizeOut / 2;i++) {
				mem8[i] = mem2[i] + mem3[i] + mem4[i];
			}
			#pragma omp parallel for
			for (i = fftSizeOut + fftSizeOut / 2;i < fftSizeOut * 2 + fftSizeOut / 2;i++) {
				mem8[i] = mem5[i] + mem6[i] + mem7[i];
			}
			#pragma omp parallel for
			for (i = fftSizeOut / 2;i < fftSizeOut * 2 + fftSizeOut / 2;i++) {
				if (min > mem8[i]) {
					min = mem8[i];
				}
				if (max < mem8[i]) {
					max = mem8[i];
				}
				if ((mem8[i] >> 40) > 0) {
					maxLv2 += (mem8[i] >> 40);
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
			}
		}
		
		if (param->dsf_mode != 1) {
			// DSD 24kz 以上のデータカット
	PRINT_LOG("3");
			param->lfc = -1;
			param->hfc = 24000;
			param->src_flag = 2;

			memset(mem2,0,wkMemSize * sizeof (SSIZE));
			memset(mem3,0,wkMemSize * sizeof (SSIZE));
			memset(mem4,0,wkMemSize * sizeof (SSIZE));
			memset(mem5,0,wkMemSize * sizeof (SSIZE));
			memset(mem6,0,wkMemSize * sizeof (SSIZE));
			memset(mem7,0,wkMemSize * sizeof (SSIZE));

			pIn[0]	= &mem1[((fftSizeOut / 2) * 0)];
			pOut[0] = &mem2[((fftSizeOut / 2) * 0)];
			pIn[1]	= &mem1[((fftSizeOut / 2) * 1)];
			pOut[1] = &mem3[((fftSizeOut / 2) * 1)];
			pIn[2]	= &mem1[((fftSizeOut / 2) * 2)];
			pOut[2] = &mem4[((fftSizeOut / 2) * 2)];
			pIn[3]	= &mem1[((fftSizeOut / 2) * 2)];
			pOut[3] = &mem5[((fftSizeOut / 2) * 2)];
			pIn[4]	= &mem1[((fftSizeOut / 2) * 3)];
			pOut[4] = &mem6[((fftSizeOut / 2) * 3)];
			pIn[5]	= &mem1[((fftSizeOut / 2) * 4)];
			pOut[5] = &mem7[((fftSizeOut / 2) * 4)];

			// 1
			fftFilterSub(pIn[0],pOut[0],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,0);
			// 2
			fftFilterSub(pIn[1],pOut[1],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,1);
			// 3
			fftFilterSub(pIn[2],pOut[2],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,2);
			// 4
			fftFilterSub(pIn[3],pOut[3],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,0);
			// 5
			fftFilterSub(pIn[4],pOut[4],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,1);
			// 6
			fftFilterSub(pIn[5],pOut[5],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,2);

			memset(mem8,0,fftSizeOut * 3 * sizeof (SSIZE));
			#pragma omp parallel for
			for (i = fftSizeOut / 2;i < fftSizeOut + fftSizeOut / 2;i++) {
				mem8[i] = mem2[i] + mem3[i] + mem4[i];
			}
			#pragma omp parallel for
			for (i = fftSizeOut + fftSizeOut / 2;i < fftSizeOut * 2 + fftSizeOut / 2;i++) {
				mem8[i] = mem5[i] + mem6[i] + mem7[i];
			}
			#pragma omp parallel for
			for (i = fftSizeOut / 2;i < fftSizeOut * 2 + fftSizeOut / 2;i++) {
				if (min > mem8[i]) {
					min = mem8[i];
				}
				if (max < mem8[i]) {
					max = mem8[i];
				}
				if ((mem8[i] >> 40) > 0) {
					maxLv2 += (mem8[i] >> 40);
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
			}

			// 24kHz 以下のデータカット
			param->lfc = 24000;
			param->hfc = -1;
			param->src_flag = 2;

			memset(mem2,0,wkMemSize * sizeof (SSIZE));
			memset(mem3,0,wkMemSize * sizeof (SSIZE));
			memset(mem4,0,wkMemSize * sizeof (SSIZE));
			memset(mem5,0,wkMemSize * sizeof (SSIZE));
			memset(mem6,0,wkMemSize * sizeof (SSIZE));
			memset(mem7,0,wkMemSize * sizeof (SSIZE));

			pIn[0]	= &mem1[((fftSizeOut / 2) * 0)];
			pOut[0] = &mem2[((fftSizeOut / 2) * 0)];
			pIn[1]	= &mem1[((fftSizeOut / 2) * 1)];
			pOut[1] = &mem3[((fftSizeOut / 2) * 1)];
			pIn[2]	= &mem1[((fftSizeOut / 2) * 2)];
			pOut[2] = &mem4[((fftSizeOut / 2) * 2)];
			pIn[3]	= &mem1[((fftSizeOut / 2) * 2)];
			pOut[3] = &mem5[((fftSizeOut / 2) * 2)];
			pIn[4]	= &mem1[((fftSizeOut / 2) * 3)];
			pOut[4] = &mem6[((fftSizeOut / 2) * 3)];
			pIn[5]	= &mem1[((fftSizeOut / 2) * 4)];
			pOut[5] = &mem7[((fftSizeOut / 2) * 4)];

			// 1
			fftFilterSub(pIn[0],pOut[0],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,0);
			// 2
			fftFilterSub(pIn[1],pOut[1],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,1);
			// 3
			fftFilterSub(pIn[2],pOut[2],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,2);
			// 4
			fftFilterSub(pIn[3],pOut[3],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,0);
			// 5
			fftFilterSub(pIn[4],pOut[4],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,1);
			// 6
			fftFilterSub(pIn[5],pOut[5],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,2);


			memset(mem1,0,fftSizeOut * 3 * sizeof (SSIZE));
			#pragma omp parallel for
			for (i = fftSizeOut / 2;i < fftSizeOut + fftSizeOut / 2;i++) {
				mem1[i] = mem2[i] + mem3[i] + mem4[i];
			}
			#pragma omp parallel for
			for (i = fftSizeOut + fftSizeOut / 2;i < fftSizeOut * 2 + fftSizeOut / 2;i++) {
				mem1[i] = mem5[i] + mem6[i] + mem7[i];
			}

			if (param->dsf_mode == 2) {
				ana_abe(0,fftSizeOut * 3,mem1,mem5,param);
				#pragma omp parallel for
				for (i = 0;i < fftSizeOut * 3;i++) {
					mem1[i] = mem5[i];
				}
			}
			// 24kHz 以下のデータカット
			param->lfc = 24000;
			param->hfc = -1;
			param->src_flag = 2;

			memset(mem2,0,wkMemSize * sizeof (SSIZE));
			memset(mem3,0,wkMemSize * sizeof (SSIZE));
			memset(mem4,0,wkMemSize * sizeof (SSIZE));
			memset(mem5,0,wkMemSize * sizeof (SSIZE));
			memset(mem6,0,wkMemSize * sizeof (SSIZE));
			memset(mem7,0,wkMemSize * sizeof (SSIZE));

			pIn[0]	= &mem1[((fftSizeOut / 2) * 0)];
			pOut[0] = &mem2[((fftSizeOut / 2) * 0)];
			pIn[1]	= &mem1[((fftSizeOut / 2) * 1)];
			pOut[1] = &mem3[((fftSizeOut / 2) * 1)];
			pIn[2]	= &mem1[((fftSizeOut / 2) * 2)];
			pOut[2] = &mem4[((fftSizeOut / 2) * 2)];
			pIn[3]	= &mem1[((fftSizeOut / 2) * 2)];
			pOut[3] = &mem5[((fftSizeOut / 2) * 2)];
			pIn[4]	= &mem1[((fftSizeOut / 2) * 3)];
			pOut[4] = &mem6[((fftSizeOut / 2) * 3)];
			pIn[5]	= &mem1[((fftSizeOut / 2) * 4)];
			pOut[5] = &mem7[((fftSizeOut / 2) * 4)];

			// 1
			fftFilterSub(pIn[0],pOut[0],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,0);
			// 2
			fftFilterSub(pIn[1],pOut[1],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,1);
			// 3
			fftFilterSub(pIn[2],pOut[2],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,2);
			// 4
			fftFilterSub(pIn[3],pOut[3],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,0);
			// 5
			fftFilterSub(pIn[4],pOut[4],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,1);
			// 6
			fftFilterSub(pIn[5],pOut[5],fftw_io[6],fftw_io[6],fftw_p[6],fftw_ip[6],param,2);


			#pragma omp parallel for
			for (i = fftSizeOut / 2;i < fftSizeOut + fftSizeOut / 2;i++) {
				mem8[i] += mem2[i] + mem3[i] + mem4[i];
			}
			#pragma omp parallel for
			for (i = fftSizeOut + fftSizeOut / 2;i < fftSizeOut * 2 + fftSizeOut / 2;i++) {
				mem8[i] += mem5[i] + mem6[i] + mem7[i];
			}
		}

		if (startInSample + fftSizeIn / 2 >= 0) {
			if (outRemain >= fftSizeOut * 2) {
PRINT_LOG("6");
				wr = fio_write(mem8 + (fftSizeOut / 2),sizeof (SSIZE),fftSizeOut * 2,fp_w);
				if (wr != fftSizeOut * 2) {
					char s[100];
					sprintf(s,"%ld:fio_write(%ld,%ld)",wr,sizeof (SSIZE),fftSizeOut);
					param->err = STATUS_FILE_WRITE_ERR;
					PRINT_LOG(s);
					return;
				}
			} else {
				wr = fio_write(mem8 + (fftSizeOut / 2),sizeof (SSIZE),outRemain,fp_w);
				if (wr != outRemain) {
					param->err = STATUS_FILE_WRITE_ERR;
					PRINT_LOG("ERROR");
					return;
				}
			}
			if (outRemain >= fftSizeOut * 2) {
				outRemain -= fftSizeOut * 2;
			} else {
				break;
			}
		}
PRINT_LOG("7");
	}
	PRINT_LOG("end");

	al_free(mem1);
	al_free(mem2);
	al_free(mem3);
	al_free(mem4);
	al_free(mem5);
	al_free(mem6);
	al_free(mem7);
	al_free(mem8);
	
	// 1
	fftw_destroy_plan(fftw_p[0]);
	fftw_destroy_plan(fftw_ip[0]);
	fftw_destroy_plan(fftw_p[1]);
	fftw_destroy_plan(fftw_ip[1]);
	fftw_destroy_plan(fftw_p[2]);
	fftw_destroy_plan(fftw_ip[2]);
	fftw_destroy_plan(fftw_p[3]);
	fftw_destroy_plan(fftw_ip[3]);
	fftw_destroy_plan(fftw_p[4]);
	fftw_destroy_plan(fftw_ip[4]);
	fftw_destroy_plan(fftw_p[5]);
	fftw_destroy_plan(fftw_ip[5]);
	fftw_destroy_plan(fftw_p[6]);
	fftw_destroy_plan(fftw_ip[6]);
	fftw_free(fftw_io[0]);
	fftw_free(fftw_io[1]);
	fftw_free(fftw_io[2]);
	fftw_free(fftw_io[3]);
	fftw_free(fftw_io[4]);
	fftw_free(fftw_io[5]);
	fftw_free(fftw_io[6]);
	if (lr == 0) {
		param->l_min = min;
		param->l_max = max;
		param->l_maxLv = maxLv;
		param->l_tmpLv = maxLv2;
		param->l_cntLv = ns;
	} else {
		param->r_min = min;
		param->r_max = max;
		param->r_maxLv = maxLv;
		param->r_tmpLv = maxLv2;
		param->r_cntLv = ns;
	}
}
//---------------------------------------------------------------------------
// Function   : fftFilterSub
// Description: FFT によるフィルタ処理(サブ関数)
// ---
//	param		:変換パラメータ
//
void fftFilterSub(SSIZE *pIn,SSIZE *pOut,fftw_complex *fftw_in,fftw_complex *fftw_out,fftw_plan fftw_p,fftw_plan fftw_ip,PARAM_INFO *param,int id)
{
	long inSampleR,outSampleR;
	long wkSampleR;
	long fftSizeIn,fftSizeOut,i,j,n;
	long cutOff;
	long hfc,lfc;
	double nx;
	double p;
	long validIndex;
	long index1k,index15k,index19k,width1k,width_l,width_h,index_l,index_h,index_b;
	long h;
	double pw;
	double rate;
	double wid;
PRINT_LOG("Start");
	inSampleR = param->inSampleR;
	outSampleR = param->outSampleR;
	if (param->src_flag == 2) {
		inSampleR = param->outSampleR;
	}

	fftSizeIn = inSampleR / 2;
	fftSizeOut = outSampleR / 2;
	if (param->inSampleR == 2822400*2) {
		fftSizeIn = inSampleR / 4;
		fftSizeOut = outSampleR / 4;
	}

	for (i = 0;i < fftSizeOut;i++) {
		fftw_out[i][0] = 0;
		fftw_out[i][1] = 0;
	}

	// FFT 初期設定
	copyToFFTW(fftw_in,pIn,fftSizeIn);

	windowFFTW(fftw_in,fftSizeIn);
	
	// FFT
	fftw_execute(fftw_p);

	// 高域削除
	if (inSampleR <= outSampleR) {
		wkSampleR = inSampleR;
	} else {
		wkSampleR = outSampleR;
	}
	hfc = wkSampleR / 2;
	
	if (param->hfc != -1) {
		if (hfc > param->hfc) {
			hfc = param->hfc;
		}
	}
	cutOff = ((double)fftSizeOut / outSampleR) * hfc;
	cutFFTW(fftw_out,cutOff,fftSizeOut);

	if (param->lfc != -1) {
		cutOff = ((double)fftSizeOut / outSampleR) * param->lfc;
		for (i = 1;i < cutOff;i++) {
			fftw_out[i][0] = 0;
			fftw_out[i][1] = 0;
		}
	}

	// 半分のデータを復元
	for (i = 1;i < fftSizeOut / 2;i++) {
		fftw_out[fftSizeOut - i][0] = fftw_out[i][0];
		fftw_out[fftSizeOut - i][1] = fftw_out[i][1] * -1;
	}

	fftw_out[0][0] = 0;
	fftw_out[0][1] = 0;
	fftw_out[1][0] = 0;
	fftw_out[1][1] = 0;

	// invert FFT
	fftw_execute(fftw_ip);

	// 出力
	for (i = 0;i < fftSizeOut;i++) {
		pOut[i] = (SSIZE)(fftw_in[i][0] / fftSizeOut);
	}
PRINT_LOG("End");
}
#if 0
//---------------------------------------------------------------------------
// Function   : ana_lpf
// Description: ローパスフィルター関数
// ---
//	inByte		:入力バイト数
//	inSample	:入力サンプル数(チャンネルごと)
//	fp_r		:入力FIO
//	fp_w1		:出力FIO(Left)
//	fp_w2		:出力FIO(Right)
//	param		:変換パラメータ
//
void ana_lpf(UI64 inByte,UI64 inSample,FIO *fp_r,FIO *fp_w1,FIO *fp_w2,PARAM_INFO *param)
{
	unsigned char *bit_buffer;
	SSIZE *l_data_buffer,*r_data_buffer;
	SSIZE l_delay[10],r_delay[10];
	SSIZE total;
	unsigned char mask;
	UI64 l_remain,r_remain;
	SSIZE ana_lv;
	SSIZE ana_data;
	SSIZE l_min,l_max;
	SSIZE r_min,r_max;
	SSIZE l_ana_lv,r_ana_lv;
	int i,j,n;
	int ptr_in_byte,ptr_out_byte;
	int l_ptr_delay,r_ptr_delay;
	fio_size rs,ws;
	int lr_flag;
	int updown_flag;
	char dbs[100];
	bit_buffer	  = (unsigned char *)malloc(4096);
	l_data_buffer = (SSIZE *)malloc(4096 * 8 * sizeof (SSIZE));
	r_data_buffer = (SSIZE *)malloc(4096 * 8 * sizeof (SSIZE));
	if (bit_buffer == NULL || l_data_buffer == NULL || r_data_buffer == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	l_ana_lv = 0;
	r_ana_lv = 0;
	l_ptr_delay = 5;
	r_ptr_delay = 5;
	for (i = 0;i < 5;i++) {
		l_delay[i] = 0;
		r_delay[i] = 0;
	}
	l_min = r_min = 0;
	l_max = r_max = 0;

	lr_flag = 0;
	l_remain = inSample;
	r_remain = inSample;

	total = 0;
	while (inByte > 0) {
		if (inByte >= 4096) {
			rs = fio_read(bit_buffer,1,4096,fp_r);
			if (rs != 4096 || fp_r->error) {
				sprintf(dbs,"total=%lld",total);
				PRINT_LOG(dbs);
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			inByte -= 4096;
		} else {
			rs = fio_read(bit_buffer,1,inByte,fp_r);
			if (rs != inByte || fp_r->error) {
				sprintf(dbs,"total=%lld",total);
				PRINT_LOG(dbs);
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			inByte = 0;
		}
		total += rs;
//		mask = 0x80;
		mask = 0x01;
		ptr_in_byte = 0;
		ptr_out_byte = 0;
		if (lr_flag == 0) {
			do {
				if ((bit_buffer[ptr_in_byte] & mask)) {
					ana_lv = (SSIZE)1 << 4;
				} else {
					ana_lv = (SSIZE)-1 << 4;
				}
				l_ana_lv += ana_lv;
				l_delay[l_ptr_delay] = l_ana_lv;
				l_delay[l_ptr_delay - 5] = l_ana_lv;
				ana_data = 0;
				ana_data += l_delay[l_ptr_delay - 0];
				ana_data += l_delay[l_ptr_delay - 1];
				ana_data += l_delay[l_ptr_delay - 2];
				ana_data += l_delay[l_ptr_delay - 3];
				ana_data += l_delay[l_ptr_delay - 4];
				ana_data /= 5;
				
				l_ptr_delay++;
				if (l_ptr_delay == 10) {
					l_ptr_delay = 5;
				}

				if (l_remain > 0) {
//					l_data_buffer[ptr_out_byte] = ana_data;
					l_data_buffer[ptr_out_byte] = l_ana_lv;
					if (l_min > l_data_buffer[ptr_out_byte]) {
						l_min = l_data_buffer[ptr_out_byte];
					}
					if (l_max < l_data_buffer[ptr_out_byte]) {
						l_max = l_data_buffer[ptr_out_byte];
					}

					ptr_out_byte ++;
					l_remain--;
				}
//				mask >>= 1;
				mask <<= 1;
				if (mask == 0) {
//					mask = 0x80;
					mask = 0x01;
					ptr_in_byte++;
				}
			} while (ptr_in_byte < rs);
		} else {
			do {
				if ((bit_buffer[ptr_in_byte] & mask)) {
					ana_lv = (SSIZE)1 << 4;
				} else {
					ana_lv = (SSIZE)-1 << 4;
				}
				r_ana_lv += ana_lv;
				r_delay[r_ptr_delay] = r_ana_lv;
				r_delay[r_ptr_delay - 5] = r_ana_lv;
				ana_data = 0;
				ana_data += r_delay[r_ptr_delay - 0];
				ana_data += r_delay[r_ptr_delay - 1];
				ana_data += r_delay[r_ptr_delay - 2];
				ana_data += r_delay[r_ptr_delay - 3];
				ana_data += r_delay[r_ptr_delay - 4];
				ana_data /= 5;
				
				r_ptr_delay++;
				if (r_ptr_delay == 10) {
					r_ptr_delay = 5;
				}

				if (r_remain > 0) {
//					r_data_buffer[ptr_out_byte] = ana_data;
					r_data_buffer[ptr_out_byte] = r_ana_lv;
					if (r_min > r_data_buffer[ptr_out_byte]) {
						r_min = r_data_buffer[ptr_out_byte];
					}
					if (r_max < r_data_buffer[ptr_out_byte]) {
						r_max = r_data_buffer[ptr_out_byte];
					}
					ptr_out_byte ++;
					r_remain--;
				}
//				mask >>= 1;
				mask <<= 1;
				if (mask == 0) {
//					mask = 0x80;
					mask = 0x01;
					ptr_in_byte++;
				}
			} while (ptr_in_byte < rs);
		}
		if (param->channel == 1) {
			lr_flag = 1;
		}
		if (lr_flag == 1) {
			ws = fio_write(l_data_buffer,sizeof (SSIZE),ptr_out_byte,fp_w1);
			if (ws != ptr_out_byte || fp_w1->error) {
				PRINT_LOG("");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
			if (param->channel == 2) {
				ws = fio_write(r_data_buffer,sizeof (SSIZE),ptr_out_byte,fp_w2);
				if (ws != ptr_out_byte || fp_w2->error) {
					PRINT_LOG("");
					param->err = STATUS_FILE_WRITE_ERR;
					break;
				}
			}
		}
		lr_flag++;
		lr_flag &= 0x01;
	}
	param->l_min = l_min;
	param->l_max = l_max;
	param->r_min = r_min;
	param->r_max = r_max;
}
//---------------------------------------------------------------------------
// Function   : fir_lpf
// Description: ローパスフィルター関数
// ---
//	inByte		:入力バイト数
//	inSample	:入力サンプル数
//	fp_r		:入力FIO
//	fp_w1		:出力FIO(Left)
//	fp_w2		:出力FIO(Right)
//	param		:変換パラメータ
//
void fir_lpf(UI64 inByte,UI64 inSample,FIO *fp_r,FIO *fp_w1,FIO *fp_w2,PARAM_INFO *param)
{
	static unsigned char bit_buffer[4096];
	SSIZE *l_data_buffer,*r_data_buffer;
	double *l_fir_data,*r_fir_data;
	unsigned char mask;
	UI64 l_remain,r_remain;
	double ana_lv;
	double fir_sum;
	int i,j,n;
	int ptr_in_byte,ptr_out_byte;
	int l_ptr_fir,r_ptr_fir;
	fio_size rs,ws;
	int lr_flag;
	int updown_flag;

	l_data_buffer = (SSIZE *)malloc(4096 * 8 * sizeof (SSIZE));
	r_data_buffer = (SSIZE *)malloc(4096 * 8 * sizeof (SSIZE));
	l_fir_data = (double *)malloc(FIR_N * 2 * sizeof (SSIZE));
	r_fir_data = (double *)malloc(FIR_N * 2 * sizeof (SSIZE));
	if (l_data_buffer == NULL || r_data_buffer == NULL || l_fir_data == NULL || r_fir_data == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	param->dc_flag = 0;
	lr_flag = 0;
	l_remain = r_remain = inSample;
	l_ptr_fir = r_ptr_fir = FIR_N;
	memset(l_fir_data,0,FIR_N * 2 * sizeof (SSIZE));
	memset(r_fir_data,0,FIR_N * 2 * sizeof (SSIZE));

	while (inByte > 0) {
		if (inByte >= 4096) {
			rs = fio_read(bit_buffer,1,4096,fp_r);
			if (rs != 4096 || fp_r->error) {
				PRINT_LOG("");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			inByte -= rs;
		} else {
			rs = fio_read(bit_buffer,1,inByte,fp_r);
			if (rs != inByte || fp_r->error) {
				PRINT_LOG("");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			inByte = 0;
		}
//		mask = 0x80;
		mask = 0x01;
		ptr_in_byte = 0;
		ptr_out_byte = 0;

		if (lr_flag == 0) {
			do {
				if ((bit_buffer[ptr_in_byte] & mask)) {
					updown_flag = 1;
				} else {
					updown_flag = 0;
				}
				if (updown_flag == 1) {
					ana_lv = 1.0;
				} else {
					ana_lv = -1.0;
				}
				l_fir_data[l_ptr_fir] = ana_lv;
				l_fir_data[l_ptr_fir - FIR_N] = ana_lv;
				fir_sum = 0;
				for (i = l_ptr_fir,n = 0;n < FIR_N;i--,n++) {
					fir_sum += fir_lpf_n[n] * l_fir_data[l_ptr_fir - n];
				}
				l_ptr_fir++;
				if (l_ptr_fir >= FIR_N * 2) {
					l_ptr_fir = FIR_N;
				}
				if (l_remain > 0) {
					l_data_buffer[ptr_out_byte] = (SSIZE)((double)((SSIZE)1 << 43) * fir_sum);
					ptr_out_byte++;
					l_remain--;
				}
//				mask >>= 1;
				mask <<= 1;
				if (mask == 0) {
//					mask = 0x80;
					mask = 0x01;
					ptr_in_byte++;
				}
			} while (ptr_in_byte < rs);
		} else {
			do {
				if ((bit_buffer[ptr_in_byte] & mask)) {
					updown_flag = 1;
				} else {
					updown_flag = 0;
				}
				if (updown_flag == 1) {
					ana_lv = 1.0;
				} else {
					ana_lv = -1.0;
				}
				r_fir_data[r_ptr_fir] = ana_lv;
				r_fir_data[r_ptr_fir - FIR_N] = ana_lv;
				fir_sum = 0;
				for (i = r_ptr_fir,n = 0;n < FIR_N;i--,n++) {
					fir_sum += fir_lpf_n[n] * r_fir_data[r_ptr_fir - n];
				}
				r_ptr_fir++;
				if (r_ptr_fir >= FIR_N * 2) {
					r_ptr_fir = FIR_N;
				}
				if (r_remain > 0) {
					r_data_buffer[ptr_out_byte] = (SSIZE)((double)((SSIZE)1 << 43) * fir_sum);
					ptr_out_byte++;
					r_remain--;
				}
//				mask >>= 1;
				mask <<= 1;
				if (mask == 0) {
//					mask = 0x80;
					mask = 0x01;
					ptr_in_byte++;
				}
			} while (ptr_in_byte < rs);
		}
		if (param->channel == 1) {
			lr_flag = 1;
		}
		if (lr_flag == 1) {
			ws = fio_write(l_data_buffer,sizeof (SSIZE),ptr_out_byte,fp_w1);
			if (ws != ptr_out_byte || fp_w1->error) {
				PRINT_LOG("");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
			if (param->channel == 2) {
				ws = fio_write(r_data_buffer,sizeof (SSIZE),ptr_out_byte,fp_w2);
				if (ws != ptr_out_byte || fp_w2->error) {
					PRINT_LOG("");
					param->err = STATUS_FILE_WRITE_ERR;
					break;
				}
			}
		}
		lr_flag++;
		lr_flag &= 0x01;
	}
}
//---------------------------------------------------------------------------
// Function   : nbit_lpf
// Description: ローパスフィルター関数
// ---
//	inSample	:入力サンプル数
//	fp_r		:入力FIO
//	fp_w1		:出力FIO(Left)
//	fp_w2		:出力FIO(Right)
//	param		:変換パラメータ
//
void nbit_lpf(UI64 inByte,UI64 inSample,FIO *fp_r,FIO *fp_w1,FIO *fp_w2,PARAM_INFO *param)
{
	unsigned char *bit_buffer;
	int *l_nbit_buffer,*r_nbit_buffer;
	SSIZE *l_data_buffer,*r_data_buffer;
	unsigned char mask;
	UI64 l_remain,r_remain;
	double nbit_r;
	SSIZE nbit_data;
	int i,j,n;
	int ptr_in_byte,ptr_out_byte;
	int l_ptr_nbit,r_ptr_nbit;
	fio_size rs,ws;
	int lr_flag;
	int updown_flag;

	bit_buffer	  = (unsigned char *)malloc(4096);
	l_data_buffer = (SSIZE *)malloc(4096 * 8 * sizeof (SSIZE));
	r_data_buffer = (SSIZE *)malloc(4096 * 8 * sizeof (SSIZE));
	l_nbit_buffer = (int *)malloc(8192 * sizeof (int));
	r_nbit_buffer = (int *)malloc(8192 * sizeof (int));
	if (bit_buffer == NULL || l_data_buffer == NULL || r_data_buffer == NULL || l_nbit_buffer == NULL || r_nbit_buffer == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	param->dc_flag = 0;
	l_ptr_nbit = r_ptr_nbit = 32;
	lr_flag = 0;
	l_remain = r_remain = inSample;
	
	for (i = 0;i < 24;i++) {
		if ((i & 1) == 0) {
			l_nbit_buffer[i] = 0;
			r_nbit_buffer[i] = 0;
		} else {
			l_nbit_buffer[i] = 1;
			r_nbit_buffer[i] = 1;
		}
	}

	while (inByte > 0) {
		if (inByte >= 4096) {
			rs = fio_read(bit_buffer,1,4096,fp_r);
			if (rs != 4096 || fp_r->error) {
				PRINT_LOG("");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			inByte -= rs;
		} else {
			rs = fio_read(bit_buffer,1,inByte,fp_r);
			if (rs != inByte || fp_r->error) {
				PRINT_LOG("");
				param->err = STATUS_FILE_READ_ERR;
				break;
			}
			inByte = 0;
		}
//		mask = 0x80;
		mask = 0x01;
		ptr_in_byte = 0;
		ptr_out_byte = 0;
		if (lr_flag == 0) {
			do {
				if ((bit_buffer[ptr_in_byte] & mask)) {
					l_nbit_buffer[l_ptr_nbit] = 1;
					l_nbit_buffer[l_ptr_nbit - 24] = 1;
				} else {
					l_nbit_buffer[l_ptr_nbit] = 0;
					l_nbit_buffer[l_ptr_nbit - 24] = 0;
				}
				for (i = 0,j = 0;i < 24;i++) {
					if (l_nbit_buffer[l_ptr_nbit - i] == 1) {
						j++;
					}
				}
				nbit_r = (double)1.0 * ((double)j / 24.0);

				nbit_data = (SSIZE)1 << 60;
				nbit_data = (SSIZE)((double)nbit_data * nbit_r);
				nbit_data -= (SSIZE)1 << (60 - 1);
				nbit_data >>= (60 - 46);

				if (l_remain > 0) {
					l_data_buffer[ptr_out_byte] = nbit_data;
					ptr_out_byte ++;
					l_remain--;
				}
				l_ptr_nbit++;
				if (l_ptr_nbit >= 24*2) {
					l_ptr_nbit = 24;
				}
				
//				mask >>= 1;
				mask <<= 1;
				if (mask == 0) {
//					mask = 0x80;
					mask = 0x01;
					ptr_in_byte++;
				}
			} while (ptr_in_byte < rs);
		} else {
			do {
				if ((bit_buffer[ptr_in_byte] & mask)) {
					r_nbit_buffer[r_ptr_nbit] = 1;
					r_nbit_buffer[r_ptr_nbit - 24] = 1;
				} else {
					r_nbit_buffer[r_ptr_nbit] = 0;
					r_nbit_buffer[r_ptr_nbit - 24] = 0;
				}
				for (i = 0,j = 0;i < 24;i++) {
					if (r_nbit_buffer[r_ptr_nbit - i] == 1) {
						j++;
					}
				}
				nbit_r = (double)1.0 * ((double)j / 24.0);

				nbit_data = (SSIZE)1 << 60;
				nbit_data = (SSIZE)((double)nbit_data * nbit_r);
				nbit_data -= (SSIZE)1 << (60 - 1);
				nbit_data >>= (60 - 46);

				if (r_remain > 0) {
					r_data_buffer[ptr_out_byte] = nbit_data;
					ptr_out_byte ++;
					r_remain--;
				}
				r_ptr_nbit++;
				if (r_ptr_nbit >= 24*2) {
					r_ptr_nbit = 24;
				}

//				mask >>= 1;
				mask <<= 1;
				if (mask == 0) {
//					mask = 0x80;
					mask = 0x01;
					ptr_in_byte++;
				}
			} while (ptr_in_byte < rs);
		}
		if (param->channel == 1) {
			lr_flag = 1;
		}
		if (lr_flag == 1) {
			ws = fio_write(l_data_buffer,sizeof (SSIZE),ptr_out_byte,fp_w1);
			if (ws != ptr_out_byte || fp_w1->error) {
				PRINT_LOG("");
				param->err = STATUS_FILE_WRITE_ERR;
				break;
			}
			if (param->channel == 2) {
				ws = fio_write(r_data_buffer,sizeof (SSIZE),ptr_out_byte,fp_w2);
				if (ws != ptr_out_byte || fp_w2->error) {
					PRINT_LOG("");
					param->err = STATUS_FILE_WRITE_ERR;
					break;
				}
			}
		}
		lr_flag++;
		lr_flag &= 0x01;
	}
}
#endif
//---------------------------------------------------------------------------
// Function   : onebit2nbit
// Description: 1bit データを 64bit データへ変換して読み込む
// ---
//	offset		:入力オフセット(サンプル毎)
//	n			:サンプル数
//	buffer		:データバッファ
//	fp_r		:入力FIO
//	param		:変換パラメータ
//
void onebit2nbit(SSIZE offset,SSIZE sample,SSIZE *buffer,FIO *fp_r,PARAM_INFO *param)
{
	unsigned char *bit_buffer;
	int *delay_buffer;
	unsigned char mask;
	double nbit_r;
	SSIZE nbit_data;
	SSIZE seek_ptr;
	int i,j,n,bit_count_p,bit_count_m;
	int ptr_in_byte,ptr_out_byte;
	int ptr_delay;
	int delay;
	int n_delay;
	int zero_count;
	fio_size rs;
	char sss[50];
	
	PRINT_LOG("Start");

	memset(buffer,0,sample * sizeof (SSIZE));
	if (offset < 0) {
		if ((offset * -1) >= sample) {
			// ファイルから読み込むデータがないのでリターンする
			PRINT_LOG("No Data");
			return;
		}
		buffer += (offset * -1);
		sample -= (offset * -1);
		offset	= 0;
	}

	if (offset + sample > param->n_sample) {
		sample = param->n_sample - offset;
		sprintf(sss,"offset:%lld,n:%lld",offset,sample);
		PRINT_LOG(sss);
	}

	if (sample <= 0) {
		return;
	}

	delay = 24;

	bit_buffer	 = (unsigned char *)malloc(4096);
	delay_buffer = (int *)malloc(delay * 2 * sizeof (int));
	if (bit_buffer == NULL || delay_buffer == NULL) {
		param->err = STATUS_MEM_ALLOC_ERR;
		return;
	}

	ptr_delay = delay;
	
	// delay_buffer には3値を使う(1,-1,0)
	for (i = 0;i < delay * 2;i++) {
		delay_buffer[i] = 0;	// 無音
	}

	seek_ptr = offset - delay;
	n_delay = delay;
	if ((seek_ptr + delay) >= 0) {
		// delay バッファに以前の値をセットする
		if (seek_ptr < 0) {
			n_delay += seek_ptr;
			seek_ptr = 0;
		}

		ptr_out_byte = 0;
		while (n_delay > 0) {
			fio_seek(fp_r,(seek_ptr / 8) + param->data_offset,SEEK_SET);
			if (fp_r->error) {
				param->err = STATUS_FILE_READ_ERR;
				return;
			}
			rs = fio_read(bit_buffer,1,(n_delay + 7) / 8,fp_r);
			if (fp_r->error) {
				PRINT_LOG("");
				param->err = STATUS_FILE_READ_ERR;
				return;
			}
			if (rs == 0) {
				break;
			}
			ptr_in_byte = 0;
			mask = 0x01 << (seek_ptr % 8);
			if (bit_buffer[ptr_in_byte] == 0x69 && bit_buffer[ptr_in_byte + 1] == 0x69 && bit_buffer[ptr_in_byte + 2] == 0x69) {
				for (i = 0;i < 8 * 3;i++) {
					delay_buffer[i] = 0;
				}
				n_delay = 0;
			} else {
				do {
					if ((bit_buffer[ptr_in_byte] & mask)) {
						delay_buffer[ptr_out_byte] = 1;
					} else {
						delay_buffer[ptr_out_byte] = -1;
					}
					mask <<= 1;
					if (mask == 0x00) {
						mask = 0x01;
						ptr_in_byte++;
						rs--;
					}
					ptr_out_byte++;
					seek_ptr++;
					n_delay--;
				} while (rs && n_delay > 0);
			}
		}
	}

	bit_count_p = bit_count_m = -1;
	ptr_out_byte = 0;
	zero_count = 0;

	while (sample > 0) {
		seek_ptr = offset;
		seek_ptr /= 8;			// bit
		fio_seek(fp_r,seek_ptr,SEEK_SET);
		if (fp_r->error) {
			param->err = STATUS_FILE_READ_ERR;
			break;
		}
		rs = fio_read(bit_buffer,1,4096,fp_r);
		if (fp_r->error) {
			PRINT_LOG("");
			param->err = STATUS_FILE_READ_ERR;
			break;
		}

		seek_ptr = offset;
		seek_ptr /= 8;			// bit
		ptr_in_byte = 0;
		mask = 0x01 << (offset % 8);

		do {
			if (bit_count_p != -1 && bit_count_m != -1) {
				if (delay_buffer[ptr_delay - delay] == 1) {
					bit_count_p--;
				} else if (delay_buffer[ptr_delay - delay] == -1) {
					bit_count_m--;
				}
			}
			if (zero_count == 0 && ptr_in_byte + 2 < rs && bit_buffer[ptr_in_byte + 0] == 0x69 && bit_buffer[ptr_in_byte + 1] == 0x69 && bit_buffer[ptr_in_byte + 2] == 0x69) {
				zero_count = 24;
				zero_count -= (offset % 8);
			} else if (zero_count == 0 && ptr_in_byte + 2 == rs && bit_buffer[ptr_in_byte + 0] == 0x69 && bit_buffer[ptr_in_byte + 1] == 0x69) {
				zero_count = 16;
				zero_count -= (offset % 8);
			} else if (zero_count == 0 && sample > 0 && ptr_in_byte + 1 == rs && bit_buffer[ptr_in_byte + 0] == 0x69) {
				zero_count = 8;
				zero_count -= (offset % 8);
			}

			if (zero_count == 0) {
				if ((bit_buffer[ptr_in_byte] & mask)) {
					delay_buffer[ptr_delay] = 1;
					delay_buffer[ptr_delay - delay] = 1;
					if (bit_count_p != -1) {
						bit_count_p++;
					}
				} else {
					delay_buffer[ptr_delay] = -1;
					delay_buffer[ptr_delay - delay] = -1;
					if (bit_count_m != -1) {
						bit_count_m++;
					}
				}
			} else {
				delay_buffer[ptr_delay] = 0;
				delay_buffer[ptr_delay - delay] = 0;
			}
			if (bit_count_p == -1 && bit_count_m == -1) {
				for (i = 0,bit_count_p = 0,bit_count_m = 0;i < delay;i++) {
					if (delay_buffer[ptr_delay - i] == 1) {
						bit_count_p++;
					} else if (delay_buffer[ptr_delay - i] == -1) {
						bit_count_m++;
					}
				}
			}
			if ((bit_count_p + bit_count_m) > 0) {
				nbit_r = (double)1.0 * ((double)bit_count_p / (bit_count_p + bit_count_m));
				if ((bit_count_p + bit_count_m) < delay) {
					nbit_r = (nbit_r / delay) * (bit_count_p + bit_count_m) + ((double)0.5 / delay) * (delay - (bit_count_p + bit_count_m));
				}
				if (param->inSampleR == 2822400) {
					nbit_data = (SSIZE)1 << 53;
					nbit_data = (SSIZE)((double)nbit_data * nbit_r);
					nbit_data -= (SSIZE)1 << (53 - 1);
				} else {
					nbit_data = (SSIZE)1 << 51;
					nbit_data = (SSIZE)((double)nbit_data * nbit_r);
					nbit_data -= (SSIZE)1 << (51 - 1);
				}
			} else {
				if (param->inSampleR == 2822400) {
					nbit_data = (SSIZE)1 << 53;
					nbit_data = (SSIZE)((double)nbit_data * 0.5);
					nbit_data -= (SSIZE)1 << (53 - 1);
				} else {
					nbit_data = (SSIZE)1 << 51;
					nbit_data = (SSIZE)((double)nbit_data * 0.5);
					nbit_data -= (SSIZE)1 << (51 - 1);
				}
			}
			if (zero_count > 0) {
				zero_count--;
			}
			if (sample > 0) {
				buffer[ptr_out_byte] = nbit_data;
				ptr_out_byte ++;
				offset++;
				sample--;
			}
			ptr_delay++;
			if (ptr_delay >= delay*2) {
				ptr_delay = delay;
			}
			mask <<= 1;
			if (mask == 0) {
				mask = 0x01;
				ptr_in_byte++;
			}
		} while (ptr_in_byte < rs);
	}
	free(bit_buffer);
	free(delay_buffer);
	PRINT_LOG("End");
}
//---------------------------------------------------------------------------
// Function   : ana_abe
// Description: 高域ノイズ軽減処理
// ---
//	start		:開始位置
//	nSample		:サンプル数
//	i_buffer	:入力バッファ
//	o_buffer	:出力バッファ
//	param		:パラメーター構造体
//	
//
void ana_abe(SSIZE start,UI64 nSample,SSIZE *i_buffer,SSIZE *o_buffer,PARAM_INFO *param)
{
	int i,j,n;
	
	SSIZE min,max,dd;
PRINT_LOG("Start");
	if (start < 3) {
		for (i = start + 3,j = 0;j < nSample;i++,j++) {
			for (n = 0,min = i_buffer[i],max = i_buffer[i];n < 3;n++) {
				if (min > i_buffer[i - n]) {
					min = i_buffer[i - n];
				} else if (max < i_buffer[i - n]) {
					max = i_buffer[i - n];
				}
			}
			o_buffer[i] = (min + max) / 2;
		}
	} else {
		for (i = start,j = 0;j < nSample;i++,j++) {
			for (n = 0,min = i_buffer[i],max = i_buffer[i];n < 3;n++) {
				if (min > i_buffer[i - n]) {
					min = i_buffer[i - n];
				} else if (max < i_buffer[i - n]) {
					max = i_buffer[i - n];
				}
			}
			o_buffer[i] = (min + max) / 2;
		}
	}
	if (param->inSampleR == 2822400) {
		if (start < 10) {
			for (i = start + 10,j = 0;j < nSample;i++,j++) {
				for (n = 0,min = o_buffer[i],max = o_buffer[i];n < 10;n++) {
					if (min > o_buffer[i - n]) {
						min = o_buffer[i - n];
					} else if (max < o_buffer[i - n]) {
						max = o_buffer[i - n];
					}
				}
				o_buffer[i] = (min + max) / 2;
			}
		} else {
			for (i = start,j = 0;j < nSample;i++,j++) {
				for (n = 0,min = o_buffer[i],max = o_buffer[i];n < 10;n++) {
					if (min > o_buffer[i - n]) {
						min = o_buffer[i - n];
					} else if (max < o_buffer[i - n]) {
						max = o_buffer[i - n];
					}
				}
				o_buffer[i] = (min + max) / 2;
			}
		}

		for (i = start + 1,j = 0;j + 1 < nSample;i++,j++) {
			o_buffer[i] = (o_buffer[i - 1] + o_buffer[i + 1]) / 2;
		}
		for (i = start + 1,j = 0;j + 1 < nSample;i++,j++) {
			o_buffer[i] = (o_buffer[i - 1] + o_buffer[i + 1]) / 2;
		}
	}
#if 0
	for (i = start + 1,j = 0;j + 1 < nSample;i++,j++) {
		o_buffer[i] = (o_buffer[i - 1] + o_buffer[i + 1]) / 2;
	}
	for (i = start + 1,j = 0;j + 1 < nSample;i++,j++) {
		o_buffer[i] = (o_buffer[i - 1] + o_buffer[i + 1]) / 2;
	}
	for (i = start + 1,j = 0;j + 1 < nSample;i++,j++) {
		o_buffer[i] = (o_buffer[i - 1] + o_buffer[i + 1]) / 2;
	}
	for (i = start + 1,j = 0;j + 1 < nSample;i++,j++) {
		o_buffer[i] = (o_buffer[i - 1] + o_buffer[i + 1]) / 2;
	}
#endif
	PRINT_LOG("End");
}
//---------------------------------------------------------------------------
// Function   : copyToFFTW
// Description: fftw用配列に値をコピーする
// ---
//	
//
inline void copyToFFTW(fftw_complex *fftw,SSIZE *buf,long size)
{
	long i;
	
	for (i = 0;i + 64 < size;i+=64) {
		fftw[i + 0][0] = buf[0];
		fftw[i + 0][1] = 0;
		fftw[i + 1][0] = buf[1];
		fftw[i + 1][1] = 0;
		fftw[i + 2][0] = buf[2];
		fftw[i + 2][1] = 0;
		fftw[i + 3][0] = buf[3];
		fftw[i + 3][1] = 0;
		fftw[i + 4][0] = buf[4];
		fftw[i + 4][1] = 0;
		fftw[i + 5][0] = buf[5];
		fftw[i + 5][1] = 0;
		fftw[i + 6][0] = buf[6];
		fftw[i + 6][1] = 0;
		fftw[i + 7][0] = buf[7];
		fftw[i + 7][1] = 0;
		fftw[i + 8][0] = buf[8];
		fftw[i + 8][1] = 0;
		fftw[i + 9][0] = buf[9];
		fftw[i + 9][1] = 0;
		fftw[i + 10][0] = buf[10];
		fftw[i + 10][1] = 0;
		fftw[i + 11][0] = buf[11];
		fftw[i + 11][1] = 0;
		fftw[i + 12][0] = buf[12];
		fftw[i + 12][1] = 0;
		fftw[i + 13][0] = buf[13];
		fftw[i + 13][1] = 0;
		fftw[i + 14][0] = buf[14];
		fftw[i + 14][1] = 0;
		fftw[i + 15][0] = buf[15];
		fftw[i + 15][1] = 0;
		fftw[i + 16][0] = buf[16];
		fftw[i + 16][1] = 0;
		fftw[i + 17][0] = buf[17];
		fftw[i + 17][1] = 0;
		fftw[i + 18][0] = buf[18];
		fftw[i + 18][1] = 0;
		fftw[i + 19][0] = buf[19];
		fftw[i + 19][1] = 0;
		fftw[i + 20][0] = buf[20];
		fftw[i + 20][1] = 0;
		fftw[i + 21][0] = buf[21];
		fftw[i + 21][1] = 0;
		fftw[i + 22][0] = buf[22];
		fftw[i + 22][1] = 0;
		fftw[i + 23][0] = buf[23];
		fftw[i + 23][1] = 0;
		fftw[i + 24][0] = buf[24];
		fftw[i + 24][1] = 0;
		fftw[i + 25][0] = buf[25];
		fftw[i + 25][1] = 0;
		fftw[i + 26][0] = buf[26];
		fftw[i + 26][1] = 0;
		fftw[i + 27][0] = buf[27];
		fftw[i + 27][1] = 0;
		fftw[i + 28][0] = buf[28];
		fftw[i + 28][1] = 0;
		fftw[i + 29][0] = buf[29];
		fftw[i + 29][1] = 0;
		fftw[i + 30][0] = buf[30];
		fftw[i + 30][1] = 0;
		fftw[i + 31][0] = buf[31];
		fftw[i + 31][1] = 0;
		fftw[i + 32][0] = buf[32];
		fftw[i + 32][1] = 0;
		fftw[i + 33][0] = buf[33];
		fftw[i + 33][1] = 0;
		fftw[i + 34][0] = buf[34];
		fftw[i + 34][1] = 0;
		fftw[i + 35][0] = buf[35];
		fftw[i + 35][1] = 0;
		fftw[i + 36][0] = buf[36];
		fftw[i + 36][1] = 0;
		fftw[i + 37][0] = buf[37];
		fftw[i + 37][1] = 0;
		fftw[i + 38][0] = buf[38];
		fftw[i + 38][1] = 0;
		fftw[i + 39][0] = buf[39];
		fftw[i + 39][1] = 0;
		fftw[i + 40][0] = buf[40];
		fftw[i + 40][1] = 0;
		fftw[i + 41][0] = buf[41];
		fftw[i + 41][1] = 0;
		fftw[i + 42][0] = buf[42];
		fftw[i + 42][1] = 0;
		fftw[i + 43][0] = buf[43];
		fftw[i + 43][1] = 0;
		fftw[i + 44][0] = buf[44];
		fftw[i + 44][1] = 0;
		fftw[i + 45][0] = buf[45];
		fftw[i + 45][1] = 0;
		fftw[i + 46][0] = buf[46];
		fftw[i + 46][1] = 0;
		fftw[i + 47][0] = buf[47];
		fftw[i + 47][1] = 0;
		fftw[i + 48][0] = buf[48];
		fftw[i + 48][1] = 0;
		fftw[i + 49][0] = buf[49];
		fftw[i + 49][1] = 0;
		fftw[i + 50][0] = buf[50];
		fftw[i + 50][1] = 0;
		fftw[i + 51][0] = buf[51];
		fftw[i + 51][1] = 0;
		fftw[i + 52][0] = buf[52];
		fftw[i + 52][1] = 0;
		fftw[i + 53][0] = buf[53];
		fftw[i + 53][1] = 0;
		fftw[i + 54][0] = buf[54];
		fftw[i + 54][1] = 0;
		fftw[i + 55][0] = buf[55];
		fftw[i + 55][1] = 0;
		fftw[i + 56][0] = buf[56];
		fftw[i + 56][1] = 0;
		fftw[i + 57][0] = buf[57];
		fftw[i + 57][1] = 0;
		fftw[i + 58][0] = buf[58];
		fftw[i + 58][1] = 0;
		fftw[i + 59][0] = buf[59];
		fftw[i + 59][1] = 0;
		fftw[i + 60][0] = buf[60];
		fftw[i + 60][1] = 0;
		fftw[i + 61][0] = buf[61];
		fftw[i + 61][1] = 0;
		fftw[i + 62][0] = buf[62];
		fftw[i + 62][1] = 0;
		fftw[i + 63][0] = buf[63];
		fftw[i + 63][1] = 0;
		buf += 64;
	}
	for (;i < size;i++) {
		fftw[i][0] = *buf++;
		fftw[i][1] = 0;
	}
}
//---------------------------------------------------------------------------
// Function   : windowFFTW
// Description: FFTW用Window関数
// ---
//	
//
inline void windowFFTW(fftw_complex *fftw,long size)
{
	long i,j;

	// ウインドウサイズ毎に定数化する
	switch (size) {
		case (4096 * 1):
			#pragma omp parallel for
			for (i = 0;i < ((4096 * 1) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(4096 * 1));
			}
			#pragma omp parallel for
			for (i = ((4096 * 1) - 1) / 2;i < (4096 * 1);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(4096 * 1));
			}
			break;
		case (4096 * 2):
			#pragma omp parallel for
			for (i = 0;i < ((4096 * 2) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(4096 * 2));
			}
			#pragma omp parallel for
			for (i = ((4096 * 2) - 1) / 2;i < (4096 * 2);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(4096 * 2));
			}
			break;
		case (4096 * 4):
			#pragma omp parallel for
			for (i = 0;i < ((4096 * 4) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(4096 * 4));
			}
			#pragma omp parallel for
			for (i = ((4096 * 4) - 1) / 2;i < (4096 * 4);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(4096 * 4));
			}
			break;
		case (4096 * 8):
			#pragma omp parallel for
			for (i = 0;i < ((4096 * 8) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(4096 * 8));
			}
			#pragma omp parallel for
			for (i = ((4096 * 8) - 1) / 2;i < (4096 * 8);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(4096 * 8));
			}
			break;
		case (4096 * 16):
			#pragma omp parallel for
			for (i = 0;i < ((4096 * 16) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(4096 * 16));
			}
			#pragma omp parallel for
			for (i = ((4096 * 16) - 1) / 2;i < (4096 * 16);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(4096 * 16));
			}
			break;
		case (32000 * 1):
			#pragma omp parallel for
			for (i = 0;i < ((32000 * 1) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(32000 * 1));
			}
			#pragma omp parallel for
			for (i = ((32000 * 1) - 1) / 2;i < (32000 * 1);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(32000 * 1));
			}
			break;
		case (44100 * 1):
			#pragma omp parallel for
			for (i = 0;i < ((44100 * 1) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(44100 * 1));
			}
			#pragma omp parallel for
			for (i = ((44100 * 1) - 1) / 2;i < (44100 * 1);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(44100 * 1));
			}
			break;
		case (48000 * 1):
			#pragma omp parallel for
			for (i = 0;i < ((48000 * 1) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(48000 * 1));
			}
			#pragma omp parallel for
			for (i = ((48000 * 1) - 1) / 2;i < (48000 * 1);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(48000 * 1));
			}
			break;
		case (44100 * 2):
			#pragma omp parallel for
			for (i = 0;i < ((44100 * 2) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(44100 * 2));
			}
			#pragma omp parallel for
			for (i = ((44100 * 2) - 1) / 2;i < (44100 * 2);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(44100 * 2));
			}
			break;
		case (48000 * 2):
			#pragma omp parallel for
			for (i = 0;i < ((48000 * 2) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(48000 * 2));
			}
			#pragma omp parallel for
			for (i = ((48000 * 2) - 1) / 2;i < (48000 * 2);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(48000 * 2));
			}
			break;
		case (44100 * 4):
			#pragma omp parallel for
			for (i = 0;i < ((44100 * 4) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(44100 * 4));
			}
			#pragma omp parallel for
			for (i = ((44100 * 4) - 1) / 2;i < (44100 * 4);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(44100 * 4));
			}
			break;
		case (48000 * 4):
			#pragma omp parallel for
			for (i = 0;i < ((48000 * 4) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(48000 * 4));
			}
			#pragma omp parallel for
			for (i = ((48000 * 4) - 1) / 2;i < (48000 * 4);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(48000 * 4));
			}
			break;
		case (44100 * 8):
			#pragma omp parallel for
			for (i = 0;i < ((44100 * 8) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(44100 * 8));
			}
			#pragma omp parallel for
			for (i = ((44100 * 8) - 1) / 2;i < (44100 * 8);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(44100 * 8));
			}
			break;
		case (48000 * 8):
			#pragma omp parallel for
			for (i = 0;i < ((48000 * 8) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(48000 * 8));
			}
			#pragma omp parallel for
			for (i = ((48000 * 8) - 1) / 2;i < (48000 * 8);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(48000 * 8));
			}
			break;
		case (44100 * 16):
			#pragma omp parallel for
			for (i = 0;i < ((44100 * 16) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(44100 * 16));
			}
			#pragma omp parallel for
			for (i = ((44100 * 16) - 1) / 2;i < (44100 * 16);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(44100 * 16));
			}
			break;
		case (48000 * 16):
			#pragma omp parallel for
			for (i = 0;i < ((48000 * 16) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(48000 * 16));
			}
			#pragma omp parallel for
			for (i = ((48000 * 16) - 1) / 2;i < (48000 * 16);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(48000 * 16));
			}
			break;
		case (44100 * 32):
			#pragma omp parallel for
			for (i = 0;i < ((44100 * 32) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(44100 * 32));
			}
			#pragma omp parallel for
			for (i = ((44100 * 32) - 1) / 2;i < (44100 * 32);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(44100 * 32));
			}
			break;
		case (48000 * 32):
			#pragma omp parallel for
			for (i = 0;i < ((48000 * 32) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(48000 * 32));
			}
			#pragma omp parallel for
			for (i = ((48000 * 32) - 1) / 2;i < (48000 * 32);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(48000 * 32));
			}
			break;
		case (44100 * 64):
			#pragma omp parallel for
			for (i = 0;i < ((44100 * 64) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(44100 * 64));
			}
			#pragma omp parallel for
			for (i = ((44100 * 64) - 1) / 2;i < (44100 * 64);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(44100 * 64));
			}
			break;
		case (48000 * 64):
			#pragma omp parallel for
			for (i = 0;i < ((48000 * 64) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(48000 * 64));
			}
			#pragma omp parallel for
			for (i = ((48000 * 64) - 1) / 2;i < (48000 * 64);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(48000 * 64));
			}
			break;
		case (44100 * 128):
			#pragma omp parallel for
			for (i = 0;i < ((44100 * 128) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(44100 * 128));
			}
			#pragma omp parallel for
			for (i = ((44100 * 128) - 1) / 2;i < (44100 * 128);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(44100 * 128));
			}
			break;
		case (48000 * 128):
			#pragma omp parallel for
			for (i = 0;i < ((48000 * 128) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(48000 * 128));
			}
			#pragma omp parallel for
			for (i = ((48000 * 128) - 1) / 2;i < (48000 * 128);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(48000 * 128));
			}
			break;
		default:
			for (i = 0;i < ((size) - 1) / 2;i++) {
				fftw[i][0] = fftw[i][0] * (2.0 * i / (double)(size));
			}
			for (i = ((size) - 1) / 2;i < (size);i++) {
				fftw[i][0] = fftw[i][0] * (2.0 - 2.0 * i / (double)(size));
			}
			break;
	}
}
//---------------------------------------------------------------------------
// Function   : cutFFTW
// Description: FFTW用カットオフ関数
// ---
//	
//
inline void cutFFTW(fftw_complex *fftw,long index,long size)
{
	long i;

	// 64 個ずつ
	for (i = index;i + 64 < size;i+= 64) {
		fftw[i + 0][0] = 0;
		fftw[i + 0][1] = 0;
		fftw[i + 1][0] = 0;
		fftw[i + 1][1] = 0;
		fftw[i + 2][0] = 0;
		fftw[i + 2][1] = 0;
		fftw[i + 3][0] = 0;
		fftw[i + 3][1] = 0;
		fftw[i + 4][0] = 0;
		fftw[i + 4][1] = 0;
		fftw[i + 5][0] = 0;
		fftw[i + 5][1] = 0;
		fftw[i + 6][0] = 0;
		fftw[i + 6][1] = 0;
		fftw[i + 7][0] = 0;
		fftw[i + 7][1] = 0;
		fftw[i + 8][0] = 0;
		fftw[i + 8][1] = 0;
		fftw[i + 9][0] = 0;
		fftw[i + 9][1] = 0;
		fftw[i + 10][0] = 0;
		fftw[i + 10][1] = 0;
		fftw[i + 11][0] = 0;
		fftw[i + 11][1] = 0;
		fftw[i + 12][0] = 0;
		fftw[i + 12][1] = 0;
		fftw[i + 13][0] = 0;
		fftw[i + 13][1] = 0;
		fftw[i + 14][0] = 0;
		fftw[i + 14][1] = 0;
		fftw[i + 15][0] = 0;
		fftw[i + 15][1] = 0;
		fftw[i + 16][0] = 0;
		fftw[i + 16][1] = 0;
		fftw[i + 17][0] = 0;
		fftw[i + 17][1] = 0;
		fftw[i + 18][0] = 0;
		fftw[i + 18][1] = 0;
		fftw[i + 19][0] = 0;
		fftw[i + 19][1] = 0;
		fftw[i + 20][0] = 0;
		fftw[i + 20][1] = 0;
		fftw[i + 21][0] = 0;
		fftw[i + 21][1] = 0;
		fftw[i + 22][0] = 0;
		fftw[i + 22][1] = 0;
		fftw[i + 23][0] = 0;
		fftw[i + 23][1] = 0;
		fftw[i + 24][0] = 0;
		fftw[i + 24][1] = 0;
		fftw[i + 25][0] = 0;
		fftw[i + 25][1] = 0;
		fftw[i + 26][0] = 0;
		fftw[i + 26][1] = 0;
		fftw[i + 27][0] = 0;
		fftw[i + 27][1] = 0;
		fftw[i + 28][0] = 0;
		fftw[i + 28][1] = 0;
		fftw[i + 29][0] = 0;
		fftw[i + 29][1] = 0;
		fftw[i + 30][0] = 0;
		fftw[i + 30][1] = 0;
		fftw[i + 31][0] = 0;
		fftw[i + 31][1] = 0;
		fftw[i + 32][0] = 0;
		fftw[i + 32][1] = 0;
		fftw[i + 33][0] = 0;
		fftw[i + 33][1] = 0;
		fftw[i + 34][0] = 0;
		fftw[i + 34][1] = 0;
		fftw[i + 35][0] = 0;
		fftw[i + 35][1] = 0;
		fftw[i + 36][0] = 0;
		fftw[i + 36][1] = 0;
		fftw[i + 37][0] = 0;
		fftw[i + 37][1] = 0;
		fftw[i + 38][0] = 0;
		fftw[i + 38][1] = 0;
		fftw[i + 39][0] = 0;
		fftw[i + 39][1] = 0;
		fftw[i + 40][0] = 0;
		fftw[i + 40][1] = 0;
		fftw[i + 41][0] = 0;
		fftw[i + 41][1] = 0;
		fftw[i + 42][0] = 0;
		fftw[i + 42][1] = 0;
		fftw[i + 43][0] = 0;
		fftw[i + 43][1] = 0;
		fftw[i + 44][0] = 0;
		fftw[i + 44][1] = 0;
		fftw[i + 45][0] = 0;
		fftw[i + 45][1] = 0;
		fftw[i + 46][0] = 0;
		fftw[i + 46][1] = 0;
		fftw[i + 47][0] = 0;
		fftw[i + 47][1] = 0;
		fftw[i + 48][0] = 0;
		fftw[i + 48][1] = 0;
		fftw[i + 49][0] = 0;
		fftw[i + 49][1] = 0;
		fftw[i + 50][0] = 0;
		fftw[i + 50][1] = 0;
		fftw[i + 51][0] = 0;
		fftw[i + 51][1] = 0;
		fftw[i + 52][0] = 0;
		fftw[i + 52][1] = 0;
		fftw[i + 53][0] = 0;
		fftw[i + 53][1] = 0;
		fftw[i + 54][0] = 0;
		fftw[i + 54][1] = 0;
		fftw[i + 55][0] = 0;
		fftw[i + 55][1] = 0;
		fftw[i + 56][0] = 0;
		fftw[i + 56][1] = 0;
		fftw[i + 57][0] = 0;
		fftw[i + 57][1] = 0;
		fftw[i + 58][0] = 0;
		fftw[i + 58][1] = 0;
		fftw[i + 59][0] = 0;
		fftw[i + 59][1] = 0;
		fftw[i + 60][0] = 0;
		fftw[i + 60][1] = 0;
		fftw[i + 61][0] = 0;
		fftw[i + 61][1] = 0;
		fftw[i + 62][0] = 0;
		fftw[i + 62][1] = 0;
		fftw[i + 63][0] = 0;
		fftw[i + 63][1] = 0;
	}
	// 残り
	for (;i < size;i++) {
		fftw[i + 0][0] = 0;
		fftw[i + 0][1] = 0;
	}
}
//---------------------------------------------------------------------------
// Function   : al_malloc
// Description: 16バイト境界対応malloc関数
// ---
// 返すポインタの16バイト前にmallocで確保したメモリ領域のアドレスを入れる
//
void *al_malloc(long size)
{
	void *ptr;
	void *ret;
	int align;

	ptr = malloc(size + 32);
	if (ptr) {
		ret = ptr;
		align = (int)ptr % 16;
		if (align != 0) {
			align = 16 - align;
			ret = (char *)ret + align;
		}
		*((SSIZE *)ret) = (SSIZE)ptr;

		ret = (char *)ret + 16;
	} else {
		ret = NULL;
	}
	return ret;
}
//---------------------------------------------------------------------------
// Function   : al_free
// Description: 16バイト境界対応free関数
// ---
// 
//
void *al_free(void *ptr)
{
	void *p;
	
	if (ptr) {
		p = (char *)ptr - 16;
		p = (void *)(*((SSIZE *)p));
		free(p);
	}
}
