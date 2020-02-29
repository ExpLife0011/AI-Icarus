#include "AI_Shellcmd.h"

//�Ƿ�����Ͼ仰��
bool IsRelevance = false;
OpenAIShellCmd AIShellCmd = Cmd_None;
void GetRunPath(char* RunPathBuf, int slen)
{
	GetModuleFileNameA(GetModuleHandle(NULL), RunPathBuf, slen);
	char* pchar = strrchr(RunPathBuf, '\\');
	if (pchar)pchar[1] = 0;
}
void GetAutoLoginUserName(char *UserName,UINT slen)
{
	//��ȡSteamĿ¼
	const char *SteamPathKey = "SOFTWARE\\Valve\\Steam";
	//HKEY_LOCAL_MACHINE\SOFTWARE\Classes\steam\Shell\Open\Command
	HKEY hKey;
	int Rkey = RegOpenKeyExA(HKEY_CURRENT_USER, SteamPathKey, NULL, KEY_WOW64_64KEY | KEY_READ, &hKey);
	char KeyData[1024] = { 0 }, *wp = nullptr;
	DWORD DataSize = 1024;
	int Rread = RegQueryValueExA(hKey, "AutoLoginUser", NULL, NULL, (BYTE*)KeyData, &DataSize);
	if (KeyData[0] != 0) {
		ZeroMemory(UserName, slen);
		CopyMemory(UserName, KeyData, strlen(KeyData));
	}

}
void SetAutoLoginUserName(const char *UserName)
{
	//��ȡSteamĿ¼
	const char *SteamPathKey = "SOFTWARE\\Valve\\Steam";
	//HKEY_LOCAL_MACHINE\SOFTWARE\Classes\steam\Shell\Open\Command
	HKEY hKey;
	int Rkey = RegOpenKeyExA(HKEY_CURRENT_USER, SteamPathKey, NULL, KEY_WOW64_64KEY | KEY_WRITE, &hKey);
	char KeyData[1024] = { 0 };
	DWORD DataSize = 1024;
	int Rread = RegSetValueExA(hKey, "AutoLoginUser", NULL, REG_SZ, (BYTE*)UserName, strlen(UserName));

}
void GetSteamPath(char *SteamPath)
{
	//��ȡSteamĿ¼
	const char *SteamPathKey = "SOFTWARE\\Classes\\steam\\Shell\\Open\\Command";
	//HKEY_LOCAL_MACHINE\SOFTWARE\Classes\steam\Shell\Open\Command
	HKEY hKey;
	int Rkey = RegOpenKeyExA(HKEY_LOCAL_MACHINE, SteamPathKey, NULL, KEY_WOW64_64KEY | KEY_READ, &hKey);
	char KeyData[1024] = { 0 }, *wp = nullptr;
	DWORD DataSize = 1024;
	int Rread = RegQueryValueExA(hKey, NULL, NULL, NULL, (BYTE*)KeyData, &DataSize);
	if (KeyData[0] == '\"') {
		wp = (char*)((char*)KeyData + (sizeof(char)));
		wp = strchr(wp, '\"');
		ZeroMemory(wp, 1);
		//ʹ��steam������Ϸ
		wp = (char*)((char*)KeyData + (sizeof(char)));
		CopyMemory(SteamPath, wp, (strlen(wp)*(sizeof(char))));
		strrchr(SteamPath, '\\')[1] = 0;
	}
}
void GetLoginUserInfo(std::vector<SteamUserInfo> &pSteamUserInfo,const char *SteamConfig)
{
	FILE *pFile = fopen(SteamConfig, "rb");
	if (!pFile) {
		const char* filename = "audio\\tts_Open.wav"; //�ϳɵ������ļ�����
		const char* pSay_Text = "����,������˹��ȡSteam��¼�û���Ϣ����.";
		Play_AI_Speech(pSay_Text, filename, false);
		return;
	}
	fseek(pFile, 0, SEEK_END);
	int flen = ftell(pFile);
	void *pStruser = malloc(flen + 2);
	if (!pStruser) {
		const char* filename = "audio\\tts_Open.wav"; //�ϳɵ������ļ�����
		const char* pSay_Text = "����,������˹�����ڴ�ʧ��.";
		Play_AI_Speech(pSay_Text, filename, false);
		return;
	}
	fseek(pFile, 0, SEEK_SET);
	fread(pStruser, flen, 1, pFile);
	char *ptemp = nullptr;
	ptemp = (char*)pStruser;
	wchar_t pUTF8[256] = { 0 };
	char pACP[256] = { 0 };
	while (ptemp)
	{
		Mod_Text Text;
		SteamUserInfo pTemp2;
		char *pUserName = Text.GetTextMiddle((char*)ptemp, "\"AccountName\"		\"", "\"");
		if (!pUserName)break;
		strcpy(pTemp2.pUserName, pUserName);
		char *pPersonaName = Text.GetTextMiddle((char*)ptemp, "\"PersonaName\"		\"", "\"");
		if (!pPersonaName)break;
		ZeroMemory(pACP, 256);
		ZeroMemory(pUTF8, 256);
		MultiByteToWideChar(CP_UTF8, NULL, pPersonaName, strlen(pPersonaName), pUTF8, 256);
		WideCharToMultiByte(CP_ACP, NULL, pUTF8, 256, pACP, 256, NULL, NULL);
		strcpy(pTemp2.pPersonaName, pACP);
		pSteamUserInfo.push_back(pTemp2);
		ptemp = strstr((char*)ptemp, "PersonaName");
		if (ptemp)ptemp += strlen("PersonaName");
	}
	fclose(pFile);
	free(pStruser);
}
void RunSteam()
{
	Mod_Process process;
	process.TerminateProcess("Steam.exe");
	process.TerminateProcess("Steam.exe");
	char SteamPath[1024] = { 0 };
	char SteamFile[1024] = { 0 };
	GetSteamPath(SteamPath);
	strcpy(SteamFile, SteamPath);
	strcat(SteamFile, "Steam.exe");
	ShellExecuteA(NULL, "open", "explorer.exe", SteamFile, SteamPath, SW_SHOWNORMAL);
}
void SwitchSteam(UINT nIndex)
{
	char SteamPath[1024] = { 0 };
	char SteamConfig[1024] = { 0 };
	std::vector<SteamUserInfo> pSteamUserInfo;
	GetSteamPath(SteamPath);
	strcpy(SteamConfig, SteamPath);
	strcat(SteamConfig, "config\\loginusers.vdf");
	GetLoginUserInfo(pSteamUserInfo, SteamConfig);
	if (pSteamUserInfo.empty()) {
		const char* filename = "audio\\tts_Open.wav"; //�ϳɵ������ļ�����
		const char* pSay_Text = "����,������˹û���ҵ��κ��ʺ���Ϣ,������Ҫ�Լ���¼Ŷ.";
		Play_AI_Speech(pSay_Text, filename, false);
		return;
	}
	if (nIndex == 0x6FF) {
		const char* filename = "audio\\tts_Open.wav"; //�ϳɵ������ļ�����
		char pSay_Text[4096] = { 0 };
		wsprintfA(pSay_Text, "������һ����%d���ʺ�,��Ҫ�л���һ���˺���?", pSteamUserInfo.size());
		for (UINT i = 0; i < pSteamUserInfo.size(); i++) {
			strcat(pSay_Text, "\n%d ");
			wsprintfA(pSay_Text, pSay_Text, i);
			strcat(pSay_Text, pSteamUserInfo[i].pPersonaName);
		}
		Play_AI_Speech(pSay_Text, filename, false);
		AIShellCmd = Steam_Cmd_Switch;
		SetEvent(events[EVT_START]);
	}
	else {
		const char* filename = "audio\\tts_Open.wav"; //�ϳɵ������ļ�����
		char pSay_Text[4096] = { 0 };
		if (nIndex < pSteamUserInfo.size()) {
			wsprintfA(pSay_Text, "����,������˹����Ϊ����¼ %s �����ĵȴ�", pSteamUserInfo[nIndex].pPersonaName);
			Play_AI_Speech(pSay_Text, filename, false);
			SetAutoLoginUserName(pSteamUserInfo[nIndex].pUserName);
			RunSteam();
		}
		else {
			wsprintfA(pSay_Text, "����,������˹�޷���¼���˻�,ԭ���Ǵ��ݵ�����̫���� ����ֵΪ %d.", nIndex);
			Play_AI_Speech(pSay_Text, filename, false);
		}
	}
}
void _stdcall OpenSteam()
{
	char SteamPath[1024] = { 0 };
	char SteamFile[1024] = { 0 };
	char UserName[1024];
	GetSteamPath(SteamPath);
	//�ж�Steam�Ƿ��Ѿ�����
	lib_Process::Process process;
	if (process.IsProcesNameExist("Steam.exe")) {
		const char* filename = "audio\\tts_Open.wav"; //�ϳɵ������ļ�����
		const char* pSay_Text = "����,Steam�Ѿ���������,����Ҫ�л�Steam�˺���?";
		Play_AI_Speech(pSay_Text, filename, false);
		AIShellCmd = Steam_Cmd_Isexst;
		SetEvent(events[EVT_START]);
		return;
	}
	const char* filename = "audio\\tts_Open.wav"; //�ϳɵ������ļ�����
	char pSay_Text[256];
	GetAutoLoginUserName(UserName, 1024);
	wsprintfA(pSay_Text, "����,�����ڵ�¼���˺��� %s ,��ҪΪ�����л��˺���?", UserName);
	Play_AI_Speech(pSay_Text, filename, false);
	AIShellCmd = Steam_Cmd_Open;
	SetEvent(events[EVT_START]);
	return;
}
void ShutDownComputer()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES Token_Privileges;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
	LookupPrivilegeValueA(NULL, "SeShutdownPrivilege", &Token_Privileges.Privileges->Luid);
	Token_Privileges.PrivilegeCount = 1;
	Token_Privileges.Privileges->Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &Token_Privileges, NULL, NULL, NULL);
	int ret = ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, SHTDN_REASON_MAJOR_APPLICATION);
	if (!ret) {
		ret = GetLastError();
		if (ret = 1314) {
			const char* filename = "audio\\tts_Open.wav"; //�ϳɵ������ļ�����
			const char *pSay_Text = "����,���Թر�ʧ��,ԭ����������˹û��Ȩ��.";
			Play_AI_Speech(pSay_Text, filename, false);
		}
		else {
			const char* filename = "audio\\tts_Open.wav"; //�ϳɵ������ļ�����
			char pSay_text2[500];
			wsprintfA(pSay_text2, "����,���Թر�ʧ�� ��������� %d .", ret);
			Play_AI_Speech(pSay_text2, filename, false);
		}
	}
}
void _stdcall AI_ShellCmd(char *pString)
{
	const char* filename = "audio\\tts_Open.wav"; //�ϳɵ������ļ�����
	if (strstr(pString, "��steam") || strstr(pString, "��steam") ||
		strstr(pString, "����Ƶ")//��Ϊ����ԭ����ʱ���ʶ�����Ƶ.....
		) {
		const char *pSay_Text = "�õ�����,����Ϊ����steam,Ҫ���ĵȴ�Ŷ.";
		Play_AI_Speech(pSay_Text, filename, false);
		OpenSteam();
	}
	else if (strstr(pString, "ʱ��") || strstr(pString, "����")) {
		SYSTEMTIME ptime;
		GetLocalTime(&ptime);
		char pSay_text2[500];
		wsprintfA(pSay_text2, "����ʱ���� %d��%d��%d�� %d��%d��%d��.", ptime.wYear, ptime.wMonth, ptime.wDay, ptime.wHour, ptime.wMinute, ptime.wSecond);
		Play_AI_Speech(pSay_text2, filename, false);
	}
	else if (strstr(pString, "���ǽ���") || strstr(pString, "û�к���") || strstr(pString, "����˵��") || strstr(pString, "û��˵��")) {
		const char *pSay_Text = "�ð�,��ʲô��Ҫ���ܷԸ�������˹.";
		Play_AI_Speech(pSay_Text, filename, false);
	}
	else if (strstr(pString, "���Թػ�") || strstr(pString, "�ص�����") || strstr(pString, "�رյ���") || strstr(pString, "���Թص�") || strstr(pString, "�ص���")) {
		const char *pSay_Text = "�õ�,���ڹرռ����.";
		Play_AI_Speech(pSay_Text, filename, false);
		ShutDownComputer();
	}
	else {
		int Num = 0;
		switch (AIShellCmd)
		{
		case Answer_Cmd_No_ops:
			AIShellCmd = Cmd_None;
			if (strstr(pString, "������") || strstr(pString, "û����") || strstr(pString, "����Ҫ��")) {
				const char *pSay_Text = "�ð�,��ʲô��Ҫ���ܷԸ�������˹.";
				Play_AI_Speech(pSay_Text, filename, false);
			}
			else {
				goto Else_Exitr;
			}
			return;
		case Steam_Cmd_Isexst://���Steam�Ѿ�����,��ѯ���Ƿ��л��˺�
			AIShellCmd = Cmd_None;
			if (strstr(pString, "������") || strstr(pString, "û����") || strstr(pString, "����Ҫ") || strstr(pString, "���л�")) {
				const char *pSay_Text = "�õ�����,����Ϊ�����´�Steam,�����ĵȴ�.";
				Play_AI_Speech(pSay_Text, filename, false);
				RunSteam();
			}
			else if(strstr(pString, "Ҫ��") || strstr(pString, "��Ҫ") || strstr(pString, "���˺�") || strstr(pString, "ȷ��"))
			{
				const char *pSay_Text = "�õ�����,����Ҫ�л�ʲô�ʺ���?.";
				Play_AI_Speech(pSay_Text, filename, false);
				SwitchSteam(0x6FF);
			}
			else {
				goto Else_Exitr;
			}
			return;
		case Steam_Cmd_Open://���Steamû������,ͬ��ѯ���Ƿ��л��˺�
			AIShellCmd = Cmd_None;
			if (strstr(pString, "������") || strstr(pString, "û����") || strstr(pString, "����Ҫ") || strstr(pString, "���л�")) {
				const char *pSay_Text = "�õ�����,����Ϊ�����´�Steam,�����ĵȴ�.";
				Play_AI_Speech(pSay_Text, filename, false);
				RunSteam();
			}
			else if (strstr(pString, "Ҫ��") || strstr(pString, "��Ҫ") || strstr(pString, "���˺�") || strstr(pString, "ȷ��"))
			{
				const char *pSay_Text = "�õ�����,����Ϊ����ȡSteam�ʺ���Ϣ.";
				Play_AI_Speech(pSay_Text, filename, false);
				SwitchSteam(0x6FF);
			}
			else {
				goto Else_Exitr;
			}
			return;
		case Steam_Cmd_Switch://���Steamû������,ͬ��ѯ���Ƿ��л��˺�
			AIShellCmd = Cmd_None;
			if (pString[0] <= '9'&&pString[0] >= '0') {
				Num = atoi(pString);
				SwitchSteam(Num);
			}
			else {
				const char *pSay_Text_ = "������ѡ���ʺ���ȷ������.";
				Play_AI_Speech(pSay_Text_, filename, false);
				AIShellCmd = Steam_Cmd_Switch;
				SetEvent(events[EVT_START]);
			}
			return;
		default:
			break;
		}
		char pSay_Text[256];
		wsprintfA(pSay_Text, "������˵�� %s ,������˹�޷�����,����Ҫ������˹������������?.",pString);
		Play_AI_Speech(pSay_Text, filename, false);
		AIShellCmd = Answer_Cmd_No_ops;
		SetEvent(events[EVT_START]);
		return;
	Else_Exitr:
		const char *pSay_Text_ = "������˵ʲô,������˹�޷�������˵���ͼ.";
		Play_AI_Speech(pSay_Text_, filename, false);
	}

}