#include <windows.h>

#include "SemiFullScreenMod.h"
#include "BlackBackGroundMod.h"


ParamIniFile param = {0,0,-1};




void WINAPI On_������(HANDLE hCallDLLHandle) {
	ReadSelfIniFile();

	if ( param.BackColorRGB >= 0 ) {
		BlackBackGroundMod::BlackForm::mform = gcnew BlackBackGroundMod::BlackForm(); 
		BlackBackGroundMod::BlackForm::SetBackColor( param.BackColorRGB );
	}
}

void WINAPI On_������() {
	BlackBackGroundMod::BlackForm::mform->Close();
}

void WINAPI On_�𑜓x�ύX����() {
	SemiFullScreenModExecute();
}

// �Â��o�[�W�����̂��̂ւ̑Ή�
void WINAPI On_���C���Q�[���J�n() {
	SemiFullScreenModExecute();
}

// �S�̂̎��s�B������dll�̖��O�ƈ�v����.ini�t�@�C���̏��Ɋ�Â��āA�S��ʂۂ��؂�ւ���
void SemiFullScreenModExecute() {
	static BOOL isScreenChanged = FALSE;
	if ( isScreenChanged ) { return; }

	int iDeskW = System::Windows::Forms::Screen::PrimaryScreen->Bounds.Width;
	int iDeskH = System::Windows::Forms::Screen::PrimaryScreen->Bounds.Height;
	// �L���Ȑ��l�������Ă��āA���݂̉𑜓x�Ƃ͈قȂ邱��
	if ( param.ScreenW > 0 && param.ScreenH > 0 && 
		!( param.ScreenW == iDeskW && param.ScreenH == iDeskH) ) {
		// ��ʃT�C�Y�ύX
		TenshouWindowsResize(&param);
	}

	isScreenChanged = TRUE;
}

void ReadSelfIniFile() {
	// �������g(����dll)�̃t���p�X
	String^ selfFullPath = System::Reflection::Assembly::GetExecutingAssembly()->Location;
	// �t���p�X����t�@�C����(�g���q���O)
	String^ strBaseName = System::IO::Path::GetFileNameWithoutExtension(selfFullPath);
	String^ strIniName = strBaseName + ".ini"; // ini�̖��O������

	if ( System::IO::File::Exists( strBaseName + ".dll" ) ) {
		System::Windows::Forms::MessageBox::Show( "�Â��o�[�W������ " + strBaseName + ".dll ���c���Ă��܂��B\n�Q�����݂���Ɗm���Ɍ�쓮���܂��̂ŁA�Â������폜���Ă��������B");
	}

	// ini�t�@�C����ǂݍ��ݐݒ�l��ǂݍ���
	ImportSelfIniFile(strIniName, &param);

}

// .ini�t�@�C������ǂݍ���ŁA�p�����[�^�Ƃ��Ċi�[����B
void ImportSelfIniFile(String^ strIniFileName, ParamIniFile *param) {

	try {
		// StreamReader �̐V�����C���X�^���X
		System::IO::StreamReader^ cReader = gcnew System::IO::StreamReader(strIniFileName, System::Text::Encoding::Default);

		// �ǂݍ��݂ł��镶�����Ȃ��Ȃ�܂ŌJ��Ԃ�
		while (cReader->Peek() >= 0) {
			// �t�@�C���� 1 �s���ǂݍ���
			String^ stBuffer = cReader->ReadLine();
	       
			// �}�b�`������B�{��Lua��g�ݍ��ނׂ������A���̒��x�ɑg�ݍ��ނƑ�Ȃ̂ŁB�ǂ������l�����M����Ȃ����낤����\�L���������ĊȈՃp�[�T�[�ōς܂���B
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

		// �j��
		cReader->Close();
	} catch(Exception^) {
		System::Windows::Forms::MessageBox::Show(".ini�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B");
	}
}

// �S��ʂۂ��؂�ւ���Bparam�ɂ͏c�E���̃T�C�Y�������Ă���B
void TenshouWindowsResize(ParamIniFile *param) {

	int iScreenWidth  = param->ScreenW;
	int iScreenHeight = param->ScreenH;

	HWND hwnd = FindWindow("Tenshouki95", NULL);

	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	// �t���X�N���[���p
	static DEVMODE devmode; 

	// �������N���A
	ZeroMemory( &devmode, sizeof(devmode) );

	//���݂̉�ʏ����擾
	BOOL res = EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devmode);

	// �𑜓x�͎w��̂��̂�
	// devmode.dmSize = sizeof(devmode);
	devmode.dmPelsWidth  = iScreenWidth;
	devmode.dmPelsHeight = iScreenHeight;
	// devmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

	if ( BlackBackGroundMod::BlackForm::mform ) {
		// �w�i�����T�C�Y(
		BlackBackGroundMod::BlackForm::mform->Height = iScreenHeight;
		BlackBackGroundMod::BlackForm::mform->Width = iScreenWidth;
	}

	// ���̃A�v�����s���𑜓x�ؑցB
	ChangeDisplaySettings( &devmode, CDS_FULLSCREEN );

	// �A�v���ɂ̓^�C�g���o�[�̕��⋫�E�����i������{�[�_�[�j������̂ł��̃s�N�Z���������߂�
	int iBorderX = GetSystemMetrics(SM_CXSIZEFRAME); // ���E����X����
	int iBorderY = GetSystemMetrics(SM_CYSIZEFRAME); // ���E����Y����
	int iHeightCaption = GetSystemMetrics(SM_CYCAPTION);   // �^�C�g���o�[�̍���

	// �����W�́A�w��̕�����^�񒆂ɔz�u
	int iLeft = (iScreenWidth-clientRect.right)/2;
	// ����W�̐^�񒆂͋��߂Ă������Ƃ܂��g��Ȃ����ȁB
	int iTop  = (iScreenHeight-clientRect.bottom)/2;

	// �g�їp�𑜓x�Ȃǂ�����̂�TOP�͏�Ɉ�ԏ�ŁA���E�͐^�񒆁B
	MoveWindow(hwnd, iLeft-iBorderX, iTop, 0, 0, TRUE);

}

