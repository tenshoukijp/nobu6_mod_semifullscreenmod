#pragma once

#include <windows.h>
#include <string>
#include <map>

#include "SemiFullScreenMod.h"


using namespace std;

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;

map<std::string, POINT> map_resolution;



ref class DisplaySelectionForm : public Form {
	// ラベル
	Label^ lblResolution;
	// 解像度のリストボックス
	ListBox^ lboxResolution;

	// 選択色の色パネル
	Panel^ pnlBackColor;
	// 色ダイアログ表示するためのボタン
	Button^ btnSelectBackColor;

	// 色ダイアログ
	System::Windows::Forms::ColorDialog^ colorDialog;
	// 全て設定のボタン
	Button^ btnAllSetting;
	

public:
	DisplaySelectionForm() {
		this->Height = 280;
		this->Width = 300;
		this->MaximizeBox = false;

		this->FormBorderStyle = ::FormBorderStyle::FixedSingle;
		this->ShowInTaskbar = false;

		SetFormIcon();

		lblResolution = gcnew Label();
		lblResolution->Text = "ゲーム時に以下の画面解像度に切替:";
		lblResolution->Left = 10;
		lblResolution->Top = 10;
		lblResolution->Width  = this->ClientSize.Width - 20;
		lblResolution->Height = 15;

		lboxResolution = gcnew ListBox();
		lboxResolution->Left = 10;
		lboxResolution->Top = 30;
		lboxResolution->Height = 147;
		lboxResolution->Width  = 120;
		lboxResolution->Font = gcnew System::Drawing::Font("MS Gothic", 10 );

		pnlBackColor = gcnew Panel();
		pnlBackColor->BackColor = System::Drawing::Color::Black;
		pnlBackColor->Top = 30;
		pnlBackColor->Left = 140;
		pnlBackColor->Width = 130;
		pnlBackColor->Height = 100;
		pnlBackColor->Click += gcnew EventHandler(this, &DisplaySelectionForm::btnSelectBackColor_Click);

		btnSelectBackColor = gcnew Button();
		btnSelectBackColor->Text = "背景色を選ぶ";
		btnSelectBackColor->Top = pnlBackColor->Bottom + 10;
		btnSelectBackColor->Left = pnlBackColor->Left;
		btnSelectBackColor->Height = 30;
		btnSelectBackColor->Width = pnlBackColor->Width;

		btnSelectBackColor->Click += gcnew EventHandler(this, &DisplaySelectionForm::btnSelectBackColor_Click);

		colorDialog = gcnew System::Windows::Forms::ColorDialog();

		btnAllSetting = gcnew Button();
		btnAllSetting->Text = "設定を保存";
		btnAllSetting->Height = 30;
		btnAllSetting->Width = 100;
		btnAllSetting->Top = 195;
		btnAllSetting->Left = (this->ClientSize.Width-btnAllSetting->Width)/2;

		btnAllSetting->Click += gcnew EventHandler(this, &DisplaySelectionForm::btnAllSetting_Click);


		this->Controls->Add( lblResolution );
		this->Controls->Add( lboxResolution );
		this->Controls->Add( pnlBackColor );
		this->Controls->Add( btnSelectBackColor );
		this->Controls->Add( btnAllSetting );

		ReadSelfIniFile();

		// パースしたので、バックカラーを設定
		if (param.BackColorRGB >= 0 ) {
			SetPanelBackColor(param.BackColorRGB);
		} else {
			SetPanelBackColor(0x112244); // 適当に弱青系
		}

		SetDisplayResolutions();
	}

	void SetFormIcon() {
		// このプロジェクトのアセンブリのタイプを取得。
		System::Reflection::Assembly^ prj_assebmly = GetType()->Assembly;
		System::Resources::ResourceManager^ r = gcnew System::Resources::ResourceManager(String::Format("{0}.SemiFullScreenMod", prj_assebmly->GetName()->Name), prj_assebmly);

		System::Drawing::Icon^ iconform = (System::Drawing::Icon^)(r->GetObject("icon"));
		this->Icon = iconform;

	}

	void SetDisplayResolutions() {
		static DEVMODE devmode;          // ディスプレイモード

		char sz[100];
		for ( int index=0; TRUE; index++ ) {
			// 列挙
			if( !EnumDisplaySettings( NULL, index, &devmode ) ){
				break;
			}
			
			if ( devmode.dmBitsPerPel >= 16 ) {
				wsprintf(sz, " %4d x %4d", devmode.dmPelsWidth, devmode.dmPelsHeight);

				// 文字列で連結したものをキー、幅・高さの構造体を値として格納。mapを使うのは、EnumDisplaySettingsでは同じ解像度がたくさん出てくるため。
				POINT rs;
				rs.x = devmode.dmPelsWidth;
				rs.y = devmode.dmPelsHeight;
				std::string key = std::string(sz);
				map_resolution[ key ] = rs;
			}

			index++;
		}

		for (map<string, POINT>::iterator it = map_resolution.begin(); it != map_resolution.end(); it++ ) {
			lboxResolution->Items->Add( gcnew String( it->first.c_str() ) );
		}

		// ちょっと格納方法にしくったか・・・
		// パースしたものと同じものを選択
		BOOL isExist = false;
		for (int i = 0; i < lboxResolution->Items->Count; i++ ) {
			String^ key = (String^)lboxResolution->Items[i];
			wsprintf(sz, "%s", key);
			if ( map_resolution[sz].x == param.ScreenW && map_resolution[sz].y == param.ScreenH ) {
				lboxResolution->SelectedIndex = i;
			}
		}

		// 選択されたものが存在しなかったら、現在のデスクトップ解像度と同じものを選択する。
		// (これは絶対に選ばれるはずだが･･･)
		if ( lboxResolution->SelectedIndex < 0 ) {
			for (int i = 0; i < lboxResolution->Items->Count; i++ ) {
				String^ key = (String^)lboxResolution->Items[i];
				wsprintf(sz, "%s", key);
				if ( map_resolution[sz].x == System::Windows::Forms::Screen::PrimaryScreen->Bounds.Width &&
					 map_resolution[sz].y == System::Windows::Forms::Screen::PrimaryScreen->Bounds.Height ) {
					lboxResolution->SelectedIndex = i;
				}
			}
		}
		
	}

	void SetPanelBackColor(__int64 rgb) {
		__int64 argb = rgb = rgb | 0xFF000000; // アルファの成分を無条件で足しこみ
		Color color = Color::FromArgb( (int)argb );
		this->pnlBackColor->BackColor = color;
	}

	void btnSelectBackColor_Click(Object^ o, EventArgs^ e) {
		System::Windows::Forms::DialogResult dr = colorDialog->ShowDialog();
		if (dr == System::Windows::Forms::DialogResult::OK) {
          // colorDialog1.Color.R, colorDialog1.Color.G, colorDialog1.Color.B);
			pnlBackColor->BackColor = colorDialog->Color;
		}
	}

	void btnAllSetting_Click(Object^ o, EventArgs^ e) {

		int iSelectedIndex = lboxResolution->SelectedIndex;
		if ( iSelectedIndex < 0 ) {
			iSelectedIndex = lboxResolution->Items->Count -1; // ありえないはずだが、万が一の時は、最後の要素の番号とする。
		}
		String^ key = (String^)lboxResolution->Items[iSelectedIndex];
		char sz[100];
		wsprintf(sz, "%s", key);
		int w = map_resolution[sz].x;
		int h = map_resolution[sz].y;


		// 自分自身(このdll)のフルパス
		String^ selfFullPath = System::Reflection::Assembly::GetExecutingAssembly()->Location;
		// フルパスからファイル名(拡張子除外)
		String^ strBaseName = System::IO::Path::GetFileNameWithoutExtension(selfFullPath);
		String^ strIniName = strBaseName + ".ini"; // iniの名前を決定


		System::Text::Encoding^ sjisEnc = System::Text::Encoding::GetEncoding("Shift_JIS"); // 天翔記の設定データは、天翔記本体と文字コードを合わせてて、全てcp932だよ。

		System::IO::StreamWriter^ writer = gcnew System::IO::StreamWriter(strIniName, false, sjisEnc);

		writer->WriteLine("-- プレイ時にデスクトップの「横」解像度を以下に自動切換");

		String^ line = String::Format( "cmd[\"environment_semi_fullscreen_w\"]={0}\n", w );
		writer->WriteLine(line);

		writer->WriteLine("-- プレイ時にデスクトップの「縦」解像度を以下に自動切換");
		line = String::Format( "cmd[\"environment_semi_fullscreen_h\"]={0}\n", h );
		writer->WriteLine(line);

		writer->WriteLine("-- ゲーム時にデスクトップ全体を以下のRGB(0x000000=黒、0xffffff=白、0xFF0000=赤)色のフレームで覆う。");
		line = String::Format( "cmd[\"environment_semi_fullscreen_back_color\"]=0x{0:X2}{1:X2}{2:X2}", pnlBackColor->BackColor.R, pnlBackColor->BackColor.G, pnlBackColor->BackColor.B );
		writer->WriteLine(line);

		writer->Close();

	}



};