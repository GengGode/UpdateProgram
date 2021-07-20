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

	//myVersion = QString("1.1.0");

	updataVer = new UpdataModule();
	updataVer->setData(url_Version);
	connect(updataVer, &UpdataModule::finish, this, &UpdateProgram::step0_Download);
	updataVer->getFile();

	//this->hide();
	//this->step0_Download();
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
}

void UpdateProgram::Mini()
{
}

void UpdateProgram::Start()
{
}

void UpdateProgram::ReTry()
{
}

void UpdateProgram::step0_Download()
{
	ui.MainPage->setCurrentIndex(0);
	if (getUrlVersion() > myVersion)
	{
		this->show();
#ifdef _DEBUG
		step1_Download();
#else

		updataPkg = new UpdataModule();
		updataPkg->setData(url_Pkg);
		connect(updataPkg, &UpdataModule::finish, this, &UpdateProgram::step1_Download);
		updataPkg->getFile();
#endif
	}
	else
	{
		emit this->close();
	}
}

void UpdateProgram::step1_Download()
{
	ui.MainPage->setCurrentIndex(0);
	ui.MainPage->show();

	QString SourcePath = QApplication::applicationDirPath() + "/download/source.7z";

	installPkg = new Process7zWorker(NULL);
	installPkg->setZipFilePath(SourcePath);
#ifdef _DEBUG
	installPkg->setUnZipFilePath(QApplication::applicationDirPath() + "/source/");
#else
	installPkg->setUnZipFilePath(UnzipPath);
#endif

	connect(installPkg, &Process7zWorker::unZipError, this, &UpdateProgram::step2_Unzip_Error);
	connect(installPkg, &Process7zWorker::unZipProcess, this, &UpdateProgram::step2_Unzip_Process);
	connect(installPkg, &Process7zWorker::unZipFinished, this, &UpdateProgram::step2_Unzip_Finished);
	connect(ui.progressBar, &QProgressBar::valueChanged, this, &UpdateProgram::step2_Unzip_TestProcess);

	emit installPkg->start();

	bool res = false;
}

void UpdateProgram::step2_Unzip()
{
	//unZip_7z = new Process7zWorker(NULL);
	//unZip_7z->setZipFilePath(SourcePath);
	//unZip_7z->setUnZipFilePath(InstallPath + InstallDirName);

	//connect(unZip_7z, &Process7zWorker::unZipError, this, &TianLiInstallationPackage::unZip_Error);
	//connect(unZip_7z, &Process7zWorker::unZipProcess, this, &TianLiInstallationPackage::unZip_Process);
	//connect(unZip_7z, &Process7zWorker::unZipFinished, this, &TianLiInstallationPackage::unZip_finished);

	//connect(this, &TianLiInstallationPackage::unZip, unZip_7z, &Process7zWorker::unzip);
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
	QPropertyAnimation *animation = new QPropertyAnimation(ui.progressBar, "value");
	animation->setDuration(300);
	animation->setStartValue(ui.progressBar->value());
	animation->setEndValue(value);
	animation->setEasingCurve(QEasingCurve::InOutQuad);
	animation->start(QAbstractAnimation::DeleteWhenStopped);

}

void UpdateProgram::step2_Unzip_TestProcess(int value)
{
	ui.label_Text->setText(QString::fromLocal8Bit("少女祈祷中 ") + QString::number(value) + "%...");
}

void UpdateProgram::step2_Unzip_Finished()
{
	emit step3_Start();
}

void UpdateProgram::step3_Start()
{
	ui.MainPage->setCurrentIndex(1);
	ui.MainPage->show();
}


