//这个文件包含在各个VCPROJ里，根据情况进行设置，打开new重载
//目前client_front client_ui client_gameround 三个工程设置了 _NEDMALLOC
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

//初始化
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

//开/关内存记录
 void SWEnableStackWalker_New(bool enable)
{
 	setIsRecord(enable);
}


#endif

#endif