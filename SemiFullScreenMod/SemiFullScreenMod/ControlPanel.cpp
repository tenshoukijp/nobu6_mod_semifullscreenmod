#include <windows.h>
#include <cpl.h>
#include "resource.h"

#include "DisplaySelectionForm.h"


using namespace System;
using namespace System::Windows::Forms;

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

LONG APIENTRY CPlApplet(HWND hwndCPl, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	switch (uMsg) {

	case CPL_INIT:
		return 1;
	/*
		CPL_INITは、CPlAppletに最初に送られてくるメッセージです。
		一般には、このメッセージでcplファイル内で扱うことになるであろうデータを 初期化したりするのですが、
		今回は大規模のプログラムを作るわけではないので、 特に何も処理をしていません。
		しかし、このメッセージでは初期化に問題がなければ0以外の値を返すべきなので、
		きちんとメッセージを捕らえ、0以外の値を返すようにします。 
	*/

	case CPL_GETCOUNT:
		return 1;
	/*
		CPL_GETCOUNTは、サポートするダイアログの数を返します。
		一般には、ダイアログを1つ表示するだけでしょうから1を返すことになるでしょう。
		プロパティシートを表示するようなときは、ダイアログが複数になりますが、
		そのような場合でも1を返してください。 
	*/

	case CPL_INQUIRE:
		{
		LPCPLINFO lpcpl = (LPCPLINFO)lParam2;

		lpcpl->idIcon = IDI_ICON;
		lpcpl->idName = IDS_STRING102;
		lpcpl->idInfo = IDS_STRING103;
		lpcpl->lData  = (LPARAM)GetModuleHandle(TEXT("SemiFullScreenMod.cpl"));

		break;
		}
	/*
		CPL_INQUIREは、cplファイルをロードしたプロセスが、
		そのcplファイルに含まれているアイコンや文字列を取得するために送られます。
		これらの情報はCPLINFO構造体で表され、lParam2はそのアドレスを指しているので、
		LPCPLINFO型として受け取り、メンバを初期化します。
		
		idIconはアイコンの識別子、idNameはアイコンの名前を示す文字列の識別子、
		idInfoにはアイコンの説明文を示す文字列の識別子を指定します。
		
		lDataは初期化する必要のないメンバですが、
		このメンバはCPL_DBLCLKのlParam2に相当することになるため、CPL_DBLCLKはそれを利用しています。
	*/
	
	case CPL_DBLCLK:
		{
		OutputDebugString("CPL_DBLCLK");
		HINSTANCE hinst = (HINSTANCE)lParam2;

		// DialogBox(hinst, MAKEINTRESOURCE(IDD_DIALOG1), hwndCPl, DialogProc);
		DisplaySelectionForm^ form = gcnew DisplaySelectionForm();
		form->ShowDialog();
		
		break;
		}
	/*
		CPL_DBLCLKは、ユーザーがアイコンをクリックしたときに送られます。
		このlParam2には、CPL_INQUIREで設定したlDataと同じ値が返ります。
		つまり、GetModuleHandleの戻り値と同じです。
		もちろん、CPL_DBLCLKでGetModuleHandleを呼び出しても問題はありません。
		モジュールのハンドルはDialogBoxの第1引数が必要としますが、
		ここで指定するハンドルは必ずmycpl.cplのハンドルでなければなりません。
		GetModuleHandleの第1引数にNULLを指定すると、
		cplファイルをロードしたプロセスのEXEモジュールのアドレスが返ることになるため、
		この点は十分に注意しておくべきです。
		DialogBoxは、モーダルダイアログボックスを作成するため、
		ダイアログプロシージャがダイアログを破棄するまで制御を返しません。
		第4引数に指定したDlgProc(ダイアログプロシージャ)は、以下のようになっています。 
	*/

	case CPL_STOP:
		break;
	
	case CPL_EXIT:
		break;

	default:
		break;

	}

	return 0;
}



void WINAPI OnInitialize(HMODULE hmodule) {
	// MessageBox(NULL, L"OK", L"OK", NULL);
}