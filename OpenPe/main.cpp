#include "openpe.h"
#include <Windows.h>
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	argc =0;
	QApplication a(argc, NULL);
	OpenPE w;
	w.show();
	return a.exec();
}

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		{
			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main, (LPVOID)hInstance, 0, NULL);
			if (hThread != NULL)
				break;
			else
				return FALSE;
		}
	}
	return TRUE;
}
