// ADBSDialog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "AndroidScreenMonitor.h"
#include "ADBSDialog.h"


// CADBSDialog ダイアログ

IMPLEMENT_DYNAMIC(CADBSDialog, CDialog)

CADBSDialog::CADBSDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CADBSDialog::IDD, pParent)
{

}

CADBSDialog::~CADBSDialog()
{
}

void CADBSDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CADBSDialog, CDialog)
END_MESSAGE_MAP()


// CADBSDialog メッセージ ハンドラ
