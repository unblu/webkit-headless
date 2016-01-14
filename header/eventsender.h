/*
 * eventsender.h
 *
 *  Created on: Oct 23, 2012
 *      Author: mike
 */

#ifndef EVENTSENDER_H_
#define EVENTSENDER_H_

#include "jqobject.h"

#include <QWebPage>

class EventSender : public JQObject
{
	Q_OBJECT

public:
	EventSender(QWebPage *page);
	virtual ~EventSender();

	void keyDown(int key, const QString& text, const Qt::KeyboardModifiers modifiers);
	void keyUp(int key, const QString& text, const Qt::KeyboardModifiers modifiers);

	void mouseDown(int button = 0, int buttons = 0, const Qt::KeyboardModifiers modifiers = Qt::NoModifier);
	void mouseUp(int button = 0, int buttons = 0, const Qt::KeyboardModifiers modifiers = Qt::NoModifier);
	void mouseMoveTo(const QPoint& pos, int buttons = 0, const Qt::KeyboardModifiers modifiers = Qt::NoModifier);
	void mouseScrollBy(int deltaX, int deltaY, int buttons = 0, const Qt::KeyboardModifiers modifiers = Qt::NoModifier);

	void contextMenuRequest(const Qt::KeyboardModifiers modifiers = Qt::NoModifier);

private:
	void sendEvent(QEvent *event);
	void handleScrollPosition(int deltaX, int deltaY);

	inline void resetClickCount() { m_clickCount = 0; }

protected:
	void timerEvent(QTimerEvent *event);

private:
	QPoint m_mousePos;
	QPoint m_clickPos;
	QWebPage *m_pWebPage;
	int m_currentButton;
	int m_clickCount;
	QBasicTimer m_clickTimer;
};


#endif /* EVENTSENDER_H_ */
