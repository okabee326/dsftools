//---------------------------------------------------------------------------

#ifndef Unit4H
#define Unit4H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
//---------------------------------------------------------------------------
class TForm4 : public TForm
{
__published:	// IDE �Ǘ��̃R���|�[�l���g
	TTreeView *tvTree;
	TValueListEditor *tvDesc;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall tvTreeClick(TObject *Sender);
private:	// ���[�U�[�錾
public:		// ���[�U�[�錾
	__fastcall TForm4(TComponent* Owner);
	AnsiString filename;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm4 *Form4;
//---------------------------------------------------------------------------
#endif
