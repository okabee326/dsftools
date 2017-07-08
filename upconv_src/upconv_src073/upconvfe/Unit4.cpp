//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit4.h"
#include "PLG_AudioIO.h"
#include <stdio.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm4 *Form4;
//---------------------------------------------------------------------------
__fastcall TForm4::TForm4(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm4::FormShow(TObject *Sender)
{
	int retValue;
	char *info;
	char *p,*cr;
	TTreeNode *parent;
	TTreeNode *n1;
	TTreeNode *n2;
	TTreeNode *n3;
	TTreeNode *n4;
	TTreeNode *n5;
	TTreeNode *n;
	TStringList *tsList;
	int level;
	parent = n1 = n2 = n3 = n4 = n5 = n = NULL;
	level  = 0;

	tvTree->Items->Clear();
	while (tvDesc->RowCount > 2) {
		tvDesc->DeleteRow(2);
	}


	info = (char *)HeapAlloc(GetProcessHeap(),0,4 * 1024 * 1024L);
	if (info) {
		if (strlen(filename.c_str()) > 0) {
			memset(info,0,4 * 1024 * 1024L);
			retValue = PLG_GetChunkInfo(filename.c_str(),info,4 * 1024 * 1024L);
			if (retValue == STATUS_SUCCESS) {
				p = info;
				do {
					cr = strchr(p,'\n');
					if (cr) {
						*cr = '\0';
					}
					if (p == NULL || strlen(p) == 0) {
						break;
					}
					if (p[0] == 'A') {
						switch (level) {
							case 0:
								n = tvTree->Items->Add(NULL,(UnicodeString)(p + 1));
								parent = n;
								tsList = new TStringList;
								n->Data = tsList;
								break;
							case 1:
								n = tvTree->Items->AddChild(parent,(UnicodeString)(p + 1));
								n1 = n;
								tsList = new TStringList;
								n->Data = tsList;
								break;
							case 2:
								n = tvTree->Items->AddChild(n1,(UnicodeString)(p + 1));
								n2 = n;
								tsList = new TStringList;
								n->Data = tsList;
								break;
							case 3:
								n = tvTree->Items->AddChild(n2,(UnicodeString)(p + 1));
								n3 = n;
								tsList = new TStringList;
								n->Data = tsList;
								break;
							case 4:
								n = tvTree->Items->AddChild(n3,(UnicodeString)(p + 1));
								n4 = n;
								tsList = new TStringList;
								n->Data = tsList;
								break;
						}
					} else if (p[0] == 'D') {
						level++;
					} else if (p[0] == 'I') {
						switch (level) {
							case 0:
								tsList = (TStringList*)parent->Data;
								tsList->Add((UnicodeString)(p + 1));
								break;
							case 1:
								tsList = (TStringList*)n1->Data;
								tsList->Add((UnicodeString)(p + 1));
								break;
							case 2:
								tsList = (TStringList*)n2->Data;
								tsList->Add((UnicodeString)(p + 1));
								break;
							case 3:
								tsList = (TStringList*)n3->Data;
								tsList->Add((UnicodeString)(p + 1));
								break;
							case 4:
								tsList = (TStringList*)n4->Data;
								tsList->Add((UnicodeString)(p + 1));
								break;
						}
					}
					if (cr) {
						p = cr + 1;
					}
				} while (cr != NULL);
                tvTree->FullExpand();
			}
		}
		HeapFree(GetProcessHeap(),0,info);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm4::tvTreeClick(TObject *Sender)
{
	TStringList *tsList;
	TTreeNode *Node;
	UnicodeString s;
	UnicodeString k,v;
	int i,pos,n;

    tvDesc->Visible = true;
	while (tvDesc->RowCount > 2) {
		tvDesc->DeleteRow(2);
	}
	Node = tvTree->Selected;
	if (Node) {
		tsList = (TStringList *)Node->Data;
		for (i = 0;i < tsList->Count;i++) {
			s = tsList->Strings[i];
			pos = s.LastDelimiter("\t");
			n = s.Length();
			k = s.SubString(0,pos - 1);
			v = s.SubString(pos + 1,n);
			if (k == "Offset") {
				tvDesc->Values["Offset"] = v;
			} else if (k == "Size") {
				tvDesc->Values["Size"] = v;
			} else {
				tvDesc->InsertRow(k,v,true);
			}
			tvDesc->ItemProps[i]->ReadOnly = true;
		}
	}
}
//---------------------------------------------------------------------------


