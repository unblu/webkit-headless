/*
 * networkaccessmanager.cpp
 *
 *  Created on: Jan 25, 2013
 *      Author: mike
 */

#include "webkit.h"
#include "cookiejar.h"
#include "networkaccessmanager.h"

NetworkAccessManager::NetworkAccessManager(QObject *parent) : QNetworkAccessManager(parent)
{
	setCookieJar(CookieJar::instance());
}

NetworkAccessManager::~NetworkAccessManager()
{
#ifdef U_NAM_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	disconnect(this, 0, 0, 0);
}

void NetworkAccessManager::setProxy(const QString& proxyUrl)
{
	QUrl url(proxyUrl);
	QNetworkProxy::ProxyType type = QNetworkProxy::HttpProxy;
#ifdef U_NAM_DEBUG
	qDebug() << Q_FUNC_INFO << "Setting proxy to: " << url.scheme() << url.host() << url.port();
#endif
	if (url.scheme() == "socks5") type = QNetworkProxy::Socks5Proxy;

	QNetworkAccessManager::setProxy(QNetworkProxy(type, url.host(), url.port(), url.userName(), url.password()));
}

QNetworkReply* NetworkAccessManager::createRequest(Operation op, const QNetworkRequest& request, QIODevice *outgoingData)
{
	QNetworkRequest req(request);

	if(!QSslSocket::supportsSsl()) {
		if(req.url().scheme().toLower() == QLatin1String("https"))
			qCritical() << "Request using https scheme without SSL support";
	} else {
		QSslConfiguration sslConfig = req.sslConfiguration();
#ifdef U_NAM_DEBUG
		qDebug() << Q_FUNC_INFO << "NAM: ssl support, configuring it now.";
#endif
		sslConfig.setProtocol(QSsl::TlsV1SslV3);
		sslConfig.setPeerVerifyMode(QSslSocket::AutoVerifyPeer);

		req.setSslConfiguration(sslConfig);
	}

    if(op == QNetworkAccessManager::PostOperation) {
        QString contentType = req.header(QNetworkRequest::ContentTypeHeader).toString();
        if(contentType.isEmpty()) {
            req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        }
    }

    return QNetworkAccessManager::createRequest(op, req, outgoingData);
}
