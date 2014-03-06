#pragma once
#include "StackWalker.h"
#include <stdio.h>
#include <string>
#include <list>

class STACKFRAME64;
// Simple implementation of an additional output to the console:
class MyStackWalker : public StackWalker
{
public:
	MyStackWalker() : StackWalker() 
	{

	}
	MyStackWalker(DWORD dwProcessId, HANDLE hProcess) : StackWalker(dwProcessId, hProcess) {}

	void SetOutStr(std::string* str)
	{
		m_Str = str;
	}

	void SetStackFrameList(std::list<STACKFRAME64*>* list)
	{
		m_Frames = list;
	}
	virtual void OnCallStackEntryOnlyFrame(void* frame)
	{
		m_Frames->push_back((STACKFRAME64*)frame);
	}
	virtual void OnOutput(LPCSTR szText) 
	{
		(*m_Str) += szText;
		//fwrite(szText,strlen(szText),1,m_File);
		//fflush(m_File);
		//printf(szText); 
		//StackWalker::OnOutput(szText); 
	}
private:
	FILE* m_File;
	std::string* m_Str;
	std::list<STACKFRAME64*>* m_Frames;
};