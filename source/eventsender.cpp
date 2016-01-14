/*
 * eventsender.cpp
 *
 *  Created on: Oct 23, 2012
 *      Author: mike
 */

#include "eventsender.h"

#include <QKeyEvent>
#include <QMouseEvent>

EventSender::EventSender(QWebPage *page) : JQObject(page), m_pWebPage(page), m_currentButton(0)
{
	resetClickCount();
}

EventSender::~EventSender()
{
#ifdef U_EVTSND_DEBUG
	qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif

	disconnect(this, 0, 0, 0);

	m_pWebPage = NULL;
}

void EventSender::keyDown(int key, const QString& text, const Qt::KeyboardModifiers modifiers)
{
	QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, key, modifiers, text);
#ifdef U_EVTSND_DEBUG
	qDebug() << Q_FUNC_INFO << key << text << modifiers;
#endif
	if(!event->matches(QKeySequence::Copy) && !event->matches(QKeySequence::Cut) && ! event->matches(QKeySequence::Paste))
		sendEvent(event);
	else
		delete event;
}

void EventSender::keyUp(int key, const QString& text, const Qt::KeyboardModifiers modifiers)
{
	QKeyEvent *event = new QKeyEvent(QEvent::KeyRelease, key, modifiers, text);
#ifdef U_EVTSND_DEBUG
	qDebug() << Q_FUNC_INFO << key << text << modifiers;
#endif
	if(!event->matches(QKeySequence::Copy) && !event->matches(QKeySequence::Cut) && ! event->matches(QKeySequence::Paste))
		sendEvent(event);
	else
		delete event;
}

void EventSender::mouseDown(int button, int buttons, const Qt::KeyboardModifiers modifiers)
{
	Qt::MouseButton mouseButton;
	Qt::MouseButtons mouseButtons(buttons);

	switch(button) {
	case 0:
		mouseButton = Qt::LeftButton;
		break;
	case 1:
		mouseButton = Qt::MiddleButton;
		break;
	case 2:
		mouseButton = Qt::RightButton;
		break;
	case 3:
		mouseButton = Qt::MiddleButton;
		break;
	default:
		mouseButton = Qt::LeftButton;
		break;
	}

	if(m_currentButton == button && (m_mousePos - m_clickPos).manhattanLength() < QApplication::startDragDistance() && m_clickTimer.isActive())
		m_clickCount++;
	else
		m_clickCount = 1;

	m_clickPos = m_mousePos;
	m_currentButton = button;
#ifdef U_EVTSND_DEBUG
	qDebug() << Q_FUNC_INFO << mouseButton << m_mousePos << mouseButtons << m_clickCount;
#endif
	QMouseEvent *event = new QMouseEvent((m_clickCount == 2) ? QEvent::MouseButtonDblClick : QEvent::MouseButtonPress, m_mousePos, m_mousePos, mouseButton, mouseButtons, modifiers);
	sendEvent(event);

	m_clickTimer.start(QApplication::doubleClickInterval(), this);
}

void EventSender::mouseUp(int button, int buttons, const Qt::KeyboardModifiers modifiers)
{
	Qt::MouseButton mouseButton;
	Qt::MouseButtons mouseButtons(buttons);

	switch(button) {
	case 0:
		mouseButton = Qt::LeftButton;
		break;
	case 1:
		mouseButton = Qt::MiddleButton;
		break;
	case 2:
		mouseButton = Qt::RightButton;
		break;
	case 3:
		mouseButton = Qt::MiddleButton;
		break;
	default:
		mouseButton = Qt::LeftButton;
		break;
	}
#ifdef U_EVTSND_DEBUG
	qDebug() << Q_FUNC_INFO << mouseButton << m_mousePos << mouseButtons;
#endif
	QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonRelease, m_mousePos, m_mousePos, mouseButton, mouseButtons, modifiers);
	sendEvent(event);
}

void EventSender::contextMenuRequest(Qt::KeyboardModifiers modifiers)
{
#ifdef U_EVTSND_DEBUG
	qDebug() << Q_FUNC_INFO << m_mousePos << modifiers;
#endif
	sendEvent(new QContextMenuEvent(QContextMenuEvent::Mouse, m_mousePos, m_mousePos, modifiers));
}

void EventSender::mouseMoveTo(const QPoint& pos, int buttons, const Qt::KeyboardModifiers modifiers)
{
	Qt::MouseButtons mouseButtons(buttons);

	m_mousePos = pos;
#ifdef U_EVTSND_DEBUG
	qDebug() << Q_FUNC_INFO << m_mousePos << mouseButtons;
#endif
	QCursor::setPos(m_mousePos);
	QMouseEvent *event = new QMouseEvent(QEvent::MouseMove, m_mousePos, m_mousePos, Qt::NoButton, mouseButtons, modifiers);
	sendEvent(event);
}

void EventSender::mouseScrollBy(int deltaX, int deltaY, int buttons, const Qt::KeyboardModifiers modifiers)
{
	int dx = 0, dy = 0;
	Qt::MouseButtons mouseButtons(buttons);

	if(deltaX) {
		dx = ((deltaX * 40) >> 3) / 15;
		QWheelEvent *event = new QWheelEvent(m_mousePos, m_mousePos, dx, mouseButtons, modifiers, Qt::Horizontal);
		sendEvent(event);
	}
	if(deltaY) {
		dy = ((deltaY * 40) >> 3) / 15;
		QWheelEvent *event = new QWheelEvent(m_mousePos, m_mousePos, dy, mouseButtons, modifiers, Qt::Vertical);
		sendEvent(event);
	}
#ifdef U_EVTSND_DEBUG
	qDebug() << Q_FUNC_INFO << deltaX << deltaY << dx << dy << mouseButtons;
#endif
	handleScrollPosition(dx, dy);
}

void EventSender::sendEvent(QEvent *event)
{
#ifdef U_EVTSND_DEBUG
	qDebug() << Q_FUNC_INFO << m_pWebPage << event;
#endif
	QApplication::sendEvent(m_pWebPage, event);
	delete event;
}

void EventSender::timerEvent(QTimerEvent *event)
{
#ifdef U_EVTSND_DEBUG
	qDebug() << Q_FUNC_INFO;
#endif
	Q_UNUSED(event);
	m_clickTimer.stop();
}

void EventSender::handleScrollPosition(int deltaX, int deltaY)
{
	QWebFrame *frame = m_pWebPage->frameAt(m_mousePos);
	if(frame == m_pWebPage->mainFrame()) {
		m_pWebPage->mainFrame()->scroll(-deltaX, -deltaY);
	} else {
		int mainFrameX = deltaX;
		int mainFrameY = deltaY;
		if(!frame->scrollBarGeometry(Qt::Horizontal).isEmpty()) {
			frame->scroll(-deltaX, 0);
			mainFrameX = 0;
		}
		if(!frame->scrollBarGeometry(Qt::Vertical).isEmpty()) {
			frame->scroll(0, -deltaY);
			mainFrameY = 0;
		}
		m_pWebPage->mainFrame()->scroll(-mainFrameX, -mainFrameY);
	}
}
