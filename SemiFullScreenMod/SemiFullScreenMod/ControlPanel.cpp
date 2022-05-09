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
		CPL_INIT�́ACPlApplet�ɍŏ��ɑ����Ă��郁�b�Z�[�W�ł��B
		��ʂɂ́A���̃��b�Z�[�W��cpl�t�@�C�����ň������ƂɂȂ�ł��낤�f�[�^�� �����������肷��̂ł����A
		����͑�K�͂̃v���O���������킯�ł͂Ȃ��̂ŁA ���ɉ������������Ă��܂���B
		�������A���̃��b�Z�[�W�ł͏������ɖ�肪�Ȃ����0�ȊO�̒l��Ԃ��ׂ��Ȃ̂ŁA
		������ƃ��b�Z�[�W��߂炦�A0�ȊO�̒l��Ԃ��悤�ɂ��܂��B 
	*/

	case CPL_GETCOUNT:
		return 1;
	/*
		CPL_GETCOUNT�́A�T�|�[�g����_�C�A���O�̐���Ԃ��܂��B
		��ʂɂ́A�_�C�A���O��1�\�����邾���ł��傤����1��Ԃ����ƂɂȂ�ł��傤�B
		�v���p�e�B�V�[�g��\������悤�ȂƂ��́A�_�C�A���O�������ɂȂ�܂����A
		���̂悤�ȏꍇ�ł�1��Ԃ��Ă��������B 
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
		CPL_INQUIRE�́Acpl�t�@�C�������[�h�����v���Z�X���A
		����cpl�t�@�C���Ɋ܂܂�Ă���A�C�R���╶������擾���邽�߂ɑ����܂��B
		�����̏���CPLINFO�\���̂ŕ\����AlParam2�͂��̃A�h���X���w���Ă���̂ŁA
		LPCPLINFO�^�Ƃ��Ď󂯎��A�����o�����������܂��B
		
		idIcon�̓A�C�R���̎��ʎq�AidName�̓A�C�R���̖��O������������̎��ʎq�A
		idInfo�ɂ̓A�C�R���̐�����������������̎��ʎq���w�肵�܂��B
		
		lData�͏���������K�v�̂Ȃ������o�ł����A
		���̃����o��CPL_DBLCLK��lParam2�ɑ������邱�ƂɂȂ邽�߁ACPL_DBLCLK�͂���𗘗p���Ă��܂��B
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
		CPL_DBLCLK�́A���[�U�[���A�C�R�����N���b�N�����Ƃ��ɑ����܂��B
		����lParam2�ɂ́ACPL_INQUIRE�Őݒ肵��lData�Ɠ����l���Ԃ�܂��B
		�܂�AGetModuleHandle�̖߂�l�Ɠ����ł��B
		�������ACPL_DBLCLK��GetModuleHandle���Ăяo���Ă����͂���܂���B
		���W���[���̃n���h����DialogBox�̑�1�������K�v�Ƃ��܂����A
		�����Ŏw�肷��n���h���͕K��mycpl.cpl�̃n���h���łȂ���΂Ȃ�܂���B
		GetModuleHandle�̑�1������NULL���w�肷��ƁA
		cpl�t�@�C�������[�h�����v���Z�X��EXE���W���[���̃A�h���X���Ԃ邱�ƂɂȂ邽�߁A
		���̓_�͏\���ɒ��ӂ��Ă����ׂ��ł��B
		DialogBox�́A���[�_���_�C�A���O�{�b�N�X���쐬���邽�߁A
		�_�C�A���O�v���V�[�W�����_�C�A���O��j������܂Ő����Ԃ��܂���B
		��4�����Ɏw�肵��DlgProc(�_�C�A���O�v���V�[�W��)�́A�ȉ��̂悤�ɂȂ��Ă��܂��B 
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