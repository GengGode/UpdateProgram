#pragma once
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>
#include <QObject>

class HttpDownload :public QObject
{
	Q_OBJECT
public:
	HttpDownload(QWidget *parent);
	QNetworkAccessManager m_downloadManager;
	QString m_url;
	int m_timeInterval;
	int m_currentDownload;
	int m_intervalDownload;
};

//#include "downloadmanager.h"
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

#define DOWNLOAD_FILE_SUFFIX    "_tmp"

class DownLoadManager :public QObject
{
	Q_OBJECT
public:
	DownLoadManager(QObject *parent);
private:
	QNetworkAccessManager *m_networkManager;
	bool m_isSupportBreakPoint;
	QUrl m_url;
	bool m_isStop;
	QString m_fileName;
	int m_bytesCurrentReceived;
	QNetworkReply *m_reply;
	qint64 m_bytesReceived ;
	qint64 m_bytesTotal ;
public:
	void setDownInto(bool isSupportBreakPoint);
	QString getDownloadUrl();
	void downloadFile(QString url, QString fileName);
	//removeFile(m_fileName);
	void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

signals:
	void signalDownloadProcess( qint64 v, qint64 u);
}


DownLoadManager::DownLoadManager(QObject *parent)
	: QObject(parent)
	//, m_networkManager(NULL)
	//, m_url(QUrl(""))
	//, m_fileName("")
	//, m_isSupportBreakPoint(false)
	//, m_bytesReceived(0)
	//, m_bytesTotal(0)
	//, m_bytesCurrentReceived(0)
	//, m_isStop(true)
{
	m_networkManager = new QNetworkAccessManager(this);
}

DownLoadManager::~DownLoadManager()
{}

// �����Ƿ�֧�ֶϵ�����;
void DownLoadManager::setDownInto(bool isSupportBreakPoint)
{
	m_isSupportBreakPoint = isSupportBreakPoint;
}

// ��ȡ��ǰ��������;
QString DownLoadManager::getDownloadUrl()
{
	return m_url.toString();
}

// ��ʼ�����ļ��������������Ӻ��ļ���·��;
void DownLoadManager::downloadFile(QString url, QString fileName)
{
	// ��ֹ��ε����ʼ���ذ�ť�����ж�����أ�ֻ����ֹͣ��־����Ϊtrueʱ�Ž�������;
	if (m_isStop)
	{
		m_isStop = false;
		m_url = QUrl(url);

		// ������ô�url�л�ȡ�ļ����������Ƕ����е�url����Ч;
//      QString fileName = m_url.fileName();

		// ����ǰ�ļ�������Ϊ��ʱ�ļ������������ʱ�޸Ļ���;
		m_fileName = fileName + DOWNLOAD_FILE_SUFFIX;

		// �����ǰ���ص��ֽ���Ϊ0��ô˵��δ���ع�������������
		// ����Ҫ��Ȿ���Ƿ����֮ǰ���ص���ʱ�ļ����������ɾ��
		if (m_bytesCurrentReceived <= 0)
		{
			removeFile(m_fileName);
		}

		QNetworkRequest request;
		request.setUrl(m_url);

		// ���֧�ֶϵ�����������������ͷ��Ϣ
		if (m_isSupportBreakPoint)
		{
			QString strRange = QString("bytes=%1-").arg(m_bytesCurrentReceived);
			request.setRawHeader("Range", strRange.toLatin1());
		}

		// ��������;
		m_reply = m_networkManager->get(request);

		connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));
		connect(m_reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
		connect(m_reply, SIGNAL(finished()), this, SLOT(onFinished()));
		connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
	}
}

// ���ؽ�����Ϣ;
void DownLoadManager::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	if (!m_isStop)
	{
		m_bytesReceived = bytesReceived;
		m_bytesTotal = bytesTotal;
		// �������ؽ���;(���� m_bytesCurrentReceived ��Ϊ�˶ϵ�����ʱ֮ǰ���ص��ֽ�)
		emit signalDownloadProcess(m_bytesReceived + m_bytesCurrentReceived, m_bytesTotal + m_bytesCurrentReceived);
	}
}

// ��ȡ�������ݣ����浽�ļ���;
void DownLoadManager::onReadyRead()
{
	if (!m_isStop)
	{
		QFile file(m_fileName);
		if (file.open(QIODevice::WriteOnly | QIODevice::Append))
		{
			file.write(m_reply->readAll());
		}
		file.close();
	}
}

// �������;
void DownLoadManager::onFinished()
{
	m_isStop = true;
	// http����״̬��;
	QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	if (m_reply->error() == QNetworkReply::NoError)
	{
		// ��������ʱ�ļ�;
		QFileInfo fileInfo(m_fileName);
		if (fileInfo.exists())
		{
			int index = m_fileName.lastIndexOf(DOWNLOAD_FILE_SUFFIX);
			QString realName = m_fileName.left(index);
			QFile::rename(m_fileName, realName);
		}
	}
	else
	{
		// �д����������;
		QString strError = m_reply->errorString();
		qDebug() << "__________" + strError;
	}

	emit signalReplyFinished(statusCode.toInt());
}

// ���ع����г��ִ��󣬹ر����أ����ϱ���������δ�ϱ��������ͣ����Լ���������ϱ�;
void DownLoadManager::onError(QNetworkReply::NetworkError code)
{
	QString strError = m_reply->errorString();
	qDebug() << "__________" + strError;

	closeDownload();
	emit signalDownloadError();
}

// ֹͣ���ع���;
void DownLoadManager::stopWork()
{
	m_isStop = true;
	if (m_reply != NULL)
	{
		disconnect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));
		disconnect(m_reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
		disconnect(m_reply, SIGNAL(finished()), this, SLOT(onFinished()));
		disconnect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
		m_reply->abort();
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

// ��ͣ���ذ�ť������,��ͣ��ǰ����;
void DownLoadManager::stopDownload()
{
	// ����m_isStop����Ϊ�˱�����ε����ͣ���ذ�ť������m_bytesCurrentReceived ����ͣ�ۼ�;
	if (!m_isStop)
	{
		//��¼��ǰ�Ѿ������ֽ���
		m_bytesCurrentReceived += m_bytesReceived;
		stopWork();
	}
}

// ���ò���;
void DownLoadManager::reset()
{
	m_bytesCurrentReceived = 0;
	m_bytesReceived = 0;
	m_bytesTotal = 0;
}

// ɾ���ļ�;
void DownLoadManager::removeFile(QString fileName)
{
	// ɾ�������ص���ʱ�ļ�;
	QFileInfo fileInfo(fileName);
	if (fileInfo.exists())
	{
		QFile::remove(fileName);
	}
}

// ֹͣ���ذ�ť�����£��ر����أ����ò�������ɾ�����ص���ʱ�ļ�;
void DownLoadManager::closeDownload()
{
	stopWork();
	reset();
	removeFile(m_fileName);
}