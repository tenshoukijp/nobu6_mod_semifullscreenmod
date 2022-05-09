#include <windows.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

#pragma comment(lib, "user32.lib")

namespace BlackBackGroundMod
{
	ref class BlackForm : System::Windows::Forms::Form
	{

		Timer^ timer;
		int iForeGroundTryCnt;

	public:
		BlackForm()
		{
			this->ControlBox = false;
			this->Text = "";
			this->Shown += gcnew EventHandler(this, &BlackForm::Form_Shown);
			this->MouseDown += gcnew MouseEventHandler(this, &BlackForm::Form_MouseDown);


			this->Bounds = System::Windows::Forms::Screen::PrimaryScreen->Bounds;

			this->FormBorderStyle = ::FormBorderStyle::None;
			this->BackColor = System::Drawing::Color::Black;
			this->Show();

			timer = gcnew Timer();
			timer->Enabled = true;
			timer->Interval = 200;
			timer->Tick += gcnew EventHandler(this, &BlackForm::timer_OnTick);
		}

	public:
		static BlackForm^ mform;

		void Form_Shown(Object^ o, EventArgs^ e) {
			HWND hwnd = FindWindow("Tenshouki95", NULL);
			SetForegroundWindow(hwnd);
			SetActiveWindow(hwnd);
		}

		void Form_MouseDown(Object^ o, MouseEventArgs^ e) {
			// 左ボタン以外はスルー
			if ( (e->Button & ::MouseButtons::Left) != ::MouseButtons::Left ) {
				return;
			}

			HWND hwnd = FindWindow("Tenshouki95", NULL);
			if (IsIconic(hwnd)) {
				OpenIcon(hwnd);
			}
			SetForegroundWindow(hwnd);
			SetActiveWindow(hwnd);
		}

	public:
		static Color color;

	private:
		void timer_OnTick(Object^ o, EventArgs^ e) {
			
			this->Bounds = System::Windows::Forms::Screen::PrimaryScreen->Bounds;
		}

	public:
		static void SetBackColor(__int64 rgb) {
			__int64 argb = rgb = rgb | 0xFF000000; // アルファの成分を無条件で足しこみ
			color = Color::FromArgb( (int)argb );
			mform->BackColor = color;
		}

	protected:
		virtual void WndProc(Message %m) override
		{
			switch (m.Msg)
			{
			case WM_MOUSEACTIVATE:
				m.Result = safe_cast<IntPtr>(WM_NCACTIVATE);
				return;
			}
			// Console::WriteLine(m.Msg);
			System::Windows::Forms::Form::WndProc(m);
		}

		/// <summary>
		/// 表示される時にアクティブにならないようにする
		/// </summary>
		property bool ShowWithoutActivation
		{
			virtual bool get() override
			{
				return true;
			}
		}

	};


}


