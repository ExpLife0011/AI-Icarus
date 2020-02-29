#pragma once

#ifndef _AI_H
#define _AI_H

#include <qisr.h>
#include <qivw.h>
#include <qtts.h>
#include <msp_cmn.h>
#include <msp_types.h>
#include <msp_errors.h>
#include "iat_online_record.h"
#include "AI_Shellcmd.h"
#include "AI_Awaken.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <tchar.h>

#pragma warning(disable:4996)
#pragma comment(lib,"winmm.lib")
#ifdef _WIN64
#pragma comment(lib,"msc_x64.lib")
#else
#pragma comment(lib,"msc.lib")
#endif // _WIN64


//װ������ʶ��ϵͳ
bool InstallAI();
//ж������ʶ��ϵͳ
bool UninstallAI();

//AI���Ѵ����߳�
static void AIAwakenThread();
//AI����ʶ�����߳�
static void AIReordThread();
//AI¼������
static void GetReord(void* pReordBuf, UINT *len);

//AI������Ϣ������
static int AI_ivw_msg_proc(const char *sessionID, int msg, int param1, int param2, const void *info, void *userData);
//AI���Ѻ���
static void AI_ivw_proc(void* pReordBuf, UINT Buflen, const char* session_begin_param);
//AI��������
static void AI_Say();

#endif // _AI_H