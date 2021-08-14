#include "UpdateProgram.h"

UpdateProgram::UpdateProgram(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	
	this->setWindowFlags(Qt::FramelessWindowHint);

	setAttribute(Qt::WA_TranslucentBackground, true);  // 背景透明
	QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
	shadow->setOffset(0, 0);
	shadow->setColor(QColor(15, 15, 15, 50));
	shadow->setBlurRadius(15);
	ui.widget_Main->setGraphicsEffect(shadow);

	ui.MainPage->setCurrentIndex(0);

	connect(ui.pushButton_Exit, &QPushButton::clicked, this, &UpdateProgram::Exit);
	connect(ui.pushButton_Mini, &QPushButton::clicked, this, &UpdateProgram::Mini);

	step0_Download();
}

VersionNumber UpdateProgram::getUrlVersion()
{	
	QString filePath = QApplication::applicationDirPath() + "/download/";
	QString fileName = url_Version.fileName();
	QString savePath = filePath + fileName;

	QString line="0.0.0";
	QFile readVer(savePath);
	if (!readVer.open(QIODevice::ReadOnly | QIODevice::Text))
	{

	}
	else
	{
		line = readVer.readLine();
	}
	return VersionNumber(line);
}

void UpdateProgram::Exit()
{
	if (0)
	{

	}
	else
	{
		this->close();
	}
}

void UpdateProgram::Mini()
{
	this->showMinimized();
}

void UpdateProgram::Start()
{

}

void UpdateProgram::ReTry()
{

}

void UpdateProgram::step0_Download()
{
	updataVer = new UpdataModule();
	updataVer->setData(url_Version);
	connect(updataVer, &UpdataModule::finish, this, &UpdateProgram::step1_Download);
	updataVer->getFile();

	QString myVersionFilePath =  QApplication::applicationDirPath().section("/", 0, -2) + "/version.tag";

	QFile myVersionFile(myVersionFilePath);
	if (!myVersionFile.open(QIODevice::ReadOnly | QIODevice::Text)) 
	{
		this->close();
	}
	QString line = myVersionFile.readLine();
	myVersion = VersionNumber(line);
}

void UpdateProgram::step1_Download()
{
	ui.MainPage->setCurrentIndex(0);
	if (getUrlVersion() > myVersion)
	{
		this->show();

		updataPkg = new UpdataModule();
		updataPkg->setData(url_Pkg);
		connect(updataPkg, &UpdataModule::finish, this, &UpdateProgram::step2_Unzip);
		connect(updataPkg, &UpdataModule::updateProgress, this, &UpdateProgram::step1_Download_Process);
		connect(ui.progressBar, &QProgressBar::valueChanged, this, &UpdateProgram::step1_Download_TextProcess);

		updataPkg->getFile();

	}
	else
	{
		emit this->close();
	}
}

void UpdateProgram::step1_Download_Process(qint64 bytesSent, qint64 bytesTotal)
{
	if (ui.progressBar->value() > (int)(100.0 * bytesSent / bytesTotal))
	{
		ui.progressBar->setValue((int)(100.0 * bytesSent / bytesTotal));
		return;
	}

	QPropertyAnimation *animation = new QPropertyAnimation(ui.progressBar, "value");
	animation->setDuration(300);
	animation->setStartValue(ui.progressBar->value());
	animation->setEndValue((int)(100.0 * bytesSent / bytesTotal));
	animation->setEasingCurve(QEasingCurve::InOutQuad);
	animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void UpdateProgram::step1_Download_TextProcess(int value)
{
	ui.label_Text->setText(QString::fromLocal8Bit("少女祈祷中 ") + QString::number(value) + "%...");
}

void UpdateProgram::step2_Unzip()
{
	ui.MainPage->setCurrentIndex(0);
	ui.MainPage->show();

	QString SourcePath = QApplication::applicationDirPath() + "/download/source.7z";
	UnzipPath = QApplication::applicationDirPath().section("/", 0, -2);

	SourcePath= "\"" + SourcePath + "\"";
	UnzipPath = "\"" + UnzipPath + "\"";

	installPkg = new Process7zWorker(NULL);
	installPkg->setZipFilePath(SourcePath);
	installPkg->setUnZipFilePath(UnzipPath);

	connect(installPkg, &Process7zWorker::unZipError, this, &UpdateProgram::step2_Unzip_Error);
	connect(installPkg, &Process7zWorker::unZipProcess, this, &UpdateProgram::step2_Unzip_Process);
	connect(installPkg, &Process7zWorker::unZipFinished, this, &UpdateProgram::step2_Unzip_Finished);
	connect(ui.progressBar, &QProgressBar::valueChanged, this, &UpdateProgram::step2_Unzip_TextProcess);

	emit installPkg->start();

	bool res = false;
}

void UpdateProgram::step2_Unzip_Error(int errorCode)
{
	switch (errorCode)
	{
	case 0:
	{
		break;
	}
	case 1:
	{
		break;
	}
	case 2:
	{
		break;
	}
	}
}

void UpdateProgram::step2_Unzip_Process(int value)
{
	if (ui.progressBar->value() > value)
	{
		ui.progressBar->setValue(value);
		return;
	}

	QPropertyAnimation *animation = new QPropertyAnimation(ui.progressBar, "value");
	animation->setDuration(300);
	animation->setStartValue(ui.progressBar->value());
	animation->setEndValue(value);
	animation->setEasingCurve(QEasingCurve::InOutQuad);
	animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void UpdateProgram::step2_Unzip_TextProcess(int value)
{
	ui.label_Text->setText(QString::fromLocal8Bit("女神降临中 ") + QString::number(value) + "%...");
}

void UpdateProgram::step2_Unzip_Finished()
{
	emit step3_Start();
}

void UpdateProgram::step3_Start()
{
	ui.MainPage->setCurrentIndex(1);
	ui.MainPage->show();
	connect(ui.pushButton_Start, &QPushButton::clicked, this, &UpdateProgram::step3_Start_RunLauncher);
}

void UpdateProgram::step3_Start_RunLauncher()
{
	QString command = "\"" + QApplication::applicationDirPath().section("/", 0, -2) + ExportName + "\"";
	TCHAR szCmdLine[1024] = {};

	command.toWCharArray(szCmdLine);
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	PROCESS_INFORMATION pi;

	bool res = CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	if (res != true)
	{
		int k = GetLastError();
	}
	this->close();
}


