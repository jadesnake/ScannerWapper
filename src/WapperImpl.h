#pragma once
#include "ScanWapper.h"
class TwainApp;
class CapabilityImpl;
class WapperImpl : public ScanWapper
{
public:
	enum OutWay{
		OUT_NATIVE,
		OUT_FILES,
		OUT_MEMORY,
	};
	WapperImpl(void);
	~WapperImpl(void);
	bool initialize();
	const std::vector<ScanWapper::Dev>& EnumAll();
	WapperImpl::Capabilities SupportedCaps();
	const std::wstring& GetLastMsg();
	void exit();
	bool connect(const Dev& dev);
	bool disconnect();
	bool enableUI(HWND hWnd,bool bShowUI);
	bool startOutFiles();
	void setFileName(const char* fn);
	void setCallEvent(ScanWapper::CallEvent *e);
	TwainApp* GetTwainApp();
	void setOutPath(const char* path);
	void OnScanOneFile(const char *path);
	void OnScanEnd();
	void DoNextWinMsg();
protected:
	void DestoryUI();
 	size_t GetAllDevs();
	size_t GetAllCaps();
	void ClearCaps();
	bool Start();
	void OnScanReady();
private:
	std::string  mPath;
	std::wstring mLastMsg;
	std::vector<ScanWapper::Dev> mDevs;
	std::vector<CapabilityImpl*> mCaps;
	HWND mWinUI;
	HWND mUsrUI;
	TW_IDENTITY mMyInfo;
	TwainApp *mTwainApp;
	ScanWapper::CallEvent *mCallEvent;
	bool	 bShowUI;
	OutWay	 mOutWay;
};
extern WapperImpl wapperImpl;