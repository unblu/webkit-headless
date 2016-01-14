/*
 * downloadmanager.cpp
 *
 *  Created on: Dec 6, 2012
 *      Author: mike
 */

#include "downloadmanager.h"
#include "webkit.h"
#include "window.h"
#include "utils.h"

DownloadItem::DownloadItem(QNetworkReply *reply, Window *window, DownloadManager *mgr)
	: JQObject(), m_pReply(reply), m_pWindow(window), m_pManager(mgr)
{
#ifdef U_DLMGR_DEBUG
	qDebug() << Q_FUNC_INFO << reply->url() << window->getUrl();
#endif
	m_id = QUuid::createUuid();

	init();
}

DownloadItem::~DownloadItem()
{
#ifdef U_DLMGR_DEBUG
	qDebug() << Q_FUNC_INFO << m_pWindow->parent();
#endif
	if(m_pWindow->parent() != NULL && m_pWindow->getUrl().isEmpty()) {
		m_pWindow->close();
	}
	m_pReply->deleteLater();
}

void DownloadItem::init()
{
	bool ok;

	if(m_pReply == NULL)
		return;
#ifdef U_DLMGR_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	m_url = m_pReply->url();
	m_pReply->setParent(this);
	m_bChunked = m_pReply->rawHeader("Transfer-Encoding") == "chunked";
	m_nContentLen = m_pReply->header(QNetworkRequest::ContentLengthHeader).toInt(&ok);
	if(!ok)
		m_nContentLen = -1;

	initiateStreaming();

	if(m_pReply->error() != QNetworkReply::NoError) {
		error(m_pReply->error());
		return;
	}

	if(m_nContentLen > 0 && m_pReply->bytesAvailable() >= m_nContentLen) {
		downloadReadyRead();
		finished();
		return;
	}

	connect(m_pReply, SIGNAL(readyRead()), SLOT(downloadReadyRead()), Qt::QueuedConnection);
	connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(error(QNetworkReply::NetworkError)), Qt::QueuedConnection);
	connect(m_pReply, SIGNAL(downloadProgress(qint64, qint64)), SLOT(downloadProgress(qint64, qint64)), Qt::QueuedConnection);
	connect(m_pReply, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()), Qt::QueuedConnection);
	connect(m_pReply, SIGNAL(finished()), SLOT(finished()), Qt::QueuedConnection);
}

void DownloadItem::initiateStreaming()
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
	jobject downloadManager = WebKit::instance()->getDownloadManager();
#ifdef U_DLMGR_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, downloadManager)) {
		jstring jcontentDisp = NULL;
		jobject jwin = Window::getWindowClass(env, m_pWindow);
		jstring jid = Utils::getJString(env, m_id.toString());
		jstring juri = Utils::getJString(env, m_url.toString());
		jstring jcontentType = Utils::getJString(env, m_pReply->header(QNetworkRequest::ContentTypeHeader).toString());
		QVariant contentDisp = m_pReply->header(QNetworkRequest::ContentDispositionHeader);

		if(contentDisp.isValid() && contentDisp.toString().length() > 0)
			jcontentDisp = Utils::getJString(env, contentDisp.toString());

		Utils::invokeMethod(env, downloadManager, "openDownload", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;)V", jwin, jid, juri, jcontentType, (jint)m_nContentLen, jcontentDisp);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void DownloadItem::downloadReadyRead()
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
	jobject downloadManager = WebKit::instance()->getDownloadManager();
#ifdef U_DLMGR_DEBUG
	qDebug() << Q_FUNC_INFO << m_bChunked << QThread::currentThreadId();
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, downloadManager)) {
		QByteArray data = m_pReply->readAll();
		jstring jid = Utils::getJString(env, m_id.toString());
		jbyteArray jbyteData = Utils::getByteArray(env, (uchar*)data.constData(), data.length());

		Utils::invokeMethod(env, downloadManager, "sendChunk", "(Ljava/lang/String;[B)V", jid, jbyteData);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void DownloadItem::error(QNetworkReply::NetworkError)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
	jobject downloadManager = WebKit::instance()->getDownloadManager();
#ifdef U_DLMGR_DEBUG
	qDebug() << Q_FUNC_INFO << m_pReply->errorString() << m_url;
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, downloadManager)) {
		Utils::invokeMethod(env, downloadManager, "abortDownload", "(Ljava/lang/String;)V", Utils::getJString(env, m_id.toString()));
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);

	deleteLater();
}

void DownloadItem::metaDataChanged()
{
#ifdef U_DLMGR_DEBUG
	qDebug() << Q_FUNC_INFO << "not handled.";
#endif
}

void DownloadItem::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
#ifdef U_DLMGR_DEBUG
	qDebug() << Q_FUNC_INFO << bytesReceived << bytesTotal;
#endif
	Q_UNUSED(bytesReceived);
	Q_UNUSED(bytesTotal);
}

void DownloadItem::finished()
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
	jobject downloadManager = WebKit::instance()->getDownloadManager();
#ifdef U_DLMGR_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, downloadManager)) {
		jobject jbloburi = Utils::invokeMethod(env, downloadManager, "closeDownload", "(Ljava/lang/String;)Ljava/lang/String;", Utils::getJString(env, m_id.toString())).value<jobject>();

		m_blobUri = Utils::getQString(env, jbloburi);
		m_pWindow->processDownloadItem(this);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);

	deleteLater();
}

DownloadManager::DownloadManager(QObject *parent) : JQObject(parent)
{

}

DownloadManager::~DownloadManager()
{

}

void DownloadManager::download(Window *win, QNetworkReply *reply)
{
	if(!reply || reply->url().isEmpty())
		return;

#ifdef U_DLMGR_DEBUG
	bool ok;
	QVariant TEHeader = reply->rawHeader("Transfer-Encoding");
	QVariant CLHeader = reply->header(QNetworkRequest::ContentLengthHeader);
	int contentLength = CLHeader.toInt(&ok);
	QString transferEncoding = TEHeader.toString();

	qWarning() << Q_FUNC_INFO << "starting download for " << reply->url() << "with content-length " << contentLength << transferEncoding;
#endif

	(void) new DownloadItem(reply, win, this);
}
