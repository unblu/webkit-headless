/*
 * event.cpp
 *
 *  Created on: Oct 9, 2012
 *      Author: mike
 */

#include "event.h"
#include "utils.h"
#include "jvmcontext.h"

jobject Event::classEvent;
jmethodID Event::eventInitMID;
jfieldID Event::eventTypeFID;

jobject Event::classKeyEvent;
jmethodID Event::keyEventInitMID;
jfieldID Event::keyEventKeyCodeFID;
jfieldID Event::keyEventCharCodeFID;
jfieldID Event::keyEventModifiersFID;

jobject Event::classMouseEvent;
jmethodID Event::mouseEventInitMID;
jfieldID Event::mouseEventButtonFID;
jfieldID Event::mouseEventButtonsFID;
jfieldID Event::mouseEventKeyModifiersFID;
jfieldID Event::mouseEventPositionFID;

jobject Event::classMouseWheelEvent;
jmethodID Event::mouseWheelEventInitFID;
jfieldID Event::mouseWheelEventDeltaXFID;
jfieldID Event::mouseWheelEventDeltaYFID;
jfieldID Event::mouseWheelEventButtonsFID;
jfieldID Event::mouseWheelEventKeyModifiersFID;

Event::Event()
{

}

Event::Event(const QString& type, const QVariantMap& data) : m_eventType(type), m_eventData(data)
{
#ifdef U_EVT_DEBUG
	qDebug() << Q_FUNC_INFO << type << data;
#endif
}

Event::~Event()
{

}

bool Event::isKeyEvent() const
{
	return (m_eventType == "keydown" || m_eventType == "keyup" || m_eventType == "keypress");
}

bool Event::isMouseEvent() const
{
	return (m_eventType == "mousedown" || m_eventType == "mouseup" || m_eventType == "mousemove" || m_eventType == "mousewheel" || m_eventType == "doubleclick");
}

Event* Event::createEvent(JNIEnv *env, jobject eventObj)
{
	QVariantMap eventData;
	QString eventType = Utils::getQString(env, env->GetObjectField(eventObj, eventTypeFID));

	eventType = eventType.toLower();
	if(env->IsInstanceOf(eventObj, (jclass)classKeyEvent)) {
		eventData[EVENT_TYPE_KEY_KEYCODE] = QVariant::fromValue(env->GetIntField(eventObj, keyEventKeyCodeFID));
		eventData[EVENT_TYPE_KEY_CHARCODE] = QVariant::fromValue(env->GetIntField(eventObj, keyEventCharCodeFID));
		eventData[EVENT_TYPE_KEY_MODIFIERS] = QVariant::fromValue(env->GetIntField(eventObj, keyEventModifiersFID));
	} else if(env->IsInstanceOf(eventObj, (jclass)classMouseEvent)){
		jobject jpoint = env->GetObjectField(eventObj, mouseEventPositionFID);
		int button = env->GetIntField(eventObj, mouseEventButtonFID);
		int buttons = env->GetIntField(eventObj, mouseEventButtonsFID);
		int modifiers = env->GetIntField(eventObj, mouseEventKeyModifiersFID);
		QPoint pos = Utils::getQPoint(env, jpoint);

		eventData[EVENT_TYPE_MOUSE_BUTTON] = QVariant::fromValue(button);
		eventData[EVENT_TYPE_MOUSE_BUTTONS] = QVariant::fromValue(buttons);
		eventData[EVENT_TYPE_MOUSE_POS_X] = QVariant::fromValue(pos.x());
		eventData[EVENT_TYPE_MOUSE_POS_Y] = QVariant::fromValue(pos.y());
		eventData[EVENT_TYPE_KEY_MODIFIERS] = QVariant::fromValue(modifiers);
	} else {
		eventData[EVENT_TYPE_MOUSE_BUTTONS] = QVariant::fromValue(env->GetIntField(eventObj, mouseWheelEventButtonsFID));
		eventData[EVENT_TYPE_MOUSE_WHEEL_DELTA_X] = QVariant::fromValue(env->GetIntField(eventObj, mouseWheelEventDeltaXFID));
		eventData[EVENT_TYPE_MOUSE_WHEEL_DELTA_Y] = QVariant::fromValue(env->GetIntField(eventObj, mouseWheelEventDeltaYFID));
		eventData[EVENT_TYPE_KEY_MODIFIERS] = QVariant::fromValue(env->GetIntField(eventObj, mouseWheelEventKeyModifiersFID));
	}

	return new Event(eventType, eventData);
}

void Event::loadJavaClasses(JNIEnv *env)
{
	if(!LOAD_CREF(env, Event, "com/unblu/webkit/main/server/core/internal/Event")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain class com.unblu.webkit.main.server.core.internal.Event");
	} else if(!LOAD_FID(env, eventTypeFID, classEvent, "type", "Ljava/lang/String;")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain type field ID for class com.unblu.webkit.main.server.core.internal.Event");
	} else if(!LOAD_CREF(env, KeyEvent, "com/unblu/webkit/main/server/core/internal/KeyEvent")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain class com.unblu.webkit.main.server.core.internal.KeyEvent");
	} else if(!LOAD_MID(env, keyEventInitMID, classKeyEvent, "<init>", "(Ljava/lang/String;III)V")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain constructor for class com.unblu.webkit.main.server.core.internal.KeyEvent");
	} else if(!LOAD_FID(env, keyEventKeyCodeFID, classKeyEvent, "keyCode", "I")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain keyCode field ID for class com.unblu.webkit.main.server.core.internal.KeyEvent");
	} else if(!LOAD_FID(env, keyEventCharCodeFID, classKeyEvent, "charCode", "I")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain charCode field ID for class com.unblu.webkit.main.server.core.internal.KeyEvent");
	} else if(!LOAD_FID(env, keyEventModifiersFID, classKeyEvent, "modifiers", "I")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain modifiers field ID for class com.unblu.webkit.main.server.core.internal.KeyEvent");
	} else if(!LOAD_CREF(env, MouseEvent, "com/unblu/webkit/main/server/core/internal/MouseEvent")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain class com.unblu.webkit.main.server.core.internal.MouseEvent");
	} else if(!LOAD_MID(env, mouseEventInitMID, classMouseEvent, "<init>", "(Ljava/lang/String;IIILjava/awt/Point;)V")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain constructor for class com.unblu.webkit.main.server.core.internal.MouseEvent");
	} else if(!LOAD_FID(env, mouseEventButtonFID, classMouseEvent, "button", "I")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain button field ID for class com.unblu.webkit.main.server.core.internal.MouseEvent");
	} else if(!LOAD_FID(env, mouseEventButtonsFID, classMouseEvent, "buttons", "I")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain buttons field ID for class com.unblu.webkit.main.server.core.internal.MouseEvent");
	} else if(!LOAD_FID(env, mouseEventKeyModifiersFID, classMouseEvent, "modifiers", "I")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain modifiers field ID for class com.unblu.webkit.main.server.core.internal.MouseEvent");
	} else if(!LOAD_FID(env, mouseEventPositionFID, classMouseEvent, "position", "Ljava/awt/Point;")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain position field ID for class com.unblu.webkit.main.server.core.internal.MouseEvent");
	} else if(!LOAD_CREF(env, MouseWheelEvent, "com/unblu/webkit/main/server/core/internal/MouseWheelEvent")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain class com.unblu.webkit.main.server.core.internal.MouseWheelEvent");
	} else if(!LOAD_MID(env, mouseWheelEventInitFID, classMouseWheelEvent, "<init>", "(IIII)V")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain constructor for class com.unblu.webkit.main.server.core.internal.MouseWheelEvent");
	} else if(!LOAD_FID(env, mouseWheelEventDeltaXFID, classMouseWheelEvent, "deltaX", "I")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain deltaX field ID for class com.unblu.webkit.main.server.core.internal.MouseWheelEvent");
	} else if(!LOAD_FID(env, mouseWheelEventDeltaYFID, classMouseWheelEvent, "deltaY", "I")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain deltaY field ID for class com.unblu.webkit.main.server.core.internal.MouseWheelEvent");
	} else if(!LOAD_FID(env, mouseWheelEventButtonsFID, classMouseWheelEvent, "buttons", "I")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain buttons field ID for class com.unblu.webkit.main.server.core.internal.MouseWheelEvent");
	} else if(!LOAD_FID(env, mouseWheelEventKeyModifiersFID, classMouseWheelEvent, "modifiers", "I")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain modifiers field ID for class com.unblu.webkit.main.server.core.internal.MouseWheelEvent");
	}
}
