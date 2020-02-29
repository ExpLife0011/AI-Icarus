#pragma once
#ifndef _AI_text_to_speech_H
#define _AI_text_to_speech_H
#include <qivw.h>
#include <qtts.h>
#include <msp_cmn.h>
#include <msp_types.h>
#include <msp_errors.h>

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <tchar.h>

#pragma warning(disable:4996)

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

//ȡ����Ŀ¼
void GetRunPath(char* RunPathBuf, int slen);
//�ı�ת����
int _stdcall text_to_speech(const char* src_text, const char* des_path, const char* params);
//����AI����
void _stdcall Play_AI_Speech(const char* src_text, const char* des_path, bool IsASYNC = false);
#endif