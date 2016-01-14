/*
 * window.cpp
 *
 *  Created on: Oct 2, 2012
 *      Author: mike
 */

#include "utils.h"
#include "event.h"
#include "webkit.h"
#include "window.h"
#include "window_private.h"
#include "unbluwebpopupcombo.h"
#include "jvmcontext.h"

jobject Window::classWindow;
jmethodID Window::windowInitMID;
jfieldID Window::windowHandleFID;
jfieldID Window::windowUuidFID;

jobject Window::classComplexResultData;
jmethodID Window::complexResultDataInitMID;
jfieldID Window::complexResultDataReturnValueFID;
jfieldID Window::complexResultDataReturnDataFID;

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_dispose(JNIEnv *env, jobject obj)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
		win->close();
	}
}

JNIEXPORT jboolean JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_isClosing(JNIEnv *env, jobject obj)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
#ifdef U_WIN_DEBUG
		qDebug() << Q_FUNC_INFO << win->isClosing();
#endif
		return (jboolean) win->isClosing();
	}

	return true;
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_setClosing(JNIEnv *env, jobject obj, jboolean closing)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
#ifdef U_WIN_DEBUG
		qDebug() << Q_FUNC_INFO << (bool) closing;
#endif
		win->setClosing((bool) closing);
	}
}

JNIEXPORT jboolean JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_isDestroyed(JNIEnv *env, jobject obj)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
#ifdef U_WIN_DEBUG
		qDebug() << Q_FUNC_INFO << win->isDestroyed();
#endif
		return (jboolean) win->isDestroyed();
	}

	return true;
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_setDestroyed(JNIEnv *env, jobject obj, jboolean destroyed)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
#ifdef U_WIN_DEBUG
		qDeub() << Q_FUNC_INFO << (bool) destroyed;
#endif
		win->setDestroyed((bool) destroyed);
	}
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_load(JNIEnv *env, jobject obj, jstring url)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
		QString address = Utils::getQString(env, url);
		win->load(address, QVariant(), WebKit::instance()->defaultPageSettings());
	}
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_triggerAction(JNIEnv *env, jobject obj, jstring action)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
		win->triggerAction(Utils::getQString(env, action));
	}
}

JNIEXPORT jintArray JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_render(JNIEnv *env, jobject obj, jobject clipRect, jboolean useJpeg)
{
	jintArray result = NULL;
	Window *win = Window::getWindowClass(env, obj);

	Utils::pushFrame(env);

	if(win != NULL) {
		QRect clippingRect = Utils::getQRect(env, clipRect);
		QImage buffer = win->render(clippingRect, (bool)useJpeg);
		result = Utils::getIntArray(env, buffer.constBits(), buffer.byteCount() >> 2);
	}

	return (jintArray) Utils::popFrame(env, result);
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_sendEvent(JNIEnv *env, jobject obj, jobject eventObj)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
		Event *event = Event::createEvent(env, eventObj);

		if(event != NULL) {
			win->sendEvent(event);
		}
	}
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_setWindowSize(JNIEnv *env, jobject obj, jobject size)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
		QSize qSize = Utils::getQSize(env, size);
		win->setViewportSize(qSize);
	}
}

JNIEXPORT jobject JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_getWindowSize(JNIEnv *env, jobject obj)
{
	jobject result = NULL;
	Window *win = Window::getWindowClass(env, obj);

	Utils::pushFrame(env);

	if(win != NULL) {
		QSize size = win->getViewportSize();
		result = Utils::getAWTDimension(env, size);
	}

	return Utils::popFrame(env, result);
}

JNIEXPORT jobject JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_getContentSize(JNIEnv *env, jobject obj)
{
	jobject result = NULL;
	Window *win = Window::getWindowClass(env, obj);

	Utils::pushFrame(env);

	if(win != NULL) {
		QSize size = win->getContentSize();
		result = Utils::getAWTDimension(env, size);
	}

	return Utils::popFrame(env, result);
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_setEventHandler(JNIEnv *env, jobject obj, jobject eventHandler)
{
	Window *win = Window::getWindowClass(env, obj);
	jobject refObj = Utils::newGlobalRef(env, eventHandler);

	if(win != NULL) {
		jobject old = win->setEventHandler(refObj);
		Utils::deleteGlobalRef(env, old);
		return;
	}
	Utils::deleteGlobalRef(env, refObj);
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_historyBack(JNIEnv *env, jobject obj)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
		win->historyBack();
	}
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_historyForward(JNIEnv *env, jobject obj)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
		win->historyForward();
	}
}

JNIEXPORT jstring JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_getPageTitle(JNIEnv *env, jobject obj)
{
	jstring result = NULL;
	Window *win = Window::getWindowClass(env, obj);

	Utils::pushFrame(env);

	if(win != NULL) {
		QString title = win->getPageTitle();
		result = Utils::getJString(env, title);
	}

	return (jstring) Utils::popFrame(env, result);
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_reload(JNIEnv *env, jobject obj, jboolean bypassCache)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
		win->reloadPage(bypassCache);
	}
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_popupSelectionChangeAndClose(JNIEnv *env, jobject obj, jint index)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
		win->popupSelectionChangedAndClose(index);
	}
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_popupClose(JNIEnv *env, jobject obj)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
		win->popupClose();
	}
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_setScrollPosition(JNIEnv *env, jobject obj, jobject scrollPos)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
		QPoint pos = Utils::getQPoint(env, scrollPos);
		win->setScrollPosition(pos);
	}
}

JNIEXPORT jobject JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_getScrollPosition(JNIEnv *env, jobject obj)
{
	jobject result = NULL;
	Window *win = Window::getWindowClass(env, obj);

	Utils::pushFrame(env);

	if(win != NULL) {
		QPoint scrollPos = win->getScrollPosition();
		result = Utils::getAWTPoint(env, scrollPos);
	}

	return Utils::popFrame(env, result);
}

JNIEXPORT void JNICALL Java_com_unblu_webkit_main_server_core_internal_Window_setViewportSizeOffset(JNIEnv *env, jobject obj, jobject viewportSizeOffset)
{
	Window *win = Window::getWindowClass(env, obj);

	if(win != NULL) {
		QPoint offset = Utils::getQPoint(env, viewportSizeOffset);
		win->setViewportSizeOffset(offset);
	}
}

Window::Window(const QVariantMap& settings, const QPoint& screenSizeOffset, QObject *parent)
: JQObject(parent), m_nOperationState(0), m_bDestroyed(false), m_bClosing(false), m_eventHandler(NULL), m_javaInstance(NULL)
{
	setObjectName("Window");

	m_uId = QUuid::createUuid();

	m_pWindowPrivate = new WindowPrivate(this);
	m_pWindowPrivate->moveToThread(QApplication::instance()->thread());

	connectSignals();
	createWindow(settings, screenSizeOffset);
}

Window::Window(WindowPrivate *window, QObject *parent)
: JQObject(parent), m_pWindowPrivate(window),  m_nOperationState(WINDOW_CREATE), m_bDestroyed(false), m_bClosing(false),
  m_eventHandler(NULL), m_javaInstance(NULL)
{
	setObjectName("Window");

	m_uId = QUuid::createUuid();

	m_pWindowPrivate->setWindow(this);
	m_pWindowPrivate->moveToThread(QApplication::instance()->thread());

	connectSignals();
}

Window::~Window()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	disconnect(this, 0, 0, 0);

	removeEventHandler();
	clearJavaInstance();

	m_pWindowPrivate = NULL;
	m_eventHandler = NULL;
	m_javaInstance = NULL;
}

void Window::close()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	setClosing(true);

	emit requestWindowClose();

	deleteLater();
}

void Window::setClosing(bool closing)
{
	m_bClosing = closing;
}

void Window::setDestroyed(bool destroyed)
{
	m_bDestroyed = destroyed;
}

jobject Window::setEventHandler(jobject eventHandler)
{
	jobject old = m_eventHandler;
	m_eventHandler = eventHandler;
	return old;
}

void Window::processDownloadItem(DownloadItem *item)
{
	const QNetworkReply *reply = item->reply();

	QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
	if (contentType.compare("application/pdf", Qt::CaseInsensitive) == 0) {
		m_pWindowPrivate->openPdfJsView(reply->request().originatingObject(), item->blobUri(), reply->url());
	}
}

void Window::load(const QString& address, const QVariant& op, const QVariantMap& settings)
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	if(!isMainThread())
		emit requestLoad(address, op, settings);
	else
		m_pWindowPrivate->load(address, op, settings);
}

void Window::sendEvent(Event *event)
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << event;
#endif
	if(isClosing() || isDestroyed()) return;

	if(!isMainThread()) {
		emit requestSendEvent(event);
	} else {
		m_pWindowPrivate->sendEvent(event->getEventType(), event->getEventData());
		delete event;
	}
}

QImage Window::render(const QRect& clipRect, bool useJpeg)
{
	QImage dest;

	if(isClosing() || isDestroyed()) return dest;

	if(!isMainThread()) {
		m_mutex.lock();
		m_nOperationState &= ~WINDOW_RENDER;
		emit requestRender(clipRect, &dest, useJpeg);
		while(!(m_nOperationState&WINDOW_RENDER))
			m_waiter.wait(&m_mutex);
		m_mutex.unlock();
	} else
		dest = m_pWindowPrivate->render(clipRect, useJpeg);

	return dest;
}

QSize Window::getContentSize()
{
	QSize ctSize;

	if(isClosing() || isDestroyed()) return ctSize;

	if(!isMainThread()) {
		m_mutex.lock();
		m_nOperationState &= ~WINDOW_GETCONTENTSIZE;
		emit requestGetContentSize(&ctSize);
		while(!(m_nOperationState&WINDOW_GETCONTENTSIZE))
			m_waiter.wait(&m_mutex);
		m_mutex.unlock();
	} else
		ctSize = m_pWindowPrivate->contentSize();

	return ctSize;
}

void Window::historyBack()
{
	if(isClosing() || isDestroyed()) return;

	if(!isMainThread()) {
		m_mutex.lock();
		m_nOperationState &= ~WINDOW_HISTORY_BACK;
		emit requestHistoryBack();
		while(!(m_nOperationState&WINDOW_HISTORY_BACK))
			m_waiter.wait(&m_mutex);
		m_mutex.unlock();
	} else
		m_pWindowPrivate->historyBack();
}

void Window::historyForward()
{
	if(isClosing() || isDestroyed()) return;

	if(!isMainThread()) {
		m_mutex.lock();
		m_nOperationState &= ~WINDOW_HISTORY_FORWARD;
		emit requestHistoryForward();
		while(!(m_nOperationState&WINDOW_HISTORY_FORWARD))
			m_waiter.wait(&m_mutex);
		m_mutex.unlock();
	} else
		m_pWindowPrivate->historyForward();
}

void Window::reloadPage(bool bypassCache)
{
	if(isClosing() || isDestroyed()) return;

	if(!isMainThread()) {
		m_mutex.lock();
		m_nOperationState &= ~WINDOW_PAGERELOAD;
		emit requestPageReload(bypassCache);
		while(!(m_nOperationState&WINDOW_PAGERELOAD))
			m_waiter.wait(&m_mutex);
		m_mutex.unlock();
	} else
		m_pWindowPrivate->reloadPage(bypassCache);
}

void Window::setViewportSize(const QSize& size)
{
	if(isClosing() || isDestroyed()) return;

	if(!isMainThread()) {
		m_mutex.lock();
		m_nOperationState &= ~WINDOW_SETVIEWPORTSIZE;
		emit requestSetViewportSize(size);
		while(!(m_nOperationState&WINDOW_SETVIEWPORTSIZE))
			m_waiter.wait(&m_mutex);
		m_mutex.unlock();
	} else
		m_pWindowPrivate->setViewportSize(size);
}

QSize Window::getViewportSize()
{
	QSize vpSize;

	if(isClosing() || isDestroyed()) return vpSize;

	if(!isMainThread()) {
		m_mutex.lock();
		m_nOperationState &= ~WINDOW_GETVIEWPORTSIZE;
		emit requestGetViewportSize(&vpSize);
		while(!(m_nOperationState&WINDOW_GETVIEWPORTSIZE))
			m_waiter.wait(&m_mutex);
		m_mutex.unlock();
	} else
		vpSize = m_pWindowPrivate->viewportSize();

	return vpSize;
}

void Window::setScrollPosition(const QPoint& scrollPos)
{
	if(isClosing() || isDestroyed()) return;

	if(!isMainThread()) {
		m_mutex.lock();
		m_nOperationState &= ~WINDOW_SETSCROLLPOSITION;
		emit requestSetScrollPosition(scrollPos);
		while(!(m_nOperationState&WINDOW_SETSCROLLPOSITION))
			m_waiter.wait(&m_mutex);
		m_mutex.unlock();
	} else
		m_pWindowPrivate->setScrollPosition(scrollPos);
}

QPoint Window::getScrollPosition()
{
	QPoint scrollPos;

	if(isClosing() || isDestroyed()) return scrollPos;

	if(!isMainThread()) {
		m_mutex.lock();
		m_nOperationState &= ~WINDOW_GETSCROLLPOSITION;
		emit requestGetScrollPosition(&scrollPos);
		while(!(m_nOperationState&WINDOW_GETSCROLLPOSITION))
			m_waiter.wait(&m_mutex);
		m_mutex.unlock();
	} else
		scrollPos = m_pWindowPrivate->scrollPosition();

	return scrollPos;
}

void Window::setViewportSizeOffset(const QPoint& viewportSizeOffset)
{
	if(isClosing() || isDestroyed()) return;

	if(!isMainThread()) {
		m_mutex.lock();
		m_nOperationState &= ~WINDOW_SETVIEWPORTSIZE_OFFSET;
		emit requestSetViewportSizeOffset(viewportSizeOffset);
		while(!(m_nOperationState&WINDOW_SETVIEWPORTSIZE_OFFSET))
			m_waiter.wait(&m_mutex);
		m_mutex.unlock();
	} else
		m_pWindowPrivate->setViewportSizeOffset(viewportSizeOffset);
}

QString Window::getPageTitle()
{
	QString pageTitle;

	if(isClosing() || isDestroyed()) return pageTitle;

	if(!isMainThread()) {
		m_mutex.lock();
		m_nOperationState &= ~WINDOW_GETPAGETITLE;
		emit requestPageTitle(&pageTitle);
		while(!(m_nOperationState&WINDOW_GETPAGETITLE))
			m_waiter.wait(&m_mutex);
		m_mutex.unlock();
	} else
		pageTitle = m_pWindowPrivate->title();

	return pageTitle;
}

QUrl Window::getUrl()
{
	QUrl url;

	if(isClosing() || isDestroyed()) return url;

	if(!isMainThread()) {
		m_mutex.lock();
		m_nOperationState &= ~WINDOW_GETURL;
		emit requestUrl(&url);
		while(!(m_nOperationState&WINDOW_GETURL))
			m_waiter.wait(&m_mutex);
		m_mutex.unlock();
	} else
		url = m_pWindowPrivate->url();

	return url;
}

void Window::triggerAction(const QString& action)
{
	if(isClosing() || isDestroyed()) return;

	if(!isMainThread()) {
		m_mutex.lock();
		m_nOperationState &= ~WINDOW_TRIGGERACTION;
		emit requestTriggerAction(action);
		while(!(m_nOperationState&WINDOW_TRIGGERACTION))
			m_waiter.wait(&m_mutex);
		m_mutex.unlock();
	} else
		m_pWindowPrivate->triggerAction(action);
}

void Window::popupSelectionChangedAndClose(int index)
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	if(isClosing() || isDestroyed()) return;

	if(m_pOpenPopup != NULL) {
		if(UnbluWebPopup *popup = qobject_cast<UnbluWebPopup*>(m_pOpenPopup)) {
			popup->activeChanged(index);
		}
	}
	popupClose();
}

void Window::popupClose()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	if(isClosing() || isDestroyed()) return;

	if(m_pOpenPopup != NULL) {
		if(UnbluWebPopup *popup = qobject_cast<UnbluWebPopup*>(m_pOpenPopup)) {
			popup->hidePopup();
		}
		handlePopupDialog(NULL);
		m_pOpenPopup = NULL;
	}
}

void Window::doneRender()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	wakeFunction(WINDOW_RENDER);
}

void Window::doneCreateWindow()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	wakeFunction(WINDOW_CREATE);
}

void Window::doneSetViewportSize()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	wakeFunction(WINDOW_SETVIEWPORTSIZE);
}

void Window::doneGetViewportSize()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	wakeFunction(WINDOW_GETVIEWPORTSIZE);
}

void Window::doneHistoryBack()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	wakeFunction(WINDOW_HISTORY_BACK);
}

void Window::doneHistoryForward()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	wakeFunction(WINDOW_HISTORY_FORWARD);
}

void Window::donePageTitle()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	wakeFunction(WINDOW_GETPAGETITLE);
}

void Window::doneUrl()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	wakeFunction(WINDOW_GETURL);
}

void Window::doneTriggerAction()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	wakeFunction(WINDOW_TRIGGERACTION);
}

void Window::donePageReload()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	wakeFunction(WINDOW_PAGERELOAD);
}

void Window::doneSetScrollPosition()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	wakeFunction(WINDOW_SETSCROLLPOSITION);
}

void Window::doneGetScrollPosition()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	wakeFunction(WINDOW_GETSCROLLPOSITION);
}

void Window::doneGetContentSize()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	wakeFunction(WINDOW_GETCONTENTSIZE);
}

void Window::doneSetViewportSizeOffset()
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	wakeFunction(WINDOW_SETVIEWPORTSIZE_OFFSET);
}

void Window::handleRepaintRequest(const QRect& dirtyRect)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << dirtyRect;
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);
		jobject jrect = Utils::getAWTRectangle(env, dirtyRect);

		Utils::invokeMethod(env, m_eventHandler, "onRepaint", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/awt/Rectangle;)V", jwin, jrect);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

bool Window::handleJavaScriptInterrupt()
{
	JNIEnv *env;
	bool retVal = false;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	Utils::pushFrame(env);
	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);

		retVal = (bool) Utils::invokeMethod(env, m_eventHandler, "onInterruptJavaScript", "(Lcom/unblu/webkit/main/server/core/internal/Window;)Z", jwin).value<jboolean>();
	}
	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
	return retVal;
}

void Window::handleJavaScriptAlert(const QString& msg)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << msg << m_eventHandler;
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);
		jstring jstr = Utils::getJString(env, msg);

		Utils::invokeMethod(env, m_eventHandler, "onJavaScriptAlert", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/lang/String;)V", jwin, jstr);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

bool Window::handleJavaScriptConfirm(const QString& msg)
{
	JNIEnv *env;
	bool retVal = false;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << msg;
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);
		jstring jstr = Utils::getJString(env, msg);
		jobject jresult = Utils::invokeMethod(env, m_eventHandler, "onJavaScriptConfirm", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/lang/String;)Lcom/unblu/headlessbrowser/model/shared/core/ComplexResultData;", jwin, jstr).value<jobject>();

		retVal = (bool) env->GetBooleanField(jresult, complexResultDataReturnValueFID);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
	return retVal;
}

bool Window::handleJavaScriptPrompt(const QString& msg, const QString& defaultValue, QString *result)
{
	JNIEnv *env;
	bool retVal = false;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << msg << defaultValue;
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);
		jstring jmsg = Utils::getJString(env, msg);
		jstring jdef = Utils::getJString(env, defaultValue);
		jobject jresult = Utils::invokeMethod(env, m_eventHandler, "onJavaScriptPrompt", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/lang/String;Ljava/lang/String;)Lcom/unblu/headlessbrowser/model/shared/core/ComplexResultData;", jwin, jmsg, jdef).value<jobject>();

		*result = Utils::getQString(env, env->GetObjectField(jresult, complexResultDataReturnDataFID));
		retVal = (bool) env->GetBooleanField(jresult, complexResultDataReturnValueFID);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
	return retVal;
}

QVariantMap Window::handleNavigationRequest(const QUrl& requestUrl, const QString& navigationType, bool isMainFrame)
{
	JNIEnv *env;
	QVariantMap result;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << requestUrl << navigationType << isMainFrame;
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);
		jstring jurl = Utils::getJString(env, requestUrl.toString());
		jstring jtype = Utils::getJString(env, navigationType);
		jobject jresult = Utils::invokeMethod(env, m_eventHandler, "onNavigationRequested", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/lang/String;Ljava/lang/String;Z)Lcom/unblu/headlessbrowser/model/shared/core/ComplexResultData;", jwin, jurl, jtype, (jboolean)isMainFrame).value<jobject>();

		QString returnData = Utils::getQString(env, env->GetObjectField(jresult, complexResultDataReturnDataFID));
		if(!returnData.isEmpty()) {
			result[COMPLEX_RESULT_RETURNDATA] = QVariant::fromValue(returnData);
		}
		result[COMPLEX_RESULT_RETURNVALUE] = QVariant::fromValue((bool)env->GetBooleanField(jresult, complexResultDataReturnValueFID));
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
	return result;
}

QVariantMap Window::handleErrorExtension(const QWebPage::ErrorPageExtensionOption *option, bool isMainFrame)
{
	JNIEnv *env;
	QVariantMap result;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << option->domain << option->error << option->errorString << option->url;
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);
		jstring jerrstr = Utils::getJString(env, option->errorString);
		jstring jurl = Utils::getJString(env, option->url.toString());
		jobject jresult = Utils::invokeMethod(env, m_eventHandler, "onErrorExtension", "(Lcom/unblu/webkit/main/server/core/internal/Window;IILjava/lang/String;Ljava/lang/String;Z)Lcom/unblu/headlessbrowser/model/shared/core/ComplexResultData;", jwin, (jint)option->domain, (jint)option->error, jerrstr, jurl, (jboolean)isMainFrame).value<jobject>();

		QString returnData = Utils::getQString(env, env->GetObjectField(jresult, complexResultDataReturnDataFID));
		if(!returnData.isEmpty()) {
			result[COMPLEX_RESULT_RETURNDATA] = QVariant::fromValue(returnData);
		}
		result[COMPLEX_RESULT_RETURNVALUE] = QVariant::fromValue((bool)env->GetBooleanField(jresult, complexResultDataReturnValueFID));
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
	return result;
}

void Window::handleIconChange(const QByteArray& buffer)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);
		jbyteArray result = Utils::getByteArray(env, (uchar*)buffer.data(), buffer.size());

		Utils::invokeMethod(env, m_eventHandler, "onIconChanged", "(Lcom/unblu/webkit/main/server/core/internal/Window;[B)V", jwin, result);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::handleMicroFocusChange(const QRect& cursor)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << cursor;
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);
		jobject jcursor = cursor.isValid() ? Utils::getAWTRectangle(env, cursor) : NULL;

		Utils::invokeMethod(env, m_eventHandler, "onMicroFocusChanged", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/awt/Rectangle;)V", jwin, jcursor);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::handleSelectionChange(const QString& text)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << text;
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);
		jstring jstr = Utils::getJString(env, text);

		Utils::invokeMethod(env, m_eventHandler, "onSelectionChanged", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/lang/String;)V", jwin, jstr);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::handleCursorChange(Qt::CursorShape shape, const QImage& cursorImage)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << shape;
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		jbyteArray jdata = NULL;
		jobject jwin = getWindowClass(env, this);
		jstring jtype = Utils::getJString(env, getCSSCursorType(shape));

		if(!cursorImage.isNull()) {
			QByteArray bytes;
			QBuffer buffer(&bytes);

			buffer.open(QIODevice::WriteOnly);
			cursorImage.save(&buffer, "CUR");

			jdata = Utils::getByteArray(env, (uchar*)bytes.data(), bytes.size());
		}
		Utils::invokeMethod(env, m_eventHandler, "onCursorChanged", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/lang/String;[B)V", jwin, jtype, jdata);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::handlePopupDialog(QObject *object)
{
	JNIEnv *env;
	jobject jpopup = NULL;
	jobject jdialogdata = NULL;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << object;
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);

		if(UnbluWebPopup *popup = qobject_cast<UnbluWebPopup*>(object)) {
			m_pOpenPopup = popup;

			connect(popup, SIGNAL(destroyed(QObject*)), this, SLOT(handlePopupDestroyed(QObject*)), Qt::AutoConnection);

			jpopup = popup->createPopupDialog(env);
			if(UnbluWebPopupCombo *combo = qobject_cast<UnbluWebPopupCombo*>(popup->content())) {
				jdialogdata = combo->createSelectItemData(env);
			}
			Utils::invokeMethod(env, jpopup, "setDialogData", "(Ljava/lang/Object;)V", jdialogdata);
		}

		Utils::invokeMethod(env, m_eventHandler, "onPopupDialog", "(Lcom/unblu/webkit/main/server/core/internal/Window;Lcom/unblu/headlessbrowser/model/server/core/input/PopupDialog;)V", jwin, jpopup);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::handlePopupDestroyed(QObject *obj)
{
	Q_UNUSED(obj);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	if(m_pOpenPopup != NULL) {
		handlePopupDialog(NULL);
		m_pOpenPopup = NULL;
	}
}

void Window::handleContextMenu(const QPoint& pos)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	Utils::pushFrame(env);
	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);
		jobject jpos = Utils::getAWTPoint(env, pos);
		Utils::invokeMethod(env, m_eventHandler, "onContextMenu", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/awt/Point;)V", jwin, jpos);
	}
	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::handleAuthenticationRequest(QNetworkReply *reply, QAuthenticator *authenticator)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
	const QUrl& url = reply->request().url();
	QString origin = url.toEncoded(QUrl::RemoveUserInfo | QUrl::RemovePath | QUrl::RemoveQuery | QUrl::RemoveFragment);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	Utils::pushFrame(env);
	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);
		jobject jresult = Utils::invokeMethod(env, m_eventHandler, "onAuthenticationRequired", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/lang/String;)Lcom/unblu/headlessbrowser/model/shared/core/ComplexResultData;", jwin, Utils::getJString(env, origin)).value<jobject>();

		bool retVal = (bool) env->GetBooleanField(jresult, complexResultDataReturnValueFID);
		if(retVal) {
			QString returnData = Utils::getQString(env, env->GetObjectField(jresult, complexResultDataReturnDataFID));
			QVariantMap jsonData = QJsonDocument::fromJson(returnData.toUtf8()).toVariant().toMap();

			authenticator->setUser(jsonData["username"].toString());
			authenticator->setPassword(jsonData["password"].toString());
		}
	}
	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::siteTrusted(const QString& origin, bool trusted)
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << origin << trusted;
#endif
	QString windowOrigin = m_pWindowPrivate->url().toEncoded(QUrl::RemoveUserInfo | QUrl::RemovePath | QUrl::RemoveQuery | QUrl::RemoveFragment);
	if (origin != windowOrigin) return;

	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);

	Utils::pushFrame(env);
	if(!Utils::isNullObject(env, m_eventHandler)) {
		Utils::invokeMethod(env, m_eventHandler, "onSiteTrusted", "(Lcom/unblu/webkit/main/server/core/internal/Window;Z)V", getWindowClass(env, this), (jboolean) trusted);
	}
	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::handleSslErrors(QNetworkReply *reply, const QList<QSslError>& errors)
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << reply->request().url.host() << + ":" << reply->request().url.port();
#endif
	const QUrl& url = reply->request().url();
	const QSslConfiguration& sslConfig = reply->sslConfiguration();

	// if the server presented no certificate or we are not connected just return and let it fail.
	if(sslConfig.peerCertificate().isNull())
		return;

	const QByteArray& digest = sslConfig.peerCertificate().digest();
	if(WebKit::instance()->certificateException().contains(QPair<QString, int>(url.host(), url.port()))) {
		const QByteArray& storedDigest = WebKit::instance()->certificateException().value(QPair<QString, int>(url.host(), url.port()));

		if(storedDigest == digest) {
			reply->ignoreSslErrors();
			return;
		}
	}

	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);

	Utils::pushFrame(env);
	if(!Utils::isNullObject(env, m_eventHandler)) {
		QString authority;
		QList<int> sslErrorCodes;
		QStringList sslErrorMessages;
		const QStringList& commonNamesList = sslConfig.peerCertificate().subjectInfo(QSslCertificate::CommonName);

		if(url.port() != -1)
			authority = QString("%1:%2").arg(url.host()).arg(url.port());
		else
			authority = QString("%1").arg(url.host());

		foreach(const QSslError& e, errors) {
			sslErrorCodes.append(e.error());
			sslErrorMessages.append(e.errorString());
		}

		jobject jwin = getWindowClass(env, this);
		jobject jcnlist = Utils::getJStringArray(env, commonNamesList);
		jobject jerrorMessages = Utils::getJStringArray(env, sslErrorMessages);
		jobject jerrorCodes = Utils::getJIntegerArray(env, sslErrorCodes.toVector().data(), sslErrorCodes.size());
		jstring jauthority = Utils::getJString(env, authority);
		bool ignoreSslErrors = (bool) Utils::invokeMethod(env, m_eventHandler, "onSslErrors", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/lang/String;[Ljava/lang/Integer;[Ljava/lang/String;[Ljava/lang/String;)Z", jwin, jauthority, jerrorCodes, jerrorMessages, jcnlist).value<jboolean>();

		if(ignoreSslErrors) {
			WebKit::instance()->certificateException().insert(QPair<QString, int>(url.host(), url.port()), digest);
			reply->ignoreSslErrors();
		} else {
			reply->abort();

			// only set back if we are in the main frame loader
			if(m_pWindowPrivate->mainFrame() == m_pWindowPrivate->currentFrame()) {
				bool trusted;
				bool *pTrusted = NULL;
				QUrl baseUrl = m_pWindowPrivate->mainFrame()->url();

				if(baseUrl.isEmpty())
					baseUrl = "about:blank";
				else {
					QString origin = baseUrl.toEncoded(QUrl::RemoveUserInfo | QUrl::RemovePath | QUrl::RemoveQuery | QUrl::RemoveFragment);

					if (WebKit::instance()->trustedSites().contains(origin)) {
						pTrusted = &trusted;
						*pTrusted = WebKit::instance()->trustedSites().find(origin).value() == true;
					}
				}
				handleUrlChanged(baseUrl.toString(), pTrusted);
			}
		}
	}
	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::handleUrlChanged(const QString& url, bool *trusted)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);

	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jtrusted = NULL;
		jobject jwin = getWindowClass(env, this);
		jstring jurl = Utils::getJString(env, url);

		if (trusted != NULL) {
			jtrusted = Utils::getJBoolean(env, *trusted);
		}
		Utils::invokeMethod(env, m_eventHandler, "onUrlChanged", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/lang/String;Ljava/lang/Boolean;)V", jwin, jurl, jtrusted);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::handleTitleChanged(const QString& title)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);

	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);
		jstring jtitle = Utils::getJString(env, title);

		Utils::invokeMethod(env, m_eventHandler, "onTitleChanged", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/lang/String;)V", jwin, jtitle);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::handleInitialLayoutComplete()
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);

	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		Utils::invokeMethod(env, m_eventHandler, "onInitialLayout", "(Lcom/unblu/webkit/main/server/core/internal/Window;)V", getWindowClass(env, this));
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::handleLoadStarted()
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);

	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		Utils::invokeMethod(env, m_eventHandler, "onLoadStarted", "(Lcom/unblu/webkit/main/server/core/internal/Window;)V", getWindowClass(env, this));
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::handleLoadProgress(int progress)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);

	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		Utils::invokeMethod(env, m_eventHandler, "onLoadProgress", "(Lcom/unblu/webkit/main/server/core/internal/Window;I)V", getWindowClass(env, this), (jint)progress);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::handleLoadFinished(bool result)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);

	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		Utils::invokeMethod(env, m_eventHandler, "onLoadFinished", "(Lcom/unblu/webkit/main/server/core/internal/Window;Z)V", getWindowClass(env, this), (jboolean)result);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::handleDownloadRequest(QNetworkReply *reply)
{
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	WebKit::instance()->downloadManager()->download(this, reply);
}

void Window::handleOnScroll(const QPoint& scrollPos)
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << scrollPos;
#endif
	Utils::pushFrame(env);

	if(!Utils::isNullObject(env, m_eventHandler)) {
		jobject jwin = getWindowClass(env, this);
		jobject jsbPos = Utils::getAWTPoint(env, scrollPos);

		Utils::invokeMethod(env, m_eventHandler, "onScrollPositionChanged", "(Lcom/unblu/webkit/main/server/core/internal/Window;Ljava/awt/Point;)V", jwin, jsbPos);
	}

	Utils::popFrame(env);

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::createWindow(const QVariantMap& settings, const QPoint& screenSizeOffset)
{
	if(!isMainThread()) {
		connect(this, SIGNAL(requestCreateWindow(const QVariantMap&, const QPoint&)), m_pWindowPrivate, SLOT(handleCreateWindow(const QVariantMap&, const QPoint&)));
		m_mutex.lock();
		emit requestCreateWindow(settings, screenSizeOffset);
		while(!(m_nOperationState&WINDOW_CREATE))
			m_waiter.wait(&m_mutex);
		m_mutex.unlock();
		disconnect(this, SIGNAL(requestCreateWindow(const QVariantMap&, const QPoint&)), m_pWindowPrivate, SLOT(handleCreateWindow(const QVariantMap&, const QPoint&)));
	} else
		m_pWindowPrivate->createWindow(settings, screenSizeOffset);
}

void Window::connectSignals()
{
	connect(this, SIGNAL(requestWindowClose()), m_pWindowPrivate, SLOT(handleWindowClose()), Qt::AutoConnection);
	connect(this, SIGNAL(requestLoad(const QString&, const QVariant&, const QVariantMap&)), m_pWindowPrivate, SLOT(handleLoad(const QString&, const QVariant&, const QVariantMap&)), Qt::AutoConnection);
	connect(this, SIGNAL(requestSetViewportSize(const QSize&)), m_pWindowPrivate, SLOT(handleSetViewportSize(const QSize&)), Qt::AutoConnection);
	connect(this, SIGNAL(requestGetViewportSize(QSize*)), m_pWindowPrivate, SLOT(handleGetViewportSize(QSize*)), Qt::AutoConnection);
	connect(this, SIGNAL(requestRender(const QRect&, QImage*, bool)), m_pWindowPrivate, SLOT(handleRender(const QRect&, QImage*, bool)), Qt::AutoConnection);
	connect(this, SIGNAL(requestSendEvent(Event*)), m_pWindowPrivate, SLOT(handleSendEvent(Event*)), Qt::AutoConnection);
	connect(this, SIGNAL(requestHistoryBack()), m_pWindowPrivate, SLOT(handleHistoryBack()), Qt::AutoConnection);
	connect(this, SIGNAL(requestHistoryForward()), m_pWindowPrivate, SLOT(handleHistoryForward()), Qt::AutoConnection);
	connect(this, SIGNAL(requestPageTitle(QString*)), m_pWindowPrivate, SLOT(handlePageTitle(QString*)), Qt::AutoConnection);
	connect(this, SIGNAL(requestUrl(QUrl*)), m_pWindowPrivate, SLOT(handleUrl(QUrl*)), Qt::AutoConnection);
	connect(this, SIGNAL(requestPageReload(bool)), m_pWindowPrivate, SLOT(handlePageReload(bool)), Qt::AutoConnection);
	connect(this, SIGNAL(requestSetScrollPosition(const QPoint&)), m_pWindowPrivate, SLOT(handleSetScrollPosition(const QPoint&)), Qt::AutoConnection);
	connect(this, SIGNAL(requestGetScrollPosition(QPoint*)), m_pWindowPrivate, SLOT(handleGetScrollPosition(QPoint*)), Qt::AutoConnection);
	connect(this, SIGNAL(requestGetContentSize(QSize*)), m_pWindowPrivate, SLOT(handleGetContentSize(QSize*)), Qt::AutoConnection);
	connect(this, SIGNAL(requestSetViewportSizeOffset(const QPoint&)), m_pWindowPrivate, SLOT(handleSetViewportSizeOffset(const QPoint&)), Qt::AutoConnection);
	connect(this, SIGNAL(requestTriggerAction(const QString&)), m_pWindowPrivate, SLOT(handleTriggerAction(const QString&)), Qt::AutoConnection);
}

void Window::wakeFunction(int nFunction)
{
	m_mutex.lock();
	m_nOperationState |= nFunction;
	m_waiter.wakeAll();
	m_mutex.unlock();
}

void Window::removeEventHandler()
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	if(!Utils::isNullObject(env, m_eventHandler)) {
		Utils::deleteGlobalRef(env, m_eventHandler);
		m_eventHandler = NULL;
	}

	m_pJVMContext->releaseEnvironment(env, attached);
}

void Window::clearJavaInstance()
{
	JNIEnv *env;
	bool attached = m_pJVMContext->getEnvironment(env);
#ifdef U_WIN_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	if(!Utils::isNullObject(env, m_javaInstance)) {
		Utils::deleteGlobalRef(env, m_javaInstance);
		m_javaInstance = NULL;
	}

	m_pJVMContext->releaseEnvironment(env, attached);
}

QString Window::getCSSCursorType(Qt::CursorShape shape)
{
	static const char *css_cursor[] = {
		"default",					// Qt::ArrowCursor
		"",							// Qt::UpArrowCursor
		"crosshair",				// Qt::CrossCursor
		"wait",						// Qt::WaitCursor
		"text",						// Qt::IBeamCursor
		"ns-resize",				// Qt::SizeVerCursor
		"ew-resize",				// Qt::SizeHorCursor
		"nesw-resize",				// Qt::SizeBDiagCursor
		"nwse-resize",				// Qt::SizeFDiagCursor
		"move",						// Qt::SizeAllCursor
		"",							// Qt::BlankCursor
		"",							// Qt::SplitVCursor
		"",							// Qt::SplitHCursor
		"pointer",					// Qt::PointingHandCursor
		"not-allowed",				// Qt::ForbiddenCursor
		"help",						// Qt::WhatsThisCursor
		"",							// Qt::BusyCursor
		"",							// Qt::OpenHandCursor
		"",							// Qt::ClosedHandCursor
		"",							// Qt::DragCopyCursor
		"copy",							// Qt::DragMoveCursor
		"",							// Qt::DragLinkCursor
		"",							// nothing defined here
		"",							// nothing defined here
		"url",						// Qt::BitmapCursor
		"url",						// Qt::CustomCursor
		NULL
	};
	return QString(css_cursor[(int) shape]);
}

void Window::loadJavaClasses(JNIEnv *env)
{
	if(!LOAD_CREF(env, ComplexResultData, "com/unblu/headlessbrowser/model/shared/core/ComplexResultData")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain class com.unblu.headlessbrowser.model.shared.core.ComplexResultData");
	} else if(!LOAD_MID(env, complexResultDataInitMID, classComplexResultData, "<init>", "()V")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain constructor for class com.unblu.headlessbrowser.model.shared.core.ComplexResultData");
	} else if(!LOAD_FID(env, complexResultDataReturnValueFID, classComplexResultData, "returnValue", "Z")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain returnValue field ID for class com.unblu.headlessbrowser.model.shared.core.ComplexResultData");
	} else if(!LOAD_FID(env, complexResultDataReturnDataFID, classComplexResultData, "returnData", "Ljava/lang/Object;")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain returnData field ID for class com.unblu.headlessbrowser.model.shared.core.ComplexResultData");
	} else if(!LOAD_CREF(env, Window, "com/unblu/webkit/main/server/core/internal/Window")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain class com.unblu.webkit.main.server.core.internal.Window");
	} else if (!LOAD_MID(env, windowInitMID, classWindow, "<init>", "(Ljava/lang/String;J)V")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain constructor for class com.unblu.webkit.main.server.core.internal.Window");
	} else if(!LOAD_FID(env, windowHandleFID, classWindow, "handle", "J")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain handle field ID for class com.unblu.webkit.main.server.core.internal.Window");
	} else if(!LOAD_FID(env, windowUuidFID, classWindow, "id", "Ljava/lang/String;")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain id field ID for class com.unblu.webkit.main.server.core.internal.Window");
	}
}

jobject Window::getWindowClass(JNIEnv *env, Window *win)
{
	if(Utils::isNullObject(env, win->m_javaInstance)) {
		jstring uuid = Utils::getJString(env, win->getId().toString());
		jobject jwin = Utils::newObject(env, (jclass)classWindow, windowInitMID, uuid, A2L(win));
		win->m_javaInstance = Utils::newGlobalRef(env, jwin);
	}
	return env->NewLocalRef(win->m_javaInstance);
}

Window* Window::getWindowClass(JNIEnv *env, jobject clazz)
{
	return (Window*)L2A(Utils::getLongField(env, clazz, windowHandleFID));
}

Window* Window::clearWindowClass(JNIEnv *env, jobject clazz)
{
	Window *win = getWindowClass(env, clazz);
	Utils::setLongField(env, clazz, windowHandleFID, 0);
	return win;
}
