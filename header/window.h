/*
 * window.h
 *
 *  Created on: Oct 2, 2012
 *      Author: mike
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include "jqobject.h"

#define WINDOW_CREATE					0x00000001
#define WINDOW_SETVIEWPORTSIZE			0x00000002
#define WINDOW_RENDER					0x00000004
#define WINDOW_HISTORY_BACK				0x00000008
#define WINDOW_HISTORY_FORWARD			0x00000010
#define WINDOW_GETVIEWPORTSIZE			0x00000020
#define WINDOW_GETCONTENTSIZE			0x00000040
#define WINDOW_GETPAGETITLE				0x00000080
#define WINDOW_PAGERELOAD				0x00000100
#define WINDOW_SETSCROLLPOSITION		0x00000200
#define WINDOW_GETSCROLLPOSITION		0x00000400
#define WINDOW_TRIGGERACTION			0x00000800
#define WINDOW_SETVIEWPORTSIZE_OFFSET	0x00001000
#define WINDOW_GETURL					0x00002000

class Event;
class WindowPrivate;
class DownloadItem;

class Window : public JQObject
{
	Q_OBJECT

public:
	Window(const QVariantMap& settings, const QPoint& screenSizeOffset = QPoint(), QObject *parent = NULL);
	Window(WindowPrivate *window, QObject *parent = NULL);
	virtual ~Window();

	const QUuid& getId() const { return m_uId; }

	QImage render(const QRect& clipRect = QRect(), bool useJpeg = false);
	void load(const QString& address, const QVariant& op, const QVariantMap& settings);
	void sendEvent(Event *event);
	void close();

	bool isDestroyed() const { return m_bDestroyed; }
	bool isClosing() const { return m_bClosing; }

	void setScrollPosition(const QPoint& scrollPos);
	QPoint getScrollPosition();

	void setViewportSize(const QSize& size);
	QSize getViewportSize();

	void setViewportSizeOffset(const QPoint& viewportSizeOffset);

	QSize getContentSize();

	void historyBack();
	void historyForward();

	void reloadPage(bool bypassCache = false);

	QString getPageTitle();

	QUrl getUrl();

	void popupClose();
	void popupSelectionChangedAndClose(int index);

	void triggerAction(const QString& action);

	jobject setEventHandler(jobject eventHandler);

	void processDownloadItem(DownloadItem *item);

	static void loadJavaClasses(JNIEnv *env);
	static jobject getWindowClass(JNIEnv *env, Window *win);
	static Window* getWindowClass(JNIEnv *env, jobject clazz);
	static Window* clearWindowClass(JNIEnv *env, jobject clazz);

signals:
	void requestCreateWindow(const QVariantMap& settings, const QPoint& screenSizeOffset);
	void requestSetViewportSize(const QSize& size);
	void requestGetViewportSize(QSize *result);
	void requestRender(const QRect& clipRect, QImage* dest, bool useJpeg);
	void requestLoad(const QString& address, const QVariant& op, const QVariantMap& settings);
	void requestSendEvent(Event *event);
	void requestHistoryBack();
	void requestHistoryForward();
	void requestPageReload(bool bypassCache);
	void requestPageTitle(QString *result);
	void requestUrl(QUrl *result);
	void requestTriggerAction(const QString& action);
	void requestWindowClose();
	void requestSetScrollPosition(const QPoint& scrollPos);
	void requestGetScrollPosition(QPoint *result);
	void requestGetContentSize(QSize *result);
	void requestSetViewportSizeOffset(const QPoint& viewportSizeOffset);

public slots:
	void doneCreateWindow();
	void doneSetViewportSize();
	void doneGetViewportSize();
	void doneRender();
	void doneHistoryBack();
	void doneHistoryForward();
	void donePageTitle();
	void doneUrl();
	void doneTriggerAction();
	void donePageReload();
	void doneSetScrollPosition();
	void doneGetScrollPosition();
	void doneGetContentSize();
	void doneSetViewportSizeOffset();
	void handleLoadStarted();
	void handleInitialLayoutComplete();
	void handleLoadFinished(bool result);
	void handleLoadProgress(int progress);
	void handleUrlChanged(const QString& url, bool *trusted);
	void handleTitleChanged(const QString& title);
	void handleRepaintRequest(const QRect& dirtyRect);
	void handleJavaScriptAlert(const QString& msg);
	bool handleJavaScriptInterrupt();
	bool handleJavaScriptConfirm(const QString& msg);
	bool handleJavaScriptPrompt(const QString& msg, const QString& defaultValue, QString *result);
	void handlePopupDialog(QObject *object);
	void handleIconChange(const QByteArray& buffer);
	void handleMicroFocusChange(const QRect& cursor);
	void handleDownloadRequest(QNetworkReply *reply);
	void handleOnScroll(const QPoint& scrollPos);
	void handleSelectionChange(const QString& text);
	void handlePopupDestroyed(QObject *obj);
	void handleContextMenu(const QPoint& pos);
	void handleSslErrors(QNetworkReply *reply, const QList<QSslError>& errors);
	void handleCursorChange(Qt::CursorShape shape, const QImage& cursorImage);
	void handleAuthenticationRequest(QNetworkReply *reply, QAuthenticator *authenticator);
	QVariantMap handleNavigationRequest(const QUrl& requestUrl, const QString& navigationType, bool isMainFrame);
	QVariantMap handleErrorExtension(const QWebPage::ErrorPageExtensionOption *option, bool isMainFrame);
	void setClosing(bool closing);
	void setDestroyed(bool destroyed);
	void siteTrusted(const QString& origin, bool trusted);

private:
	void connectSignals();
	void wakeFunction(int nFunction);
	void removeEventHandler();
	void clearJavaInstance();
	void createWindow(const QVariantMap& settings, const QPoint& screenSizeOffset = QPoint());

	QString getCSSCursorType(Qt::CursorShape shape);

private:
	QUuid m_uId;;
	QMutex m_mutex;
	QWaitCondition m_waiter;
	WindowPrivate *m_pWindowPrivate;
	QWebPopup *m_pOpenPopup;

	int m_nOperationState;

	bool m_bDestroyed;
	bool m_bClosing;

	jobject m_eventHandler;
	jobject m_javaInstance;

	static jobject classWindow;
	static jmethodID windowInitMID;
	static jfieldID windowHandleFID;
	static jfieldID windowUuidFID;

	static jobject classComplexResultData;
	static jmethodID complexResultDataInitMID;
	static jfieldID complexResultDataReturnValueFID;
	static jfieldID complexResultDataReturnDataFID;
};

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_dispose(JNIEnv *env, jobject obj);
JNIEXPORT jboolean JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_isClosing(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_setClosing(JNIEnv *env, jobject obj, jboolean closing);
JNIEXPORT jboolean JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_isDestroyed(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_setDestroyed(JNIEnv *env, jobject obj, jboolean destroyed);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_triggerAction(JNIEnv *env, jobject obj, jstring action);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_load(JNIEnv *env, jobject obj, jstring url);
JNIEXPORT jintArray JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_render(JNIEnv *env, jobject obj, jobject clipRect, jboolean useJpeg);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_sendEvent(JNIEnv *env, jobject obj, jobject eventObj);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_setWindowSize(JNIEnv *env, jobject obj, jobject size);
JNIEXPORT jobject JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_getWindowSize(JNIEnv *env, jobject obj);
JNIEXPORT jobject JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_getContentSize(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_setEventHandler(JNIEnv *env, jobject obj, jobject eventHandler);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_historyBack(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_historyForward(JNIEnv *env, jobject obj);
JNIEXPORT jstring JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_getPageTitle(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_reload(JNIEnv *env, jobject obj, jboolean bypassCache);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_popupSelectionChangeAndClose(JNIEnv *env, jobject obj, jint index);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_popupClose(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_setScrollPosition(JNIEnv *env, jobject obj, jobject scrollPos);
JNIEXPORT jobject JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_getScrollPosition(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_setViewportSizeOffset(JNIEnv *env, jobject obj, jobject viewportSizeOffset);

#ifdef __cplusplus
}
#endif

#endif /* WINDOW_H_ */
