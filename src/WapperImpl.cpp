#include "StdAfx.h"
#include "WapperImpl.h"
#include "TwainApp.h"
#include "CapabilityImpl.h"
#include "Dir.h"
WapperImpl wapperImpl;
namespace UI
{
	LRESULT CALLBACK UiProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
	{
		
		return DefWindowProc(hWnd, uMessage, wParam, lParam);	
	}
	HWND CreateUI(long pt)
	{
		HINSTANCE hinst = ::GetModuleHandle(L"ScanWapper.dll");
		WNDCLASSEXW wc = {0};
		wc.cbSize = sizeof(wc);
		wc.lpfnWndProc = UiProc;
		wc.hInstance = hinst;
		wc.lpszClassName = L"SCANNER_DEF_PROC";
		::RegisterClassExW(&wc);
		HWND winUI = ::CreateWindowW(L"SCANNER_DEF_PROC",L"scanner wapper",0, 0, 0, 0, 0,NULL, 0, hinst, 0);
		//if(winUI)
		//	::ShowWindow(winUI,SW_HIDE);
		return winUI;
	}
}
//*----------------------------------------------------------------------------------------
WapperImpl::WapperImpl(void)
{
	mUsrUI=NULL;
	mWinUI=NULL;
	mTwainApp=NULL;
	bShowUI = false;
	mCallEvent = NULL;
	mPath = (char*)CT2CA(GetAppPath());
}
WapperImpl::~WapperImpl(void)
{
	if(mTwainApp)
	{
		delete mTwainApp;
		mTwainApp = NULL;
	}
}
void WapperImpl::setCallEvent(ScanWapper::CallEvent *e)
{
	mCallEvent = e;
}
const std::vector<ScanWapper::Dev>& WapperImpl::EnumAll()
{
	return mDevs;
}
const std::wstring& WapperImpl::GetLastMsg()
{
	return mLastMsg;
}
void WapperImpl::exit()
{
	if(mTwainApp)
	{
		mTwainApp->exit();
		delete mTwainApp;
		mTwainApp = NULL;
	}
	DestoryUI();
}
WapperImpl::Capabilities WapperImpl::SupportedCaps()
{
	std::vector<ScanWapper::Capability*> ret;
	for(size_t t=0;t<mCaps.size();t++)
		ret.push_back(mCaps[t]);
	return ret;
}
bool WapperImpl::connect(const Dev& dev)
{
	if(mTwainApp==NULL)
	{
		mLastMsg = L"please call initialize";
		return false;
	}
	for(size_t nI=0;nI<mDevs.size();nI++)
	{
		Dev *inDev = &mDevs[nI];
		if(inDev->id==dev.id)
		{
			if(mTwainApp->m_DSMState<3)
			{
				mTwainApp->connectDSM();
				mLastMsg = (wchar_t*)CA2W(mTwainApp->m_msg.c_str());
			}
			mTwainApp->loadDS(inDev->id);
			//mTwainApp->enableDS(mWinUI,bShowUI);
			mLastMsg = (wchar_t*)CA2W(mTwainApp->m_msg.c_str());
			if(mTwainApp->m_DSMState>=4)
			{
				GetAllCaps();
				return true;
			}
			return false;
		}
	}
	mLastMsg = L"not found dev";
	return false;
}
bool WapperImpl::disconnect()
{
	if(mTwainApp==NULL)
	{
		mLastMsg = L"please call initialize";
		return false;
	}
	bool bsuc = mTwainApp->unloadDS();
	mLastMsg = (wchar_t*)CA2W(mTwainApp->m_msg.c_str());
	if(bsuc)
		ClearCaps();
	return bsuc;
}
bool WapperImpl::enableUI(HWND hWnd,bool bShowUI)
{
	if(::IsWindow(hWnd)&&bShowUI)
	{
		mUsrUI = hWnd;
		this->bShowUI = bShowUI;
	}
	else
	{
		mUsrUI = NULL;
		bShowUI= false;
	}
	return true;
}
TwainApp* WapperImpl::GetTwainApp()
{
	return mTwainApp;
}
void WapperImpl::setOutPath(const char* path)
{
	mPath = path;
}
void WapperImpl::DoNextWinMsg()
{
	if(mUsrUI)
		::PostMessage(mUsrUI,WM_PAINT,0,0);
	else
		::PostMessage(mWinUI,WM_PAINT,0,0);
}
bool WapperImpl::Start()
{
	if(mTwainApp==NULL)
	{
		mLastMsg = L"please call initialize";
		return false;
	} 
	if(mTwainApp->m_DSMState > 4)
	{
		mLastMsg = L"Already Enabled";
		return false;
	}
	mTwainApp->m_DSMessage = -1;
	bool bDs = false;
	mLastMsg.empty();
	bDs = mTwainApp->enableDS(mUsrUI?mUsrUI:mWinUI,bShowUI);
	mLastMsg = (wchar_t*)CA2W(mTwainApp->m_msg.c_str());
	if(bDs==false)
	{
		return false;
	}
	do // If showing UI and KeepUIOpen is Enabled then we will loop here until cancle
	{
		// now we have to wait until we hear something back from the DS.
		while((TW_UINT16)-1 == mTwainApp->m_DSMessage)
		{
			// If we are using callbacks, there is nothing to do here except sleep
			// and wait for our callback from the DS.  If we are not using them, 
			// then we have to poll the DSM.
			MSG msg;
 			if(mWinUI&&!::GetMessage((LPMSG)&msg,NULL, 0, 0))
			{
				TranslateMessage ((LPMSG)&msg);
				DispatchMessage ((LPMSG)&msg);
				break;//WM_QUIT
			}
			TW_EVENT twEvent = {0};
			TW_UINT16  twRC = TWRC_NOTDSEVENT;
			twRC = mTwainApp->startEvent(&msg,twEvent);
			if(!gUSE_CALLBACKS && twRC==TWRC_DSEVENT)
			{
				// check for message from Source
				switch (twEvent.TWMessage)
				{
				case MSG_XFERREADY:
				case MSG_CLOSEDSREQ:
				case MSG_NULL:
					mTwainApp->m_DSMessage = twEvent.TWMessage;
					break;
				case MSG_CLOSEDSOK:
					mLastMsg = L"Error - MSG_CLOSEDSOK in MSG_PROCESSEVENT loop for Scan";
					break;
				default:
					mLastMsg = L"nError - Unknown message in MSG_PROCESSEVENT loop for Scan";
					break;
				}
			}
			if(twRC!=TWRC_DSEVENT)
			{
				TranslateMessage ((LPMSG)&msg);
				DispatchMessage ((LPMSG)&msg);
			}
		}
		// At this point the source has sent us a callback saying that it is ready to
		// transfer the image.
		if(mTwainApp->m_DSMessage == MSG_XFERREADY)
		{
			// move to state 6 as a result of the data source. We can start a scan now.
			mTwainApp->m_DSMState = 6;
			mTwainApp->updateIMAGEINFO();
 			OnScanReady();
			mLastMsg = (wchar_t*)CA2W(mTwainApp->m_msg.c_str());
			mTwainApp->m_DSMessage = (TW_UINT16)-1;
		}
	}while(bShowUI&& mTwainApp->m_DSMessage != MSG_CLOSEDSREQ);
	mTwainApp->disableDS();
	return true;
}
bool WapperImpl::startOutFiles()
{
	if(mTwainApp==NULL)
	{
		mLastMsg = L"please call initialize";
		return false;
	}
	bool bsuc = false;
	for(size_t t=0;t<mCaps.size();t++)
	{
		if(mCaps[t]->GetName()=="ICAP_XFERMECH")
		{
			bsuc = mCaps[t]->SetValue("file");
			break;
		}
	}
	if(bsuc)
		mOutWay = OUT_FILES;
	else
		mOutWay = OUT_NATIVE;
	if(!Start())
		return false;
	return true;
}
void WapperImpl::setFileName(const char* fn)
{
	if(mTwainApp==NULL)
	{
		mLastMsg = L"please call initialize";
		return ;
	}
	mTwainApp->SetFileName(fn);
}
void WapperImpl::OnScanReady()
{
	TW_UINT32       mech;
	TW_CAPABILITY   Cap;
	Cap.Cap = ICAP_XFERMECH;
	Cap.hContainer = 0;
	if( TWCC_SUCCESS!=mTwainApp->get_CAP(Cap, MSG_GETCURRENT)||!getCurrent(&Cap, mech) )
	{
		mLastMsg = L"Error: could not get the transfer mechanism";
		return;
	}
	if(Cap.hContainer)
		_DSM_Free(Cap.hContainer);
	TW_UINT32 fileformat = TWFF_TIFF;
	Cap.Cap = ICAP_IMAGEFILEFORMAT;
	Cap.hContainer = 0;
	if(TWCC_SUCCESS==mTwainApp->get_CAP(Cap, MSG_GETCURRENT))
		getCurrent(&Cap, fileformat);
	if(Cap.hContainer)
		_DSM_Free(Cap.hContainer);
	if(mOutWay==OUT_FILES)
		mTwainApp->initiateTransfer_File(mPath.c_str(),(TW_UINT16)fileformat);
	else
		mTwainApp->initiateTransfer_Native(mPath.c_str(),(TW_UINT16)fileformat);
}
void WapperImpl::OnScanOneFile(const char *path)
{
	if(mCallEvent)
		mCallEvent->OnScanOneFile(path);
}
void WapperImpl::OnScanEnd()
{
	if(mCallEvent)
		mCallEvent->OnScanEnd();
}
bool WapperImpl::initialize()
{
	//创建message类型窗口
	if(mWinUI==NULL)
		mWinUI = UI::CreateUI(reinterpret_cast<long>(this));
	if(mTwainApp==NULL)
		mTwainApp = new TwainApp(mWinUI);
	mTwainApp->SetWapper(this);
	// Set up our Unique Application Identity
	TW_IDENTITY *pAppID = mTwainApp->getAppIdentity();
	pAppID->Version.MajorNum = 2;
	pAppID->Version.MinorNum = 1;
	pAppID->Version.Language = TWLG_ENGLISH_CANADIAN;
	pAppID->Version.Country = TWCY_CANADA;
	SSTRCPY(pAppID->Version.Info, sizeof(pAppID->Version.Info), "2.1.1");
	pAppID->ProtocolMajor = TWON_PROTOCOLMAJOR;
	pAppID->ProtocolMinor = TWON_PROTOCOLMINOR;
	pAppID->SupportedGroups = DF_APP2 | DG_IMAGE | DG_CONTROL;
	SSTRCPY(pAppID->Manufacturer, sizeof(pAppID->Manufacturer), "TWAIN Working Group");
	SSTRCPY(pAppID->ProductFamily, sizeof(pAppID->ProductFamily), "ScannerWapper");
	SSTRCPY(pAppID->ProductName, sizeof(pAppID->ProductName), "ScanWapper Supported Caps");
	memcpy(&mMyInfo,pAppID,sizeof(TW_IDENTITY));
	//Connect to the DSM just to update list
	mTwainApp->connectDSM();
	mLastMsg = (wchar_t*)CA2W(mTwainApp->m_msg.c_str());
	if(mTwainApp->m_DSMState >= 3)
	{
		GetAllDevs();
		mLastMsg = (wchar_t*)CA2W(mTwainApp->m_msg.c_str());
		mTwainApp->disconnectDSM();	
		return true;
	}
	return false;
}

void WapperImpl::DestoryUI()
{
	if(mWinUI)
	{
		::CloseWindow(mWinUI);
		::DestroyWindow(mWinUI);
		mWinUI=NULL;
	}
}
size_t WapperImpl::GetAllDevs()
{
	mDevs.clear();
	UINT nDefault = -1;
	int  i = 0;
	int  index = 0;
	pTW_IDENTITY pID = NULL;
	if( pID=mTwainApp->getDefaultDataSource() ) // Get Default
 		nDefault = pID->Id;
	while( pID = mTwainApp->getDataSource((TW_INT16)i) )
	{
		i++;
		std::stringstream ss_ver;
		ScanWapper::Dev dev;
		dev.id = pID->Id;
		dev.name = pID->ProductName;
		dev.bDefault = (pID->Id==nDefault?true:false);
		dev.manuFacturer = pID->Manufacturer;
		dev.family = pID->ProductFamily;
		ss_ver <<pID->ProtocolMajor <<'.'<<pID->ProtocolMinor;
		dev.version = ss_ver.str();
		mDevs.push_back(dev); 
	}
	return mDevs.size();
}
size_t WapperImpl::GetAllCaps()
{
	// get the supported capabilies
	ClearCaps();
 	TW_UINT32         nCount          = 0;
	TW_CAPABILITY     CapSupportedCaps;
	pTW_ARRAY_UINT16  pCapSupCaps     = 0;
	std::string       sCapName;
	CapSupportedCaps.Cap = CAP_SUPPORTEDCAPS;
	CapSupportedCaps.hContainer = 0;
	TW_INT16 code = mTwainApp->get_CAP(CapSupportedCaps, MSG_GET);
	mLastMsg = (wchar_t*)CA2W(mTwainApp->m_msg.c_str());
	if(TWCC_SUCCESS!=code)
		return mCaps.size();
	if(TWON_ARRAY != CapSupportedCaps.ConType)
		return mCaps.size();
	pCapSupCaps = (pTW_ARRAY_UINT16)_DSM_LockMemory(CapSupportedCaps.hContainer);
	if(TWTY_UINT16 != pCapSupCaps->ItemType)
		goto cleanup;
	nCount = pCapSupCaps->NumItems;
	for(TW_UINT32 i=0; i<nCount; i++)
	{
		CapabilityImpl *cap = new CapabilityImpl(pCapSupCaps->ItemList[i],mTwainApp);
		if(cap->IsSuc())
		{
			mCaps.push_back(cap);
			continue;
		}
		delete cap;
	}
cleanup:
	if(pCapSupCaps)
	{
		_DSM_UnlockMemory(CapSupportedCaps.hContainer);
	}
	if(CapSupportedCaps.hContainer)
	{
		_DSM_Free(CapSupportedCaps.hContainer);
	}
	return mCaps.size();
}
void WapperImpl::ClearCaps()
{
	for(size_t t=0;t<mCaps.size();t++)
	{
		delete mCaps[t];
	}
	mCaps.clear();
}