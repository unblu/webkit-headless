/*
 * downloadmanager.h
 *
 *  Created on: Dec 6, 2012
 *      Author: mike
 */

#ifndef DOWNLOADMANAGER_H_
#define DOWNLOADMANAGER_H_

#include "jqobject.h"

class Window;
class DownloadManager;

class DownloadItem : public JQObject
{
	Q_OBJECT

public:
	DownloadItem(QNetworkReply *reply, Window *window, DownloadManager *mgr);
	virtual ~DownloadItem();

	const QString& blobUri() const { return m_blobUri; }
	const QNetworkReply* reply() const { return m_pReply; }

private:
	void init();
	void initiateStreaming();

private slots:
	void downloadReadyRead();
	void error(QNetworkReply::NetworkError);
	void metaDataChanged();
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void finished();

private:
	QUrl m_url;
	QUuid m_id;
	QString m_blobUri;
	QNetworkReply *m_pReply;
	Window *m_pWindow;
	DownloadManager *m_pManager;
	qint64 m_nContentLen;
	bool m_bChunked;

	friend class DownloadManager;
};

class DownloadManager : public JQObject
{
	Q_OBJECT

public:
	DownloadManager(QObject *parent = NULL);
	virtual ~DownloadManager();

	void download(Window *win, QNetworkReply *reply);

private:

	friend class DownloadItem;
};

#endif /* DOWNLOADMANAGER_H_ */
