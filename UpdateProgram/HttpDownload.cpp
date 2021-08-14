#include "HttpDownload.h"
#include "downloadmanager.h"
#include <QDebug>

#define UNIT_KB 1024            //KB
#define UNIT_MB 1024*1024       //MB
#define UNIT_GB 1024*1024*1024  //GB

#define TIME_INTERVAL 300       //0.3s

HttpDownload::HttpDownload(QWidget *parent)
	: QWidget(parent)
	, m_downloadManager(NULL)
	, m_url("")
	, m_timeInterval(0)
	, m_currentDownload(0)
	, m_intervalDownload(0)
{
	ui.setupUi(this);
	initWindow();
}

HttpDownload::~HttpDownload()
{

}

void HttpDownload::initWindow()
{
	ui.progressBar->setValue(0);
	connect(ui.pButtonStart, SIGNAL(clicked()), this, SLOT(onStartDownload()));
	connect(ui.pButtonStop, SIGNAL(clicked()), this, SLOT(onStopDownload()));
	connect(ui.pButtonClose, SIGNAL(clicked()), this, SLOT(onCloseDownload()));
	// ������������ʽ;
	ui.progressBar->setStyleSheet("\
                QProgressBar\
                {\
                    border-width: 0 10 0 10;\
                    border-left: 1px, gray;\
                    border-right: 1px, gray;\
                    border-image:url(:/Resources/progressbar_back.png);\
                }\
                QProgressBar::chunk\
                {\
                    border-width: 0 10 0 10;\
                    border-image:url(:/Resources/progressbar.png);\
                }");
}

// ��ʼ����;
void HttpDownload::onStartDownload()
{
	// �ӽ����ȡ��������;
	m_url = ui.downloadUrl->text();
	if (m_downloadManager == NULL)
	{
		m_downloadManager = new DownLoadManager(this);
		connect(m_downloadManager, SIGNAL(signalDownloadProcess(qint64, qint64)), this, SLOT(onDownloadProcess(qint64, qint64)));
		connect(m_downloadManager, SIGNAL(signalReplyFinished(int)), this, SLOT(onReplyFinished(int)));
	}

	// �����Ȼ�ȡ��m_downloadManager�е�url�뵱ǰ��m_url �Աȣ����url������Ҫ���ò���,��ֹ�ļ����ز�ȫ;
	QString url = m_downloadManager->getDownloadUrl();
	if (url != m_url)
	{
		m_downloadManager->reset();
	}
	m_downloadManager->setDownInto(true);
	m_downloadManager->downloadFile(m_url, "F:/HttpDownload/MyDownloadFile.zip");
	m_timeRecord.start();
	m_timeInterval = 0;
	ui.labelStatus->setText(QStringLiteral("��������"));
}

// ��ͣ����;
void HttpDownload::onStopDownload()
{
	ui.labelStatus->setText(QStringLiteral("ֹͣ����"));
	if (m_downloadManager != NULL)
	{
		m_downloadManager->stopDownload();
	}
	ui.labelSpeed->setText("0 KB/S");
	ui.labelRemainTime->setText("0s");
}

// �ر�����;
void HttpDownload::onCloseDownload()
{
	m_downloadManager->closeDownload();
	ui.progressBar->setValue(0);
	ui.labelSpeed->setText("0 KB/S");
	ui.labelRemainTime->setText("0s");
	ui.labelStatus->setText(QStringLiteral("�ر�����"));
	ui.labelCurrentDownload->setText("0 B");
	ui.labelFileSize->setText("0 B");
}

// �������ؽ���;
void HttpDownload::onDownloadProcess(qint64 bytesReceived, qint64 bytesTotal)
{
	// �����ǰ���ؽ���;
	// �õ�������Ҫע���0����;
	qDebug() << QString("%1").arg(bytesReceived * 100 / bytesTotal + 1);
	// ���½�����;
	ui.progressBar->setMaximum(bytesTotal);
	ui.progressBar->setValue(bytesReceived);

	// m_intervalDownload Ϊ�´μ����ٶ�֮ǰ�������ֽ���;
	m_intervalDownload += bytesReceived - m_currentDownload;
	m_currentDownload = bytesReceived;

	uint timeNow = m_timeRecord.elapsed();

	// ����0.3s���¼���һ���ٶ�;
	if (timeNow - m_timeInterval > TIME_INTERVAL)
	{
		qint64 ispeed = m_intervalDownload * 1000 / (timeNow - m_timeInterval);
		QString strSpeed = transformUnit(ispeed, true);
		ui.labelSpeed->setText(strSpeed);
		// ʣ��ʱ��;
		qint64 timeRemain = (bytesTotal - bytesReceived) / ispeed;
		ui.labelRemainTime->setText(transformTime(timeRemain));

		ui.labelCurrentDownload->setText(transformUnit(m_currentDownload));
		ui.labelFileSize->setText(transformUnit(bytesTotal));

		m_intervalDownload = 0;
		m_timeInterval = timeNow;
	}
}

// �������;
void HttpDownload::onReplyFinished(int statusCode)
{
	// ����״̬���жϵ�ǰ�����Ƿ����;
	if (statusCode >= 200 && statusCode < 400)
	{
		qDebug() << "Download Failed";
	}
	else
	{
		qDebug() << "Download Success";
	}
}

// ת����λ;
QString HttpDownload::transformUnit(qint64 bytes, bool isSpeed)
{
	QString strUnit = " B";
	if (bytes <= 0)
	{
		bytes = 0;
	}
	else if (bytes < UNIT_KB)
	{
	}
	else if (bytes < UNIT_MB)
	{
		bytes /= UNIT_KB;
		strUnit = " KB";
	}
	else if (bytes < UNIT_GB)
	{
		bytes /= UNIT_MB;
		strUnit = " MB";
	}
	else if (bytes > UNIT_GB)
	{
		bytes /= UNIT_GB;
		strUnit = " GB";
	}

	if (isSpeed)
	{
		strUnit += "/S";
	}
	return QString("%1%2").arg(bytes).arg(strUnit);
}

// ת��ʱ��;
QString HttpDownload::transformTime(qint64 seconds)
{
	QString strValue;
	QString strSpacing(" ");
	if (seconds <= 0)
	{
		strValue = QString("%1s").arg(0);
	}
	else if (seconds < 60)
	{
		strValue = QString("%1s").arg(seconds);
	}
	else if (seconds < 60 * 60)
	{
		int nMinute = seconds / 60;
		int nSecond = seconds - nMinute * 60;

		strValue = QString("%1m").arg(nMinute);

		if (nSecond > 0)
			strValue += strSpacing + QString("%1s").arg(nSecond);
	}
	else if (seconds < 60 * 60 * 24)
	{
		int nHour = seconds / (60 * 60);
		int nMinute = (seconds - nHour * 60 * 60) / 60;
		int nSecond = seconds - nHour * 60 * 60 - nMinute * 60;

		strValue = QString("%1h").arg(nHour);

		if (nMinute > 0)
			strValue += strSpacing + QString("%1m").arg(nMinute);

		if (nSecond > 0)
			strValue += strSpacing + QString("%1s").arg(nSecond);
	}
	else
	{
		int nDay = seconds / (60 * 60 * 24);
		int nHour = (seconds - nDay * 60 * 60 * 24) / (60 * 60);
		int nMinute = (seconds - nDay * 60 * 60 * 24 - nHour * 60 * 60) / 60;
		int nSecond = seconds - nDay * 60 * 60 * 24 - nHour * 60 * 60 - nMinute * 60;

		strValue = QString("%1d").arg(nDay);

		if (nHour > 0)
			strValue += strSpacing + QString("%1h").arg(nHour);

		if (nMinute > 0)
			strValue += strSpacing + QString("%1m").arg(nMinute);

		if (nSecond > 0)
			strValue += strSpacing + QString("%1s").arg(nSecond);
	}

	return strValue;
}