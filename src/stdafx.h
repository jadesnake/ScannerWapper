// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��
#include <string>
#include <sstream>
#include <atlbase.h>
#include <atlstr.h>
#include <atltrace.h>
#include <algorithm>
#define TRACE ATLTRACE
#pragma comment(lib,"comctl32.lib")
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "Common.h"
#include "twain.h"
#include "CommonTWAIN.h"
#include "TwainString.h"