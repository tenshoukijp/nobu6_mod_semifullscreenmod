#pragma once

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Text::RegularExpressions;

struct ParamIniFile {
	int ScreenW;
	int ScreenH;
	__int64 BackColorRGB;
};

extern ParamIniFile param;

extern void TenshouWindowsResize(ParamIniFile *param);
extern void ImportSelfIniFile(String^ strFileName, ParamIniFile *param);
extern void SemiFullScreenModExecute();
extern void ReadSelfIniFile();
