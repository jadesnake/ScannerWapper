// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../src/ScanWapper.h"
#include "Dir.h"
#include <conio.h>
typedef ScanWapper* (*fnGetScannerWapper)();
int _tmain(int argc, _TCHAR* argv[])
{
	ScanWapper *wapper = NULL;
	CAtlString dll = AppendUrl(GetAppPath(),L"ScanWapper.dll");
	HMODULE hM = LoadLibrary(dll);
	fnGetScannerWapper func = (fnGetScannerWapper)GetProcAddress(hM,"GetScannerWapper");
	wapper = func();
	wapper->initialize();
	std::vector<ScanWapper::Dev> devs = wapper->EnumAll();
	for(size_t t=0;t<devs.size();t++)
	{
		ScanWapper::Dev *dev = &devs[t];
		if(-1!=dev->name.find("L7280+"))
		{
			if(wapper->connect(*dev))
			{
				ScanWapper::Capabilities caps = wapper->SupportedCaps();
				for(size_t t=0;t<caps.size();t++)
				{
					ScanWapper::Capability *cap = caps[t];
					std::string name = cap->GetName();
					if(name=="ICAP_IMAGEFILEFORMAT")
					{
						std::vector<std::string> vals = cap->CanSelectVals();
						cap->SetValue("jpeg");
					}
					else if(name=="ICAP_PIXELTYPE")
					{
						std::vector<std::string> vals = cap->CanSelectVals();
						cap->SetValue("rgb");
					}
					else if(-1!=name.find("ICAP_XRESOLUTION"))
					{
						std::vector<std::string> vals = cap->CanSelectVals();
						cap->SetValue("300");
					}
					else if(-1!=name.find("ICAP_YRESOLUTION"))
					{
						std::vector<std::string> vals = cap->CanSelectVals();
						cap->SetValue("300");
					}
				}
				wapper->startOutFiles();
			}			
			break;
		}
	}
	wapper->exit();
	getch();
	return 0;
}

