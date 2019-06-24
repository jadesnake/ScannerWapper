// ScanWapper.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ScanWapper.h"
#include "WapperImpl.h"
#include "TwainApp.h"
SCANWAPPER_API ScanWapper* GetScannerWapper()
{
	return &wapperImpl;
}
//////////////////////////////////////////////////////////////////////////////
/**
* Callback funtion for DS.  This is a callback function that will be called by
* the source when it is ready for the application to start a scan. This 
* callback needs to be registered with the DSM before it can be called.
* It is important that the application returns right away after recieving this
* message.  Set a flag and return.  Do not process the callback in this function.
*/
TW_UINT16 FAR PASCAL DSMCallback(pTW_IDENTITY _pOrigin,pTW_IDENTITY _pDest,
								 TW_UINT32    _DG,TW_UINT16    _DAT,
								 TW_UINT16    _MSG,TW_MEMREF    _pData)
{
	UNUSEDARG(_pDest);
	UNUSEDARG(_DG);
	UNUSEDARG(_DAT);
	UNUSEDARG(_pData);
	TW_UINT16 twrc = TWRC_FAILURE;
	// _pData stores the RefCon from the when the callback was registered
	// RefCon is a TW_INT32 and can not store a pointer for 64bit
	// we are only waiting for callbacks from our datasource, so validate
	// that the originator.
	TwainApp *twApp = wapperImpl.GetTwainApp();
	if(_pOrigin && twApp && twApp->getDataSource() && _pOrigin->Id == twApp->getDataSource()->Id )
	{
		switch (_MSG)
		{
		case MSG_XFERREADY:
		case MSG_CLOSEDSREQ:
		case MSG_CLOSEDSOK:
		case MSG_NULL:
			twApp->m_DSMessage = _MSG;
			twrc = TWRC_SUCCESS;
			break;
		default:
			TRACE("Error - Unknown message in callback routine");
			twApp->m_DSMessage = MSG_NULL;
			twrc = TWRC_FAILURE;
			break;
		}
	}
 	return twrc;
}
