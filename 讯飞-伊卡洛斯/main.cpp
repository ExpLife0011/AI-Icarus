#include "main.h"

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	_tprintf(_T("������˹����ģ�����ڼ�����....\n"));
	
	_tprintf(_T("*.��ʼװ��AIϵͳ..\n"));
	if (!InstallAI()) {
		_tprintf(_T("*.AIϵͳװ��ʧ��!\n"));
	}
	_tprintf(_T("*.AIϵͳ�ɹ�װ��!\n"));
	
	while (true)
	{
		Sleep(20);
	}
	_tprintf(_T("*.����ж��AIϵͳ..\n"));
	if (!UninstallAI()) {
		_tprintf(_T("*.AIϵͳж��ʧ��!\n"));
	}
	system("pause");
	return 0;
}