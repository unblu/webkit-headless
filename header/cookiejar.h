/*
 * cookiejar.h
 *
 *  Created on: Jan 14, 2013
 *      Author: mike
 */

#ifndef COOKIEJAR_H_
#define COOKIEJAR_H_

#include "jqobject.h"

class CookieJar : public QNetworkCookieJar
{
	Q_OBJECT

private:
	CookieJar(QObject *parent = NULL);

public:
	static CookieJar* instance();
	virtual ~CookieJar();

	bool setCookie(const QString& cookieName, const QString& value, int maxAge, const QString& path, const QString& domain, bool isSecure);
	QNetworkCookie getCookie(const QString& name, const QUrl& url);

	virtual QList<QNetworkCookie> cookiesForUrl(const QUrl& url) const;
	virtual bool setCookiesFromUrl(const QList<QNetworkCookie>& cookieList, const QUrl& url);

public slots:
	void deleteCookieForUrl(const QUrl& url, const QString& cookieName);

private:
	void purgeExpiredCookies();
	void purgeSessionCookies();
	QList<QNetworkCookie> cookies(const QUrl& url) const;
};

#endif /* COOKIEJAR_H_ */
