#pragma once
#include <QString>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QtWidgets/QMainWindow>
#include "ui_UpdateProgram.h"
#include "UpdataModule.h"
#include "Process7zWorker.h"
#include "VersionNumber.h"
//#include "QtWidgetsMessageBox2.h"

class UpdateProgram : public QWidget
{
    Q_OBJECT

public:
    UpdateProgram(QWidget *parent = Q_NULLPTR);

private:
    Ui::UpdateProgramClass ui;

private:
	QUrl url_Version = "https://github.com/GengGode/GenshinImpactNaturalLaw/releases/latest/download/version.tag";
	QUrl url_Pkg = "https://github.com/GengGode/GenshinImpactNaturalLaw/releases/latest/download/source.7z";

	UpdataModule *updataVer;
	UpdataModule *updataPkg;
	Process7zWorker *installPkg;

	VersionNumber myVersion = VersionNumber(QString("1.0.0"));

	QString ExportName = "/TianLi_Launcher.exe";
	QString UnzipPath = "../";

private:
	VersionNumber getUrlVersion();

private slots:
	void Exit();
	void Mini();
	void Start();
	void ReTry();

	void step0_Download();
	//void step0_get();

	void step1_Download();
	void step1_Download_Process(qint64 bytesSent, qint64 bytesTotal);
	void step1_Download_TextProcess(int value);

	void step2_Unzip();

	void step2_Unzip_Error(int errorCode);
	void step2_Unzip_Process(int value);
	void step2_Unzip_TextProcess(int value);
	void step2_Unzip_Finished();

	void step3_Start();
	void step3_Start_RunLauncher();

	//void errorPage();
};
