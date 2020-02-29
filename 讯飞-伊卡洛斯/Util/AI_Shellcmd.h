#pragma once
#ifndef _AI_Shellcmd_H
#define _AI_Shellcmd_H

#include "AI_text_to_speech.h"
#include "iat_online_record.h"
#include "json/json.h"
#include "ModuleSDK/Module.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#pragma warning(disable:4996)

enum OpenAIShellCmd
{
	Cmd_None,//���κ���һ��ָ��
	Steam_Cmd_Open,//��Steam
	Steam_Cmd_Isexst,//Steam�Ƿ����
	Steam_Cmd_Switch,//�л��˺�
	Answer_Cmd_No_ops,//û���κβ�����Ӧ��
};
struct SteamUserInfo
{
	char pUserName[256];
	char pPersonaName[256];
};
void _stdcall OpenSteam();
void _stdcall AI_ShellCmd(char *pString);

#endif
