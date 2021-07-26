#include "Updater_KYJG.h"

Updater_KYJG::Updater_KYJG(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	updaterDll = LoadLibraryA("Updater.dll");
	int k= GetLastError();
	typedef char*(WINAPI *GetLocalMD5_fptr)(void);
	typedef void(WINAPI *StartUpdate_fptr)(char*);
	GetLocalMD5_fptr GetLocalMD5 = (GetLocalMD5_fptr)GetProcAddress(updaterDll, "GetLocalMD5");
	StartUpdate_fptr StartUpdate = (StartUpdate_fptr)GetProcAddress(updaterDll, "StartUpdate");

	if (GetLocalMD5 == nullptr)
	{
		qDebug() << "not failt 1";
	}
	if (StartUpdate == nullptr)
	{

		qDebug() << "not failt 2";
	}
	qDebug() << "Ok";


}
