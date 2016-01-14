/*
 * window_private.h
 *
 *  Created on: Oct 4, 2012
 *      Author: mike
 */

#ifndef WINDOW_PRIVATE_H_
#define WINDOW_PRIVATE_H_

#include "jqobject.h"

#define BLANK_HTML                      "<html><head></head><body></body></html>"

class Event;
class EventSender;
class CustomWebPage;
class NetworkAccessManager;
class WebKitJS;
class DownloadItem;

class WindowPrivate : public JQObject
{
	Q_OBJECT

public:
	WindowPrivate(QObject *window = NULL, QObject *parent = NULL);
	virtual ~WindowPrivate();

	void createWindow(const QVariantMap& settings, const QPoint& screenSizeOffset = QPoint());

	bool owningWindow() const { return m_bOwningWindow; }
	void setOwningWindow(const bool bOwningWindow) { m_bOwningWindow = bOwningWindow; }

	QWebFrame* mainFrame() const { return m_pMainFrame; }
	QWebFrame* currentFrame() const;

	QImage render(const QRect& clipRect = QRect(), bool useJpeg = false);
	void sendEvent(const QString& eventType, const QVariantMap& eventData);
	void load(const QString& address, const QVariant& op, const QVariantMap& settings);

	QSize viewportSize() const;
	void setViewportSize(const QSize& size);

	QPoint scrollPosition() const;
	void setScrollPosition(const QPoint& scrollPos);

	void setViewportSizeOffset(const QPoint& viewportSizeOffset);

	QSize contentSize() const;

	QString title() const;
	QUrl url() const;

	void historyBack();
	void historyForward();

	void openPdfJsView(QObject *originatingObject, const QString& documentLocation, const QUrl& baseUrl = QUrl());
	void reloadPage(bool bypassCache = false);

	void triggerAction(const QString& action);

	void setWindow(QObject *window) { m_pWindow = window; };
	const QObject* getWindow() const { return m_pWindow; }

private slots:
	void iconChange();
	void loadStart();
	void loadFinish(bool result);
	void loadProgressing(int progress);
	void initialLayoutComplete();
	void urlChange(const QUrl& url);
	void titleChange(const QString& title);
	void windowCloseRequest();
	void handleJavaScriptWindowObjectCleared();
	void requestRepaint(const QRect& dirtyRect);
	void microFocusChange();
	void selectionChange();
	void cursorChange(const QCursor &cursor);
	void setupFrame(QWebFrame *frame);
	void handleUnsupportedContent(QNetworkReply *reply);
	void requestDownload(const QNetworkRequest& request);
	void requestPrint(QWebFrame *frame);
	void saveFrameState(QWebFrame *frame, QWebHistoryItem *item);
	void restoreFrameState(QWebFrame *frame);
	void handleSslEncrypted(QNetworkReply *reply);
	void handleSslErrors(QNetworkReply *reply,const QList<QSslError>& errors);
	void handleAuthenticationRequired(QNetworkReply *reply,QAuthenticator *authenticator);
	void handleCreateWindow(const QVariantMap& settings, const QPoint& screenSizeOffset);
	void handleSetViewportSize(const QSize& size);
	void handleGetViewportSize(QSize *result);
	void handleRender(const QRect& clipRect, QImage* dest, bool useJpeg);
	void handleLoad(const QString& address, const QVariant& op, const QVariantMap& settings);
	void handleSendEvent(Event *event);
	void handleOnScroll();
	void handleHistoryBack();
	void handleHistoryForward();
	void handlePageReload(bool bypassCache);
	void handlePageTitle(QString *result);
	void handleUrl(QUrl *result);
	void handleTriggerAction(const QString& action);
	void handleWindowClose();
	void handleSetScrollPosition(const QPoint& scrollPos);
	void handleGetScrollPosition(QPoint *result);
	void handleGetContentSize(QSize *result);
	void handleSetViewportSizeOffset(const QPoint& viewportSizeOffset);

private:
	void applySettings(const QVariantMap& settings);
	QImage renderImage(const QRect& clipRect = QRect(), bool useJpeg = false);
	void javaScriptAlert(const QString& msg);
	bool javaScriptConfirm(const QString& msg);
	bool javaScriptPrompt(const QString& msg, const QString& defaultValue, QString *result);
	bool shouldInterruptJavascript();
	QVariantMap navigationRequested(const QUrl& requestUrl, const QString& navigationType, bool isMainFrame);
	QVariantMap errorExtension(const QWebPage::ErrorPageExtensionOption *option, bool isMainFrame);
	void handleUrlChanged(const QUrl& url);
	void showPopupDialog(QObject *object);

	void showContextMenu(const QPoint& pos);

private:
	QObject *m_pWindow;
	QString m_userAgent;
	CustomWebPage *m_pWebPage;
	QWebFrame *m_pMainFrame;
	QWebInspector *m_pInspector;
	WebKitJS *m_pScriptObject;
	bool m_bOwningWindow;
	EventSender *m_pEventSender;
	bool m_bNavigationPermitted;
	bool m_bRequestError;
	bool m_bPdfJsViewer;
	bool m_bKnownRequest;;

	QPoint m_scrollPos;
	QRect m_cursor;
	QCursor m_lastMouseCursor;
	QPoint m_screenSizeOffset;
	bool m_bFieldEdit;
	QString m_lastSelectedText;

	friend class CustomWebPage;
};


#endif /* WINDOW_PRIVATE_H_ */
