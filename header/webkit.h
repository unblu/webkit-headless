/*
 * webkit.h
 *
 *  Created on: Oct 2, 2012
 *      Author: mike
 */

#ifndef WEBKIT_H_
#define WEBKIT_H_

#include "jqobject.h"
#include "downloadmanager.h"
#include "networkaccessmanager.h"

class Window;

class WebKit : public JQObject
{
	Q_OBJECT

	typedef QMap<QUuid, Window*> tWindowMap;

private:
	WebKit(QObject *parent = NULL);
	void init();

	void loadJavaWebKitClasses();

public:
	static void initialize();
	static WebKit* instance() { return webkitInstance; }
	virtual ~WebKit();

	jobject createWindow(JNIEnv *env, const QPoint& screenSizeOffset);
	void registerWindow(Window *win);
	void unregisterWindow(Window *win);

	void requestExecution(jobject processor);

	void setClipboardData(const QString& textData);

	void setScreenSize(const QSize& size);
	QSize screenSize() const { return m_screenSize; }

	jobject setEventHandler(jobject eventHandler);
	jobject getEventHandler() const { return m_eventHandler; }

	jobject setDownloadManager(jobject downloadManager);
	jobject getDownloadManager() const { return m_downloadManager; }

	void setDefaultLocale(JNIEnv *env, jstring locale);
	void initBaseConfiguration(JNIEnv *env, jstring baseUrl, jobject configuration);
	void initSessionConfiguration(JNIEnv *env, jstring sessionId, jobject configuration);

	const QString& baseUrl() const { return m_baseUrl; }
	const QString& browserId() const { return m_browserId; }
	const QString& sessionId() const { return m_sessionId; }
	bool onlyUseSslV3() const { return m_bOnlyUseSslV3; }
	const QNetworkCookie& identifierCookie() const { return m_identifierCookie; }
	const QList<QString>& identifierCookieDomainList() const { return m_identifierCookieDomainList; }

	const QString& tempStorageLocation() const { return m_tempStorageLocation; }
	const QString& dataStorageLocation() const { return m_dataStorageLocation; }

	int developerToolsRemotePort() const { return m_nDeveloperToolsRemotePort; }

	DownloadManager* downloadManager();

	QNetworkAccessManager* networkAccessManager() const { return m_pNetworkAccessManager; }

	QVariantMap& defaultPageSettings() { return m_defaultPageSettings; }

	QMap< QString, bool >& trustedSites() { return m_trustedSites; }
	QMap< QPair<QString, int>, QByteArray >& certificateException() { return m_certificateException; }

	void run(const QString& browserId);
	void stop();

signals:
	void scheduleRequestExecution(jobject processor);

private:
	void setHomeDirectory(const QString& homePath);
	void configurePdfJs();
	void installCaCertificates();
	void setApplicationVersion(const QString& applicationVersion);
	void updateWindowSize(const QSize& size);
	void callWindowCreated(JNIEnv *env, Window *win);
	void callWindowDestroyed(JNIEnv *env, Window *win);
	void createDataStorageLocation();
	void createTempStorageLocation();

private slots:
	void handleRequestExecution(jobject processor);
	void handleClipboardChanged(QClipboard::Mode mode);

public:
	static jobject classHeadlessBrowserBaseConfigurationData;
	static jmethodID headlessBrowserBaseConfigurationDataMID;
	static jfieldID hbbcdRunUserHome;
	static jfieldID hbbcdApplicationVersion;
	static jfieldID hbbcdDeveloperToolsRemotePort;

	static jobject classHeadlessBrowserSessionConfigurationData;
	static jmethodID headlessBrowserSessionConfigurationDataMID;
	static jfieldID hbscdSslSitesAllowed;
	static jfieldID hbscdLocalSitesAllowed;
	static jfieldID hbscdJavaScriptProtocolAllowed;
	static jfieldID hbscdFlashEnabled;
	static jfieldID hbscdDeveloperToolsEnabled;
	static jfieldID hbscdOnlyUseSslV3;
	static jfieldID hbscdIdentifierCookieString;
	static jfieldID hbscdIdentifierCookieDomainList;
	static jfieldID hbscdForwardProxyUrl;

private:
	QApplication *m_pApplication;
	QDesktopWidget *m_pDesktop;
	QVariantMap m_defaultPageSettings;
	tWindowMap m_windows;

	QString m_baseUrl;
	QString m_browserId;
	QString m_sessionId;
	QSize m_screenSize;
	bool m_bOnlyUseSslV3;
	QNetworkCookie m_identifierCookie;
	QList<QString> m_identifierCookieDomainList;

	QMap< QString, bool > m_trustedSites;
	QMap< QPair<QString, int>, QByteArray > m_certificateException;

	QString m_tempStorageLocation;
	QString m_dataStorageLocation;

	int m_nDeveloperToolsRemotePort;

	jobject m_eventHandler;
	jobject m_downloadManager;

	DownloadManager *m_pDownloadManager;
	NetworkAccessManager *m_pNetworkAccessManager;

	static WebKit *webkitInstance;
};

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_runApplication(JNIEnv *env, jobject obj, jstring browserId);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_stopApplication(JNIEnv *env, jobject obj);
JNIEXPORT jobject JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_createWindow(JNIEnv *env, jobject obj, jobject screenSizeOffset);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_setEventHandler(JNIEnv *env, jobject obj, jobject eventHandler);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_setScreenSize(JNIEnv *env, jobject obj, jobject screenSize);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_setClipboardData(JNIEnv *env, jobject obj, jstring clipboardData);
JNIEXPORT jobject JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_getScreenSize(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_setDownloadManager(JNIEnv *env, jobject obj, jobject downloadManager);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_setDefaultLocale(JNIEnv *env, jobject obj, jstring locale);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_initBaseConfiguration(JNIEnv *env, jobject obj, jstring baseUrl, jobject configuration);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_initSessionConfiguration(JNIEnv *env, jobject obj, jstring sessionId, jobject configuration);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_setCookie(JNIEnv *env, jobject obj, jstring name, jstring value, jint maxAge, jstring path, jstring domain, jboolean isSecure);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_requestExecution(JNIEnv *env, jobject obj, jobject processor);

#ifdef __cplusplus
}
#endif

#endif /* WEBKIT_H_ */
