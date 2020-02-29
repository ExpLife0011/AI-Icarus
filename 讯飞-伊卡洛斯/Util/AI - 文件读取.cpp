#include "AI.h"

#define IVW_AUDIO_FILE_NAME "audio\\awake2.pcm"
#define FRAME_LEN	640 //16k�����ʵ�16bit��Ƶ��һ֡�Ĵ�СΪ640B, ʱ��20ms

/* wav��Ƶͷ����ʽ */
typedef struct _wave_pcm_hdr
{
	char            riff[4];                // = "RIFF"
	int				size_8;                 // = FileSize - 8
	char            wave[4];                // = "WAVE"
	char            fmt[4];                 // = "fmt "
	int				fmt_size;				// = ��һ���ṹ��Ĵ�С : 16

	short int       format_tag;             // = PCM : 1
	short int       channels;               // = ͨ���� : 1
	int				samples_per_sec;        // = ������ : 8000 | 6000 | 11025 | 16000
	int				avg_bytes_per_sec;      // = ÿ���ֽ��� : samples_per_sec * bits_per_sample / 8
	short int       block_align;            // = ÿ�������ֽ��� : wBitsPerSample / 8
	short int       bits_per_sample;        // = ����������: 8 | 16

	char            data[4];                // = "data";
	int				data_size;              // = �����ݳ��� : FileSize - 44 
} wave_pcm_hdr;
/* Ĭ��wav��Ƶͷ������ */
wave_pcm_hdr default_wav_hdr =
{
	{ 'R', 'I', 'F', 'F' },
	0,
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	16,
	1,
	1,
	16000,
	32000,
	2,
	16,
	{'d', 'a', 't', 'a'},
	0
};
//˵���ı�
char *pSay_Text = nullptr;
//�Ƿ���
bool IsAwaken = false;
bool InstallAI()
{
	const char* login_params = "appid = 5e468965,work_dir = .";
	if (MSPLogin(NULL, NULL, login_params) != MSP_SUCCESS)return false;
	_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)AIAwakenThread, NULL, NULL, NULL);
	return true;
}
bool UninstallAI()
{
	if (MSPLogout() != MSP_SUCCESS)return false;
	return true;
}

void AIAwakenThread()
{
	const char *ssb_param = "ivw_threshold=0:1050,sst=wakeup,ivw_res_path =fo|res/ivw/wakeupresource.jet";
	char Path[256];
	while (true)
	{
		if (!IsAwaken) {
			int buflen = 1024 * 100;
			void* pReordBuf = malloc(buflen);
			if (!pReordBuf) {
				_tprintf(_T("*.�ڴ�����ʧ��,��ȷ���ڴ��Ƿ����!\n"));
				break;
			}
			ZeroMemory(pReordBuf, buflen);
			GetReord(pReordBuf, buflen);
			/*GetRunPath(Path, sizeof(Path));
			strcat(Path, IVW_AUDIO_FILE_NAME);
			FILE *pFile = fopen(Path, "wb");
			if (!pFile) {
				_tprintf(_T("*.[audio\\awake.pcm]�ļ�����ʧ��!\n"));
				continue;
			}
			fwrite(pReordBuf, 1, buflen, pFile);
			fclose(pFile);*/
			AI_ivw_proc(NULL, Path, ssb_param, pReordBuf, buflen);
			free(pReordBuf);
		}
		Sleep(1);
	}
}
void AIReordThread()
{
	const char* session_begin_params = "sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = gb2312";
	char Path[256];
	while (true)
	{
		if (IsAwaken) {
			int buflen = 1024 * 100;
			void* pReordBuf = malloc(buflen);
			if (!pReordBuf) {
				_tprintf(_T("*.�ڴ�����ʧ��,��ȷ���ڴ��Ƿ����!\n"));
				break;
			}
			ZeroMemory(pReordBuf, buflen);
			GetReord(pReordBuf, buflen);
			GetRunPath(Path, sizeof(Path));
			strcat(Path, IVW_AUDIO_FILE_NAME);
			FILE *pFile = fopen(Path, "wb");
			if (!pFile) {
				_tprintf(_T("*.[audio\\awake.pcm]�ļ�����ʧ��!\n"));
				continue;
			}
			fwrite(pReordBuf, 1, buflen, pFile);
			fclose(pFile);
			
			free(pReordBuf);
		}
		Sleep(1);
	}
}
void GetReord(void* pReordBuf, int len)
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
	WaveHdr.dwBufferLength = len;
	WaveHdr.lpData = (LPSTR)pReordBuf;
	WaveHdr.dwFlags = 0;
	WaveHdr.dwUser = 0;
	WaveHdr.dwLoops = 1;
	WaveHdr.dwBytesRecorded = 0;
	waveInPrepareHeader(hWaveIn, &WaveHdr, sizeof(WAVEHDR));
	waveInAddBuffer(hWaveIn, &WaveHdr, sizeof(WAVEHDR));
	waveInStart(hWaveIn);
	Sleep(1000);
	waveInReset(hWaveIn);
	waveInClose(hWaveIn);
}

void AI_ivw_proc(const char *grammar_list, const char* audio_filename, const char* session_begin_params , void *buf, int len)
{
	const char *session_id = NULL;
	int err_code = MSP_SUCCESS;
	FILE *f_aud = NULL;
	long audio_size = 0;
	long real_read = 0;
	long audio_count = 0;
	int count = 0;
	int audio_stat = MSP_AUDIO_SAMPLE_CONTINUE;
	char *audio_buffer = NULL;
	char sse_hints[128];
	if (NULL == audio_filename)
	{
		_tprintf(_T("*.[audio\\awake.pcm]�ļ�����ΪNULL!\n"));
		return;
	}

	/*f_aud = fopen(audio_filename, "rb");
	if (NULL == f_aud)
	{
		_tprintf(_T("*.[audio\\awake.pcm]�ļ���ʧ��!\n"));
		return;
	}
	fseek(f_aud, 0, SEEK_END);
	audio_size = ftell(f_aud);
	fseek(f_aud, 0, SEEK_SET);
	audio_buffer = (char *)malloc(audio_size);
	if (NULL == audio_buffer)
	{
		_tprintf(_T("*.�ڴ�����ʧ��,��ȷ���ڴ��Ƿ����!\n"));
		goto exit;
	}
	real_read = fread((void *)audio_buffer, 1, audio_size, f_aud);
	if (real_read != audio_size)
	{
		_tprintf(_T("*.[audio\\awake.pcm]�ļ����ݶ�ȡʧ��!\n"));
		goto exit;
	}
*/

	session_id = QIVWSessionBegin(grammar_list, session_begin_params, &err_code);
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
	err_code = QIVWAudioWrite(session_id, buf, len, audio_stat);
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
	if (NULL != f_aud)
	{
		fclose(f_aud);
	}
	if (NULL != audio_buffer)
	{
		free(audio_buffer);
	}
}

int AI_ivw_msg_proc(const char *sessionID, int msg, int param1, int param2, const void *info, void *userData)
{
	if (MSP_IVW_MSG_ERROR == msg) //���ѳ�����Ϣ
	{
		printf("*.AI���ѳ��� �������<%d>\n", param1);
	}
	else if (MSP_IVW_MSG_WAKEUP == msg) //���ѳɹ���Ϣ
	{
		printf("*.AI�ɹ����� ������Ϣ{ %s }\n", (char*)info);
		pSay_Text = const_cast<char*>("����,����,��ʲô��Ҫ������˹��������?");
		_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)AI_Say, NULL, NULL, NULL);
	}
	return 0;
}
void GetRunPath(char* RunPathBuf, int slen)
{
	GetModuleFileNameA(GetModuleHandle(NULL), RunPathBuf, slen);
	char* pchar = strrchr(RunPathBuf, '\\');
	if (pchar)pchar[1] = 0;
}

void AI_Say()
{
	const char* session_begin_params = "voice_name = x_xiaofang, text_encoding = gb2312, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";
	const char* filename = "audio\\tts_Say.wav"; //�ϳɵ������ļ�����
	int ret = text_to_speech(pSay_Text, filename, session_begin_params);
	if (MSP_SUCCESS != ret)
	{
		_tprintf(_T("*.AI���緢��ʧ��,�������<%d>\n"), ret);
	}
	printf("*.AI����˵< %s >\n", pSay_Text);
	pSay_Text = nullptr;
	char Path[256];
	GetRunPath(Path, sizeof(Path));
	strcat(Path, filename);
	PlaySoundA(Path, NULL, SND_FILENAME | SND_SYNC);
	_tprintf(_T("*.AI���﷢�����\n"), pSay_Text);
	IsAwaken = true;
}
int text_to_speech(const char* src_text, const char* des_path, const char* params)
{
	int          ret = -1;
	FILE*        fp = NULL;
	const char*  sessionID = NULL;
	unsigned int audio_len = 0;
	wave_pcm_hdr wav_hdr = default_wav_hdr;
	int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;

	if (NULL == src_text || NULL == des_path)
	{
		_tprintf(_T("*.AI���緢��ʧ��,AI�����ı�ΪNULL.\n"), ret);
		return ret;
	}
	fp = fopen(des_path, "wb");
	if (NULL == fp)
	{
		printf("*.AI���緢��ʧ��,��[ %s ]�ļ�ʧ��.\n", des_path);
		return ret;
	}
	/* ��ʼ�ϳ� */
	sessionID = QTTSSessionBegin(params, &ret);
	if (MSP_SUCCESS != ret)
	{
		_tprintf(_T("*.AI������[QTTSSessionBegin]ִ��ʧ��,�������<%d>!\n"), ret);
		fclose(fp);
		return ret;
	}
	ret = QTTSTextPut(sessionID, src_text, (unsigned int)strlen(src_text), NULL);
	if (MSP_SUCCESS != ret)
	{
		_tprintf(_T("*.AI������[QTTSTextPut]ִ��ʧ��,�������<%d>!\n"), ret);
		QTTSSessionEnd(sessionID, "TextPutError");
		fclose(fp);
		return ret;
	}
	_tprintf(_T("*.AI����˼��˵Щʲô"), ret);
	fwrite(&wav_hdr, sizeof(wav_hdr), 1, fp); //���wav��Ƶͷ��ʹ�ò�����Ϊ16000
	while (1)
	{
		/* ��ȡ�ϳ���Ƶ */
		const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
		if (MSP_SUCCESS != ret)
			break;
		if (NULL != data)
		{
			fwrite(data, audio_len, 1, fp);
			wav_hdr.data_size += audio_len; //����data_size��С
		}
		if (MSP_TTS_FLAG_DATA_END == synth_status)
			break;
		_tprintf(_T("."));
		Sleep(200); //��ֹƵ��ռ��CPU
	}
	_tprintf(_T("\n"));
	if (MSP_SUCCESS != ret)
	{
		_tprintf(_T("*.AI������[QTTSAudioGet]ִ��ʧ��,�������<%d>!\n"), ret);
		QTTSSessionEnd(sessionID, "AudioGetError");
		fclose(fp);
		return ret;
	}
	/* ����wav�ļ�ͷ���ݵĴ�С */
	wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);

	/* ��������������д���ļ�ͷ��,��Ƶ�ļ�Ϊwav��ʽ */
	fseek(fp, 4, 0);
	fwrite(&wav_hdr.size_8, sizeof(wav_hdr.size_8), 1, fp); //д��size_8��ֵ
	fseek(fp, 40, 0); //���ļ�ָ��ƫ�Ƶ��洢data_sizeֵ��λ��
	fwrite(&wav_hdr.data_size, sizeof(wav_hdr.data_size), 1, fp); //д��data_size��ֵ
	fclose(fp);
	fp = NULL;
	/* �ϳ���� */
	ret = QTTSSessionEnd(sessionID, "Normal");
	if (MSP_SUCCESS != ret)
	{
		_tprintf(_T("*.AI������[QTTSSessionEnd]ִ��ʧ��,�������<%d>!\n"), ret);
	}

	return ret;
}