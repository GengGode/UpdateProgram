#pragma once
#include <QDebug>
#include <QtWidgets/QMainWindow>
#include "ui_Updater_KYJG.h"
#include <Windows.h>

class Updater_KYJG : public QMainWindow
{
    Q_OBJECT

public:
    Updater_KYJG(QWidget *parent = Q_NULLPTR);

private:
    Ui::Updater_KYJGClass ui;
private:
	//INSTANCE 
	HINSTANCE updaterDll = nullptr;
};
