// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� SCANWAPPER_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// SCANWAPPER_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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
		//��ѡ����
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
