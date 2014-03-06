//����ļ������ڸ���VCPROJ���������������ã���new����
//Ŀǰclient_front client_ui client_gameround �������������� _NEDMALLOC
//#ifdef _NEDMALLOC

#ifndef STACK_WALKER_INCLUDE_H_192083012908301294214
#define STACK_WALKER_INCLUDE_H_192083012908301294214

#include <Windows.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)


 typedef void (*voidPtrIntVoid)(void* p,int size);
 typedef void (*voidPtrVoid)(void* p);
 typedef void (*voidVoid)();
 typedef bool (*boolVoid)();
 typedef bool (*voidBool)(bool is);
 
 static voidVoid initWs = NULL;
 static voidPtrIntVoid addPtr = NULL;
 static voidPtrVoid removePtr = NULL;
 static voidVoid dumpFile = NULL;
 static boolVoid isRecord = NULL;
 static voidBool setIsRecord = NULL;
 
 bool first_new = true;


void SWAddPtr(void* p,int size)
{
	if(addPtr)
	{
		addPtr(p,size);
	}
}
void SWRemovePtr(void* p)
{
	if(removePtr)
	{
		removePtr(p);
	}
}
void SWDumpMemMap()
{
	if (dumpFile)
	{
		dumpFile();
	}
}

//��ʼ��
void InitStackWalker()
{
	if (first_new)
	{
#ifdef _DEBUG
		HINSTANCE handle = LoadLibraryA("StackWalker_d.dll");
#else
		HINSTANCE handle = LoadLibraryA("StackWalker.dll");
#endif
		 
		if(handle)
		{
		 	initWs = (voidVoid)GetProcAddress(handle,"Init");
		 	addPtr = (voidPtrIntVoid)GetProcAddress(handle,"AddPtr");
		 	removePtr = (voidPtrVoid)GetProcAddress(handle,"RemovePtr");
		 	dumpFile = (voidVoid)GetProcAddress(handle,"WriteLeak");
		 	isRecord = (boolVoid)GetProcAddress(handle,"IsRecord");
		 	setIsRecord = (voidBool)GetProcAddress(handle,"SetIsRecord");
		 	initWs();
		}
		first_new = false;
	}
}

//��/���ڴ��¼
 void SWEnableStackWalker_New(bool enable)
{
 	setIsRecord(enable);
}


#endif

#endif