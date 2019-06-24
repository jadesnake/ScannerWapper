// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 SCANWAPPER_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// SCANWAPPER_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#pragma once
#ifdef SCANWAPPER_EXPORTS
#define SCANWAPPER_API __declspec(dllexport)
#else
#define SCANWAPPER_API __declspec(dllimport)
#endif
#include <string>
#include <vector>
#include <comutil.h>
class ScanWapper{
public:
	struct Dev
	{
		Dev(){
			id = -1;
			bDefault=false;
		}
		UINT id;
		std::string name;
		std::string manuFacturer;
		std::string version;
		std::string family;
		bool bDefault;	//
	};
	class Capability
	{
	public:
		virtual ~Capability(){ }
		virtual const std::string& GetName()=0;
		virtual std::string GetType()=0;
		virtual bool SetValue(const std::string& var)=0;
		virtual const std::string& GetValue()=0;
		virtual const std::wstring& GetLastMsg()=0;
		//可选数据
		virtual std::vector<std::string> CanSelectVals()=0;
	};
	class CallEvent
	{
	public:
		virtual ~CallEvent(){}
		virtual void OnScanOneFile(const char *path)=0;
		virtual void OnScanEnd()=0;
	};
	typedef std::vector<Capability*> Capabilities;
	virtual ~ScanWapper(){  }
	virtual bool initialize()=0;
	virtual const std::vector<Dev>& EnumAll()=0;
	virtual Capabilities SupportedCaps()=0;
	virtual const std::wstring& GetLastMsg()=0;
	virtual void exit()=0;
	virtual bool connect(const Dev& dev)=0;
	virtual bool disconnect()=0;
	virtual bool enableUI(HWND hWnd,bool bShowUI)=0;
	virtual bool startOutFiles()=0;
	virtual void setFileName(const char* fn) = 0;
	virtual void setCallEvent(CallEvent *e) = 0;
	virtual void setOutPath(const char* path)=0;
};

#if defined(__cplusplus)
extern "C"{
#endif
	SCANWAPPER_API ScanWapper* GetScannerWapper();
#if defined(__cplusplus)
};
#endif
