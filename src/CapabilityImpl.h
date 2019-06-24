#pragma once
#include "ScanWapper.h"
class TwainApp;
class ManVal
{
public:
	std::string name;
	std::string val;
	void clear(){
		name.clear();
		val.clear();
	}
};
class CapVar
{
public:
	CapVar(){
		nType=-1;
	}
	void clear(){
		nType = -1;
		val.clear();
		all.clear();
	}
	std::string findVal(const char* v){
		for(size_t t=0;t<all.size();t++)
		{
			if(all[t].name==v||all[t].val==v)
				return all[t].val;
		}
		return "";
	}
	UINT nType;		//容器类型
	ManVal val;
	std::vector<ManVal> all;
};
class CapabilityImpl : public ScanWapper::Capability
{
public:
	CapabilityImpl(UINT unID,TwainApp *app);
	const std::string& GetName();
	std::string GetType();
	bool SetValue(const std::string& var);
	const std::string& GetValue();
	std::vector<std::string> CanSelectVals();
	const std::wstring& GetLastMsg();
	bool IsSuc();
protected:
	std::string TypeToStr(int nType);
private:
	UINT   CapID;
	bool   byChanged;
	bool   bReadOnly;
	CapVar capVar;		//属性值
	std::string name;
	int	   type;
	std::string capConType;
	std::string getORset;
	TwainApp *mApp;
	bool   bSuc;
	std::wstring msg;
};