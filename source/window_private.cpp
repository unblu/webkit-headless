/*
 * window_private.cpp
 *
 *  Created on: Oct 4, 2012
 *      Author: mike
 */


#include "utils.h"
#include "event.h"
#include "webkit.h"
#include "window.h"
#include "cookiejar.h"
#include "eventsender.h"
#include "window_private.h"
#include "customwebpage.h"
#include "networkaccessmanager.h"
#include "webkitjs.h"

#define WEBKIT_OBJECT_NAME				"unbluBrowser"
#define WEBKIT_OBJECT_PRESENT			"typeof(window."WEBKIT_OBJECT_NAME") !== \"undefined\";"
#define WEBKIT_SCROLL_HANDLER			"(function(){ \
											function dom_addEventListener(node, type, listener) { \
												if(node.addEventListener) { \
													node.addEventListener(type, listener, false); \
												} else { \
													node.attachEvent('on' + type, listener, true); \
												} \
											} \
											dom_addEventListener(window, 'scroll', "WEBKIT_OBJECT_NAME".onscroll); \
										 })();"

static void injectWebKitObjIntoFrame(QWebFrame *frame, QObject *injection)
{
	if(frame->evaluateJavaScript(WEBKIT_OBJECT_PRESENT).toBool() == false) {
		frame->addToJavaScriptWindowObject(WEBKIT_OBJECT_NAME, injection, QWebFrame::QtOwnership);
		frame->evaluateJavaScript(WEBKIT_SCROLL_HANDLER);
	}
}

static QImage getImageFromQCursor(const QCursor& cursor)
{
	QImage cursorImage;
	bool isPixmap = !cursor.pixmap().isNull();

	if(isPixmap)
		cursorImage = cursor.pixmap().toImage();
	else {
		QBitmap copy = *cursor.bitmap();

		copy.setMask(*cursor.mask());
		cursorImage = copy.toImage();
	}

	return cursorImage;
}

static bool sortQSslErrors(const QSslError& left, const QSslError& right)
{
	return left.error() < right.error();
}

WindowPrivate::WindowPrivate(QObject *window, QObject *parent)
: JQObject(parent), m_pWindow(window), m_pWebPage(NULL), m_pMainFrame(NULL),
  m_pInspector(NULL), m_bOwningWindow(false), m_pEventSender(NULL), m_bNavigationPermitted(true),
  m_bRequestError(false), m_bPdfJsViewer(false), m_bKnownRequest(false), m_screenSizeOffset(0, 0),
  m_bFieldEdit(false)
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
}

WindowPrivate::~WindowPrivate()
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	QMetaObject::invokeMethod(m_pWindow, "setDestroyed", Qt::DirectConnection, Q_ARG(bool, true));

	disconnect(this, 0, 0, 0);

	WebKit::instance()->unregisterWindow((Window*)m_pWindow);

	m_pWindow = NULL;
	m_pWebPage = NULL;
	m_pEventSender = NULL;
	m_pMainFrame = NULL;
}

QWebFrame* WindowPrivate::currentFrame() const
{
	return (m_pWebPage != NULL ? m_pWebPage->currentFrame() : NULL);
}

void WindowPrivate::applySettings(const QVariantMap& settings)
{
	QWebSettings *opt = m_pWebPage->settings();
	bool developerToolsEnabled = settings[PAGE_SETTINGS_DEVELOPER_TOOLS].toBool();

    opt->setAttribute(QWebSettings::AutoLoadImages, settings[PAGE_SETTINGS_LOAD_IMAGES].toBool());
    opt->setAttribute(QWebSettings::JavascriptEnabled, settings[PAGE_SETTINGS_JS_ENABLED].toBool());
    opt->setAttribute(QWebSettings::XSSAuditingEnabled, settings[PAGE_SETTINGS_XSS_AUDITING].toBool());
    opt->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, settings[PAGE_SETTINGS_LOCAL_ACCESS_REMOTE].toBool());
    //opt->setAttribute(QWebSettings::WebSecurityEnabled, settings[PAGE_SETTINGS_WEB_SECURITY_ENABLED].toBool());
    opt->setAttribute(QWebSettings::JavascriptCanOpenWindows, settings[PAGE_SETTINGS_JS_CAN_OPEN_WINDOWS].toBool());
    opt->setAttribute(QWebSettings::JavascriptCanCloseWindows, settings[PAGE_SETTINGS_JS_CAN_CLOSE_WINDOWS].toBool());
    opt->setAttribute(QWebSettings::PluginsEnabled, settings[PAGE_SETTINGS_PLUGINS_ENABLED].toBool());
    opt->setAttribute(QWebSettings::DeveloperExtrasEnabled, developerToolsEnabled);

    if(developerToolsEnabled) {
    	m_pWebPage->setProperty("_q_webInspectorServerPort", WebKit::instance()->developerToolsRemotePort());
    	m_pInspector->setPage(m_pWebPage);
    }
}

void WindowPrivate::handleCreateWindow(const QVariantMap& settings, const QPoint& screenSizeOffset)
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	createWindow(settings, screenSizeOffset);

	QMetaObject::invokeMethod(m_pWindow, "doneCreateWindow", Qt::DirectConnection);

}

void WindowPrivate::handleWindowClose()
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	QMetaObject::invokeMethod(m_pWindow, "setClosing", Qt::DirectConnection, Q_ARG(bool, true));

	disconnect(m_pMainFrame, 0, this, 0);
	disconnect(m_pWebPage, 0, this, 0);
	disconnect(WebKit::instance()->networkAccessManager(), 0, this, 0);

	if(m_pWebPage != NULL) m_pWebPage->deleteLater();
	if(m_pEventSender != NULL) m_pEventSender->deleteLater();

	deleteLater();
}

void WindowPrivate::handleLoad(const QString& address, const QVariant& op, const QVariantMap& settings)
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	load(address, op, settings);
}

void WindowPrivate::handleSetViewportSize(const QSize& size)
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << size;
#endif
	setViewportSize(size);
	QMetaObject::invokeMethod(m_pWindow, "doneSetViewportSize", Qt::DirectConnection);
}

void WindowPrivate::handleGetViewportSize(QSize *result)
{
	*result = viewportSize();
	QMetaObject::invokeMethod(m_pWindow, "doneGetViewportSize", Qt::DirectConnection);
}

void WindowPrivate::handleGetContentSize(QSize *result)
{
	*result = contentSize();
	QMetaObject::invokeMethod(m_pWindow, "doneGetContentSize", Qt::DirectConnection);
}

void WindowPrivate::handleRender(const QRect& clipRect, QImage* dest, bool useJpeg)
{
	*dest = render(clipRect, useJpeg);
	QMetaObject::invokeMethod(m_pWindow, "doneRender", Qt::DirectConnection);
}

void WindowPrivate::handleSendEvent(Event *event)
{
	sendEvent(event->getEventType(), event->getEventData());
	delete event;
}

void WindowPrivate::handleHistoryBack()
{
	historyBack();
	QMetaObject::invokeMethod(m_pWindow, "doneHistoryBack", Qt::DirectConnection);
}

void WindowPrivate::handleHistoryForward()
{
	historyForward();
	QMetaObject::invokeMethod(m_pWindow, "doneHistoryForward", Qt::DirectConnection);
}

void WindowPrivate::handlePageReload(bool bypassCache)
{
	reloadPage(bypassCache);
	QMetaObject::invokeMethod(m_pWindow, "donePageReload", Qt::DirectConnection);
}

void WindowPrivate::handlePageTitle(QString *result)
{
	*result = title();
	QMetaObject::invokeMethod(m_pWindow, "donePageTitle", Qt::DirectConnection);
}

void WindowPrivate::handleUrl(QUrl *result)
{
	*result = url();
	QMetaObject::invokeMethod(m_pWindow, "doneUrl", Qt::DirectConnection);
}

void WindowPrivate::handleTriggerAction(const QString& action)
{
	triggerAction(action);
	QMetaObject::invokeMethod(m_pWindow, "doneTriggerAction", Qt::DirectConnection);
}

void WindowPrivate::handleSetScrollPosition(const QPoint& scrollPos)
{
	setScrollPosition(scrollPos);
	QMetaObject::invokeMethod(m_pWindow, "doneSetScrollPosition", Qt::DirectConnection);
}

void WindowPrivate::handleGetScrollPosition(QPoint *result)
{
	*result = scrollPosition();
	QMetaObject::invokeMethod(m_pWindow, "doneGetScrollPosition", Qt::DirectConnection);
}

void WindowPrivate::handleSetViewportSizeOffset(const QPoint& viewportSizeOffset)
{
	setViewportSizeOffset(viewportSizeOffset);
	QMetaObject::invokeMethod(m_pWindow, "doneSetViewportSizeOffset", Qt::DirectConnection);
}

void WindowPrivate::handleUrlChanged(const QUrl& url)
{
	bool trusted;
	bool *pTrusted = NULL;
	QUrl realUrl = url;

	if (url.scheme() == "qrc" && url.path() == "/pdf.js/web/viewer.html") {
		QUrlQuery urlQuery(url);
		QString baseUrl = urlQuery.queryItemValue("baseUrl");
		realUrl = QUrl(baseUrl);
	}

	QString origin = realUrl.toEncoded(QUrl::RemoveUserInfo | QUrl::RemovePath | QUrl::RemoveQuery | QUrl::RemoveFragment);
	if (WebKit::instance()->trustedSites().contains(origin)) {
		pTrusted = &trusted;
		*pTrusted = WebKit::instance()->trustedSites().find(origin).value() == true;
	}
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << origin << " - " << pTrusted << " - " << trusted
#endif
	QMetaObject::invokeMethod(m_pWindow, "handleUrlChanged", Qt::DirectConnection, Q_ARG(const QString&, realUrl.toString()), Q_ARG(bool*, pTrusted));
}

QImage WindowPrivate::render(const QRect& clipRect, bool useJpeg)
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	if(m_pMainFrame->contentsSize().isEmpty()) {
		return QImage();
	}

	return renderImage(clipRect, useJpeg);
}

void WindowPrivate::sendEvent(const QString& eventType, const QVariantMap& eventData)
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << eventType;
	qDebug() << Q_FUNC_INFO << eventData;
#endif
	const QString type = eventType.toLower();
	if(type == "keydown" || type == "keyup") {
		int key = 0;
		QString text;
		Qt::KeyboardModifiers modifiers(Qt::NoModifier);
		QVariant arg1 = eventData[EVENT_TYPE_KEY_KEYCODE];
		QVariant arg2 = eventData[EVENT_TYPE_KEY_CHARCODE];
		QVariant arg3 = eventData[EVENT_TYPE_KEY_MODIFIERS];
		if(arg1.isValid()) key = arg1.toInt();
		if(arg2.isValid() && arg2.toInt() > 0) {
			int keysym = arg2.toInt();
			if(keysym >= Qt::Key_Escape) {
#if defined(Q_OS_MAC)
				text = QChar(key);
#endif
				key = keysym;
			} else
				text = QChar(keysym);
		}
		if(arg3.isValid() && arg3.toInt() > 0) modifiers = Qt::KeyboardModifiers(arg3.toInt());

		if(type == "keydown")
			m_pEventSender->keyDown(key, text, modifiers);
		else if(type == "keyup")
			m_pEventSender->keyUp(key, text, modifiers);

		return;
	}

	if(type == "mousedown" || type == "mouseup") {
		int x = eventData[EVENT_TYPE_MOUSE_POS_X].toInt();
		int y = eventData[EVENT_TYPE_MOUSE_POS_Y].toInt();
		int button = eventData[EVENT_TYPE_MOUSE_BUTTON].toInt();
		int buttons = eventData[EVENT_TYPE_MOUSE_BUTTONS].toInt();
		int modifiers = eventData[EVENT_TYPE_KEY_MODIFIERS].toInt();

		m_pEventSender->mouseMoveTo(QPoint(x, y), buttons, Qt::KeyboardModifiers(modifiers));

		if(type == "mousedown")
			m_pEventSender->mouseDown(button, buttons, Qt::KeyboardModifiers(modifiers));
		else if(type == "mouseup")
			m_pEventSender->mouseUp(button, buttons, Qt::KeyboardModifiers(modifiers));

		return;
	}

	if(type == "contextmenu") {
		int modifiers = eventData[EVENT_TYPE_KEY_MODIFIERS].toInt();
		m_pEventSender->contextMenuRequest(Qt::KeyboardModifiers(modifiers));
		return;
	}

	if(type == "mousemove") {
		int x = eventData[EVENT_TYPE_MOUSE_POS_X].toInt();
		int y = eventData[EVENT_TYPE_MOUSE_POS_Y].toInt();
		int buttons = eventData[EVENT_TYPE_MOUSE_BUTTONS].toInt();
		int modifiers = eventData[EVENT_TYPE_KEY_MODIFIERS].toInt();
		m_pEventSender->mouseMoveTo(QPoint(x, y), buttons, Qt::KeyboardModifiers(modifiers));

		return;
	}

	if(type == "mousewheel") {
		int deltaX = eventData[EVENT_TYPE_MOUSE_WHEEL_DELTA_X].toInt();
		int deltaY = eventData[EVENT_TYPE_MOUSE_WHEEL_DELTA_Y].toInt();
		int buttons = eventData[EVENT_TYPE_MOUSE_BUTTONS].toInt();
		int modifiers = eventData[EVENT_TYPE_KEY_MODIFIERS].toInt();
		m_pEventSender->mouseScrollBy(deltaX, deltaY, buttons, Qt::KeyboardModifiers(modifiers));

		return;
	}
}

void WindowPrivate::load(const QString& address, const QVariant& op, const QVariantMap& settings)
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId() << address;
#endif
	QString operation;
	QByteArray body;
	QNetworkRequest request;

	applySettings(settings);
	m_pWebPage->triggerAction(QWebPage::Stop);

	if(op.type() == QVariant::String)
		operation = op.toString();

	if(op.type() == QVariant::Map) {
		operation = op.toMap().value("operation").toString();
		body = op.toMap().value("data").toByteArray();
		if(op.toMap().contains("headers")) {
			QMapIterator<QString, QVariant> it(op.toMap().value("headers").toMap());
			while(it.hasNext()) {
				it.next();
				request.setRawHeader(it.key().toUtf8(), it.value().toString().toUtf8());
			}
		}
	}

	if(operation.isEmpty())
		operation = "get";

	QNetworkAccessManager::Operation networkOp = QNetworkAccessManager::UnknownOperation;
	operation = operation.toLower();
	if(operation == "get")
		networkOp = QNetworkAccessManager::GetOperation;
	else if(operation == "head")
		networkOp = QNetworkAccessManager::HeadOperation;
	else if(operation == "put")
		networkOp = QNetworkAccessManager::PutOperation;
	else if(operation == "post")
		networkOp = QNetworkAccessManager::PostOperation;
	else if(operation == "delete")
		networkOp = QNetworkAccessManager::DeleteOperation;

	if(networkOp == QNetworkAccessManager::UnknownOperation)
		return;

	if(address == "about:blank") {
		m_pMainFrame->setHtml(BLANK_HTML);
	} else {
		QUrl url = QUrl::fromEncoded(address.toUtf8());

		if(url.scheme().isEmpty()) {
			url.setScheme("http");
		}
#ifdef U_WINPRIV_DEBUG
		qDebug() << Q_FUNC_INFO << url << operation;
#endif
		request.setUrl(url);
		m_pMainFrame->load(request, networkOp, body);
	}
}

QSize WindowPrivate::viewportSize() const
{
	return m_pWebPage->viewportSize();
}

void WindowPrivate::setViewportSize(const QSize& size)
{
	QSize realSize = QSize(size.width() + m_screenSizeOffset.x(), size.height() + m_screenSizeOffset.y());
	m_pWebPage->setViewportSize(realSize);
}

QPoint WindowPrivate::scrollPosition() const
{
	return m_pMainFrame->scrollPosition();
}

void WindowPrivate::setScrollPosition(const QPoint& scrollPos)
{
	m_pMainFrame->setScrollPosition(scrollPos);
}

void WindowPrivate::setViewportSizeOffset(const QPoint& viewportSizeOffset)
{
	if(viewportSizeOffset != m_screenSizeOffset) {
		m_screenSizeOffset = viewportSizeOffset;
		setViewportSize(WebKit::instance()->screenSize());
	}
}

QSize WindowPrivate::contentSize() const
{
	return m_pMainFrame->contentsSize();
}

QString WindowPrivate::title() const
{
	return m_pMainFrame->title();
}

QUrl WindowPrivate::url() const
{
	return m_pMainFrame->url();
}

void WindowPrivate::triggerAction(const QString& action)
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << action;
#endif
	if(action.compare("paste", Qt::CaseInsensitive) == 0) {
		/*
		if(m_cursor.isValid()) {
			QWebFrame *frame = m_pWebPage->frameAt(m_cursor.topLeft());
			if(frame != NULL) {
				QWebHitTestResult result = frame->hitTestContent(m_cursor.topLeft());

				if(result.isContentEditable()) {
					qCritical() << "we are in an editable field, so paste to it";
				}
			}
		}
		*/
		m_pWebPage->triggerAction(QWebPage::Paste);
	}
}

void WindowPrivate::historyBack()
{
	QWebHistory *history = m_pWebPage->history();
	if(history != NULL) {
		if(!m_bNavigationPermitted) {
			history->goToItem(history->currentItem());
		} else if(history->canGoBack()){
			history->back();
		}
	}
}

void WindowPrivate::historyForward()
{
	QWebHistory *history = m_pWebPage->history();
	if(history != NULL) {
		if(history->canGoForward()) {
			history->forward();
		}
	}
}

void WindowPrivate::openPdfJsView(QObject *originatingObject, const QString& documentLocation, const QUrl& baseUrl)
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << baseUrl << documentLocation << originatingObject;
#endif
	QWebFrame *frame = NULL;

	m_bPdfJsViewer = true;
	m_bKnownRequest = true;
	m_bNavigationPermitted = true;
	m_bRequestError = false;
	if ((frame = qobject_cast<QWebFrame*>(originatingObject)) == NULL) frame = m_pMainFrame;

	frame->load(QUrl("qrc:/pdf.js/web/viewer.html?file=" + documentLocation + "&baseUrl=" + baseUrl.toString()));
}

void WindowPrivate::reloadPage(bool bypassCache)
{
	QWebPage::WebAction action = bypassCache ? QWebPage::ReloadAndBypassCache : QWebPage::Reload;
	m_pWebPage->triggerAction(action);
}

bool WindowPrivate::shouldInterruptJavascript()
{
	bool retVal;
	QMetaObject::invokeMethod(m_pWindow, "handleJavaScriptInterrupt", Qt::DirectConnection, Q_RETURN_ARG(bool, retVal));
	return retVal;
}

void WindowPrivate::javaScriptAlert(const QString& msg)
{
	QMetaObject::invokeMethod(m_pWindow, "handleJavaScriptAlert", Qt::DirectConnection, Q_ARG(const QString&, msg));
}

bool WindowPrivate::javaScriptConfirm(const QString& msg)
{
	bool retVal;
	QMetaObject::invokeMethod(m_pWindow, "handleJavaScriptConfirm", Qt::DirectConnection, Q_RETURN_ARG(bool, retVal), Q_ARG(const QString&, msg));
	return retVal;
}

bool WindowPrivate::javaScriptPrompt(const QString& msg, const QString& defaultValue, QString *result)
{
	bool retVal;
	QMetaObject::invokeMethod(m_pWindow, "handleJavaScriptPrompt", Qt::DirectConnection, Q_RETURN_ARG(bool, retVal), Q_ARG(const QString&, msg), Q_ARG(const QString&, defaultValue), Q_ARG(QString*, result));
	return retVal;
}

QVariantMap WindowPrivate::navigationRequested(const QUrl& requestUrl, const QString& navigationType, bool isMainFrame)
{
	QVariantMap retVal;
	QMetaObject::invokeMethod(m_pWindow, "handleNavigationRequest", Qt::DirectConnection, Q_RETURN_ARG(QVariantMap, retVal), Q_ARG(const QUrl&, requestUrl), Q_ARG(const QString&, navigationType), Q_ARG(bool, isMainFrame));
	return retVal;
}

QVariantMap WindowPrivate::errorExtension(const QWebPage::ErrorPageExtensionOption *option, bool isMainFrame)
{
	QVariantMap retVal;
	QMetaObject::invokeMethod(m_pWindow, "handleErrorExtension", Qt::DirectConnection, Q_RETURN_ARG(QVariantMap, retVal), Q_ARG(const QWebPage::ErrorPageExtensionOption*, option), Q_ARG(bool, isMainFrame));
	return retVal;
}

void WindowPrivate::showPopupDialog(QObject *object)
{
	QMetaObject::invokeMethod(m_pWindow, "handlePopupDialog", Qt::DirectConnection, Q_ARG(QObject*, object));
}

void WindowPrivate::showContextMenu(const QPoint& pos)
{
	QMetaObject::invokeMethod(m_pWindow, "handleContextMenu", Qt::DirectConnection, Q_ARG(const QPoint&, pos));
}

void WindowPrivate::iconChange()
{
	QByteArray bytes;
	QBuffer buffer(&bytes);
	QSize requestedSize(64, 64);
	QIcon icon = m_pMainFrame->icon();
	QSize actualSize = icon.actualSize(requestedSize);
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << requestedSize << actualSize;
#endif
	if(actualSize.isValid()) {
		QImage rawBuffer = icon.pixmap(requestedSize).toImage();

		buffer.open(QIODevice::WriteOnly);
		rawBuffer.save(&buffer, "PNG");

		QMetaObject::invokeMethod(m_pWindow, "handleIconChange", Qt::DirectConnection, Q_ARG(const QByteArray&, bytes));
	}
}

void WindowPrivate::loadStart()
{
	QMetaObject::invokeMethod(m_pWindow, "handleLoadStarted", Qt::DirectConnection);
}

void WindowPrivate::loadProgressing(int progress)
{
	QMetaObject::invokeMethod(m_pWindow, "handleLoadProgress", Qt::DirectConnection, Q_ARG(int, progress));
}

void WindowPrivate::loadFinish(bool result)
{
	m_bKnownRequest = false;
	injectWebKitObjIntoFrame(m_pMainFrame, m_pScriptObject);
	QMetaObject::invokeMethod(m_pWindow, "handleLoadFinished", Qt::DirectConnection, Q_ARG(bool, result));
}

void WindowPrivate::initialLayoutComplete()
{
	QMetaObject::invokeMethod(m_pWindow, "handleInitialLayoutComplete", Qt::DirectConnection);
}

void WindowPrivate::requestRepaint(const QRect& dirtyRect)
{
	microFocusChange();
	QMetaObject::invokeMethod(m_pWindow, "handleRepaintRequest", Qt::DirectConnection, Q_ARG(const QRect&, dirtyRect));
}

void WindowPrivate::urlChange(const QUrl& url)
{
	if(m_bNavigationPermitted && !m_bRequestError) {
		handleUrlChanged(url);
	}
}

void WindowPrivate::titleChange(const QString& title)
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << title << title.isNull();
#endif
	QMetaObject::invokeMethod(m_pWindow, "handleTitleChanged", Qt::DirectConnection, Q_ARG(const QString&, title));
}

void WindowPrivate::requestDownload(const QNetworkRequest& request)
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << &request;
#endif
	QNetworkReply *reply = m_pWebPage->networkAccessManager()->get(request);
	QMetaObject::invokeMethod(m_pWindow, "handleDownloadRequest", Qt::DirectConnection, Q_ARG(QNetworkReply*, reply));
}

void WindowPrivate::requestPrint(QWebFrame *frame)
{
	Q_UNUSED(frame)
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
}

void WindowPrivate::windowCloseRequest()
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	handleWindowClose();
}

void WindowPrivate::microFocusChange()
{
	QRect cursor = m_pWebPage->inputMethodQuery(Qt::ImMicroFocus).toRect();
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << m_cursor << cursor << cursor.isValid() << cursor.isNull() << m_bFieldEdit;
#endif
	if(cursor == m_cursor) return;

	if(cursor.isValid()) {
		QWebFrame *frame = m_pWebPage->frameAt(cursor.topLeft());
		if(frame != NULL) {
			QWebHitTestResult result = frame->hitTestContent(cursor.topLeft());

			if(result.isContentEditable()) {
				m_cursor = cursor;
				m_bFieldEdit = true;
				QMetaObject::invokeMethod(m_pWindow, "handleMicroFocusChange", Qt::DirectConnection, Q_ARG(const QRect&, m_cursor));
			} else if(m_bFieldEdit) {
				m_bFieldEdit = false;
				m_cursor = QRect(0, 0, 0, 0);
				QMetaObject::invokeMethod(m_pWindow, "handleMicroFocusChange", Qt::DirectConnection, Q_ARG(const QRect&, m_cursor));
			}
		} else if(m_bFieldEdit) {
			m_bFieldEdit = false;
			m_cursor = QRect(0, 0, 0, 0);
			QMetaObject::invokeMethod(m_pWindow, "handleMicroFocusChange", Qt::DirectConnection, Q_ARG(const QRect&, m_cursor));
		}
	} else if(m_bFieldEdit) {
		m_bFieldEdit = false;
		m_cursor = QRect(0, 0, 0, 0);
		QMetaObject::invokeMethod(m_pWindow, "handleMicroFocusChange", Qt::DirectConnection, Q_ARG(const QRect&, m_cursor));
	}
}

void WindowPrivate::selectionChange()
{
	QString selectedText = "";

	if(m_bFieldEdit) {
		QWebFrame *frame = m_pWebPage->frameAt(m_cursor.topLeft());
		if(frame != NULL) {
			QWebHitTestResult result = frame->hitTestContent(m_cursor.topLeft());

			if(result.isContentEditable())
				selectedText = m_pWebPage->inputMethodQuery(Qt::ImCurrentSelection).toString();
		}
	} else
		selectedText = m_pWebPage->selectedText();
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << selectedText << m_lastSelectedText << m_bFieldEdit;
#endif
	if(selectedText.compare(m_lastSelectedText)) {
		m_lastSelectedText = selectedText;
		QMetaObject::invokeMethod(m_pWindow, "handleSelectionChange", Qt::DirectConnection, Q_ARG(const QString&, selectedText));
	}
}

void WindowPrivate::cursorChange(const QCursor &cursor)
{
	QImage cursorImage;
	bool triggerHandler = false;
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << cursor.shape();
#endif
	if(cursor.shape() == Qt::BitmapCursor || cursor.shape() == Qt::CustomCursor) {
		bool imageChanged = true;
		QImage currCursor = getImageFromQCursor(cursor);

		if(m_lastMouseCursor.shape() == Qt::BitmapCursor || m_lastMouseCursor.shape() == Qt::CustomCursor)
			imageChanged = !(currCursor == getImageFromQCursor(m_lastMouseCursor));

		if(imageChanged) {
			triggerHandler = true;
			cursorImage = currCursor;
		}
	} else if(cursor.shape() != m_lastMouseCursor.shape())
		triggerHandler = true;

	if(triggerHandler) {
		m_lastMouseCursor = cursor;
		QMetaObject::invokeMethod(m_pWindow, "handleCursorChange", Qt::DirectConnection, Q_ARG(Qt::CursorShape, cursor.shape()), Q_ARG(const QImage&, cursorImage));
	}
}

void WindowPrivate::saveFrameState(QWebFrame *frame, QWebHistoryItem *item)
{
	Q_UNUSED(frame);
	Q_UNUSED(item);
}

void WindowPrivate::restoreFrameState(QWebFrame *frame)
{
	Q_UNUSED(frame);
}

void WindowPrivate::setupFrame(QWebFrame *frame)
{
	if(frame != NULL) {
		if(!(frame == m_pMainFrame)) {
			frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAsNeeded);
			frame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
		} else
			injectWebKitObjIntoFrame(frame, m_pScriptObject);
	}
}

void WindowPrivate::createWindow(const QVariantMap& settings, const QPoint& screenSizeOffset)
{
	QString storageLocation = WebKit::instance()->tempStorageLocation();
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << storageLocation;
#endif
	m_pWebPage = new CustomWebPage(this);
	m_pInspector = new QWebInspector();
	m_pScriptObject = new WebKitJS(this);
	m_pEventSender = new EventSender(m_pWebPage);
	m_pMainFrame = m_pWebPage->mainFrame();
	m_screenSizeOffset = screenSizeOffset;

	connect(m_pWebPage, SIGNAL(loadProgress(int)), SLOT(loadProgressing(int)), Qt::AutoConnection);
	connect(m_pWebPage, SIGNAL(frameCreated(QWebFrame*)), SLOT(setupFrame(QWebFrame*)), Qt::AutoConnection);
	connect(m_pWebPage, SIGNAL(repaintRequested(const QRect&)), SLOT(requestRepaint(const QRect&)), Qt::AutoConnection);
	connect(m_pWebPage, SIGNAL(windowCloseRequested()), SLOT(windowCloseRequest()), Qt::AutoConnection);
	connect(m_pWebPage, SIGNAL(microFocusChanged()), SLOT(microFocusChange()), Qt::AutoConnection);
	connect(m_pWebPage, SIGNAL(cursorChanged(const QCursor&)), SLOT(cursorChange(const QCursor&)), Qt::AutoConnection);
	connect(m_pWebPage, SIGNAL(unsupportedContent(QNetworkReply*)), SLOT(handleUnsupportedContent(QNetworkReply*)), Qt::AutoConnection);
	connect(m_pWebPage, SIGNAL(downloadRequested(const QNetworkRequest&)), SLOT(requestDownload(const QNetworkRequest&)), Qt::AutoConnection);
	connect(m_pWebPage, SIGNAL(printRequested(QWebFrame*)), SLOT(requestPrint(QWebFrame*)), Qt::AutoConnection);
	connect(m_pWebPage, SIGNAL(selectionChanged()), SLOT(selectionChange()), Qt::AutoConnection);
	connect(m_pWebPage, SIGNAL(saveFrameStateRequested(QWebFrame*, QWebHistoryItem*)), SLOT(saveFrameState(QWebFrame*, QWebHistoryItem*)), Qt::AutoConnection);
	connect(m_pWebPage, SIGNAL(restoreFrameStateRequested(QWebFrame*)), SLOT(restoreFrameState(QWebFrame*)), Qt::AutoConnection);

	connect(m_pMainFrame, SIGNAL(loadStarted()), this, SLOT(loadStart()), Qt::AutoConnection);
	connect(m_pMainFrame, SIGNAL(loadFinished(bool)), this, SLOT(loadFinish(bool)), Qt::AutoConnection);
	connect(m_pMainFrame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(handleJavaScriptWindowObjectCleared()), Qt::AutoConnection);
	connect(m_pMainFrame, SIGNAL(initialLayoutCompleted()), SLOT(initialLayoutComplete()), Qt::AutoConnection);
	connect(m_pMainFrame, SIGNAL(urlChanged(const QUrl&)), SLOT(urlChange(const QUrl&)), Qt::AutoConnection);
	connect(m_pMainFrame, SIGNAL(iconChanged()), SLOT(iconChange()), Qt::AutoConnection);
	connect(m_pMainFrame, SIGNAL(titleChanged(const QString&)), SLOT(titleChange(const QString&)), Qt::AutoConnection);

	connect(WebKit::instance()->networkAccessManager(), SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), SLOT(handleAuthenticationRequired(QNetworkReply*,QAuthenticator*)), Qt::AutoConnection);
	connect(WebKit::instance()->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*,const QList<QSslError>&)), SLOT(handleSslErrors(QNetworkReply*,const QList<QSslError>&)), Qt::AutoConnection);
	connect(WebKit::instance()->networkAccessManager(), SIGNAL(encrypted(QNetworkReply*)), SLOT(handleSslEncrypted(QNetworkReply*)), Qt::AutoConnection);

	applySettings(settings);

	QPalette palette = m_pWebPage->palette();
	palette.setBrush(QPalette::Base, Qt::transparent);
	m_pWebPage->setPalette(palette);

	m_pWebPage->setNetworkAccessManager(WebKit::instance()->networkAccessManager());

	m_pWebPage->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

	m_pWebPage->settings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
	m_pWebPage->settings()->setOfflineStoragePath(storageLocation);

	m_pWebPage->settings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);
	m_pWebPage->settings()->setOfflineWebApplicationCachePath(storageLocation);

	m_pWebPage->settings()->setAttribute(QWebSettings::FrameFlatteningEnabled, false);

	m_pWebPage->settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
	m_pWebPage->settings()->setLocalStoragePath(storageLocation);

	m_pWebPage->settings()->setAttribute(QWebSettings::DnsPrefetchEnabled, true);
	m_pWebPage->settings()->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);

	m_pMainFrame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
	m_pMainFrame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);

	m_pMainFrame->setHtml(BLANK_HTML);

	setViewportSize(WebKit::instance()->screenSize());
}

QImage WindowPrivate::renderImage(const QRect& clipRect, bool useJpeg)
{
	QSize contentSize = m_pWebPage->viewportSize();
	QRect frameRect = QRect(QPoint(0,0), contentSize);
	Qt::GlobalColor fillColor = Qt::transparent;
	QImage::Format colorFormat = QImage::Format_ARGB32_Premultiplied;

	if(!clipRect.isNull())
		frameRect = clipRect;

	if(useJpeg) {
		fillColor = Qt::white;
		colorFormat = QImage::Format_RGB32;
	}
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << m_pMainFrame->contentsSize() << m_pWebPage->viewportSize();
#endif
	QPainter painter;
	QImage buffer(frameRect.size(), colorFormat);

	buffer.fill(fillColor);

    painter.begin(&buffer);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    m_pMainFrame->renderAbsolute(&painter, QRegion(frameRect));
    painter.end();

    return buffer;
}

void WindowPrivate::handleJavaScriptWindowObjectCleared()
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	m_pMainFrame->setFocus();
	injectWebKitObjIntoFrame(m_pMainFrame, m_pScriptObject);
}

void WindowPrivate::handleUnsupportedContent(QNetworkReply* reply)
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << reply->url() << reply->header(QNetworkRequest::ContentTypeHeader);
#endif
	QMetaObject::invokeMethod(m_pWindow, "handleDownloadRequest", Qt::DirectConnection, Q_ARG(QNetworkReply*, reply));
}

void WindowPrivate::handleAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	if(reply->request().originatingObject() != mainFrame() && reply->request().originatingObject() != currentFrame())
		return;

	QMetaObject::invokeMethod(m_pWindow, "handleAuthenticationRequest", Qt::DirectConnection, Q_ARG(QNetworkReply*, reply), Q_ARG(QAuthenticator*, authenticator));
}

void WindowPrivate::handleSslEncrypted(QNetworkReply *reply)
{
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	if(reply->request().originatingObject() != mainFrame() && reply->request().originatingObject() != currentFrame())
		return;

	bool trusted = true;
	const QUrl& url = reply->request().url();

	if(WebKit::instance()->certificateException().contains(QPair<QString, int>(url.host(), url.port())))
		trusted = false;

	QString origin = url.toEncoded(QUrl::RemoveUserInfo | QUrl::RemovePath | QUrl::RemoveQuery | QUrl::RemoveFragment);
	WebKit::instance()->trustedSites().insert(origin, trusted);

	QMetaObject::invokeMethod(m_pWindow, "siteTrusted", Qt::DirectConnection, Q_ARG(const QString&, origin), Q_ARG(bool, trusted));
}

void WindowPrivate::handleSslErrors(QNetworkReply *reply, const QList<QSslError>& errors)
{
	if(reply->request().originatingObject() != mainFrame() && reply->request().originatingObject() != currentFrame())
		return;

	QList<QSslError> errorList = errors;

	std::sort(errorList.begin(), errorList.end(), sortQSslErrors);
#ifdef U_WINPRIV_DEBUG
	foreach(const QSslError& e, errorList) {
		qDebug() << Q_FUNC_INFO << e.error() << " - " << e.certificate();
	}

#endif
	QMetaObject::invokeMethod(m_pWindow, "handleSslErrors", Qt::DirectConnection, Q_ARG(QNetworkReply*, reply), Q_ARG(const QList<QSslError>&, errors));
}

void WindowPrivate::handleOnScroll()
{
	QPoint scrollPos = m_pMainFrame->scrollPosition();
#ifdef U_WINPRIV_DEBUG
	qDebug() << Q_FUNC_INFO << scrollPos;
#endif
	microFocusChange();
	QMetaObject::invokeMethod(m_pWindow, "handleOnScroll", Qt::DirectConnection, Q_ARG(const QPoint&, scrollPos));
}
