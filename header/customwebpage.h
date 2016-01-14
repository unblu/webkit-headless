/*
 * customwebpage.h
 *
 *  Created on: Oct 25, 2012
 *      Author: mike
 */

#ifndef CUSTOMWEBPAGE_H_
#define CUSTOMWEBPAGE_H_

#include "jqobject.h"

class WindowPrivate;

class CustomWebPage : public QWebPage
{
	Q_OBJECT

public:
	CustomWebPage(WindowPrivate *parent = NULL);
	virtual ~CustomWebPage();

	void showPopupDialog(QObject *object);

	void saveHistoryState();
	void restoreHistoryState();

	virtual bool supportsExtension(Extension extension) const;
	virtual bool extension(Extension extension, const ExtensionOption *option = NULL, ExtensionReturn *output = NULL);

	virtual bool shouldInterruptJavaScript();

public slots:
	bool event(QEvent *e);

protected:
	virtual bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest& request, NavigationType type);

	virtual void javaScriptAlert(QWebFrame *originatingFrame, const QString& msg);
	virtual bool javaScriptConfirm(QWebFrame *originatingFrame, const QString& msg);
	virtual bool javaScriptPrompt(QWebFrame *originatingFrame, const QString& msg, const QString& defaultValue, QString *result);

	virtual QString userAgentForUrl(const QUrl& url) const;

	virtual QWebPage* createWindow(WebWindowType type);

private:
	QString m_userAgent;
	WindowPrivate *m_pWindow;

	QByteArray m_history;

	friend class WindowPrivate;
};

#endif /* CUSTOMWEBPAGE_H_ */
