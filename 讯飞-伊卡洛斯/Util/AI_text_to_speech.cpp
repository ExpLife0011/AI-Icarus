#include "AI_text_to_speech.h"

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

int _stdcall text_to_speech(const char* src_text, const char* des_path, const char* params)
{
	int          ret = -1;
	FILE*        fp = NULL;
	const char*  sessionID = NULL;
	unsigned int audio_len = 0;
	wave_pcm_hdr wav_hdr = default_wav_hdr;
	int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;
	const char* session_begin_param = "voice_name = x_xiaofang, text_encoding = gb2312, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";
	if (!params)params = session_begin_param;
	if (NULL == src_text || NULL == des_path)
	{
		_tprintf(_T("*.AI���緢��ʧ��,AI�����ı�ΪNULL.\n"));
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
	//_tprintf(_T("*.AI����˼��˵Щʲô"));
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
		//_tprintf(_T("."));
		Sleep(20); //��ֹƵ��ռ��CPU
	}
	//_tprintf(_T("\n"));
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

void _stdcall Play_AI_Speech(const char* src_text, const char* des_path, bool IsASYNC)
{
	SYSTEMTIME ptime;
	GetLocalTime(&ptime);
	int ret = text_to_speech(src_text, des_path, NULL);
	if (MSP_SUCCESS != ret)
	{
		_tprintf(_T("*.AI���緢��ʧ��,�������<%d>\n"), ret);
	}
	char Path[256];
	GetRunPath(Path, sizeof(Path));
	strcat(Path, des_path);
	printf("*.AI����˵< %s >\n", src_text);
	if (IsASYNC) {
		PlaySoundA(Path, NULL, SND_FILENAME | SND_ASYNC);
	}
	else {
		PlaySoundA(Path, NULL, SND_FILENAME | SND_SYNC);
	}
	_tprintf(_T("*.AI���﷢�����\n"));
}