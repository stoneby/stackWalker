// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"

#include "MyStack.h"
#include <stdio.h>
#include <string>
#include <map>

void DumpMemoryUse();

enum
{
	MEMORY_LEAK = 0,
	MEMORY_USE = 1,
};

int _MemoryMode = MEMORY_LEAK;

//999��ʾ�������������е�ģ�鶼ͳ��
//1000��ʾͳ������ģ���������
int _CurModuleID = 1000;

// enum StatisticsMode 
// {
// 	ALL_INCLUDE_ENGINE = 1000,
// 	ALL_NOT_ENGINE = 999,
// 	ENGINE = 51,
// };

///��intת����char��
inline std::string ItoA(int i)
{
	char buff[32];
	sprintf(buff, "%d", i);
	return buff;
}

CRITICAL_SECTION _Cs; //����һ���������



typedef std::map<void*, std::list<STACKFRAME64*>* > MemoryMap;
typedef std::map<void*, std::list<STACKFRAME64*>* >::iterator MemoryMapIter;
typedef std::map<void*, int> SizeMapDef;
typedef std::map<void*, int>::iterator SizeMapIter;
MemoryMap PtrMap;
SizeMapDef	SizeMap;
MyStackWalker* _StackWalker = NULL;

bool _IsRecord = false;
int _CurNewTimes = 0;
int _RecordNewTimes = 0;
int _CurFileIndex  = 0;

BOOL APIENTRY DllMain( HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	_CurModuleID = (int)GetPrivateProfileInt("Module","ID",1000,"./stack_walker_module.ini");
	if(_StackWalker == NULL)
	{
		_StackWalker = new MyStackWalker();
	}
	InitializeCriticalSection(&_Cs); //����ʼ������ 

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void ReleaseFrames(std::list<STACKFRAME64*>* frames)
{
	std::list<STACKFRAME64*>::iterator it = frames->begin();
	std::list<STACKFRAME64*>::iterator end = frames->end();
	for (; it != end; ++it)
	{
		STACKFRAME64* ptr = *it;
		_StackWalker->ReleaseSTACKFRAME64(ptr);
	}
	frames->clear();
}

extern "C" __declspec(dllexport) bool IsRecord()
{
	return _IsRecord;
}

extern "C" __declspec(dllexport) void SetIsRecord(bool is)
{
	_IsRecord = is;
}

extern "C" __declspec(dllexport) void SetRecordMode(int mode)
{
	_MemoryMode = mode;
}

extern "C" __declspec(dllexport) void Init()
{


}

void RealseAllRecords()
{
	MemoryMapIter it = PtrMap.begin();
	MemoryMapIter end = PtrMap.end();
	for(;it != end; ++it)
	{
		ReleaseFrames(it->second);
	}
	PtrMap.clear();
	SizeMap.clear();
}


void AddPtrInternal(void* p,int size)
{
	EnterCriticalSection(&_Cs); //�����ٽ���
	std::list<STACKFRAME64*>* frames = new std::list<STACKFRAME64*>();
	_StackWalker->SetStackFrameList(frames);
	_StackWalker->ShowCallStackOnlyFrame();
	PtrMap[p] = frames;
	SizeMap[p] = size;
	if(_MemoryMode == MEMORY_USE)
	{
		++_CurNewTimes;
		if(_CurNewTimes - _RecordNewTimes >= 100000)
		{
			_RecordNewTimes = _CurNewTimes;
			DumpMemoryUse();
			RealseAllRecords();
		}
	}
	LeaveCriticalSection(&_Cs); //���ٽ���
}

// extern "C" __declspec(dllexport) void AddPtrByModule(int moduleID,void* p,int size)
// {
// 	//��ʾͳ������ģ��
// 	if(_CurModuleID == ALL_INCLUDE_ENGINE)
// 	{
// 		AddPtrInternal(p,size);
// 	}
// 	else if(_CurModuleID == ALL_NOT_ENGINE)
// 	{
// 		if(moduleID != ENGINE)
// 		{
// 			AddPtrInternal(p,size);
// 		}
// 	}
// 	else if(moduleID == _CurModuleID)
// 	{
// 		AddPtrInternal(p,size);
// 	}
// }

extern "C" __declspec(dllexport) void AddPtr(void* p,int size)
{
	AddPtrInternal(p,size);
	return;
}


extern "C" __declspec(dllexport) void DumpUsedMemoryToTile()
{
	EnterCriticalSection(&_Cs); //�����ٽ���
	_RecordNewTimes = _CurNewTimes;
	DumpMemoryUse();
	RealseAllRecords();
	LeaveCriticalSection(&_Cs); //���ٽ���
}

void RemovePtrInternal(void* p)
{
	EnterCriticalSection(&_Cs); //�����ٽ���
	if(_MemoryMode == MEMORY_LEAK)
	{
		MemoryMapIter it = PtrMap.find(p);
		if(it != PtrMap.end())
		{
			ReleaseFrames(it->second);
			PtrMap.erase(it);
		}
		SizeMapIter sizeIt = SizeMap.find(p);
		if(sizeIt != SizeMap.end())
		{
			SizeMap.erase(sizeIt);
		}
	}
	else
	{
		//ʲô������
	}
	LeaveCriticalSection(&_Cs); //���ٽ���
}

// extern "C" __declspec(dllexport) void RemovePtrByModule(int moduleID, void* p)
// {
// 	if(moduleID == _CurModuleID)
// 	{
// 		RemovePtrInternal(p);
// 	}
// }

extern "C" __declspec(dllexport) void RemovePtr(void* p)
{
	RemovePtrInternal(p);
	return;
}




void WriteToFile(FILE* file)
{
	MemoryMapIter it = PtrMap.begin();
	MemoryMapIter end = PtrMap.end();
	int totalMem = 0;
	int i = 0;
	for (; it != end; ++it)
	{
		std::list<STACKFRAME64*>* frames = it->second;
		std::string result;
		_StackWalker->SetOutStr(&result);
		SizeMapIter sizeIt = SizeMap.find(it->first);
		char tmp[20];
		itoa(sizeIt->second,tmp,10);
		result += tmp;
		result += "\n";
		totalMem += sizeIt->second;
		_StackWalker->FlushStackFrams((void*)frames);
		fwrite(result.c_str(),result.size(),1,file);
		fwrite("\n",1,1,file);
		++i;
		if(i >= 1000)
		{
			fflush(file);
			i=0;
		}
	}
	std::string totalStr = "total size:";
	char tmp[20];
	itoa(totalMem,tmp,10);
	totalStr += tmp;
	fwrite(totalStr.c_str(),totalStr.size(),1,file);
	fflush(file);
	fclose(file);
}

void DumpMemoryUse()
{
	std::string s = "Memory/Memory_" + ItoA(_CurFileIndex) + ".txt";
	++_CurFileIndex;
	FILE* file = fopen(s.c_str(),"w+");
	WriteToFile(file);
}

extern "C" __declspec(dllexport) void WriteLeak()
{

	FILE* file = NULL;
	if(file == NULL)
	{
		file = fopen("MemoryLeak.txt","w+");
	}
	WriteToFile(file);
}