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
	// ���x��
	Label^ lblResolution;
	// �𑜓x�̃��X�g�{�b�N�X
	ListBox^ lboxResolution;

	// �I��F�̐F�p�l��
	Panel^ pnlBackColor;
	// �F�_�C�A���O�\�����邽�߂̃{�^��
	Button^ btnSelectBackColor;

	// �F�_�C�A���O
	System::Windows::Forms::ColorDialog^ colorDialog;
	// �S�Đݒ�̃{�^��
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
		lblResolution->Text = "�Q�[�����Ɉȉ��̉�ʉ𑜓x�ɐؑ�:";
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
		btnSelectBackColor->Text = "�w�i�F��I��";
		btnSelectBackColor->Top = pnlBackColor->Bottom + 10;
		btnSelectBackColor->Left = pnlBackColor->Left;
		btnSelectBackColor->Height = 30;
		btnSelectBackColor->Width = pnlBackColor->Width;

		btnSelectBackColor->Click += gcnew EventHandler(this, &DisplaySelectionForm::btnSelectBackColor_Click);

		colorDialog = gcnew System::Windows::Forms::ColorDialog();

		btnAllSetting = gcnew Button();
		btnAllSetting->Text = "�ݒ��ۑ�";
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

		// �p�[�X�����̂ŁA�o�b�N�J���[��ݒ�
		if (param.BackColorRGB >= 0 ) {
			SetPanelBackColor(param.BackColorRGB);
		} else {
			SetPanelBackColor(0x112244); // �K���Ɏ�n
		}

		SetDisplayResolutions();
	}

	void SetFormIcon() {
		// ���̃v���W�F�N�g�̃A�Z���u���̃^�C�v���擾�B
		System::Reflection::Assembly^ prj_assebmly = GetType()->Assembly;
		System::Resources::ResourceManager^ r = gcnew System::Resources::ResourceManager(String::Format("{0}.SemiFullScreenMod", prj_assebmly->GetName()->Name), prj_assebmly);

		System::Drawing::Icon^ iconform = (System::Drawing::Icon^)(r->GetObject("icon"));
		this->Icon = iconform;

	}

	void SetDisplayResolutions() {
		static DEVMODE devmode;          // �f�B�X�v���C���[�h

		char sz[100];
		for ( int index=0; TRUE; index++ ) {
			// ��
			if( !EnumDisplaySettings( NULL, index, &devmode ) ){
				break;
			}
			
			if ( devmode.dmBitsPerPel >= 16 ) {
				wsprintf(sz, " %4d x %4d", devmode.dmPelsWidth, devmode.dmPelsHeight);

				// ������ŘA���������̂��L�[�A���E�����̍\���̂�l�Ƃ��Ċi�[�Bmap���g���̂́AEnumDisplaySettings�ł͓����𑜓x����������o�Ă��邽�߁B
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

		// ������Ɗi�[���@�ɂ����������E�E�E
		// �p�[�X�������̂Ɠ������̂�I��
		BOOL isExist = false;
		for (int i = 0; i < lboxResolution->Items->Count; i++ ) {
			String^ key = (String^)lboxResolution->Items[i];
			wsprintf(sz, "%s", key);
			if ( map_resolution[sz].x == param.ScreenW && map_resolution[sz].y == param.ScreenH ) {
				lboxResolution->SelectedIndex = i;
			}
		}

		// �I�����ꂽ���̂����݂��Ȃ�������A���݂̃f�X�N�g�b�v�𑜓x�Ɠ������̂�I������B
		// (����͐�΂ɑI�΂��͂��������)
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
		__int64 argb = rgb = rgb | 0xFF000000; // �A���t�@�̐����𖳏����ő�������
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
			iSelectedIndex = lboxResolution->Items->Count -1; // ���肦�Ȃ��͂������A������̎��́A�Ō�̗v�f�̔ԍ��Ƃ���B
		}
		String^ key = (String^)lboxResolution->Items[iSelectedIndex];
		char sz[100];
		wsprintf(sz, "%s", key);
		int w = map_resolution[sz].x;
		int h = map_resolution[sz].y;


		// �������g(����dll)�̃t���p�X
		String^ selfFullPath = System::Reflection::Assembly::GetExecutingAssembly()->Location;
		// �t���p�X����t�@�C����(�g���q���O)
		String^ strBaseName = System::IO::Path::GetFileNameWithoutExtension(selfFullPath);
		String^ strIniName = strBaseName + ".ini"; // ini�̖��O������


		System::Text::Encoding^ sjisEnc = System::Text::Encoding::GetEncoding("Shift_JIS"); // �V�ċL�̐ݒ�f�[�^�́A�V�ċL�{�̂ƕ����R�[�h�����킹�ĂāA�S��cp932����B

		System::IO::StreamWriter^ writer = gcnew System::IO::StreamWriter(strIniName, false, sjisEnc);

		writer->WriteLine("-- �v���C���Ƀf�X�N�g�b�v�́u���v�𑜓x���ȉ��Ɏ����؊�");

		String^ line = String::Format( "cmd[\"environment_semi_fullscreen_w\"]={0}\n", w );
		writer->WriteLine(line);

		writer->WriteLine("-- �v���C���Ƀf�X�N�g�b�v�́u�c�v�𑜓x���ȉ��Ɏ����؊�");
		line = String::Format( "cmd[\"environment_semi_fullscreen_h\"]={0}\n", h );
		writer->WriteLine(line);

		writer->WriteLine("-- �Q�[�����Ƀf�X�N�g�b�v�S�̂��ȉ���RGB(0x000000=���A0xffffff=���A0xFF0000=��)�F�̃t���[���ŕ����B");
		line = String::Format( "cmd[\"environment_semi_fullscreen_back_color\"]=0x{0:X2}{1:X2}{2:X2}", pnlBackColor->BackColor.R, pnlBackColor->BackColor.G, pnlBackColor->BackColor.B );
		writer->WriteLine(line);

		writer->Close();

	}



};