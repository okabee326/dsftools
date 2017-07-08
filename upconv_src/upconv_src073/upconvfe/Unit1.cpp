//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "Unit1.h"
#include "Unit4.h"
#include "PLG_AudioIO.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	remSec = 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AddToFileList(AnsiString filename)
{
	SOUNDFMT inFmt;
	ULONG nSample;
	TListItem *item;
	AnsiString s;
	int i;
	int ret;
	long time;
	long size;
	char fname[MAX_PATH];
	UnicodeString fn;
	ret = PLG_InfoAudioData(filename.c_str(),&inFmt,&nSample,NULL);
	if (ret == STATUS_SUCCESS) {
		fn = ExtractFileName(filename);
		item = ListView->Items->Add();

		// edtFileInfo に音声ファイルの情報を保存する
		item->Caption = fn;
		if ((AnsiString)inFmt.fmt == "wav" || (AnsiString)inFmt.fmt == "rf64") {
			time = nSample / inFmt.sample;
			s.printf("%d:%d",time / 60,time % 60);
			item->SubItems->Add(s);
			size = nSample * (inFmt.bitsPerSample / 8) * inFmt.channel;
			s.printf("%5.2fMB",(float)size / 1024 / 1024);
			item->SubItems->Add(s);
			item->SubItems->Add(L"");
		} else {
			s.printf("-");
			item->SubItems->Add(s);
			s.printf("-");
			item->SubItems->Add(s);
			item->SubItems->Add(L"");
		}
		item->Data = new char[strlen(filename.c_str()) + 1];
		strcpy((char *)item->Data,filename.c_str());
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnAddClick(TObject *Sender)
{
	TListItem *item;
	AnsiString s;
	int i;
	int ret;
	long time;
	long size;

	if (OpenDialog->Execute()) {
		for (i = 0;i < OpenDialog->Files->Count;i++) {
			AddToFileList(OpenDialog->Files->Strings[i]);
		}
		if (ListView->Items->Count > 0) {
			btnClear->Enabled = true;
		}
	}
	UpdateTotalSize();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnDelClick(TObject *Sender)
{
	TListItem *item;
	if (ListView->Selected != NULL) {
		item = ListView->Selected;
		if (item->Data) {
			delete[] item->Data;
		}
		item->Delete();
		if (ListView->Items->Count == 0) {
			btnStart->Enabled = false;
			btnClear->Enabled = false;
			btnUp->Enabled = false;
			btnDown->Enabled = false;
			btnInfo->Enabled = false;
		}
	}
	UpdateTotalSize();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnUpClick(TObject *Sender)
{
	UnicodeString s;
	TListItem *from_item,*to_item;
	if (ListView->Selected != NULL && ListView->Selected->Index > 0) {
		int index = ListView->Selected->Index;
		from_item = ListView->Selected;
		to_item   = ListView->Items->AddItem(NULL,index - 1);
		to_item->Assign(from_item);
		to_item->Selected = true;
		from_item->Delete();
		ListView->SetFocus();
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnDownClick(TObject *Sender)
{
	UnicodeString s;
	TListItem *from_item,*to_item;
	if (ListView->Selected != NULL && ListView->Selected->Index + 1 < ListView->Items->Count) {
		int index = ListView->Selected->Index;
		from_item = ListView->Selected;
		to_item   = ListView->Items->AddItem(NULL,index + 2);
		to_item->Assign(from_item);
		to_item->Selected = true;
		from_item->Delete();
		ListView->SetFocus();
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
	FILE *ifp;
	TIniFile *iniFile;
	long sampRate,hfc,lfc,overSamp,adjBE,adjFreq,delFile;
	int sig1,sig2,sig3,sig1avgLineNx,sig1Enb,sig2Enb,sig3Enb,CNR,hfa3NB,nrLV;
	int bitWidth,hfa,lfa,bom,norm,ditherLv,thread,deempha;
	int smLowData,cutDitherData;
	int os32x,hfaFast,hfaWideAna;
	int mc_c,mc_b,mc_l,mc_mono,mc_down,mc_echo,mc_level;
	int cutLowData;
	int hdc,hfc_auto;
	int bwf,rf64,w64,raw,eq;
	int postabe;
	int selOut;
	int encMode;
	int preset;
	int error;
	int poweroff;
	int parameter;
	int fileio;
	int i;
	char buffer[256];
	char title[256];
	int  d[11];
	int ssr;
	int	libsndfile_flag = 0;
	int initial_config;
	SYSTEM_INFO sysinfo;
	AnsiString AppPath = ExtractFilePath(Application->ExeName);
	AnsiString str;
	AnsiString cmdName;

	// ユーザーデータを AppData/upconvへ保存
	AnsiString	appData;
	AnsiString	fromF,toF;
	TRegistry	*xReg;

	appData = AppPath;
#if 0
	xReg = new TRegistry();
	xReg->OpenKey("\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",true);
	appData = xReg->ReadString("appData");
	xReg->CloseKey();
	delete xReg;
	appData = appData + "\\upconv";

	initial_config = 0;
	if (!DirectoryExists(appData)) {
		CreateDir(appData);
	}
	if (DirectoryExists(appData)) {
		fromF = AppPath + "\\upconvfe.ini";
		toF = appData + "\\upconvfe.ini";
		if (!FileExists(toF)) {
			initial_config = 1;
			CopyFile(fromF.c_str(),toF.c_str(),true);
		}
		fromF = AppPath + "\\preset.dat";
		toF = appData + "\\preset.dat";
		if (!FileExists(toF)) {
			initial_config = 1;
			CopyFile(fromF.c_str(),toF.c_str(),true);
		}
	}
#endif

	selectListIndex = -1;
	orgWinProc = ListView->WindowProc;
	ListView->WindowProc = ListViewWinProc;
	DragAcceptFiles(ListView->Handle,true);

	cmdName = (AnsiString)ExtractFilePath(Application->ExeName);
	cmdName += "lame.exe";
	if (PathFileExists(cmdName.c_str())) {
		OpenDialog->Filter = OpenDialog->Filter + L"|MP3|*.MP3";
	}
	cmdName = (AnsiString)ExtractFilePath(Application->ExeName);
	cmdName += "flac.exe";
	if (PathFileExists(cmdName.c_str())) {
		OpenDialog->Filter = OpenDialog->Filter + L"|FLAC|*.FLAC";
	}
	cmdName = (AnsiString)ExtractFilePath(Application->ExeName);
	cmdName += "wvunpack.exe";
	if (PathFileExists(cmdName.c_str())) {
		OpenDialog->Filter = OpenDialog->Filter + L"|WV|*.WV";
	}
	cmdName = (AnsiString)ExtractFilePath(Application->ExeName);
	cmdName += "dsf2raw.exe";
	if (PathFileExists(cmdName.c_str())) {
		OpenDialog->Filter = OpenDialog->Filter + L"|DSF|*.DSF";
	}

	cmdName = (AnsiString)ExtractFilePath(Application->ExeName);
	cmdName += "libsndfile-1.dll";
	if (PathFileExists(cmdName.c_str())) {
		libsndfile_flag = 0x01;
	}
	cmdName = (AnsiString)ExtractFilePath(Application->ExeName);
	cmdName += "sndfile-convert.exe";
	if (PathFileExists(cmdName.c_str())) {
		libsndfile_flag |= 0x02;
	}

	edtHfaParam->Lines->Clear();
//	if (initial_config == 1) {
//		toF = AppPath + "\\preset.dat";
//	} else {
		toF = appData + "\\preset.dat";
//	}
	ifp = fopen(toF.c_str(),"r");
	for (i = 0;i < 10;i++) {
		error = 1;
		if (ifp != NULL) {
			if (fgets(buffer,256,ifp) != NULL) {
				ssr = sscanf(buffer,"%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",title,&d[0],&d[1],&d[2],&d[3],&d[4],&d[5],&d[6],&d[7],&d[8],&d[9]);
				if (ssr != 11) {
					ssr = sscanf(buffer,"%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d",title,&d[0],&d[1],&d[2],&d[3],&d[4],&d[5],&d[6],&d[7],&d[8]);
					d[9] = 1;
				}
				if (ssr == 11 || ssr == 10) {
					error = 0;
					if (!(d[0] == 0 || d[0] == 1)) {
						error = 1;
					}
					if (!(d[1] >= 1 && d[1] <= 25)) {
						error = 1;
					}
					if (!(d[2] >= -44 && d[2] <= 44)) {
						error = 1;
					}
					if (!(d[3] == 0 || d[3] == 1)) {
						error = 1;
					}
					if (!(d[4] >= -44 && d[4] <= 44)) {
						error = 1;
					}
					if (!(d[5] == 0 || d[5] == 1)) {
						error = 1;
					}
					if (!(d[6] >= 0 && d[6] <= 100)) {
						error = 1;
					}
					if (!(d[7] == 0 || d[7] == 1)) {
						error = 1;
					}
					if (!(d[8] >= -44 && d[8] <= 44)) {
						error = 1;
					}
					if (!(d[9] >= 1 && d[9] <= 5)) {
						error = 1;
					}
				}
			}
		}
		if (error) {
			sprintf(title,"%s%02d","Preset",i+1);
			d[0] = 1;	// sig1Enb
			d[1] = 4;	// avgLineNx
			d[2] = 0;	// sig1
			d[3] = 1;   // sig2Enb
			d[4] = -12;   // sig2
			d[5] = 0;   // cnr
			d[6] = 0;   // nb
			d[7] = 1;   // sig3Enb
			d[8] = -3;   // sig3
			d[9] = 1;   // diffMin
		}
		str.printf("%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",title,d[0],d[1],d[2],d[3],d[4],d[5],d[6],d[7],d[8],d[9]);
		edtHfaParam->Lines->Add((UnicodeString)str);
		cmbHfaPreset->Items->Add(title);
	}
	if (ifp != NULL) {
		fclose(ifp);
	}
//	if (initial_config == 1) {
//		toF = AppPath + "\\upconvfe.ini";
//	} else {
		toF = appData + "\\upconvfe.ini";
//	}
	iniFile = new TIniFile(toF);
	sampRate = iniFile->ReadInteger("Convert","SamplingRate",44100);
	bitWidth = iniFile->ReadInteger("Convert","BitWidth",16);
	bom		 = iniFile->ReadInteger("Convert","bom",0);
	hfa 	 = iniFile->ReadInteger("Convert","hfa",0);
	hfc		 = iniFile->ReadInteger("Convert","hfc",-1);
	lfc		 = iniFile->ReadInteger("Convert","lfc",-1);
	parameter = iniFile->ReadInteger("Convert","parameter",0);
	overSamp = iniFile->ReadInteger("Convert","OverSamp",0);
	os32x	 = iniFile->ReadInteger("Convert","OverSamp32x",0);
	adjBE	 = iniFile->ReadInteger("Convert","adjBE",0);
	smLowData = iniFile->ReadInteger("Convert","smLowData",0);
	cutDitherData = iniFile->ReadInteger("Convert","cdd",0);
	cutLowData = iniFile->ReadInteger("Convert","cutLowData",0);
	adjFreq  = iniFile->ReadInteger("Convert","adjFreq",0);
	lfa		 = iniFile->ReadInteger("Convert","lfa",0);
	deempha  = iniFile->ReadInteger("Convert","deEmphasis",0);
	hdc 	 = iniFile->ReadInteger("Convert","hdc",0);
	hfc_auto = iniFile->ReadInteger("Convert","hfcAuto",0);
	eq		 = iniFile->ReadInteger("Convert","eq",0);
	postabe  = iniFile->ReadInteger("Convert","postABE",0);
	hfaFast  = iniFile->ReadInteger("Convert","hfaFast",0);
	hfaWideAna = iniFile->ReadInteger("Convert","hfaWideAna",0);
	preset	 = iniFile->ReadInteger("HFA","Preset",0);
	nrLV	 = iniFile->ReadInteger("NR","nrLV",1);
	selOut	 = iniFile->ReadInteger("Output","Sel",0);
	norm	 = iniFile->ReadInteger("Output","Normalize",0);
	ditherLv = iniFile->ReadInteger("Output","DitherLv",0);
	bwf		 = iniFile->ReadInteger("Output","BWF",0);
	rf64	 = iniFile->ReadInteger("Output","RF64",0);
	w64		 = iniFile->ReadInteger("Output","W64",0);
	thread	 = iniFile->ReadInteger("Exec","Thread",1);
	poweroff = iniFile->ReadInteger("Exec","Poweroff",0);
	delFile  = iniFile->ReadInteger("Exec","delFile",0);
	encMode	 = iniFile->ReadInteger("Encoder","Mode",0);
	eq20->Position = iniFile->ReadInteger("EQ","1",5);
	eq40->Position = iniFile->ReadInteger("EQ","2",5);
	eq60->Position = iniFile->ReadInteger("EQ","3",5);
	eq80->Position = iniFile->ReadInteger("EQ","4",5);
	eq100->Position = iniFile->ReadInteger("EQ","5",5);
	eq250->Position = iniFile->ReadInteger("EQ","6",5);
	eq500->Position = iniFile->ReadInteger("EQ","7",5);
	eq750->Position = iniFile->ReadInteger("EQ","8",5);
	eq1k->Position	= iniFile->ReadInteger("EQ","9",5);
	eq2k->Position	= iniFile->ReadInteger("EQ","10",5);
	eq4k->Position	= iniFile->ReadInteger("EQ","11",5);
	eq6k->Position	= iniFile->ReadInteger("EQ","12",5);
	eq8k->Position  = iniFile->ReadInteger("EQ","13",5);
	eq10k->Position  = iniFile->ReadInteger("EQ","14",5);
	eq12k->Position  = iniFile->ReadInteger("EQ","15",5);
	eq14k->Position  = iniFile->ReadInteger("EQ","16",5);
	eq16k->Position  = iniFile->ReadInteger("EQ","17",5);
	eq18k->Position  = iniFile->ReadInteger("EQ","18",5);
	mc_c             = iniFile->ReadInteger("MC","mc_c",0);
	mc_b             = iniFile->ReadInteger("MC","mc_b",0);
	mc_l             = iniFile->ReadInteger("MC","mc_l",0);
	mc_mono          = iniFile->ReadInteger("MC","mc_mono",0);
	mc_down          = iniFile->ReadInteger("MC","mc_down",0);
	mc_echo          = iniFile->ReadInteger("MC","mc_echo",0);
	mc_level         = iniFile->ReadInteger("MC","mc_level",0);

	fileio = iniFile->ReadInteger("Exec","fileio",0);

	switch (sampRate) {
		case 32000:
			cmbSampRate->ItemIndex = 0;
			break;
		case 44100:
			cmbSampRate->ItemIndex = 1;
			break;
		case 48000:
			cmbSampRate->ItemIndex = 2;
			break;
		case 88200:
			cmbSampRate->ItemIndex = 3;
			break;
		case 96000:
			cmbSampRate->ItemIndex = 4;
			break;
		case 176400:
			cmbSampRate->ItemIndex = 5;
			break;
		case 192000:
			cmbSampRate->ItemIndex = 6;
			break;
		case 352800:
			cmbSampRate->ItemIndex = 7;
			break;
		case 384000:
			cmbSampRate->ItemIndex = 8;
			break;
	}
	if (bitWidth == 16) {
		cmbBitwidth->ItemIndex = 0;
	} else if (bitWidth == 24) {
		cmbBitwidth->ItemIndex = 1;
	} else if (bitWidth == 32) {
		cmbBitwidth->ItemIndex = 2;
	} else if (bitWidth == 64) {
		cmbBitwidth->ItemIndex = 3;
	}
	if (parameter >= 0 && parameter <= 3) {
		cmbParam->ItemIndex = parameter;
	}
	if (norm == 1) {
		chkNormalize->Checked = true;
	}
	if (adjBE == 1) {
		chkAdjBE->Checked = true;
	}
	if (smLowData == 1) {
		chkSMLowData->Checked = true;
	}
	if (cutDitherData >= 0 && cutDitherData <= 5) {
		cmbCutDither->ItemIndex = cutDitherData;
	}

	if (cutLowData == 1) {
		chkCutLowData->Checked = true;
	}
	if (adjFreq == 1) {
		chkAdjFreq->Checked = true;
	}
	if (overSamp == 1) {
		chkOverSampling->Checked = true;
	}
	if (os32x == 1) {
		chkOs32x->Checked = true;
	}
	if (lfa == 1) {
		chkLfa->Checked = true;
	}
	if (hdc == 1) {
		chkHdc->Checked = true;
	}
	if (hfc_auto == 1) {
		chkHfcAuto->Checked = true;
	}
	if (postabe == 1) {
		chkPostABE->Checked = true;
	}
	if (hfaFast == 1) {
		chkHfaFast->Checked = true;
	}
	if (hfaWideAna == 1) {
		chkWideAna->Checked = true;
	}

	if (ditherLv >= 0 && ditherLv <= 16) {
		UpDown4->Position = ditherLv;
	}
	if (deempha == 1) {
		chkDeEmphasis->Checked = true;
	}

	if (thread >= 1 && thread <= 24) {
		cmbThread->ItemIndex = thread - 1;
	}
	if (poweroff == 1) {
		chkShutdown->Checked = true;
	}
	cmbOutFormat->ItemIndex = 0;
	if (bwf == 1) {
		cmbOutFormat->ItemIndex = 1;
	}
	if (rf64 == 1) {
		cmbOutFormat->ItemIndex = 2;
	}
	if (libsndfile_flag == 0x03) {
		if (w64 == 1) {
			cmbOutFormat->ItemIndex = 3;
		}
	}
	if (eq == 1) {
		chkEQ->Checked = true;
	}
	if (bom >= 0 && bom <= 3) {
		cmbBOM->ItemIndex = bom;
	}
	if (hfa >= 0 && hfa <= 3) {
		cmbHFA->ItemIndex = hfa;
	}
	if (hfc >= 1000 && hfc <= 192000) {
		edtHFC->Text = hfc;
	}
	if (lfc >= 0 && lfc <= 192000) {
		edtLFC->Text = lfc;
	}
	if (nrLV >= 1 && nrLV <= 5) {
		UpDown6->Position = nrLV;
	}
	if (preset >= 0 && preset < 10) {
		cmbHfaPreset->ItemIndex = preset;
		cmbHfaPreset->OnChange(Sender);
	}

	if (selOut == 0) {
		rdoToFileSrc->Checked = true;
		shp1->Style = bsLowered;
		shp2->Style = bsRaised;
	} else if (selOut == 1) {
		rdoToFileSpecify->Checked = true;
		shp1->Style = bsRaised;
		shp2->Style = bsLowered;
	}
	edtSuffix->Text = iniFile->ReadString("Output","Suffix","");

	lblOutputFolder->Text = iniFile->ReadString("Output","Folder","");
	if (lblOutputFolder->Text != "") {
		rdoToFileSpecify->Enabled = true;
	}

	if (encMode >= 0 && encMode <= 2) {
		cmbEncoder->ItemIndex = encMode;
	}
	edtFLACOption->Text = iniFile->ReadString("Encode","FLACOpt","-s -f --best");
	edtWavPackOption->Text = iniFile->ReadString("Encode","WavPackOpt","-hh -q");

	if (delFile == 1) {
		chkDeleteFiles->Checked = true;
	}
	if (fileio >= 0 && fileio <= 14) {
		cmbIO->ItemIndex = fileio;
	}
	if (mc_c) {
		chkGenCenter->Checked = true;
	}
	if (mc_b) {
		chkGenSurround->Checked = true;
	}
	if (mc_l) {
		chkGenLFE->Checked = true;
	}
	if (mc_mono) {
		chkOutMono->Checked = true;
	}
	if (mc_down) {
		chkDownMix->Checked = true;
	}
	if (mc_echo) {
		chkechoeffect->Checked = true;
	}
	if (mc_level >= 0 && mc_level <= 4) {
		cmbEffect->ItemIndex = mc_level;
	}

	Form1->Caption = L"Upconv Frontend " + iniFile->ReadString(L"Convert",L"Version",L"0.7.x");
	GetNativeSystemInfo(&sysinfo);
	if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
		Form1->Caption = Form1->Caption + L" (x64)";
	}


	delete iniFile;
}
void __fastcall TForm1::cmbHfaPresetChange(TObject *Sender)
{
	int sig1,sig2,sig3;
	int avgLineNx;
	int sig1Enb,sig2Enb,sig3Enb;
	int cnr,nb;
	int i;
	char buffer[256];
	char title[256];
	int p[11];

	sig1 = -16;
	sig2 = -10;
	avgLineNx = 4;
	sig1Enb = 1;
	sig2Enb = 1;
	cnr = 0;
	AnsiString s;

	chkSig1Enb->Checked = false;
	chkSig2Enb->Checked = false;

	s = (AnsiString)edtHfaParam->Lines->Strings[cmbHfaPreset->ItemIndex];
	if (sscanf(s.c_str(),"%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",title,&p[0],&p[1],&p[2],&p[3],&p[4],&p[5],&p[6],&p[7],&p[8],&p[9]) == 11) {
		sig1Enb = p[0];
		avgLineNx = p[1];
		sig1 = p[2];
		sig2Enb = p[3];
		sig2 = p[4];
		cnr = p[5];
		nb	= p[6];
		sig3Enb = p[7];
		sig3 = p[8];
		if (sig1Enb == 1) {
			chkSig1Enb->Checked = true;
		}
		UpDown3->Position = avgLineNx;
		UpDown1->Position = sig1;
		if (sig2Enb == 1) {
			chkSig2Enb->Checked = true;
		}
		UpDown2->Position = sig2;
		UpDown5->Position = nb;
		if (sig3Enb == 1) {
			chkSig3Enb->Checked = true;
		}
		UpDown7->Position = sig3;
		UpDown8->Position = p[9];
		edtPresetTitle->Text = cmbHfaPreset->Items->Strings[cmbHfaPreset->ItemIndex];
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	TIniFile *iniFile;
	long hfc,lfc,temp;
	int norm,multi,poweroff,delFile;
	int bit[]={16,24,32,64};
	int i;
	// ユーザーデータを AppData/upconvへ保存
	AnsiString	appData;
	AnsiString	toF;
	TRegistry	*xReg;

#if 0
	xReg = new TRegistry();
	xReg->OpenKey("\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",true);
	appData = xReg->ReadString("appData");
	xReg->CloseKey();
	delete xReg;
	appData = appData + "\\upconv";
#endif
	appData = ExtractFilePath(Application->ExeName);

	toF = appData + "\\upconvfe.ini";
	iniFile = new TIniFile(toF);
	iniFile->WriteInteger("Convert","SamplingRate",cmbSampRate->Text.ToInt());
	iniFile->WriteInteger("Convert","BitWidth",bit[cmbBitwidth->ItemIndex]);
	iniFile->WriteInteger("Convert","bom",cmbBOM->ItemIndex);
	iniFile->WriteInteger("Convert","hfa",cmbHFA->ItemIndex);
	iniFile->WriteInteger("Convert","AdjBE",chkAdjBE->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Convert","smLowData",chkSMLowData->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Convert","cdd",cmbCutDither->ItemIndex);
	iniFile->WriteInteger("Convert","CutLowData",chkCutLowData->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Convert","AdjFreq",chkAdjFreq->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Convert","OverSamp",chkOverSampling->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Convert","OverSamp32x",chkOs32x->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Convert","Lfa",chkLfa->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Convert","DeEmphasis",chkDeEmphasis->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Convert","hdc",chkHdc->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Convert","hfcAuto",chkHfcAuto->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Convert","eq",chkEQ->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Convert","postABE",chkPostABE->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Convert","hfaFast",chkHfaFast->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Convert","hfaWideAna",chkWideAna->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Convert","parameter",cmbParam->ItemIndex);
	iniFile->WriteInteger("HFA","Preset",cmbHfaPreset->ItemIndex);
	iniFile->WriteInteger("NR","nrLV",edtNRLV->Text.ToInt());
	iniFile->WriteInteger("EQ","1",eq20->Position);
	iniFile->WriteInteger("EQ","2",eq40->Position);
	iniFile->WriteInteger("EQ","3",eq60->Position);
	iniFile->WriteInteger("EQ","4",eq80->Position);
	iniFile->WriteInteger("EQ","5",eq100->Position);
	iniFile->WriteInteger("EQ","6",eq250->Position);
	iniFile->WriteInteger("EQ","7",eq500->Position);
	iniFile->WriteInteger("EQ","8",eq750->Position);
	iniFile->WriteInteger("EQ","9",eq1k->Position);
	iniFile->WriteInteger("EQ","10",eq2k->Position);
	iniFile->WriteInteger("EQ","11",eq4k->Position);
	iniFile->WriteInteger("EQ","12",eq6k->Position);
	iniFile->WriteInteger("EQ","13",eq8k->Position);
	iniFile->WriteInteger("EQ","14",eq10k->Position);
	iniFile->WriteInteger("EQ","15",eq12k->Position);
	iniFile->WriteInteger("EQ","16",eq14k->Position);
	iniFile->WriteInteger("EQ","17",eq16k->Position);
	iniFile->WriteInteger("EQ","18",eq18k->Position);
	iniFile->WriteInteger("Exec","fileio",cmbIO->ItemIndex);
	norm = 0;
	if (chkNormalize->Checked == true) {
		norm = 1;
	}
	multi = 0;
	//if (chkMultiThread->Checked == true) {
	//	multi = 1;
	//}
	poweroff = 0;
	if (chkShutdown->Checked == true) {
		poweroff = 1;
	}
	delFile = 0;
	if (chkDeleteFiles->Checked == true) {
		delFile = 1;
	}
	iniFile->WriteInteger("Output","Normalize",norm);
	iniFile->WriteInteger("Exec","Thread",cmbThread->ItemIndex + 1);
	iniFile->WriteInteger("Exec","Poweroff",poweroff);
	iniFile->WriteInteger("Exec","delFile",delFile);

	if (edtHFC->Text != "") {
		hfc = edtHFC->Text.ToInt();
	} else {
		hfc = -1;
	}
	if (edtLFC->Text != "") {
		lfc = edtLFC->Text.ToInt();
	} else {
		lfc = -1;
	}

	iniFile->WriteInteger("Convert","hfc",hfc);
	iniFile->WriteInteger("Convert","lfc",lfc);

	iniFile->WriteInteger("Output","DitherLv",UpDown4->Position);
	iniFile->WriteInteger("Output","Sel",rdoToFileSrc->Checked == true ? 0 : 1);
	iniFile->WriteString("Output","Folder",lblOutputFolder->Text);
	iniFile->WriteString("Output","Suffix",edtSuffix->Text);
	iniFile->WriteInteger("Output","BWF",cmbOutFormat->ItemIndex == 1 ? 1 : 0);
	iniFile->WriteInteger("Output","RF64",cmbOutFormat->ItemIndex == 2 ? 1 : 0);
	iniFile->WriteInteger("Output","W64",cmbOutFormat->ItemIndex == 3 ? 1 : 0);
	iniFile->WriteString("Encoder","Mode",cmbEncoder->ItemIndex);

	iniFile->WriteString("Encode","FLACOpt",edtFLACOption->Text);
	iniFile->WriteString("Encode","WavPackOpt",edtWavPackOption->Text);

	iniFile->WriteInteger("MC","mc_c",chkGenCenter->Checked == true ? 1 : 0);
	iniFile->WriteInteger("MC","mc_b",chkGenSurround->Checked == true ? 1 : 0);
	iniFile->WriteInteger("MC","mc_l",chkGenLFE->Checked == true ? 1 : 0);
	iniFile->WriteInteger("MC","mc_mono",chkOutMono->Checked == true ? 1 : 0);
	iniFile->WriteInteger("MC","mc_down",chkDownMix->Checked == true ? 1 : 0);
	iniFile->WriteInteger("MC","mc_echo",chkechoeffect->Checked == true ? 1 : 0);
	iniFile->WriteInteger("MC","mc_level",cmbEffect->ItemIndex);

	delete iniFile;

	toF = appData + "\\preset.dat";
	edtHfaParam->Lines->SaveToFile(toF);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ListViewWinProc(TMessage& Msg)
{
	TListItem *item;
	HANDLE hDrop;
	int i,nFiles;
	char szFile[MAX_PATH];

	if (Msg.Msg == WM_DROPFILES) {
		Msg.Result = 0;

		hDrop = (HANDLE)Msg.WParam;
		nFiles = DragQueryFile(hDrop,-1,NULL,0);
		for (i = 0;i < nFiles;i++) {
			if (DragQueryFile(hDrop,i,szFile,sizeof(szFile)) > 0) {
				AddToFileList(szFile);
			}
		}
		DragFinish(hDrop);
		UpdateTotalSize();
		btnClear->Enabled = true;
	} else {
		orgWinProc(Msg);
	}
}

void __fastcall TForm1::btnExitClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ListViewSelectItem(TObject *Sender,
	  TListItem *Item, bool Selected)
{
	AnsiString s;
	UnicodeString u;
	TListItem *item;
	SOUNDFMT inFmt;
	DWORD nSample;

	btnInfo->Enabled = false;
	if (ListView->SelCount > 0) {
		btnDel->Enabled = true;

		if (ListView->Selected != NULL) {
			item = ListView->Selected;
			if (item->Data) {
				s = (AnsiString)(char *)item->Data;
				u = LowerCase(ExtractFileExt((UnicodeString)s));

				if (u == ".wav" || u == ".flac" || u == ".dsf") {
					btnInfo->Enabled = true;
				}
				memset(&inFmt,0,sizeof (SOUNDFMT));
				PLG_InfoAudioData((char *)(item->Data),&inFmt,&nSample,NULL);
				if (inFmt.sample != 0) {
					lblSamp->Text = inFmt.sample;
				}
				if (inFmt.bitsPerSample != 0) {
					lblWidth->Text = inFmt.bitsPerSample;
				}
			}
		}
	} else {
		btnDel->Enabled = false;
		btnInfo->Enabled = false;
	}
	if (ListView->Selected != NULL && ListView->Selected->Index > 0) {
		btnUp->Enabled = true;
	} else {
		btnUp->Enabled = false;
	}
	if (ListView->Selected != NULL && ListView->Selected->Index + 1 < ListView->Items->Count) {
		btnDown->Enabled = true;
	} else {
		btnDown->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ListViewChange(TObject *Sender, TListItem *Item,
	  TItemChange Change)
{
	if (ListView->Items->Count > 0) {
		btnStart->Enabled = true;
	} else {
		btnStart->Enabled = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnStartClick(TObject *Sender)
{
	int ret;
	int i;
	int hfa;
	int sizeIsOver;
	SOUNDFMT inFmt;
	DWORD nSample;
	double mb,sz;
	int bit[]={16,24,32,64};
	AnsiString command;
	AnsiString sHFA,sRate,sBitWidth,sHFC,sLFC;
	sizeIsOver = false;

	for (i = 0;i < ListView->Items->Count;i++) {
		ListView->Items->Item[i]->ImageIndex = 0;
		ListView->Items->Item[i]->SubItems->Strings[2] = L"";
	}

	startTick = GetTickCount();
	btnExit->Enabled = false;
	DragAcceptFiles(ListView->Handle,false);
	fileIndex = 0;
	StatusBar1->SimpleText = "";
	btnStart->Visible = false;
	btnAbort->Visible = true;
	cpSetting->Enabled = false;
	abort = 0;
	ExecChildProcess(0);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdoToFileSrcClick(TObject *Sender)
{
	edtSuffix->Enabled = true;
	lblOutputFolder->Enabled = false;
	shp1->Style = bsLowered;
	shp2->Style = bsRaised;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdoToFileSpecifyClick(TObject *Sender)
{
	lblOutputFolder->Enabled = true;
	edtSuffix->Enabled = false;
	shp1->Style = bsRaised;
	shp2->Style = bsLowered;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ExecChildProcess(int index)
{
	int i;
	int hfa,bom;
	int mc_flag;
	int wk_num;
	char fullpath[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char drive2[_MAX_DRIVE];
	char dir2[_MAX_DIR];
	char file[_MAX_FNAME];
	char file2[_MAX_FNAME];
	char ext[_MAX_EXT];
	char ext2[_MAX_EXT];
	FILE *fp;
	AnsiString cmd,cmdParam,outDir,fileName;
	AnsiString sInRate;
	AnsiString fromFile,toFile,flacFile;
	AnsiString AppPath = ExtractFilePath(Application->ExeName);

	int bit[]={16,24,32,64};
	SOUNDFMT inFmt;
	DWORD nSample;
	MEMORYSTATUSEX mse;
	memset(&mse,0,sizeof (MEMORYSTATUSEX));
	mse.dwLength = sizeof (MEMORYSTATUSEX);
	if (GlobalMemoryStatusEx(&mse)) {
		if (mse.ullAvailPhys < 700 * 1024 * 1024) {
			int i;
			char *p;
			if (index == 0) {
				for (i = 0;i < 5;i++) {
					p = (char *)malloc(500 * 1024 * 1024);
					if (p) {
						free(p);
						Sleep(1000);
						break;
					}
				}
			}
			for (i = 0;i < 10;i++) {
				Sleep(1000);
			}
		}
	}
	mc_flag = 0;
	if (chkGenCenter->Checked || chkGenSurround->Checked || chkGenLFE->Checked) {
		mc_flag = 1;
	}

	if (index < ListView->Items->Count) {
		PLG_InfoAudioData((char *)(ListView->Items->Item[index]->Data),&inFmt,&nSample,NULL);
		ProgressBar1->Position = 0;
		ListView->Selected = ListView->Items->Item[index];
		lblFilename->Caption = L"[" + (UnicodeString)(index + 1) + L"/" + (UnicodeString)(ListView->Items->Count) + L"] " + (UnicodeString)(char *)(ListView->Items->Item[index]->Data);
		lblProcess1->Caption = "";

		cmdParam = "";
		cmdParam += "-s:";
		if (mc_flag == 0) {
			cmdParam += (AnsiString)cmbSampRate->Text;
		} else {
			cmdParam += (AnsiString)cmbSampRate->Text;

			wk_num = cmbSampRate->Text.ToInt();
			wk_num *= 2;
			cmdParam += " -ms:";
			cmdParam += wk_num;
			if (chkGenCenter->Checked) {
				cmdParam += " -C";
			}
			if (chkGenSurround->Checked) {
				cmdParam += " -SLR";
			}
			if (chkGenLFE->Checked) {
				cmdParam += " -LFE";
			}
			if (chkOutMono->Checked) {
				cmdParam += " -split";
			}
			if (chkechoeffect->Checked) {
				cmdParam += " -mcecho";
			}
			if (chkDownMix->Checked) {
            	cmdParam += " -mcdownmix";
			}
			cmdParam += " -Delay:";
			cmdParam += cmbEffect->ItemIndex;

		}
		cmdParam += " -w:";
		cmdParam += (AnsiString)bit[cmbBitwidth->ItemIndex];

		cmdParam += " -hfa:";
		cmdParam += (AnsiString)cmbHFA->ItemIndex;

		if (edtHFC->Text != "") {
			cmdParam += " -hfc:";
			cmdParam += (AnsiString)edtHFC->Text;
		}
		if (edtLFC->Text != "") {
			cmdParam += " -lfc:";
			cmdParam += (AnsiString)edtLFC->Text;
		}
		if (edtLPF->Text != "") {
			cmdParam += " -lpf:";
			cmdParam += (AnsiString)edtLPF->Text;
		}
		if (edtNR->Text != "") {
			cmdParam += " -nr:";
			cmdParam += (AnsiString)edtNR->Text;
			cmdParam += " -nrLV:";
			cmdParam += (AnsiString)edtNRLV->Text;
		}

		if (chkAdjBE->Checked == true) {
			cmdParam += " -adjBE";
			if (chkCutLowData->Checked) {
				cmdParam += " -cutLowData";
			}
			if (chkSMLowData->Checked) {
				cmdParam += " -smLowData";
			}
			if (cmbCutDither->ItemIndex != 0) {
				cmdParam += " -cutDither:";
				cmdParam += cmbCutDither->ItemIndex;
			}
		}
		strcpy(fullpath,(char *)ListView->Items->Item[index]->Data);
		_splitpath(fullpath,drive,dir,file,ext);
		if (stricmp(file,"sp_ref") == 0 && stricmp(ext,".wav") == 0) {
			cmdParam += " -spAna";
		}
		if (chkAdjFreq->Checked == true) {
			cmdParam += " -adjFreq";
		}

		if (chkLfa->Checked == true) {
			cmdParam += " -lfa:1";
		}
		cmdParam += " -overSamp:";
		if (chkOverSampling->Checked == true && chkOs32x->Checked == false) {
			cmdParam += "2";
		} else if (chkOs32x->Checked == true) {
			cmdParam += "6";
		} else {
			cmdParam += "0";
		}
		if (chkHdc->Checked == true) {
			cmdParam += " -hdc";
		}
		if (chkHfcAuto->Checked == true) {
			cmdParam += " -hfcAuto";
		}

		cmdParam += " -thread:";
		cmdParam += (AnsiString)cmbThread->Text;

		cmdParam += " -deEmphasis:";
		if (chkDeEmphasis->Checked == false) {
			cmdParam += "0";
		} else {
			if (rdoDEType1->Checked == true) {
				cmdParam += "1";
			} else {
				cmdParam += "2";
			}
		}
		if (chkPostABE->Checked == true) {
			cmdParam += " -postABE";
		}

		if (cmbHFA->ItemIndex == 2 || cmbHFA->ItemIndex == 3) {
			cmdParam += " -sig1:";
			if (chkSig1Enb->Checked == true) {
				cmdParam += "1,";
			} else {
				cmdParam += "0,";
			}
			cmdParam += (AnsiString)edtAvgLineNx->Text;
			cmdParam += ",";
			cmdParam += (AnsiString)edtSig1->Text;

			cmdParam += " -sig2:";
			if (chkSig2Enb->Checked == true) {
				cmdParam += "1,";
			} else {
				cmdParam += "0,";
			}
			cmdParam += (AnsiString)edtSig2->Text;

			cmdParam += " -sig3:";
			if (chkSig3Enb->Checked == true) {
				cmdParam += "1,";
			} else {
				cmdParam += "0,";
			}
			cmdParam += (AnsiString)edtSig3->Text;

			cmdParam += " -hfaNB:";
			cmdParam += (AnsiString)edtNB->Text;

			if (chkHfaFast->Checked == true) {
				cmdParam += " -hfaFast";
			}
			if (chkWideAna->Checked == true) {
				cmdParam += " -hfaWide";
			}
			cmdParam += " -hfaDiffMin:";
			cmdParam += edtDiffmin->Text;
		}
		cmdParam += " -m:";
		cmdParam += (AnsiString)cmbBOM->ItemIndex;

		cmdParam += " -n:";
		if (chkNormalize->Checked == true) {
			cmdParam += "1";
		} else {
			cmdParam += "0";
		}
		cmdParam += " -ditherLv:";
		cmdParam += (AnsiString)edtDitherLevel->Text;
		if (cmbOutFormat->ItemIndex == 1) {
			cmdParam += " -bwf";
		}
		if (cmbOutFormat->ItemIndex == 2) {
			cmdParam += " -rf64";
		}
		if (cmbOutFormat->ItemIndex == 3) {
			cmdParam += " -rf64";
		}
		cmdParam += " -ch:";
		cmdParam += inFmt.channel;

		if (chkEQ->Checked) {
			cmdParam += " -eq";
		}
		cmdParam += " -dsf:";
		cmdParam += cmbDSF->ItemIndex;

		cmdParam += " -fio:";
		cmdParam += cmbIO->Items->Strings[cmbIO->ItemIndex];

		if ((AnsiString)inFmt.fmt == "flac") {
			cmdParam += " -flac";
		}

		ListView->Items->Item[index]->SubItems->Strings[2] = "...";
		ListView->Items->Item[index]->ImageIndex = 1;
		ListView->Items->Item[index]->MakeVisible(true);

		if (rdoToFileSpecify->Checked == true) {
			strcpy(fullpath,(char *)ListView->Items->Item[index]->Data);
			_splitpath(fullpath,drive,dir,file,ext);
			outDir = (AnsiString)lblOutputFolder->Text;
			outDir += "\\";
			_splitpath(outDir.c_str(),drive2,dir2,file2,ext2);
			_makepath(fullpath,drive2,dir2,file,"wav");
			toPath = (AnsiString)fullpath;
		} else {
			_splitpath((char *)ListView->Items->Item[index]->Data,drive,dir,file,ext);
			if (edtSuffix->Text == "") {
				strcat(file,"_upconv");
			} else {
				strcat(file,"_");
				strcat(file,((AnsiString)edtSuffix->Text).c_str());
			}
			_makepath(fullpath,drive,dir,file,"wav");
			toPath = (AnsiString)fullpath;
		}
		// パラメータファイルの出力
		_splitpath(toPath.c_str(),drive,dir,file,ext);
		_makepath(fullpath,drive,dir,file,"param");
		fp = fopen(fullpath,"w");
		if (fp != NULL) {
			fprintf(fp,"%s ",cmdParam.c_str());
			fclose(fp);
		}
		_splitpath(toPath.c_str(),drive,dir,file,ext);
		_makepath(fullpath,drive,dir,file,"files");
		fp = fopen(fullpath,"w");
		if (fp != NULL) {
			// 削除予定ファイル名書き込み
			_splitpath(toPath.c_str(),drive,dir,file,ext);
			_makepath(fullpath,drive,dir,file,"param");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r1.param");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r2.param");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r3.param");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r4.param");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r5.param");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r6.param");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r1");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r2");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r3");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r4");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r5");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r6");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r1.tmp");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r1.tmp2");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r2.tmp");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r2.tmp2");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r3.tmp");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r3.tmp2");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r4.tmp");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r4.tmp2");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r5.tmp");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r5.tmp2");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r6.tmp");
			fprintf(fp,"%s\n",fullpath);
			_makepath(fullpath,drive,dir,file,"r6.tmp2");
			fprintf(fp,"%s\n",fullpath);
		}

		threadExec = NULL;
		doneFlags = 0x0000;
		threadExec = new Exec(true);
		threadExec->FreeOnTerminate = true;

		threadExec->cmd_d = "";		// decode
		threadExec->cmd_r = "";		// wav2raw
		threadExec->cmd_u1 = "";	// upconv
		threadExec->cmd_u2 = "";	// upconv
		threadExec->cmd_u3 = "";	// upconv
		threadExec->cmd_u4 = "";	// upconv
		threadExec->cmd_u5 = "";	// upconv
		threadExec->cmd_u6 = "";	// upconv
		threadExec->cmd_w = "";		// raw2wav
		threadExec->cmd_e = "";		// encode
		threadExec->cmd_usr = "";	// usr
		threadExec->cmd_snd = "";	//
		threadExec->cmd_mc  = "";	//

		threadExec->thread = cmbThread->Text.ToInt();

		fromFile = (AnsiString)(char *)ListView->Items->Item[index]->Data;
		toFile	 = toPath;
		threadExec->toFile = toFile;

		if ((AnsiString)inFmt.fmt != "wav" && (AnsiString)inFmt.fmt != "dsf" && (AnsiString)inFmt.fmt != "rf64") {
			threadExec->cmd_d.printf("%s_d.cmd \"%s\" \"%s.wav\"",inFmt.fmt,fromFile,toFile);
			flacFile = fromFile;
			fromFile = toFile + ".wav";
			if (fp != NULL) {
				fprintf(fp,"%s.wav\n",toFile.c_str());
			}
		}
		if (fp != NULL) {
			fclose(fp);
		}
		if ((AnsiString)inFmt.fmt != "dsf") {
			threadExec->cmd_r.printf("%s\\wav2raw.exe \"%s\" \"%s\"",AppPath.c_str(),fromFile,toFile);
		} else {
			threadExec->cmd_r.printf("%s\\dsf2raw.exe \"%s\" \"%s\"",AppPath.c_str(),fromFile,toFile);
		}
		_splitpath(toFile.c_str(),drive,dir,file,ext);
		_makepath(fullpath,drive,dir,file,"r1");
		threadExec->cmd_u1.printf("%s\\upconv.exe \"%s\" \"%s\"",AppPath.c_str(),toFile,(AnsiString)fullpath);
		if (inFmt.channel > 1) {
			_makepath(fullpath,drive,dir,file,"r2");
			threadExec->cmd_u2.printf("%s\\upconv.exe \"%s\" \"%s\"",AppPath.c_str(),toFile,(AnsiString)fullpath);
		}
		if (inFmt.channel > 2) {
			_makepath(fullpath,drive,dir,file,"r3");
			threadExec->cmd_u3.printf("%s\\upconv.exe \"%s\" \"%s\"",AppPath.c_str(),toFile,(AnsiString)fullpath);
		}
		if (inFmt.channel > 3) {
			_makepath(fullpath,drive,dir,file,"r4");
			threadExec->cmd_u4.printf("%s\\upconv.exe \"%s\" \"%s\"",AppPath.c_str(),toFile,(AnsiString)fullpath);
		}
		if (inFmt.channel > 4) {
			_makepath(fullpath,drive,dir,file,"r5");
			threadExec->cmd_u5.printf("%s\\upconv.exe \"%s\" \"%s\"",AppPath.c_str(),toFile,(AnsiString)fullpath);
		}
		if (inFmt.channel > 5) {
			_makepath(fullpath,drive,dir,file,"r6");
			threadExec->cmd_u6.printf("%s\\upconv.exe \"%s\" \"%s\"",AppPath.c_str(),toFile,(AnsiString)fullpath);
		}

		if ((AnsiString)inFmt.fmt != "flac") {
			threadExec->cmd_w.printf("%s\\raw2wav.exe \"%s\" \"%s\"",AppPath.c_str(),fromFile,toFile);
		} else {
			threadExec->cmd_w.printf("%s\\raw2wav.exe \"%s\" \"%s\" \"%s\"",AppPath.c_str(),fromFile,toFile,flacFile);
		}
		destWavFile = toFile;
		if (cmbEncoder->ItemIndex != 0) {
			if (cmbEncoder->ItemIndex == 1) {
				threadExec->cmd_e.printf("%s\\flac.exe %s",AppPath.c_str(),(AnsiString)edtFLACOption->Text);
				threadExec->cmd_e = threadExec->cmd_e + " \"" + toFile + "\"";
			}
			if (cmbEncoder->ItemIndex == 2) {
				threadExec->cmd_e.printf("%s\\wavpack.exe %s",AppPath.c_str(),(AnsiString)edtWavPackOption->Text);
				threadExec->cmd_e = threadExec->cmd_e + " \"" + toFile + "\"";
			}
		}

		if (cmbOutFormat->ItemIndex == 3) {
			_splitpath(toPath.c_str(),drive,dir,file,ext);
			_makepath(fullpath,drive,dir,file,"w64");
			threadExec->cmd_snd.printf("%s\\sndfile-convert.exe \"%s\" \"%s\"",AppPath.c_str(),toFile,fullpath);
		} else if (cmbOutFormat->ItemIndex == 4) {
			_splitpath(toPath.c_str(),drive,dir,file,ext);
			_makepath(fullpath,drive,dir,file,"aiff");
			threadExec->cmd_snd.printf("%s\\sndfile-convert.exe \"%s\" \"%s\"",AppPath.c_str(),toFile,fullpath);
		}
		if (mc_flag) {
			threadExec->cmd_mc.printf("%s\\mconv.exe \"%s\"",AppPath.c_str(),toFile);
		}

		// ファイルの削除
		_splitpath(toPath.c_str(),drive,dir,file,ext);
		_makepath(fullpath,drive,dir,file,"wav");
		fileName = (AnsiString)fullpath;
		DeleteFile(fileName);
		_splitpath(toPath.c_str(),drive,dir,file,ext);
		_makepath(fullpath,drive,dir,file,"err");
		threadExec->errFile = (AnsiString)fullpath;
		fileName = (AnsiString)fullpath;
		DeleteFile(fileName);

		if (cmbEncoder->ItemIndex != 0) {
			if (cmbEncoder->ItemIndex == 1) {
				_splitpath(toPath.c_str(),drive,dir,file,ext);
				_makepath(fullpath,drive,dir,file,"flac");
				fileName = (AnsiString)fullpath;
				DeleteFile(fileName);
			}
			if (cmbEncoder->ItemIndex == 2) {
				_splitpath(toPath.c_str(),drive,dir,file,ext);
				_makepath(fullpath,drive,dir,file,"wv");
				fileName = (AnsiString)fullpath;
				DeleteFile(fileName);
			}
		}
		threadExec->Start();
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EndExec(DWORD exitCode)
{
	char fullpath[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char file[_MAX_FNAME];
	char ext[_MAX_EXT];
	char msg[256];
	AnsiString fileName;
	int endFlag;
	double mb;
	unsigned short ID;
	int error = true;
	AnsiString s;
	FILE *fp;
	ID = exitCode >> 16;
	exitCode &= 0xFFFF;

	_splitpath(toPath.c_str(),drive,dir,file,ext);
	_makepath(fullpath,drive,dir,file,"err");
	fp = fopen(fullpath,"r");
	if (fp == NULL) {
		error = false;
	}
	if (fp != NULL) {
		if (fgets(msg,256,fp) != NULL) {
			StatusBar1->SimpleText = (AnsiString)msg;
		}
		fclose(fp);
	}
	if (error == false) {
		if (!PathFileExists(destWavFile.c_str()) && chkOutMono->Checked == false) {
			error = true;
		}
	}

	if (error) {
		ListView->Items->Item[fileIndex]->ImageIndex = 3;
	} else {
		ListView->Items->Item[fileIndex]->ImageIndex = 2;
		if (chkDeleteFiles->Checked == true) {
			unlink((char *)ListView->Items->Item[fileIndex]->Data);
		}
	}
	DWORD nowTick;
	nowTick = GetTickCount();
	nowTick = nowTick - startTick;
	nowTick /= 1000;
	s.printf("%02d:%02d:%02d",nowTick / (60 * 60),((nowTick / 60) % 60),(nowTick % 60));
	ListView->Items->Item[fileIndex]->SubItems->Strings[2] = (UnicodeString)s;
	startTick = GetTickCount();
	fileIndex++;
	if (fileIndex < ListView->Items->Count && abort == 0) {
		ExecChildProcess(fileIndex);
	} else {
		lblProcess1->Caption = "";
		ProgressBar1->Position = 0;
		lblFilename->Caption = "";
		btnStart->Visible = true;
		btnAbort->Visible = false;
		btnStart->Enabled = true;
		btnExit->Enabled = true;
		DragAcceptFiles(ListView->Handle,true);
		Timer1->Enabled = false;
		cpSetting->Enabled = true;
		if (abort == 0 && chkShutdown->Checked == true) {
			Shutdown();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Shutdown()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tknPrvlgs;
	bool res;

	res = OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	if (res) {
		res = LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&(tknPrvlgs.Privileges[0].Luid));
		if (res){
			tknPrvlgs.PrivilegeCount = 1;
			tknPrvlgs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			AdjustTokenPrivileges(hToken,false,&tknPrvlgs,sizeof(TOKEN_PRIVILEGES),0,0);
			if (GetLastError() == ERROR_SUCCESS) {
				ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE,0);
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ListViewCustomDrawItem(TCustomListView *Sender,
	  TListItem *Item, TCustomDrawState State, bool &DefaultDraw)
{
	if ((Item->Index & 0x01) == 0) {
		Sender->Canvas->Brush->Color = 0x00FFC0C0;
	} else {
		Sender->Canvas->Brush->Color = clWhite;
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
	int sec,min,hour;
	AnsiString s;
	DWORD tm;

	tm = remSec;
	sec = tm % 60;
	tm /= 60;
	min = tm % 60;
	tm /= 60;
	hour = tm;
	if (hour < 999) {
		//s.printf("Remain:%d:%02d:%02d",hour,min,sec);
		//lblRemain->Caption = s;
		//Application->ProcessMessages();
		//if (remSec > 0) {
		//	remSec--;
		//}
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::UpdateProgress1(AnsiString status,int persent)
{
	lblProcess1->Caption = status + "                    ";

	if (persent >= 0) {
		ProgressBar1->Position = persent;
	}
	Application->ProcessMessages();

	//TrayIcon->BalloonHint.printf(L"%s:%d",(UnicodeString)status,persent);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::UpdateProgress2(AnsiString status,int persent)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::UpdateProgress_usr(AnsiString status)
{
	edtUsrStatus->Lines->Add((UnicodeString)status);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EndExec_usr(DWORD exitCode)
{
	btnUsrExec->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::UpdateTotalSize()
{
	int i;
	int ret;
	SOUNDFMT inFmt;
	DWORD nSample;
	double mb,sz;
	int bit[]={16,24,32,64};
	AnsiString s;
	mb = 0;
	for (i = 0;i < ListView->Items->Count;i++) {
		ret = PLG_InfoAudioData((char *)ListView->Items->Item[i]->Data,&inFmt,&nSample,NULL);
		if (ret == STATUS_SUCCESS) {
			// テンポラリファイルのサイズ計算
			sz = nSample;
			sz /= inFmt.sample;
			sz *= cmbSampRate->Text.ToInt();
			sz *= (bit[cmbBitwidth->ItemIndex] / 8);
			sz *= inFmt.channel;
			mb += sz;
		}
	}
	mb /= 1024;
	mb /= 1024;
	s.printf("%.2lfMB",mb);
	leTotalSize->Text = s;
	lblSamp->Text = "";
	lblWidth->Text = "";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbSampRateChange(TObject *Sender)
{
	UpdateTotalSize();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbBitwidthChange(TObject *Sender)
{
	UpdateTotalSize();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnClearClick(TObject *Sender)
{
	ListView->Items->Clear();
	btnClear->Enabled = false;
	btnDel->Enabled = false;
	btnUp->Enabled = false;
	btnDown->Enabled = false;
	btnInfo->Enabled = false;
	btnStart->Enabled = false;
	UpdateTotalSize();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnPresetClick(TObject *Sender)
{
	UnicodeString uStr;
	int index = cmbHfaPreset->ItemIndex;
	uStr.printf(L"%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",edtPresetTitle->Text,chkSig1Enb->Checked == true ? 1 : 0,UpDown3->Position,UpDown1->Position,chkSig2Enb->Checked == true ? 1 : 0,UpDown2->Position,0,UpDown5->Position,chkSig3Enb->Checked == true ? 1: 0,UpDown7->Position,UpDown8->Position);
	edtHfaParam->Lines->Strings[index] = uStr;
	cmbHfaPreset->Items->Strings[index] = edtPresetTitle->Text;
	cmbHfaPreset->ItemIndex = index;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnAbortClick(TObject *Sender)
{
	if (threadExec != NULL) {
		abort = 1;
		threadExec->Terminate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::lblOutputFolderRightButtonClick(TObject *Sender)
{
	WideString root = "";
	String dir = "";
	if (SelectDirectory("Upconv",root,dir)) {
		lblOutputFolder->Text = dir;
		rdoToFileSpecify->Enabled = true;
	}

}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtSuffixChange(TObject *Sender)
{
	if (edtSuffix->Text.Length() > 0) {
		edtSuffix->RightButton->Visible = true;
	} else {
		edtSuffix->RightButton->Visible = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtSuffixRightButtonClick(TObject *Sender)
{
	edtSuffix->Text = "";
}
//---------------------------------------------------------------------------







double eq[192000];
void __fastcall TForm1::btnEqSaveClick(TObject *Sender)
{
	double per[]={1/2.25,1/2.00,1/1.75,1/1.50,1/1.25,1.00,1.25,1.50,1.75,2.00,2.25};
	double p;
	int i,n;
	long e;
	FILE *ofp;
	UnicodeString AppPath = ExtractFilePath(Application->ExeName);
	AppPath += "eq.dat";

	for (i = 0;i < 192000;i++) {
		eq[i] = 1.00;
	}
	e = 10 - eq20->Position;
	for (i = 20;i < 40;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq40->Position;
	for (i = 40;i < 60;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq60->Position;
	for (i = 60;i < 80;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq80->Position;
	for (i = 80;i < 100;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq100->Position;
	for (i = 100;i < 250;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq250->Position;
	for (i = 250;i < 500;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq500->Position;
	for (i = 500;i < 750;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq750->Position;
	for (i = 750;i < 1000;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq1k->Position;
	for (i = 1000;i < 2000;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq2k->Position;
	for (i = 2000;i < 4000;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq4k->Position;
	for (i = 4000;i < 6000;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq6k->Position;
	for (i = 6000;i < 8000;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq8k->Position;
	for (i = 8000;i < 10000;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq10k->Position;
	for (i = 10000;i < 12000;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq12k->Position;
	for (i = 12000;i < 14000;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq14k->Position;
	for (i = 14000;i < 16000;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq16k->Position;
	for (i = 16000;i < 18000;i++) {
		eq[i] = per[e];
	}
	e = 10 - eq18k->Position;
	for (i = 18000;i < 20000;i++) {
		eq[i] = per[e];
	}

	// 30 サイズの移動平均
	for (i = 1;i + 30 < 192000;i++) {
		p = 0;
		for (n = 0;n < 30;n++) {
			p += eq[i + n];
		}
		if (p > 0) {
			p /= 30;
		}
		eq[i] = p;
	}

	ofp = fopen(((AnsiString)AppPath).c_str(),"w");
	if (ofp) {
		for (i = 0;i < 192000;i++) {
			fprintf(ofp,"%.6lf\n",eq[i]);
		}
		fclose(ofp);
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::BtnResetClick(TObject *Sender)
{
	eq20->Position = 5;
	eq40->Position = 5;
	eq60->Position = 5;
	eq80->Position = 5;
	eq100->Position = 5;
	eq250->Position = 5;
	eq500->Position = 5;
	eq750->Position = 5;
	eq1k->Position = 5;
	eq2k->Position = 5;
	eq4k->Position = 5;
	eq6k->Position = 5;
	eq8k->Position = 5;
	eq10k->Position = 5;
	eq12k->Position = 5;
	eq14k->Position = 5;
	eq16k->Position = 5;
	eq18k->Position = 5;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnInfoClick(TObject *Sender)
{
	TListItem *item;
	if (ListView->Selected != NULL) {
		item = ListView->Selected;
		if (item->Data) {
			Form4->filename = (AnsiString)(char *)item->Data;
			Form4->ShowModal();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtUsrCmdLeftButtonClick(TObject *Sender)
{
	edtUsrCmd->Text = "";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtUsrCmdRightButtonClick(TObject *Sender)
{
	if (OpenDialogProgram->Execute()) {
		edtUsrCmd->Text = OpenDialogProgram->FileName;
		edtUsrCmdLine->Text = "\"" + edtUsrCmd->Text + "\"";
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtUsrParamLeftButtonClick(TObject *Sender)
{
	edtUsrParam->Text = "";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtUsrInputLeftButtonClick(TObject *Sender)
{
	edtUsrInput->Text = "";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtUsrOutputLeftButtonClick(TObject *Sender)
{
	edtUsrOutput->Text = "";
}
//---------------------------------------------------------------------------


void __fastcall TForm1::edtUsrInputRightButtonClick(TObject *Sender)
{
	if (OpenDialogUsrInput->Execute()) {
		edtUsrInput->Text = OpenDialogUsrInput->FileName;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtUsrOutputRightButtonClick(TObject *Sender)
{
	if (SaveDialogUsrOutput->Execute()) {
		edtUsrOutput->Text = SaveDialogUsrOutput->FileName;
	}
}
//---------------------------------------------------------------------------



void __fastcall TForm1::btnUsrParamSetClick(TObject *Sender)
{
	edtUsrCmdLine->Text = edtUsrCmdLine->Text + " " + edtUsrParam->Text;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnUsrInputSetClick(TObject *Sender)
{
	edtUsrCmdLine->Text = edtUsrCmdLine->Text + " \"" + edtUsrInput->Text + "\"";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnUsrOutputSetClick(TObject *Sender)
{
	edtUsrCmdLine->Text = edtUsrCmdLine->Text + " \"" + edtUsrOutput->Text + "\"";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnUsrExecClick(TObject *Sender)
{
	if (edtUsrCmdLine->Text == "") {
		return;
	}

	threadExec = new Exec(true);
	threadExec->FreeOnTerminate = true;

	threadExec->cmd_d = "";
	threadExec->cmd_r = "";
	threadExec->cmd_u1 = "";
	threadExec->cmd_u2 = "";
	threadExec->cmd_w = "";
	threadExec->cmd_e = "";
	threadExec->cmd_usr = edtUsrCmdLine->Text;

	edtUsrStatus->Lines->Clear();
	btnUsrExec->Enabled = false;
	threadExec->Start();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtUsrCmdLineLeftButtonClick(TObject *Sender)
{
	edtUsrCmdLine->Text = "";
}
//---------------------------------------------------------------------------


void __fastcall TForm1::CategoryPanel7Expand(TObject *Sender)
{
	CategoryPanel1->Collapsed = true;
	CategoryPanel2->Collapsed = true;
	CategoryPanel3->Collapsed = true;
	CategoryPanel9->Collapsed = true;
	CategoryPanelGroup1->VertScrollBar->Position = 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CategoryPanel1Expand(TObject *Sender)
{
	CategoryPanel2->Collapsed = true;
	CategoryPanel3->Collapsed = true;
	CategoryPanel7->Collapsed = true;
	CategoryPanel9->Collapsed = true;
	CategoryPanelGroup1->VertScrollBar->Position = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel2Expand(TObject *Sender)
{
	CategoryPanel1->Collapsed = true;
	CategoryPanel3->Collapsed = true;
	CategoryPanel7->Collapsed = true;
	CategoryPanel9->Collapsed = true;
	CategoryPanelGroup1->VertScrollBar->Position = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel3Expand(TObject *Sender)
{
	CategoryPanel1->Collapsed = true;
	CategoryPanel2->Collapsed = true;
	CategoryPanel7->Collapsed = true;
	CategoryPanel9->Collapsed = true;
	CategoryPanelGroup1->VertScrollBar->Position = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel9Expand(TObject *Sender)
{
	CategoryPanel1->Collapsed = true;
	CategoryPanel2->Collapsed = true;
	CategoryPanel3->Collapsed = true;
	CategoryPanel7->Collapsed = true;
	CategoryPanelGroup1->VertScrollBar->Position = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cpSettingCollapse(TObject *Sender)
{
	cpSetting->Collapsed = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel8Collapse(TObject *Sender)
{
	CategoryPanel8->Collapsed = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbParamChange(TObject *Sender)
{
	TIniFile *iniFile;
	int param1,param2,param3,param4,param5;
	TRegistry	*xReg;
	AnsiString appData;
#if 0
	xReg = new TRegistry();
	xReg->OpenKey("\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",true);
	appData = xReg->ReadString("appData");
	xReg->CloseKey();
	delete xReg;
	appData = appData + "\\upconv";
	appData = appData + "\\upconvfe.ini";
#endif
	appData = ExtractFilePath(Application->ExeName);
	appData = appData + "\\upconvfe.ini";

	iniFile = new TIniFile(appData);
	param1 = iniFile->ReadInteger("Convert","hfc_param1",0);
	param2 = iniFile->ReadInteger("Convert","hfc_param2",30000);
	param3 = iniFile->ReadInteger("Convert","hfc_param3",24000);
	param4 = iniFile->ReadInteger("Convert","hfc_param4",16000);
	param5 = iniFile->ReadInteger("Convert","hfc_param5",12000);
	switch (cmbParam->ItemIndex) {
		case 0:edtHFC->Text = "";break;
		case 1:edtHFC->Text = param2;break;
		case 2:edtHFC->Text = param3;break;
		case 3:edtHFC->Text = param4;break;
		case 4:edtHFC->Text = param5;break;
	}
}
//---------------------------------------------------------------------------



void __fastcall TForm1::btnIOAdjustClick(TObject *Sender)
{
	MEMORYSTATUSEX mse;
	SYSTEM_INFO sysinfo;
	__int64 avail,use,val;
	int i;
	int pa32;
	memset(&mse,0,sizeof (MEMORYSTATUSEX));
	mse.dwLength = sizeof (MEMORYSTATUSEX);

	GetNativeSystemInfo(&sysinfo);

	if (GlobalMemoryStatusEx(&mse)) {
		avail = mse.ullAvailPhys;
		use = 0;
		use +=	200000000;	// upconv.exe (1)
		use +=	200000000;	// upconv.exe (2)
		use +=	800000000;	// system and free area
		pa32 = 1;
		if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
			sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
			pa32 = 0;
		}

		if (avail < use + (cmbIO->Items->Strings[0].ToInt() * 2)) {
			cmbIO->ItemIndex = 0;
		} else {
			avail -= use;
			if (avail > 1 * 1000 * 1000 * 1000 && pa32 == 1) {
				avail = 1 * 1000 * 1000 * 1000;
			}
			for (i = 0;i < cmbIO->Items->Count;i++) {
				val = cmbIO->Items->Strings[i].ToInt();
				val *= 1000 * 1000 * 2;
				if (avail >= val) {
					cmbIO->ItemIndex = i;
				}
			}
		}
	}
}
//---------------------------------------------------------------------------


