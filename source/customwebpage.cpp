/*
 * customwebpage.cpp
 *
 *  Created on: Oct 25, 2012
 *      Author: mike
 */

#include "customwebpage.h"
#include "window_private.h"
#include "webkit.h"
#include "window.h"

CustomWebPage::CustomWebPage(WindowPrivate *parent) : QWebPage(parent), m_pWindow(parent)
{
	m_userAgent = QWebPage::userAgentForUrl(QUrl());
#ifdef U_CWP_DEBUG
	qDebug() << Q_FUNC_INFO << m_userAgent;
#endif
	setForwardUnsupportedContent(true);
}

CustomWebPage::~CustomWebPage()
{
#ifdef U_CWP_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	disconnect(this, 0, 0, 0);

	m_pWindow = NULL;
}

bool CustomWebPage::supportsExtension(QWebPage::Extension extension) const
{
#ifdef U_CWP_DEBUG
	qDebug() << Q_FUNC_INFO << (extension == ErrorPageExtension);
#endif
	return (extension == ErrorPageExtension);
}

bool CustomWebPage::extension(QWebPage::Extension extension, const QWebPage::ExtensionOption *option, QWebPage::ExtensionReturn *output)
{
#ifdef U_CWP_DEBUG
	qDebug() << Q_FUNC_INFO << extension << option << output;
#endif
	if(extension == ErrorPageExtension) {
		ErrorPageExtensionOption *in = (ErrorPageExtensionOption*)option;
		ErrorPageExtensionReturn *out = (ErrorPageExtensionReturn*)output;
		bool isMainFrame = (in->frame == m_pWindow->m_pMainFrame);

		// do not handle operation canceled error as this is not a real error
		if(in->domain == QtNetwork && in->error == QNetworkReply::OperationCanceledError) {
			return false;
		}

		QVariantMap result = m_pWindow->errorExtension(in, isMainFrame);
		QVariant returnData = result[COMPLEX_RESULT_RETURNDATA];
		QVariant returnValue = result[COMPLEX_RESULT_RETURNVALUE];
		bool bRequestError = returnValue.toBool();
		if(isMainFrame) {
			m_pWindow->m_bRequestError = returnValue.toBool();
		}
		if(bRequestError && !returnData.isNull()) {
			out->content = returnData.toByteArray();
		}

		return bRequestError;
	}
	return false;
}

bool CustomWebPage::event(QEvent *e)
{
	if(e->type() == QEvent::ContextMenu) {
		QContextMenuEvent *event = static_cast<QContextMenuEvent *>(e);

		if(!swallowContextMenuEvent(event)) {
			m_pWindow->showContextMenu(event->pos());
		}
		e->accept();
		return true;
	}
	return QWebPage::event(e);
}

bool CustomWebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest& request, QWebPage::NavigationType type)
{
	if (m_pWindow->m_bKnownRequest) return true;

	frame = (frame == NULL ? m_pWindow->m_pMainFrame : frame); // use mainFrame if we got a nil frame (might happen when a new window action should take place)

	QString scheme = request.url().scheme();
	bool isMainFrame = (frame == m_pWindow->m_pMainFrame);
#ifdef U_CWP_DEBUG
	qDebug() << Q_FUNC_INFO << request.url() << isMainFrame;
#endif

	if (scheme == "unblu") {
		QUrl baseUrl = request.url();
		QUrlQuery query(request.url());
		QString location = WebKit::instance()->baseUrl() + request.url().path();

		if (query.hasQueryItem("attachmentName")) {
			baseUrl = QUrl("unblu:/resource/" + query.queryItemValue("attachmentName"));
		}
		m_pWindow->openPdfJsView(frame, location, baseUrl);
		return false;
	}
	QString navigationType = "undefined";
	switch(type) {
	case NavigationTypeLinkClicked:
        navigationType = "LinkClicked";
		break;
	case NavigationTypeFormSubmitted:
        navigationType = "FormSubmitted";
		break;
	case NavigationTypeBackOrForward:
        navigationType = "BackOrForward";
		break;
	case NavigationTypeReload:
        navigationType = "Reload";
		break;
	case NavigationTypeFormResubmitted:
        navigationType = "FormResubmitted";
		break;
	case NavigationTypeOther:
        navigationType = "Other";
		break;
	}

	QVariantMap result = m_pWindow->navigationRequested(request.url(), navigationType, isMainFrame);
	QVariant returnData = result[COMPLEX_RESULT_RETURNDATA];
	QVariant returnValue = result[COMPLEX_RESULT_RETURNVALUE];
#ifdef U_CWP_DEBUG
	qDebug() << Q_FUNC_INFO << returnValue;
#endif
	bool bNavigationPermitted = returnValue.toBool();
	if(isMainFrame) {
		m_pWindow->m_bPdfJsViewer = false;
		m_pWindow->m_bRequestError = false;
		m_pWindow->m_bNavigationPermitted = bNavigationPermitted;
	}

	if(!bNavigationPermitted && !returnData.isNull()) {
		frame->setHtml(returnData.toString());
	}

	if(bNavigationPermitted && type == NavigationTypeOther && request.url().toString().toLower().startsWith("javascript:")) {
		QString javascript = request.url().toString().mid(11);

		frame->evaluateJavaScript(javascript);
		bNavigationPermitted = !bNavigationPermitted;
	}

	return bNavigationPermitted;
}

void CustomWebPage::showPopupDialog(QObject *object)
{
#ifdef U_CWP_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	m_pWindow->showPopupDialog(object);
}

void CustomWebPage::saveHistoryState()
{
	QDataStream stream(&m_history, QIODevice::ReadWrite | QIODevice::Truncate);
#ifdef U_CWP_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	stream << *history();
}

void CustomWebPage::restoreHistoryState()
{
	QDataStream stream(m_history);
#ifdef U_CWP_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	stream >> *history();
}

QString CustomWebPage::userAgentForUrl(const QUrl& url) const
{
	Q_UNUSED(url);
	return m_userAgent;
}

bool CustomWebPage::shouldInterruptJavaScript()
{
#ifdef U_CWP_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	//return m_pWindow->shouldInterruptJavascript();
	return false;
}

void CustomWebPage::javaScriptAlert(QWebFrame *originatingFrame, const QString& msg)
{
	Q_UNUSED(originatingFrame);
	m_pWindow->javaScriptAlert(msg);
}

bool CustomWebPage::javaScriptConfirm(QWebFrame *originatingFrame, const QString& msg)
{
	Q_UNUSED(originatingFrame);
	return m_pWindow->javaScriptConfirm(msg);
}

bool CustomWebPage::javaScriptPrompt(QWebFrame *originatingFrame, const QString& msg, const QString& defaultValue, QString *result)
{
	Q_UNUSED(originatingFrame);
	return m_pWindow->javaScriptPrompt(msg, defaultValue, result);
}

QWebPage* CustomWebPage::createWindow(WebWindowType type)
{
	Q_UNUSED(type);
#ifdef U_CWP_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
	WindowPrivate *newWindow = NULL;
	if(m_pWindow->owningWindow())
		newWindow = new WindowPrivate(NULL, m_pWindow);
	else
		newWindow = new WindowPrivate();

	newWindow->createWindow(WebKit::instance()->defaultPageSettings());

	WebKit::instance()->registerWindow(new Window(newWindow, newWindow));

	return newWindow->m_pWebPage;
}
