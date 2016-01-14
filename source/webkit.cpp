/*
 * webkit.cpp
 *
 *  Created on: Oct 2, 2012
 *      Author: mike
 */

#include "event.h"
#include "webkit.h"
#include "window.h"
#include "utils.h"
#include "cookiejar.h"
#include "downloadmanager.h"
#include "unbluwebpopupcombo.h"

Q_IMPORT_PLUGIN(QICOPlugin)
Q_IMPORT_PLUGIN(QCURPlugin)
Q_IMPORT_PLUGIN(UnbluWebKitPlugin)

static int argc = 3;
static const char *argv[] = { "unbluwebkit", "-platformtheme", "unblu" };

WebKit* WebKit::webkitInstance = NULL;

jobject WebKit::classHeadlessBrowserBaseConfigurationData;
jmethodID WebKit::headlessBrowserBaseConfigurationDataMID;
jfieldID WebKit::hbbcdRunUserHome;
jfieldID WebKit::hbbcdApplicationVersion;
jfieldID WebKit::hbbcdDeveloperToolsRemotePort;

jobject WebKit::classHeadlessBrowserSessionConfigurationData;
jmethodID WebKit::headlessBrowserSessionConfigurationDataMID;
jfieldID WebKit::hbscdSslSitesAllowed;
jfieldID WebKit::hbscdLocalSitesAllowed;
jfieldID WebKit::hbscdJavaScriptProtocolAllowed;
jfieldID WebKit::hbscdFlashEnabled;
jfieldID WebKit::hbscdDeveloperToolsEnabled;
jfieldID WebKit::hbscdOnlyUseSslV3;
jfieldID WebKit::hbscdIdentifierCookieString;
jfieldID WebKit::hbscdIdentifierCookieDomainList;
jfieldID WebKit::hbscdForwardProxyUrl;

/*
 * Class:     WebKit
 * Method:    sizeof
 * Signature: (I)I
 */
JNIEXPORT jobject JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_createWindow(JNIEnv *env, jobject obj, jobject screenSizeOffset)
{
	Q_UNUSED(obj);

	Utils::pushFrame(env);

	QPoint offset = Utils::getQPoint(env, screenSizeOffset);
	jobject jwin = WebKit::instance()->createWindow(env, offset);

	return Utils::popFrame(env, jwin);
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_runApplication(JNIEnv *env, jobject obj, jstring browserId)
{
	Q_UNUSED(env);
	Q_UNUSED(obj);
	WebKit::instance()->run(Utils::getQString(env, browserId));
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_stopApplication(JNIEnv *env, jobject obj)
{
	Q_UNUSED(env);
	Q_UNUSED(obj);
	WebKit::instance()->stop();
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_setDefaultLocale(JNIEnv *env, jobject obj, jstring locale)
{
	Q_UNUSED(obj);
	WebKit::instance()->setDefaultLocale(env, locale);
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_initBaseConfiguration(JNIEnv *env, jobject obj, jstring baseUrl, jobject configuration)
{
	Q_UNUSED(obj);
	WebKit::instance()->initBaseConfiguration(env, baseUrl, configuration);
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_initSessionConfiguration(JNIEnv *env, jobject obj, jstring sessionId, jobject configuration)
{
	Q_UNUSED(obj);
	WebKit::instance()->initSessionConfiguration(env, sessionId, configuration);
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_setEventHandler(JNIEnv *env, jobject obj, jobject eventHandler)
{
	Q_UNUSED(obj);

	jobject refObj = Utils::newGlobalRef(env, eventHandler);
	jobject old = WebKit::instance()->setEventHandler(refObj);
	Utils::deleteGlobalRef(env, old);
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_setDownloadManager(JNIEnv *env, jobject obj, jobject downloadManager)
{
	Q_UNUSED(obj);

	jobject refObj = Utils::newGlobalRef(env, downloadManager);
	jobject old = WebKit::instance()->setDownloadManager(refObj);
	Utils::deleteGlobalRef(env, old);
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_setScreenSize(JNIEnv *env, jobject obj, jobject screenSize)
{
	Q_UNUSED(obj);

	WebKit *kit = WebKit::instance();
	QSize size = Utils::getQSize(env, screenSize);

	kit->setScreenSize(size);
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_setClipboardData(JNIEnv *env, jobject obj, jstring clipboardData)
{
	Q_UNUSED(obj);
	WebKit::instance()->setClipboardData(Utils::getQString(env, clipboardData));
}

JNIEXPORT jobject JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_getScreenSize(JNIEnv *env, jobject obj)
{
	Q_UNUSED(obj);

	Utils::pushFrame(env);

	WebKit *kit = WebKit::instance();
	jobject size = Utils::getAWTDimension(env, kit->screenSize());

	return Utils::popFrame(env, size);
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_setCookie(JNIEnv *env, jobject obj, jstring name, jstring value, jint maxAge, jstring path, jstring domain, jboolean isSecure)
{
	Q_UNUSED(obj);
	CookieJar::instance()->setCookie(Utils::getQString(env, name), Utils::getQString(env, value), (int)maxAge, Utils::getQString(env, path), Utils::getQString(env, domain), (bool)isSecure);
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_WebKit_requestExecution(JNIEnv *env, jobject obj, jobject processor)
{
	Q_UNUSED(obj);

	jobject refObj = Utils::newGlobalRef(env, processor);
	WebKit::instance()->requestExecution(refObj);
}

WebKit::WebKit(QObject *parent)
	: JQObject(parent), m_screenSize(1024, 768), m_bOnlyUseSslV3(false), m_nDeveloperToolsRemotePort(-1),
	  m_eventHandler(NULL), m_downloadManager(NULL), m_pDownloadManager(NULL)

{
	m_pApplication = new QApplication(argc, (char**)argv);
	m_pNetworkAccessManager = new NetworkAccessManager(this);
	m_pDesktop = QApplication::desktop();
}

WebKit::~WebKit()
{

}

void WebKit::initialize()
{
	if(webkitInstance == NULL) {
		webkitInstance = new WebKit();
		webkitInstance->init();
	}
}

void WebKit::init()
{
	m_pDesktop->setVisible(true);
	m_pDesktop->setAcceptDrops(true);

	m_pApplication->setOrganizationName("unblu");
	m_pApplication->setOrganizationDomain("www.unblu.com");

	loadJavaWebKitClasses();

	Q_INIT_RESOURCE(WebKit);
	Q_INIT_RESOURCE(WebCore);
	Q_INIT_RESOURCE(InspectorBackendCommands);
	Q_INIT_RESOURCE(ca_certificates);
	Q_INIT_RESOURCE(pdf_js);

	qInstallMessageHandler(Utils::messageHandler);

	qRegisterMetaType< QImage >("QImage");
	qRegisterMetaType< QNetworkRequest >("QNetworkRequest");
	qRegisterMetaType< jobject >("jobject");
	qRegisterMetaType< Event >("Event");
	qRegisterMetaType< QList<QSslError> >("QList<QSslError>");
	qRegisterMetaType< QClipboard::Mode >("QClipboard::Mode");

	// initially default to en_US
	QLocale::setDefault(QLocale("en_US"));

	// add "unblu" as schemne
	QWebSecurityOrigin::addLocalScheme("unblu");

	m_defaultPageSettings[PAGE_SETTINGS_LOAD_IMAGES] = QVariant::fromValue(true);
	m_defaultPageSettings[PAGE_SETTINGS_JS_ENABLED] = QVariant::fromValue(true);
	m_defaultPageSettings[PAGE_SETTINGS_XSS_AUDITING] = QVariant::fromValue(false);
    m_defaultPageSettings[PAGE_SETTINGS_LOCAL_ACCESS_REMOTE] = QVariant::fromValue(false);
    m_defaultPageSettings[PAGE_SETTINGS_WEB_SECURITY_ENABLED] = QVariant::fromValue(true);
    m_defaultPageSettings[PAGE_SETTINGS_JS_CAN_OPEN_WINDOWS] = QVariant::fromValue(true);
    m_defaultPageSettings[PAGE_SETTINGS_JS_CAN_CLOSE_WINDOWS] = QVariant::fromValue(true);
    m_defaultPageSettings[PAGE_SETTINGS_PLUGINS_ENABLED] = QVariant::fromValue(false);

	connect(this, SIGNAL(scheduleRequestExecution(jobject)), this, SLOT(handleRequestExecution(jobject)), Qt::QueuedConnection);
	connect(QApplication::clipboard(), SIGNAL(changed(QClipboard::Mode)), this, SLOT(handleClipboardChanged(QClipboard::Mode)), Qt::QueuedConnection);
}

void WebKit::configurePdfJs()
{
	QUrl uri(m_baseUrl);
	QWebSecurityOrigin pdfJs(QUrl("qrc:/pdf.js/"));

	pdfJs.addAccessWhitelistEntry(uri.scheme(), uri.host(), QWebSecurityOrigin::DisallowSubdomains);
}

void WebKit::setHomeDirectory(const QString& homePath)
{
	if(!homePath.isEmpty()) {
		qputenv("HOME", homePath.toLatin1());
	}
}

void WebKit::setApplicationVersion(const QString& applicationVersion)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);

	QStringList versionElements = applicationVersion.split("/");
	if(versionElements.size() != 2) {
    	qCritical() << "Application version string seems to be wrong: " << applicationVersion;
    	Utils::throwByName(env, ERuntime, (QString("Application version string seems to be wrong: ") + applicationVersion).toLatin1().data());
	} else {
		m_pApplication->setApplicationName(versionElements.at(0));
		m_pApplication->setApplicationVersion(versionElements.at(1));
	}

	m_pJVMContext->releaseEnvironment(env, attached);
}

void WebKit::installCaCertificates()
{
	if(QSslSocket::supportsSsl()) {
		QList<QSslCertificate> caCertificates;

		caCertificates.append(QSslCertificate::fromPath(":/certs/*.pem", QSsl::Pem, QRegExp::Wildcard));
		caCertificates.append(QSslCertificate::fromPath(":/certs/*.crt", QSsl::Pem, QRegExp::Wildcard));

		QSslSocket::setDefaultCaCertificates(caCertificates);
	}
}

void WebKit::createDataStorageLocation()
{
    QDir dir;
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
    QString storageLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);

    if(!dir.mkpath(storageLocation)) {
    	qCritical() << "Could not create data storage location: " << storageLocation;
    	Utils::throwByName(env, ERuntime, (QString("Could not create data storage location: ") + storageLocation).toLatin1().data());
    }
    m_dataStorageLocation = storageLocation;

	m_pJVMContext->releaseEnvironment(env, attached);
}

void WebKit::createTempStorageLocation()
{
	QDir dir;
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
	QString storageLocation = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QLatin1Char('/') + m_browserId;

	if(!dir.mkpath(storageLocation)) {
    	qCritical() << "Could not create temp storage location: " << storageLocation;
    	Utils::throwByName(env, ERuntime, (QString("Could not create temp storage location: ") + storageLocation).toLatin1().data());
	}
	m_tempStorageLocation = storageLocation;

	m_pJVMContext->releaseEnvironment(env, attached);
}

jobject WebKit::createWindow(JNIEnv *env, const QPoint& screenSizeOffset)
{
	Window *win = new Window(m_defaultPageSettings, screenSizeOffset);

	if(win != NULL) {
		m_windows.insert(win->getId(), win);
		return Window::getWindowClass(env, win);;
	}

	return NULL;
}

void WebKit::registerWindow(Window *win)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);

	if(win != NULL) {
		m_windows.insert(win->getId(), win);
		callWindowCreated(env, win);
	}

	m_pJVMContext->releaseEnvironment(env, attached);
}

void WebKit::unregisterWindow(Window *win)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);

	qWarning() << Q_FUNC_INFO << win;
	if(win != NULL) {
		callWindowDestroyed(env, win);
		m_windows.remove(win->getId());
	}

	m_pJVMContext->releaseEnvironment(env, attached);
}

void WebKit::requestExecution(jobject processor)
{
	emit scheduleRequestExecution(processor);
}

jobject WebKit::setEventHandler(jobject eventHandler)
{
	jobject old = m_eventHandler;
	m_eventHandler = eventHandler;
	return old;
}

jobject WebKit::setDownloadManager(jobject downloadManager)
{
	jobject old = m_downloadManager;
	m_downloadManager = downloadManager;
	return old;
}

void WebKit::setScreenSize(const QSize& size)
{
	m_screenSize = size;
	updateWindowSize(size);
}

void WebKit::setClipboardData(const QString& textData)
{
	QApplication::clipboard()->setText(textData);
}

DownloadManager* WebKit::downloadManager()
{
	if(m_pDownloadManager == NULL) {
		m_pDownloadManager = new DownloadManager();
	}
	return m_pDownloadManager;
}

void WebKit::run(const QString& browserId)
{
	m_browserId = browserId;

	createTempStorageLocation();

	if(m_pApplication != NULL) {
		m_pApplication->exec();
	}
}

void WebKit::stop()
{
	QDir dir(tempStorageLocation());

	m_pNetworkAccessManager->deleteLater();

	if(m_pApplication != NULL) {
		m_pApplication->quit();
	}

	if(!dir.removeRecursively()) {
		qCritical() << "Could not remove temp storage directory: " << tempStorageLocation();
	}
}

void WebKit::setDefaultLocale(JNIEnv *env, jstring locale)
{
	QLocale::setDefault(QLocale(Utils::getQString(env, locale)));
}

void WebKit::initBaseConfiguration(JNIEnv *env, jstring baseUrl, jobject configuration)
{
	QString homePath = Utils::getQString(env, env->GetObjectField(configuration, hbbcdRunUserHome));
	QString applicationVersion = Utils::getQString(env, env->GetObjectField(configuration, hbbcdApplicationVersion));

	m_baseUrl = Utils::getQString(env, baseUrl);
	m_nDeveloperToolsRemotePort = (int)env->GetIntField(configuration, hbbcdDeveloperToolsRemotePort);

    setHomeDirectory(homePath);
    setApplicationVersion(applicationVersion);
    createDataStorageLocation();
    installCaCertificates();
    configurePdfJs();

    QWebSettings::setIconDatabasePath(dataStorageLocation());

    qCritical() << QFontDatabase().families();
}

void WebKit::initSessionConfiguration(JNIEnv *env, jstring sessionId, jobject configuration)
{
	jobject identifierCookieDomains = env->GetObjectField(configuration, hbscdIdentifierCookieDomainList);
	QString forwardProxyUrlString = Utils::getQString(env, env->GetObjectField(configuration, hbscdForwardProxyUrl));
	QString identifierCookieString = Utils::getQString(env, env->GetObjectField(configuration, hbscdIdentifierCookieString));

	m_sessionId = Utils::getQString(env, sessionId);
	m_bOnlyUseSslV3 = (bool)env->GetBooleanField(configuration, hbscdOnlyUseSslV3);

	if (!forwardProxyUrlString.isEmpty()) {
		m_pNetworkAccessManager->setProxy(forwardProxyUrlString);
	}

	if (!identifierCookieString.isEmpty()) {
		QStringList nameValue = identifierCookieString.split('=');
		QString name = nameValue.at(0);
		QString value = "";

		if(nameValue.size() > 1) value = nameValue.at(1);

		m_identifierCookie.setName(name.toUtf8());
		m_identifierCookie.setValue(value.toUtf8());
		m_identifierCookie.setPath("/");
		m_identifierCookie.setHttpOnly(true);
	}

	m_identifierCookieDomainList = Utils::getQStringArray(env, identifierCookieDomains);

	m_defaultPageSettings[PAGE_SETTINGS_PLUGINS_ENABLED] = QVariant::fromValue((bool)env->GetBooleanField(configuration, hbscdFlashEnabled));
	m_defaultPageSettings[PAGE_SETTINGS_DEVELOPER_TOOLS] = QVariant::fromValue((bool)env->GetBooleanField(configuration, hbscdDeveloperToolsEnabled));
}

void WebKit::updateWindowSize(const QSize& size)
{
	foreach(Window *window, m_windows.values()) {
		window->setViewportSize(size);
	}
}

void WebKit::callWindowCreated(JNIEnv *env, Window *win)
{
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		Utils::invokeMethod(env, m_eventHandler, "onWindowCreated", "(Lcom/unblu/webkit/main/server/core/internal/Window;)V", Window::getWindowClass(env, win));
	}

	Utils::popFrame(env);
}

void WebKit::callWindowDestroyed(JNIEnv *env, Window *win)
{
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		Utils::invokeMethod(env, m_eventHandler, "onWindowDestroyed", "(Lcom/unblu/webkit/main/server/core/internal/Window;)V", Window::getWindowClass(env, win));
	}

	Utils::popFrame(env);
}

void WebKit::handleRequestExecution(jobject processor)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);

	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, processor)) {
		Utils::invokeMethod(env, processor, "process", "()V");
		Utils::deleteGlobalRef(env, processor);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void WebKit::handleClipboardChanged(QClipboard::Mode mode)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
	const QMimeData *mimeData = QApplication::clipboard()->mimeData(mode);

	Utils::pushFrame(env);

	if(mimeData->hasText()) {
		QString plainText = mimeData->text();
		jstring jstr = Utils::getJString(env, plainText);

		if(!Utils::isNullObject(env, m_eventHandler)) {
			Utils::invokeMethod(env, m_eventHandler, "onClipboardChanged", "(Ljava/lang/String;)V", jstr);
		}
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void WebKit::loadJavaWebKitClasses()
{
	JNIEnv *env = NULL;
	bool attached = m_pJVMContext->getEnvironment(env);

	Window::loadJavaClasses(env);
	Event::loadJavaClasses(env);
	UnbluWebPopup::loadJavaClasses(env);
	UnbluWebPopupCombo::loadJavaClasses(env);

	// load base configuration class refs
	if(!LOAD_CREF(env, HeadlessBrowserBaseConfigurationData, "com/unblu/headlessbrowser/model/shared/core/HeadlessBrowserBaseConfigurationData")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain class com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserBaseConfigurationData");
	} else if(!LOAD_MID(env, headlessBrowserBaseConfigurationDataMID, classHeadlessBrowserBaseConfigurationData, "<init>", "()V")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain constructor for com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserBaseConfigurationData");
	} else if(!LOAD_FID(env, hbbcdDeveloperToolsRemotePort, classHeadlessBrowserBaseConfigurationData, "developerToolsRemotePort", "I")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain developerToolsRemotePort field ID for class com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserBaseConfigurationData");
	} else if(!LOAD_FID(env, hbbcdRunUserHome, classHeadlessBrowserBaseConfigurationData, "runUserHome", "Ljava/lang/String;")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain runUserHome field ID for class com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserBaseConfigurationData");
	} else if(!LOAD_FID(env, hbbcdApplicationVersion, classHeadlessBrowserBaseConfigurationData, "applicationVersion", "Ljava/lang/String;")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain applicationVersion field ID for class com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserBaseConfigurationData");
	}

	// load session configuration class refs
	if(!LOAD_CREF(env, HeadlessBrowserSessionConfigurationData, "com/unblu/headlessbrowser/model/shared/core/HeadlessBrowserSessionConfigurationData")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain class com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserSessionConfigurationData");
	} else if(!LOAD_MID(env, headlessBrowserSessionConfigurationDataMID, classHeadlessBrowserSessionConfigurationData, "<init>", "()V")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain constructor for com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserSessionConfigurationData");
	} else if(!LOAD_FID(env, hbscdSslSitesAllowed, classHeadlessBrowserSessionConfigurationData, "sslSitesAllowed", "Z")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain sslSitesAllowed field ID for class com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserSessionConfigurationData");
	} else if(!LOAD_FID(env, hbscdLocalSitesAllowed, classHeadlessBrowserSessionConfigurationData, "localSitesAllowed", "Z")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain localSitesAllowed field ID for class com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserSessionConfigurationData");
	} else if(!LOAD_FID(env, hbscdJavaScriptProtocolAllowed, classHeadlessBrowserSessionConfigurationData, "javaScriptProtocolAllowed", "Z")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain javaScriptProtocolAllowed field ID for class com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserSessionConfigurationData");
	} else if(!LOAD_FID(env, hbscdFlashEnabled, classHeadlessBrowserSessionConfigurationData, "flashEnabled", "Z")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain flashEnabled field ID for class com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserSessionConfigurationData");
	} else if(!LOAD_FID(env, hbscdDeveloperToolsEnabled, classHeadlessBrowserSessionConfigurationData, "developerToolsEnabled", "Z")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain developerToolsEnabled field ID for class com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserSessionConfigurationData");
	} else if(!LOAD_FID(env, hbscdOnlyUseSslV3, classHeadlessBrowserSessionConfigurationData, "onlyUseSslV3", "Z")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain onlyUseSslV3 field ID for class com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserSessionConfigurationData");
	} else if(!LOAD_FID(env, hbscdIdentifierCookieString, classHeadlessBrowserSessionConfigurationData, "identifierCookieString", "Ljava/lang/String;")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain identifierCookieString field ID for class com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserSessionConfigurationData");
	} else if(!LOAD_FID(env, hbscdIdentifierCookieDomainList, classHeadlessBrowserSessionConfigurationData, "identifierCookieDomainList", "[Ljava/lang/String;")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain identifierCookieDomainList field ID for class com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserSessionConfigurationData");
	} else if(!LOAD_FID(env, hbscdForwardProxyUrl, classHeadlessBrowserSessionConfigurationData, "forwardProxyUrl", "Ljava/lang/String;")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain forwardProxyUrl field ID for class com.unblu.headlessbrowser.model.shared.core.HeadlessBrowserSessionConfigurationData");
	}

	m_pJVMContext->releaseEnvironment(env, attached);
}
