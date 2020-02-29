#pragma once
#ifndef _LIB_PRCOESS_H
#define _LIB_PROCESS_H
#include <windows.h>
#include <iostream>
#include <sstream>
#include <TlHelp32.h>

namespace lib_Process
{
	class Process
	{
	public:
		Process();
		~Process();
		//PID��ȡ���ھ��
		//@parameter PID
		HWND _fastcall PidToHwnd(DWORD pid);
		//��������ȡ���ھ��
		//@parameter ������
		HWND _fastcall ProcessNameToHwnd(std::string ProcessName);
		//��������ȡ���ھ��
		//@parameter ������
		HWND _fastcall ProcessNameToHwnd(std::wstring ProcessName);
		//��������ȡPID
		//@parameter ������
		DWORD _fastcall ProcessNameToPid(std::string ProcessName);
		//��������ȡPID
		//@parameter ������
		DWORD _fastcall ProcessNameToPid(std::wstring ProcessName);
		//�������Ƿ����
		//@parameter ������
		bool _fastcall IsProcesNameExist(std::string ProcessName);
		//�������Ƿ����
		//@parameter ������
		bool _fastcall IsProcesNameExist(std::wstring ProcessName);
		//����ID�Ƿ����
		//@parameter ������
		bool _fastcall IsPidExist(DWORD pid);
		//��ȡģ���ַ
		//@parameter ������
		//@parameter ģ����
		DWORD _fastcall GetModuleAddress(std::string ProcessName, std::string ModuleName);
		//��ȡģ���ַ
		//@parameter ������
		//@parameter ģ����
		DWORD _fastcall GetModuleAddress(std::wstring ProcessName, std::wstring ModuleName);
		//��ȡģ���ַ
		//@parameter pid
		//@parameter ģ����
		DWORD _fastcall GetModuleAddress(DWORD pid, std::wstring ModuleName);
		//���̴���
		//@parameter �ļ���
		//@parameter ���в���
		//@parameter ���ؽṹ��
		bool _fastcall ProcessCreate(std::string FileName, std::string Parameter, std::string ProcessStruct);
		//���̴���
		//@parameter �ļ���
		//@parameter ���в���
		//@parameter ���ؽṹ��
		bool _fastcall ProcessCreate(std::wstring FileName, std::wstring Parameter, std::wstring ProcessStruct);
		//���̴���
		//@parameter �ļ���
		//@parameter ���в���
		void _fastcall ProcessRun(std::string FileName, std::string Parameter);
		//���̴���
		//@parameter �ļ���
		//@parameter ���в���
		void _fastcall ProcessRun(std::wstring FileName, std::wstring Parameter);
	private:

	};

}

#endif

