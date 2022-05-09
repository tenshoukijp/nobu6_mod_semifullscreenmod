#include <windows.h>

#include "SemiFullScreenMod.h"
#include "BlackBackGroundMod.h"


ParamIniFile param = {0,0,-1};




void WINAPI On_初期化(HANDLE hCallDLLHandle) {
	ReadSelfIniFile();

	if ( param.BackColorRGB >= 0 ) {
		BlackBackGroundMod::BlackForm::mform = gcnew BlackBackGroundMod::BlackForm(); 
		BlackBackGroundMod::BlackForm::SetBackColor( param.BackColorRGB );
	}
}

void WINAPI On_末期化() {
	BlackBackGroundMod::BlackForm::mform->Close();
}

void WINAPI On_解像度変更直後() {
	SemiFullScreenModExecute();
}

// 古いバージョンのものへの対応
void WINAPI On_メインゲーム開始() {
	SemiFullScreenModExecute();
}

// 全体の実行。自分のdllの名前と一致した.iniファイルの情報に基づいて、全画面ぽく切り替える
void SemiFullScreenModExecute() {
	static BOOL isScreenChanged = FALSE;
	if ( isScreenChanged ) { return; }

	int iDeskW = System::Windows::Forms::Screen::PrimaryScreen->Bounds.Width;
	int iDeskH = System::Windows::Forms::Screen::PrimaryScreen->Bounds.Height;
	// 有効な数値が入っていて、現在の解像度とは異なること
	if ( param.ScreenW > 0 && param.ScreenH > 0 && 
		!( param.ScreenW == iDeskW && param.ScreenH == iDeskH) ) {
		// 画面サイズ変更
		TenshouWindowsResize(&param);
	}

	isScreenChanged = TRUE;
}

void ReadSelfIniFile() {
	// 自分自身(このdll)のフルパス
	String^ selfFullPath = System::Reflection::Assembly::GetExecutingAssembly()->Location;
	// フルパスからファイル名(拡張子除外)
	String^ strBaseName = System::IO::Path::GetFileNameWithoutExtension(selfFullPath);
	String^ strIniName = strBaseName + ".ini"; // iniの名前を決定

	if ( System::IO::File::Exists( strBaseName + ".dll" ) ) {
		System::Windows::Forms::MessageBox::Show( "古いバージョンの " + strBaseName + ".dll が残っています。\n２つが混在すると確実に誤作動しますので、古い方を削除してください。");
	}

	// iniファイルを読み込み設定値を読み込む
	ImportSelfIniFile(strIniName, &param);

}

// .iniファイルから読み込んで、パラメータとして格納する。
void ImportSelfIniFile(String^ strIniFileName, ParamIniFile *param) {

	try {
		// StreamReader の新しいインスタンス
		System::IO::StreamReader^ cReader = gcnew System::IO::StreamReader(strIniFileName, System::Text::Encoding::Default);

		// 読み込みできる文字がなくなるまで繰り返す
		while (cReader->Peek() >= 0) {
			// ファイルを 1 行ずつ読み込む
			String^ stBuffer = cReader->ReadLine();
	       
			// マッチする方。本来Luaを組み込むべきだが、この程度に組み込むと大仰なので。どうせ数値しか弄くらないだろうから表記だけ似せて簡易パーサーで済ませる。
			Regex^ regW = gcnew Regex("^\\s*cmd\\[\"environment_semi_fullscreen_w\"\\]\\s*=\\s*\\d+");
			Regex^ regH = gcnew Regex("^\\s*cmd\\[\"environment_semi_fullscreen_h\"\\]\\s*=\\s*\\d+");
			Regex^ regB = gcnew Regex("^\\s*cmd\\[\"environment_semi_fullscreen_back_color\"\\]\\s*=\\s*0x[0-9a-fA-F]");

			if ( regW->Match(stBuffer)->Success ) {
				Regex^ regV = gcnew Regex("(\\d+)");
				Match^ m = regV->Match(stBuffer);
				param->ScreenW = System::Int32::Parse(m->Groups[1]->Value);
			}
			else if ( regH->Match(stBuffer)->Success ) {
				Regex^ regV = gcnew Regex("(\\d+)");
				Match^ m = regV->Match(stBuffer);
				param->ScreenH = System::Int32::Parse(m->Groups[1]->Value);
			}
			else if ( regB->Match(stBuffer)->Success ) {
				Regex^ regV = gcnew Regex("0x([0-9a-fA-F]+)");
				Match^ m = regV->Match(stBuffer);
				param->BackColorRGB = System::Int32::Parse(m->Groups[1]->Value, System::Globalization::NumberStyles::AllowHexSpecifier);
			}
		}

		// 破棄
		cReader->Close();
	} catch(Exception^) {
		System::Windows::Forms::MessageBox::Show(".iniファイルの読み込みに失敗しました。");
	}
}

// 全画面ぽく切り替える。paramには縦・横のサイズが入っている。
void TenshouWindowsResize(ParamIniFile *param) {

	int iScreenWidth  = param->ScreenW;
	int iScreenHeight = param->ScreenH;

	HWND hwnd = FindWindow("Tenshouki95", NULL);

	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	// フルスクリーン用
	static DEVMODE devmode; 

	// メモリクリア
	ZeroMemory( &devmode, sizeof(devmode) );

	//現在の画面情報を取得
	BOOL res = EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devmode);

	// 解像度は指定のものに
	// devmode.dmSize = sizeof(devmode);
	devmode.dmPelsWidth  = iScreenWidth;
	devmode.dmPelsHeight = iScreenHeight;
	// devmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

	if ( BlackBackGroundMod::BlackForm::mform ) {
		// 背景をリサイズ(
		BlackBackGroundMod::BlackForm::mform->Height = iScreenHeight;
		BlackBackGroundMod::BlackForm::mform->Width = iScreenWidth;
	}

	// このアプリ実行中解像度切替。
	ChangeDisplaySettings( &devmode, CDS_FULLSCREEN );

	// アプリにはタイトルバーの幅や境界線幅（いわゆるボーダー）があるのでそのピクセル数を求める
	int iBorderX = GetSystemMetrics(SM_CXSIZEFRAME); // 境界線幅X方向
	int iBorderY = GetSystemMetrics(SM_CYSIZEFRAME); // 境界線幅Y方向
	int iHeightCaption = GetSystemMetrics(SM_CYCAPTION);   // タイトルバーの高さ

	// 左座標は、指定の幅から真ん中に配置
	int iLeft = (iScreenWidth-clientRect.right)/2;
	// 上座標の真ん中は求めておくけとまぁ使わないかな。
	int iTop  = (iScreenHeight-clientRect.bottom)/2;

	// 携帯用解像度などもあるのでTOPは常に一番上で、左右は真ん中。
	MoveWindow(hwnd, iLeft-iBorderX, iTop, 0, 0, TRUE);

}

