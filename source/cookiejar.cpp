/*
 * cookiejar.cpp
 *
 *  Created on: Jan 14, 2013
 *      Author: mike
 */

#include "webkit.h"
#include "cookiejar.h"

CookieJar::CookieJar(QObject *parent) : QNetworkCookieJar(parent)
{
}

CookieJar::~CookieJar()
{
#ifdef U_CJ_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	purgeSessionCookies();
}

bool CookieJar::setCookie(const QString& cookieName, const QString& value, int maxAge, const QString& path, const QString& domain, bool isSecure)
{
	QNetworkCookie cookie;
	QUrl url = QString(isSecure ? "https://" : "http://" + QString(domain.startsWith('.') ? "www" : "") + domain + (path.isEmpty() ? "/" : path));
#ifdef U_CJ_DEBUG
	qDebug() << Q_FUNC_INFO << allCookies().count() << url;
#endif
	cookie.setName(cookieName.toUtf8());
	cookie.setValue(value.toUtf8());
	cookie.setDomain(domain);
	cookie.setPath(path);
	cookie.setSecure(isSecure);

	if(maxAge != 0) {
		QDateTime expireDate = QDateTime::currentDateTimeUtc().addSecs(maxAge);
		cookie.setExpirationDate(expireDate);
	}
#ifdef U_CJ_DEBUG
	qDebug() << Q_FUNC_INFO << cookie;
#endif
	return setCookiesFromUrl(QList<QNetworkCookie>() << cookie, url);
}

QNetworkCookie CookieJar::getCookie(const QString& name, const QUrl& url)
{
	purgeExpiredCookies();

	QList<QNetworkCookie> cookieList = cookies(url);
	for(int i = cookieList.count() - 1;i >= 0; --i) {
		if(cookieList.at(i).name() == name) return cookieList.at(i);
	}
	return QNetworkCookie();
}

bool CookieJar::setCookiesFromUrl(const QList<QNetworkCookie>& cookieList, const QUrl& url)
{
#ifdef U_CJ_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	purgeExpiredCookies();
	return QNetworkCookieJar::setCookiesFromUrl(cookieList, url);
}

QList<QNetworkCookie> CookieJar::cookiesForUrl(const QUrl& url) const
{
	QString requestUrl = url.toString();
	QList<QNetworkCookie> cookies = QNetworkCookieJar::cookiesForUrl(url);
	const QList<QString>& registeredDomains = WebKit::instance()->identifierCookieDomainList();


	foreach(QString d, registeredDomains) {
		if(requestUrl.startsWith(d)) {
			QNetworkCookie identifierCookie = WebKit::instance()->identifierCookie();

			identifierCookie.setDomain(d);
			identifierCookie.setSecure(d.startsWith("https") ? true : false);

			cookies.append(identifierCookie);
			break;
		}
	}

	return cookies;
}

void CookieJar::deleteCookieForUrl(const QUrl& url, const QString& cookieName)
{
#ifdef U_CJ_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	QList<QNetworkCookie> cookieList;

	if(url.isEmpty() || !url.isValid()) {
		if(cookieName.isEmpty()) {
			setAllCookies(QList<QNetworkCookie>());
		} else {
			cookieList = allCookies();

			QList<QNetworkCookie>::Iterator it = cookieList.begin();
			while(it != cookieList.end()) {
				if(it->name() == cookieName) {
					deleteCookie(*it);
					it = cookieList.erase(it);
				} else
					it++;
			}
		}
	} else {
		cookieList = cookiesForUrl(url);

		QList<QNetworkCookie>::Iterator it = cookieList.begin();
		while(it != cookieList.end()) {
			if(it->name() == cookieName || cookieName.isEmpty()) {
				deleteCookie(*it);
				it = cookieList.erase(it);

				if(!cookieName.isEmpty())
					break;
			} else
				it++;
		}
	}
}

void CookieJar::purgeExpiredCookies()
{
	QList<QNetworkCookie> cookiesList = allCookies();

	if(cookiesList.isEmpty()) return;

	int cookieCount = cookiesList.count();
	QDateTime now = QDateTime::currentDateTimeUtc();
	for(int i = cookieCount - 1;i >= 0; --i) {
		if(!cookiesList.at(i).isSessionCookie() && cookiesList.at(i).expirationDate() < now) cookiesList.removeAt(i);
	}

	if(cookieCount != cookiesList.count()) setAllCookies(cookiesList);
}

void CookieJar::purgeSessionCookies()
{
	QList<QNetworkCookie> cookiesList = allCookies();

	if(cookiesList.isEmpty()) return;

	int cookieCount = cookiesList.count();
	QDateTime now = QDateTime::currentDateTimeUtc();
	for(int i = cookieCount - 1;i >= 0; --i) {
		if(cookiesList.at(i).isSessionCookie() || !cookiesList.at(i).expirationDate().isValid() || cookiesList.at(i).expirationDate().isNull())
			cookiesList.removeAt(i);
	}

	if(cookieCount != cookiesList.count()) setAllCookies(cookiesList);
}

QList<QNetworkCookie> CookieJar::cookies(const QUrl& url) const
{
	if(url.isEmpty())
		return allCookies();
	else
		return cookiesForUrl(url);
}

CookieJar* CookieJar::instance()
{
	static CookieJar *singleton = NULL;
	if(singleton == NULL) {
		singleton = new CookieJar();
	}
	return singleton;
}
