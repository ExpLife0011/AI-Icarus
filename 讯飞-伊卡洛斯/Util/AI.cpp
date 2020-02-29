#include "AI.h"

//˵���ı�
char *pSay_Text = nullptr;
//�Ƿ���
bool IsAwaken = false, IsSay = false;
//�Ƿ��ѵ���MSC¼��.
bool IsCall = false;
//�Ƿ��Ѿ�����MSC¼��.
bool MscIsEnd = false;
//û��˵������
int NoSayCount = 0;
bool InstallAI()
{
	const char* login_params = "appid = ���APPID1,work_dir = .";
	if (MSPLogin(NULL, NULL, login_params) != MSP_SUCCESS)return false;
	_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)AIAwakenThread, NULL, NULL, NULL);
	_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)AIReordThread, NULL, NULL, NULL);
	return true;
}
bool UninstallAI()
{
	if (MSPLogout() != MSP_SUCCESS)return false;
	return true;
}
void __stdcall AwakenCallBack2(struct recorder_info *rec)
{

}
void AIAwakenThread()
{
	const char *ssb_param = "ivw_threshold=0:1350,sst=wakeup,ivw_res_path =fo|res/ivw/wakeupresource.jet";
	recorder_start(AwakenCallBack2, ssb_param);
	while (true)
	{
		if (!IsAwaken) {
			UINT buflen = 1024 * 100;
			void* pReordBuf = malloc(buflen);
			if (!pReordBuf) {
				_tprintf(_T("*.�ڴ�����ʧ��,��ȷ���ڴ��Ƿ����!\n"));
				break;
			}
			ZeroMemory(pReordBuf, buflen);
			//GetReord(pReordBuf, &buflen);
			//AI_ivw_proc(pReordBuf, buflen, ssb_param);
			free(pReordBuf);
		}
		Sleep(1);
	}
}
//AI����ʶ��¼�������ص�
void _stdcall AwakenMsgEnd()
{
	IsAwaken = false;
	IsCall = false;
	MscIsEnd = true;
}
//û��˵���Ļص��߳�
void error_call()
{
	char Path[256], FileName[50];
	GetRunPath(Path, sizeof(Path));
	srand((UINT)GetTickCount64());
	wsprintfA(FileName, "audio\\tts_error%d.wav", rand() % 3);
	strcat(Path, FileName);
	PlaySoundA(Path, NULL, SND_FILENAME | SND_SYNC);
	if (!IsCall) {
		SetEvent(events[EVT_START]);
		IsCall = true;
	}
}
//û��˵��AI��ʼ��������
void Nosay_Sleep()
{
	char Path[256];
	GetRunPath(Path, sizeof(Path));
	strcat(Path, "audio\\tts_Sleep.wav");
	PlaySoundA(Path, NULL, SND_FILENAME | SND_SYNC);
}
//AI����ʶ�����ص�
void _stdcall AwakenMsgResult(char *pString)
{
	if (MscIsEnd) {
		MscIsEnd = false;
		if (strlen(pString) < 1) {
			NoSayCount++;
			if (NoSayCount > 1) {
				NoSayCount = 0;
				_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)Nosay_Sleep, NULL, NULL, NULL);
			}
			else {
				_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)error_call, NULL, NULL, NULL);
			}
		}
		else {
			_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)AI_ShellCmd, pString, NULL, NULL);
			NoSayCount = 0;
			//AI_ShellCmd(pString);
		}
	}
}
//AI����MIC¼������ʶ��
void Run_Mic()
{
	const char* session_begin_params = "sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = gb2312";
	AI_mic(session_begin_params, AwakenMsgEnd, AwakenMsgResult);
}
void AIReordThread()
{
	_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)Run_Mic, NULL, NULL, NULL);
	while (true)
	{
		if (IsAwaken) {
			if (!IsCall) {
				SetEvent(events[EVT_START]);
				IsCall = true;
			}
		}
		Sleep(1);
	}
}
void GetReord(void* pReordBuf, UINT *len)
{
	if (len <= 0)return;
	if (!pReordBuf)return;
	HWAVEIN hWaveIn;
	WAVEFORMATEX WaveForm;
	WAVEHDR WaveHdr;
	WaveForm.nBlockAlign = 2;
	WaveForm.nSamplesPerSec = 16000;
	WaveForm.wBitsPerSample = 16;
	WaveForm.nAvgBytesPerSec = 16000;
	WaveForm.nChannels = 1;
	WaveForm.wFormatTag = WAVE_FORMAT_PCM;
	WaveForm.cbSize = sizeof(WAVEFORMATEX);
	HANDLE wait = CreateEvent(NULL, NULL, NULL, NULL);
	waveInOpen(&hWaveIn, WAVE_MAPPER, &WaveForm, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);
	WaveHdr.dwBufferLength = *len;
	WaveHdr.lpData = (LPSTR)pReordBuf;
	WaveHdr.dwFlags = 0;
	WaveHdr.dwUser = 0;
	WaveHdr.dwLoops = 1;
	WaveHdr.dwBytesRecorded = 0;
	waveInPrepareHeader(hWaveIn, &WaveHdr, sizeof(WAVEHDR));
	waveInAddBuffer(hWaveIn, &WaveHdr, sizeof(WAVEHDR));
	waveInStart(hWaveIn);
	Sleep(3000);
	*len = WaveHdr.dwBytesRecorded;
	waveInReset(hWaveIn);
	waveInClose(hWaveIn);
}
//AI���Ѵ�����
void AI_ivw_proc(void* pReordBuf, UINT Buflen, const char* session_begin_param)
{
	const char *session_id = NULL;
	int err_code = MSP_SUCCESS;
	int	aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
	char sse_hints[128];
	if (!pReordBuf) {
		_tprintf(_T("*.AI����ʧ��,¼�����ݴ���ΪNULL!\n"));
		goto exit;
	}
	session_id = QIVWSessionBegin(NULL, session_begin_param, &err_code);
	if (err_code != MSP_SUCCESS)
	{
		_tprintf(_T("*.AI������[QIVWSessionBegin]ִ��ʧ��,�������<%d>!\n"), err_code);
		goto exit;
	}
	err_code = QIVWRegisterNotify(session_id, AI_ivw_msg_proc, NULL);
	if (err_code != MSP_SUCCESS)
	{
		_snprintf(sse_hints, sizeof(sse_hints), "QIVWRegisterNotify errorCode=%d", err_code);
		_tprintf(_T("*.AI������[QIVWRegisterNotify]ִ��ʧ��,�������<%d>!\n"), err_code);
		goto exit;
	}
	err_code = QIVWAudioWrite(session_id, pReordBuf, Buflen, aud_stat);
	if (MSP_SUCCESS != err_code)
	{
		_tprintf(_T("*.AI������[QIVWAudioWrite]ִ��ʧ��,�������<%d>!\n"), err_code);
		_snprintf(sse_hints, sizeof(sse_hints), "QIVWAudioWrite errorCode=%d", err_code);
		goto exit;
	}

exit:
	if (NULL != session_id)
	{
		QIVWSessionEnd(session_id, sse_hints);
	}
}
int AI_ivw_msg_proc(const char *sessionID, int msg, int param1, int param2, const void *info, void *userData)
{
	if (MSP_IVW_MSG_ERROR == msg) //���ѳ�����Ϣ
	{
#ifdef _DEBUG
		printf("*.AI���ѳ��� �������<%d>\n", param1);
#endif // _DEBUG
	}
	else if (MSP_IVW_MSG_WAKEUP == msg) //���ѳɹ���Ϣ
	{
#ifdef _DEBUG
		printf("*.AI�ɹ����� ������Ϣ{ %s }\n", (char*)info);
#endif // _DEBUG
		
		//pSay_Text = const_cast<char*>("����,����,��ʲô��Ҫ������˹��������?");
		//AI_Say();
		if (!IsSay)_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)AI_Say, NULL, NULL, NULL);
	}
	return 0;
}
//AI˵�����ú���
void AI_Say()
{
	IsSay = true;
	char Path[256], FileName[50];
	GetRunPath(Path, sizeof(Path));
	srand((UINT)GetTickCount64());
	wsprintfA(FileName, "audio\\tts_Say%d.wav", rand() % 5);
	strcat(Path, FileName);
	PlaySoundA(Path, NULL, SND_FILENAME | SND_SYNC);
	IsSay = false;
	IsAwaken = true;
}
