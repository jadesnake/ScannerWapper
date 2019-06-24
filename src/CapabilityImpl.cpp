#include "stdafx.h"
#include "TwainApp.h"
#include "CapabilityImpl.h"
namespace CapUtils{
	ManVal QueryCurVal(TW_CAPABILITY *pCap,TW_UINT16 type,int nI)
	{
		ManVal val;
		std::stringstream ss;
		switch(type)
		{
		case TWTY_INT8:
		case TWTY_UINT8:
		case TWTY_INT16:
		case TWTY_UINT16:
		case TWTY_INT32:
		case TWTY_UINT32:
		case TWTY_BOOL:
			{
				TW_UINT32 uVal;
				if(nI==-1&&getCurrent(pCap, uVal))
				{
					val.name = convertCAP_Item_toString(pCap->Cap, uVal, type);
					ss << uVal;
					val.val = ss.str();
				}
				else if(GetItem(pCap, nI, uVal))
				{
					val.name = convertCAP_Item_toString(pCap->Cap, uVal, type);
					ss << uVal;
					val.val = ss.str();
				}
 				break;
			}
		case TWTY_STR32:
		case TWTY_STR64:
		case TWTY_STR128:
		case TWTY_STR255:
			{
				if(nI==-1)
					getCurrent(pCap, val.val);
				else
					GetItem(pCap, nI, val.val);
				val.name = val.val;
				break;
			}
		case TWTY_FIX32:
			{
				TW_FIX32 fix32;
				if(nI==-1&&getCurrent(pCap, fix32))
				{
					float fVal = FIX32ToFloat( fix32 );
					ss.precision(1);
					ss<<std::fixed<<fVal;
					val.name = ss.str();
					val.val  = ss.str();
				}
				else if(GetItem(pCap, nI, fix32))
				{
					float fVal = FIX32ToFloat( fix32 );
					ss.precision(1);
					ss<<std::fixed<<fVal;
					val.name = ss.str();
					val.val  = ss.str();
				}
				break;
			}
		case TWTY_FRAME:
			{
				TW_FRAME frame;
				if(nI==-1&&getCurrent(pCap, frame))
				{
					ss << frame.Left.Whole<<"."<<(int)((frame.Left.Frac/65536.0 + .0005)*1000);
					ss <<" "<<frame.Right.Whole<<"."<<(int)((frame.Right.Frac/65536.0 + .0005)*1000);
					ss <<" "<<frame.Top.Whole<<"."<<(int)((frame.Top.Frac/65536.0 + .0005)*1000);
					ss <<" "<<frame.Bottom.Whole<<"."<<(int)((frame.Bottom.Frac/65536.0 + .0005)*1000);
					val.name = ss.str();
					val.val  = ss.str();
				}
				else if( GetItem(pCap, nI, frame) )
				{
					ss << frame.Left.Whole<<"."<<(int)((frame.Left.Frac/65536.0 + .0005)*1000);
					ss <<" "<<frame.Right.Whole<<"."<<(int)((frame.Right.Frac/65536.0 + .0005)*1000);
					ss <<" "<<frame.Top.Whole<<"."<<(int)((frame.Top.Frac/65536.0 + .0005)*1000);
					ss <<" "<<frame.Bottom.Whole<<"."<<(int)((frame.Bottom.Frac/65536.0 + .0005)*1000);
					val.name = ss.str();
					val.val  = ss.str();
				}
				break;
			}
		default:
			{
				break;
			}
		}
		return val;
	}
	ManVal QueryCurCapVar(UINT id,TwainApp *app)
	{
		ManVal retVar;
		TW_CAPABILITY Cap = {id,TWON_DONTCARE16,0};
 		UINT CondCode = app->get_CAP(Cap, MSG_GET);
		if(CondCode!=TWCC_SUCCESS)
			return retVar;
		pTW_ONEVALUE pCapOne = NULL;
		if(Cap.hContainer)
			pCapOne = (pTW_ONEVALUE)_DSM_LockMemory(Cap.hContainer);
		if(pCapOne==NULL)
			return retVar;
 		UINT nNum = 0;
 		if(Cap.ConType==TWON_ARRAY)
		{
			pTW_ARRAY pCapArray  = (pTW_ARRAY)pCapOne;
			retVar = QueryCurVal(&Cap,pCapArray->ItemType,0);
			std::transform(retVar.name.begin(),retVar.name.end(),retVar.name.begin(),::tolower);
			_DSM_UnlockMemory(Cap.hContainer);
		}
		else if(Cap.ConType==TWON_ENUMERATION)
		{
			pTW_ENUMERATION pCapEnum  = (pTW_ENUMERATION)pCapOne;
			retVar = QueryCurVal(&Cap,pCapEnum->ItemType,pCapEnum->CurrentIndex);
			std::transform(retVar.name.begin(),retVar.name.end(),retVar.name.begin(),::tolower);
			_DSM_UnlockMemory(Cap.hContainer);
		}
		else if(Cap.ConType==TWON_ONEVALUE)
		{
			pTW_ONEVALUE pCapVal  = (pTW_ONEVALUE)pCapOne;
			retVar = QueryCurVal(&Cap,pCapVal->ItemType,-1);
			std::transform(retVar.name.begin(),retVar.name.end(),retVar.name.begin(),::tolower);
			_DSM_UnlockMemory(Cap.hContainer);
		}
		else if(Cap.ConType==TWON_RANGE)
		{
			pTW_RANGE pCapVal  = (pTW_RANGE)pCapOne;
			retVar = QueryCurVal(&Cap,pCapVal->ItemType,-1);
			std::transform(retVar.name.begin(),retVar.name.end(),retVar.name.begin(),::tolower);
			_DSM_UnlockMemory(Cap.hContainer);
		}
		return retVar;
	}
	CapVar QueryCapVar(UINT id,TW_CAPABILITY *pCap)
	{
		CapVar retVar;
		UINT nNum = 0;
		retVar.nType = pCap->ConType;
		if(retVar.nType==TWON_ARRAY)
		{
			pTW_ARRAY pCapArray  = (pTW_ARRAY)_DSM_LockMemory(pCap->hContainer);
			retVar.val = QueryCurVal(pCap,pCapArray->ItemType,0);
			std::transform(retVar.val.name.begin(),retVar.val.name.end(),retVar.val.name.begin(),::tolower);
			do
			{
				ManVal iVal = QueryCurVal(pCap,pCapArray->ItemType,nNum);
				if(iVal.val.empty())
					break;
				std::transform(retVar.val.name.begin(),retVar.val.name.end(),retVar.val.name.begin(),::tolower);
				retVar.all.push_back(iVal);
				nNum+=1;
			}while(1);
			_DSM_UnlockMemory(pCap->hContainer);
		}
		else if(retVar.nType==TWON_ENUMERATION)
		{
			pTW_ENUMERATION pCapEnum  = (pTW_ENUMERATION)_DSM_LockMemory(pCap->hContainer);
			retVar.val = QueryCurVal(pCap,pCapEnum->ItemType,pCapEnum->CurrentIndex);
			std::transform(retVar.val.name.begin(),retVar.val.name.end(),retVar.val.name.begin(),::tolower);
			do
			{
				ManVal iVal = QueryCurVal(pCap,pCapEnum->ItemType,nNum);
				if(iVal.val.empty())
					break;
				std::transform(iVal.name.begin(),iVal.name.end(),iVal.name.begin(),::tolower);
				retVar.all.push_back(iVal);
				nNum+=1;
			}while(1);
			_DSM_UnlockMemory(pCap->hContainer);
		}
		else if(retVar.nType==TWON_ONEVALUE)
		{
			pTW_ONEVALUE pCapVal  = (pTW_ONEVALUE)_DSM_LockMemory(pCap->hContainer);
			retVar.val = QueryCurVal(pCap,pCapVal->ItemType,-1);
			std::transform(retVar.val.name.begin(),retVar.val.name.end(),retVar.val.name.begin(),::tolower);
			_DSM_UnlockMemory(pCap->hContainer);
		}
		else if(retVar.nType==TWON_RANGE)
		{
			pTW_RANGE pCapVal  = (pTW_RANGE)_DSM_LockMemory(pCap->hContainer);
			retVar.val = QueryCurVal(pCap,pCapVal->ItemType,-1);
			std::transform(retVar.val.name.begin(),retVar.val.name.end(),retVar.val.name.begin(),::tolower);
			/*do
			{
			std::string iVal = QueryCurVal(pCap,pCapVal->ItemType,nNum);
			if(iVal.empty())
			break;
			retVar.all.push_back(iVal);
			nNum+=1;
			}while(1);*/
			_DSM_UnlockMemory(pCap->hContainer);
		}
		return retVar;
	}
}
CapabilityImpl::CapabilityImpl(UINT unID,TwainApp *app)
:mApp(app),type(-1)
{
	CapID = unID;
	if(TWRC_SUCCESS!=mApp->GetLabel(unID,name))
		name = convertCAP_toString(unID);
	byChanged  = 0;
	bReadOnly = FALSE;
	TW_CAPABILITY Cap = {CapID,TWON_DONTCARE16,0};
	// We will ignor what Query Supported has reproted about Message Get and get the current anyway.
	UINT CondCode = app->get_CAP(Cap, MSG_GET);
	if(CondCode!=TWCC_SUCCESS)
	{
		bSuc = false;
		msg = CA2W(convertConditionCode_toString(CondCode));
		return ;
	}
	capVar.clear();
	pTW_ONEVALUE pCap = NULL;
	if( Cap.hContainer&&(pCap=(pTW_ONEVALUE)_DSM_LockMemory(Cap.hContainer)) )
	{
		type = pCap->ItemType;
		_DSM_UnlockMemory(Cap.hContainer);
		capVar = CapUtils::QueryCapVar(CapID,&Cap);
		switch(Cap.ConType)
		{
		case TWON_ARRAY:
			capConType = "Array";
			break;
		case TWON_ENUMERATION:
			capConType = "Enumeration";
			break;
		case TWON_ONEVALUE:
			capConType = "One Value";
			break;
		case TWON_RANGE:
			capConType = "Range";
			break;
		}
		TW_UINT32 QS = 0;
		mApp->QuerySupport_CAP(Cap.Cap, QS);
		if(QS&(TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT) )
			getORset += "Get";
		if(QS & TWQC_SET)
		{
			if(0 != getORset.length())
				getORset += ", ";
			getORset += "Set";
		}
		if(QS & TWQC_RESET)
		{
			if(0 != getORset.length())
				getORset += ", ";
			getORset += "Reset";
		}
		if( ( Cap.ConType == TWON_ENUMERATION && capVar.all.size() <= 1 ) 
			|| !( QS & TWQC_SET ) )
		{
			bReadOnly = TRUE;
		}
		bSuc = true;
	}
	else
	{
		bSuc = false;
	}
}
bool CapabilityImpl::IsSuc()
{
	return bSuc;
}
const std::string& CapabilityImpl::GetName()
{
	return name;
}
std::string CapabilityImpl::GetType()
{
	return TypeToStr(type);
}
bool CapabilityImpl::SetValue(const std::string& var)
{
	std::string wantVal(var);
	if(!wantVal.empty())
		std::transform(wantVal.begin(),wantVal.end(),wantVal.begin(),::tolower);
	if(bReadOnly)
	{
		msg = L"readonly";
		return false;
	}
	TW_INT16 twrc = TWRC_FAILURE;
	if(capVar.nType==TWON_ENUMERATION)
	{
		wantVal = capVar.findVal(wantVal.c_str());
		if(wantVal.empty())
		{
			msg = L"enumeration no value";
			return false;
		}
	}
	switch(type)
	{
	case TWTY_INT8:
	case TWTY_UINT8:
	case TWTY_INT16:
	case TWTY_UINT16:
	case TWTY_INT32:
	case TWTY_UINT32:
		{
			long val = atol(wantVal.c_str());
			twrc = mApp->set_CapabilityOneValue(CapID,val,type);
		}
		break;
	case TWTY_BOOL:
		{
			if(wantVal=="false")
				twrc = mApp->set_CapabilityOneValue(CapID, 0, type);
			else if(wantVal=="true")
				twrc = mApp->set_CapabilityOneValue(CapID, 1, type);
			else
				return false;
		}
	case TWTY_FIX32:
		{
			float val;
			std::stringstream ss;
			ss << wantVal.c_str();
			ss >> val;
			TW_FIX32 TWfix32 = FloatToFIX32( val );
			twrc = mApp->set_CapabilityOneValue(CapID,&TWfix32);
			break;
		}
		break;
	case TWTY_STR32:
	case TWTY_STR64:
	case TWTY_STR128:
	case TWTY_STR255:
		twrc = mApp->set_CapabilityOneValue(CapID,var,type);
		break;
	default:
		msg = L"no support type";
		return false;
	}
	if(twrc == TWRC_SUCCESS || twrc == TWRC_CHECKSTATUS)
	{
		capVar.val = CapUtils::QueryCurCapVar(CapID,mApp);
		byChanged  = true;
		return true;
	}
	msg = (wchar_t*)CA2W(mApp->m_msg.c_str());
	return false;
}
const std::string& CapabilityImpl::GetValue()
{
	return capVar.val.val;
}
const std::wstring& CapabilityImpl::GetLastMsg()
{
	return msg;
}
std::string CapabilityImpl::TypeToStr(int nType)
{
	std::string nR;
	switch(nType)
	{
	case TWTY_INT8:
		nR = "INT8";
		break;
	case TWTY_UINT8:
		nR = "UINT8";
		break;
	case TWTY_INT16:
		nR = "INT16";	
		break;
	case TWTY_UINT16:
		nR = "UINT16";
		break;
	case TWTY_INT32:
		nR = "INT32";	
		break;
	case TWTY_UINT32:
		nR = "UINT32";	
		break;
	case TWTY_BOOL:
		nR = "BOOL";	
		break;
	case TWTY_STR32:
		nR = "STR32";
		break;
	case TWTY_STR64:
		nR = "STR64";	
		break;
	case TWTY_STR128:
		nR = "STR128";
		break;
	case TWTY_STR255:
		nR = "STR255";	
		break;
	case TWTY_FIX32:
		nR = "FIX32";	
		break;
	case TWTY_FRAME:
		nR = "FRAME";	
		break;
	default:
		return nR;
	}
	return nR;
}
std::vector<std::string> CapabilityImpl::CanSelectVals()
{
	std::vector<std::string> ret;
	for(size_t t=0;t<capVar.all.size();t++)
		ret.push_back(capVar.all[t].name);
	return ret;
}