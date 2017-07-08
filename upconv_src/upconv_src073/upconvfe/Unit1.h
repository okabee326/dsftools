//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <Mask.hpp>
#include <Dialogs.hpp>
#include <FileCtrl.hpp>
#include <XPMan.hpp>
#include <CategoryButtons.hpp>
#include "Unit2.h"
#include <ImgList.hpp>
#include <shellapi.h>
#include <dir.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ExtCtrls.hpp>
#include <IniFiles.hpp>
#include <shlwapi.h>
#include <ExtDlgs.hpp>
#include <AppEvnts.hpp>
#include <Registry.hpp>

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE 管理のコンポーネント
	TStatusBar *StatusBar1;
	TOpenDialog *OpenDialog;
	TImageList *ImageList;
	TTimer *Timer1;
	TImageList *ImageList2;
	TBalloonHint *BalloonHint;
	TOpenDialog *OpenDialog2;
	TPageControl *PageControl1;
	TTabSheet *TabSheet3;
	TCategoryPanelGroup *CategoryPanelGroup2;
	TCategoryPanel *cpSetting;
	TTabSheet *TabSheet2;
	TCategoryPanelGroup *CategoryPanelGroup1;
	TCategoryPanel *CategoryPanel3;
	TCategoryPanel *CategoryPanel2;
	TBevel *Bevel3;
	TLabel *Label17;
	TLabel *Label4;
	TLabel *Label11;
	TBevel *Bevel7;
	TLabel *Label18;
	TLabel *Label1;
	TCheckBox *chkDeEmphasis;
	TRadioButton *rdoDEType1;
	TRadioButton *rdoDEType2;
	TEdit *edtNR;
	TEdit *edtNRLV;
	TUpDown *UpDown6;
	TEdit *edtLPF;
	TCategoryPanel *CategoryPanel1;
	TLabel *Label15;
	TCheckBox *chkOverSampling;
	TCheckBox *chkLfa;
	TCheckBox *chkHdc;
	TCheckBox *chkHfcAuto;
	TTabSheet *TabSheet1;
	TCategoryPanelGroup *CategoryPanelGroup3;
	TCategoryPanel *CategoryPanel4;
	TTrackBar *eq100;
	TTrackBar *eq250;
	TTrackBar *eq500;
	TTrackBar *eq2k;
	TTrackBar *eq4k;
	TTrackBar *eq6k;
	TTrackBar *eq8k;
	TTrackBar *eq10k;
	TTrackBar *eq12k;
	TTrackBar *eq14k;
	TTrackBar *eq16k;
	TTrackBar *eq18k;
	TLabel *Label35;
	TLabel *Label36;
	TLabel *Label37;
	TLabel *Label38;
	TLabel *Label39;
	TLabel *Label40;
	TLabel *Label41;
	TLabel *Label42;
	TLabel *Label43;
	TLabel *Label44;
	TLabel *Label45;
	TLabel *Label46;
	TBevel *Bevel20;
	TBitBtn *btnEqSave;
	TLabel *Label33;
	TBitBtn *BtnReset;
	TTabSheet *TabSheet4;
	TCategoryPanelGroup *CategoryPanelGroup4;
	TCategoryPanel *CategoryPanel6;
	TButtonedEdit *edtUsrCmd;
	TLabel *Label49;
	TBevel *Bevel21;
	TLabel *Label50;
	TButtonedEdit *edtUsrParam;
	TLabel *Label51;
	TLabel *Label52;
	TButtonedEdit *edtUsrInput;
	TBitBtn *btnUsrInputSet;
	TLabel *Label53;
	TButtonedEdit *edtUsrOutput;
	TBitBtn *btnUsrOutputSet;
	TLabel *Label54;
	TButtonedEdit *edtUsrCmdLine;
	TMemo *edtUsrStatus;
	TBevel *Bevel22;
	TBitBtn *btnUsrExec;
	TOpenDialog *OpenDialogProgram;
	TBitBtn *btnUsrParamSet;
	TLabel *Label55;
	TSaveDialog *SaveDialogUsrOutput;
	TOpenDialog *OpenDialogUsrInput;
	TCheckBox *chkOs32x;
	TCategoryPanel *CategoryPanel7;
	TLabel *Label22;
	TComboBox *cmbHfaPreset;
	TLabel *Label13;
	TEdit *edtPresetTitle;
	TButton *btnPreset;
	TBevel *Bevel9;
	TLabel *Label23;
	TCheckBox *chkSig1Enb;
	TLabel *Label7;
	TEdit *edtAvgLineNx;
	TUpDown *UpDown3;
	TLabel *Label5;
	TEdit *edtSig1;
	TUpDown *UpDown1;
	TBevel *Bevel11;
	TLabel *Label25;
	TCheckBox *chkSig2Enb;
	TLabel *Label6;
	TEdit *edtSig2;
	TUpDown *UpDown2;
	TLabel *Label27;
	TLabel *Label9;
	TEdit *edtNB;
	TUpDown *UpDown5;
	TLabel *Label10;
	TBevel *Bevel14;
	TCheckBox *chkHfaFast;
	TMemo *edtHfaParam;
	TLabel *Label34;
	TCheckBox *chkCutLowData;
	TLabel *Label48;
	TComboBox *cmbCutDither;
	TCheckBox *chkSMLowData;
	TCategoryPanel *CategoryPanel8;
	TLabel *lblFilename;
	TProgressBar *ProgressBar1;
	TBitBtn *btnAbort;
	TBitBtn *btnExit;
	TBitBtn *btnStart;
	TListView *ListView;
	TBitBtn *btnAdd;
	TBitBtn *btnDel;
	TBitBtn *btnClear;
	TButton *btnUp;
	TButton *btnDown;
	TBitBtn *btnInfo;
	TLabel *Label8;
	TEdit *leTotalSize;
	TLabel *lblProcess1;
	TLabel *Label28;
	TLabel *Label29;
	TGroupBox *GroupBox2;
	TComboBox *cmbSampRate;
	TGroupBox *GroupBox3;
	TComboBox *cmbBitwidth;
	TGroupBox *GroupBox5;
	TEdit *edtHFC;
	TGroupBox *GroupBox6;
	TEdit *edtLFC;
	TGroupBox *GroupBox4;
	TComboBox *cmbHFA;
	TBevel *Bevel1;
	TBevel *Bevel17;
	TLabel *Label30;
	TRadioButton *rdoToFileSrc;
	TRadioButton *rdoToFileSpecify;
	TBevel *shp2;
	TButtonedEdit *edtSuffix;
	TButtonedEdit *lblOutputFolder;
	TBevel *Bevel12;
	TLabel *Label2;
	TBevel *shp1;
	TBevel *Bevel6;
	TCheckBox *chkNormalize;
	TCheckBox *chkAdjBE;
	TGroupBox *GroupBox9;
	TComboBox *cmbParam;
	TCheckBox *chkAdjFreq;
	TCheckBox *chkEQ;
	TBevel *Bevel5;
	TLabel *Label12;
	TLabel *Label32;
	TBevel *Bevel16;
	TBevel *Bevel10;
	TBevel *Bevel8;
	TBevel *Bevel13;
	TBevel *Bevel19;
	TLabel *Label16;
	TLabel *Label19;
	TEdit *lblSamp;
	TEdit *lblWidth;
	TBevel *Bevel2;
	TLabel *Label26;
	TCheckBox *chkSig3Enb;
	TEdit *edtSig3;
	TUpDown *UpDown7;
	TLabel *Label31;
	TTabSheet *TabSheet5;
	TBevel *Bevel15;
	TLabel *Label57;
	TComboBox *cmbBOM;
	TLabel *Label3;
	TEdit *edtDitherLevel;
	TUpDown *UpDown4;
	TComboBox *cmbOutFormat;
	TBevel *Bevel24;
	TLabel *Label58;
	TComboBox *cmbEncoder;
	TEdit *edtFLACOption;
	TLabel *Label20;
	TLabel *Label21;
	TEdit *edtWavPackOption;
	TCategoryPanelGroup *CategoryPanelGroup5;
	TCategoryPanel *CategoryPanel5;
	TLabel *Label24;
	TBevel *Bevel4;
	TCheckBox *chkDeleteFiles;
	TCheckBox *chkShutdown;
	TLabel *Label14;
	TBevel *Bevel18;
	TComboBox *cmbThread;
	TCategoryPanel *CategoryPanel9;
	TComboBox *cmbDSF;
	TBevel *Bevel23;
	TLabel *Label56;
	TLabel *Label59;
	TBevel *Bevel25;
	TComboBox *cmbIO;
	TButton *btnIOAdjust;
	TCheckBox *chkPostABE;
	TTabSheet *TabSheet6;
	TCategoryPanelGroup *CategoryPanelGroup6;
	TCategoryPanel *CategoryPanel10;
	TBevel *Bevel26;
	TLabel *Label60;
	TBevel *Bevel27;
	TLabel *Label61;
	TCheckBox *chkGenCenter;
	TCheckBox *chkGenSurround;
	TLabel *Label62;
	TBevel *Bevel28;
	TCheckBox *chkGenLFE;
	TCheckBox *chkOutMono;
	TBevel *Bevel29;
	TLabel *Label63;
	TCheckBox *chkechoeffect;
	TCheckBox *chkDownMix;
	TComboBox *cmbEffect;
	TLabel *Label64;
	TBevel *Bevel30;
	TTrackBar *eq20;
	TLabel *Label47;
	TTrackBar *eq40;
	TLabel *Label65;
	TTrackBar *eq60;
	TLabel *Label66;
	TTrackBar *eq80;
	TLabel *Label67;
	TLabel *Label68;
	TTrackBar *eq750;
	TLabel *Label69;
	TTrackBar *eq1k;
	TLabel *Label70;
	TCheckBox *chkWideAna;
	TLabel *Label71;
	TEdit *edtDiffmin;
	TUpDown *UpDown8;
	void __fastcall btnAddClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall btnExitClick(TObject *Sender);
	void __fastcall ListViewSelectItem(TObject *Sender, TListItem *Item,
		  bool Selected);
	void __fastcall btnDelClick(TObject *Sender);
	void __fastcall ListViewChange(TObject *Sender, TListItem *Item,
		  TItemChange Change);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall rdoToFileSrcClick(TObject *Sender);
	void __fastcall rdoToFileSpecifyClick(TObject *Sender);
	void __fastcall ListViewCustomDrawItem(TCustomListView *Sender,
		  TListItem *Item, TCustomDrawState State, bool &DefaultDraw);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall cmbSampRateChange(TObject *Sender);
	void __fastcall cmbBitwidthChange(TObject *Sender);
	void __fastcall btnClearClick(TObject *Sender);
	void __fastcall cmbHfaPresetChange(TObject *Sender);
	void __fastcall btnPresetClick(TObject *Sender);
	void __fastcall btnAbortClick(TObject *Sender);
	void __fastcall lblOutputFolderRightButtonClick(TObject *Sender);
	void __fastcall edtSuffixChange(TObject *Sender);
	void __fastcall edtSuffixRightButtonClick(TObject *Sender);
	void __fastcall btnUpClick(TObject *Sender);
	void __fastcall btnDownClick(TObject *Sender);
	void __fastcall btnEqSaveClick(TObject *Sender);
	void __fastcall BtnResetClick(TObject *Sender);
	void __fastcall btnInfoClick(TObject *Sender);
	void __fastcall edtUsrCmdLeftButtonClick(TObject *Sender);
	void __fastcall edtUsrCmdRightButtonClick(TObject *Sender);
	void __fastcall edtUsrParamLeftButtonClick(TObject *Sender);
	void __fastcall edtUsrInputLeftButtonClick(TObject *Sender);
	void __fastcall edtUsrOutputLeftButtonClick(TObject *Sender);
	void __fastcall edtUsrInputRightButtonClick(TObject *Sender);
	void __fastcall edtUsrOutputRightButtonClick(TObject *Sender);
	void __fastcall btnUsrParamSetClick(TObject *Sender);
	void __fastcall btnUsrInputSetClick(TObject *Sender);
	void __fastcall btnUsrOutputSetClick(TObject *Sender);
	void __fastcall btnUsrExecClick(TObject *Sender);
	void __fastcall edtUsrCmdLineLeftButtonClick(TObject *Sender);
	void __fastcall CategoryPanel7Expand(TObject *Sender);
	void __fastcall CategoryPanel1Expand(TObject *Sender);
	void __fastcall CategoryPanel2Expand(TObject *Sender);
	void __fastcall CategoryPanel3Expand(TObject *Sender);
	void __fastcall CategoryPanel9Expand(TObject *Sender);
	void __fastcall cpSettingCollapse(TObject *Sender);
	void __fastcall CategoryPanel8Collapse(TObject *Sender);
	void __fastcall cmbParamChange(TObject *Sender);
	void __fastcall btnIOAdjustClick(TObject *Sender);

private:	// ユーザー宣言
	TWndMethod	orgWinProc;
	Exec* threadExec;
	AnsiString toPath;
	AnsiString destWavFile;
	unsigned short doneFlags;
	DWORD remSec;
	DWORD totalByte;
	DWORD startTick;
	int fileIndex;
	int	abort;
	int selectListIndex;
	void __fastcall ListViewWinProc(TMessage& Msg);
	void __fastcall ExecChildProcess(int index);
	void __fastcall AddToFileList(AnsiString filename);
	void __fastcall UpdateTotalSize();
	void __fastcall Shutdown();
public:		// ユーザー宣言
	__fastcall TForm1(TComponent* Owner);
	void __fastcall EndExec(DWORD exitCode);
	void __fastcall UpdateProgress1(AnsiString status,int persent);
	void __fastcall UpdateProgress2(AnsiString status,int persent);
	void __fastcall EndExec_usr(DWORD exitCode);
	void __fastcall UpdateProgress_usr(AnsiString status);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
