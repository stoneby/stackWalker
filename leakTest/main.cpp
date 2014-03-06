

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>

#include <StackWalker\stack_wakler_include.h>

using namespace std;

void* operator new(size_t size)
{
	void* p1=malloc(size);
	SWAddPtr(p1,size);
	return malloc(size);
}
void operator delete(void* p)
{
	SWRemovePtr(p);
	free(p);
}

int main()
{

	InitStackWalker();
	
	SWEnableStackWalker_New(true);

	int* a=new int();
	delete a;

	SWDumpMemMap();
	SWEnableStackWalker_New(false);


	return 1;
}





