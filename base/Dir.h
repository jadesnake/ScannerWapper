#pragma once

#include <atlbase.h>
#include <atlstr.h>

class DirFilter
{
public:
	virtual ~DirFilter(){	}
public:
	virtual DWORD DirEnumItemProc(LPCTSTR lpItemName,LPVOID lpParam,PWIN32_FIND_DATA pInfo)=0;
	virtual bool  DirFileProc(const CAtlString&) = 0;
};

bool  GetFilePath(LPCTSTR pV,LPTSTR pOut,size_t outNum);

bool  OpenSelPathDialog(LPTSTR outSel,HWND hOwner=NULL,LPCTSTR pDefaultDir=NULL,LPCTSTR pTitle=_T("ѡ���ļ�·��"));

/*---------------------------------------------------------------------*\
		�������ܣ�ö��ָ��Ŀ¼��ִ�лص�����
		�������ƣ�EnumFloderItem
		��	  ����lpRootĿ¼������
				  dwAttributesö�ٵ��������ԣ�
				  lpParam�Զ��������
				  lpItemProc�ص��ӿ�
		�� �� ֵ��0�ɹ�������������
\*---------------------------------------------------------------------*/
DWORD  EnumFloderItem(LPCTSTR lpRoot,DWORD dwAttributes,LPVOID lpParam,DirFilter *filter);

/*---------------------------------------------------------------------*\
		�������ܣ�����N��pDirĿ¼
		�������ƣ�CheckDir
		��	  ����bCreate TRUE��������Ŀ¼��FALSE�������Ŀ¼�Ƿ���ڣ�
				  lpSecurityĿ¼��ȫ���ԣ�����ΪNULL
		�� �� ֵ��0�����ɹ�������������,-1 bCreate==FALSEʱĿ¼�����ڣ�����GetLastError����ֵ
\*---------------------------------------------------------------------*/
DWORD  CheckDir( LPCTSTR pDir,BOOL bCreate,LPSECURITY_ATTRIBUTES lpSecurity);

/*---------------------------------------------------------------------*\
		�������ܣ���ȡָ���ļ��Ĵ�С
		�������ƣ�GetFileSize
		��	  ����strPath�����ļ�����·��
		�� �� ֵ��0 ���� ��ʾʧ��
\*---------------------------------------------------------------------*/
LONGLONG QueryFileSize( LPCTSTR strPath );

/*---------------------------------------------------------------------*\
		�������ܣ�����Ŀ¼�µ������ļ���ȫ·��
		�������ƣ�GetFileListFromDir
		��	  ����path �ļ���·��
		�� �� ֵ��
		����1: CSimpleArray<CAtlString> files = GetFileListFromDir(_T("C:\\WINDOWS\\system\\"));
		����2: CSimpleArray<CAtlString> files = GetFileListFromDir(_T("C:\\WINDOWS\\system"));
\*---------------------------------------------------------------------*/
CSimpleArray<CAtlString> GetFileListFromDir(CAtlString path, BOOL recursive=FALSE);

//��ȡ��������Ŀ¼
CAtlString GetAppPath();
CAtlString getAppFilePath();
//��ȡ��̬���ӿ�(dll������·��
//pszFileName:dll�ļ���(������·��)
CAtlString GetDllPath(LPCTSTR pszFileName);

//�ж��ļ�����Ŀ¼�Ƿ����
BOOL IsPathFind(const CAtlString& strPath);

//��ȡlocal appdata ·��
CAtlString GetLocalAppDataPath();

//��ȡ������������
CString GetProcessFullName(HANDLE h);

//ɾ���ļ���
bool DeleteDir(const CAtlString &dir);

//׷��·��
CAtlString AppendUrl(const CAtlString& a, const CAtlString& b);

//������·���л�ȡ�ļ���
CAtlString PathGetFileName(const CAtlString& f);

//������·���л�ȡ·��
CAtlString FindFilePath(const CAtlString& path);

//��ȡ��ǰӦ������
CAtlString GetAppName();

bool CopyDirByFilter(const CString& dir, const CString& dst,DirFilter *fun);

bool CopyDir(const CAtlString& dir, const CAtlString& dst,DirFilter *fun);

bool MoveDir(const CAtlString& dir, const CAtlString& dst);

DWORD CountDirFiles(const CAtlString& dir);
