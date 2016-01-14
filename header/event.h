/*
 * event.h
 *
 *  Created on: Oct 9, 2012
 *      Author: mike
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "core.h"

#define EVENT_TYPE_KEY_KEYCODE			"keycode"
#define EVENT_TYPE_KEY_CHARCODE			"charcode"
#define EVENT_TYPE_KEY_MODIFIERS		"modifiers"
#define EVENT_TYPE_MOUSE_BUTTON			"mousebutton"
#define EVENT_TYPE_MOUSE_BUTTONS		"mousebuttons"
#define EVENT_TYPE_MOUSE_POS_X			"mouse_x"
#define EVENT_TYPE_MOUSE_POS_Y			"mouse_y"
#define EVENT_TYPE_MOUSE_WHEEL_DELTA_X	"mouse_wheel_delta_x"
#define EVENT_TYPE_MOUSE_WHEEL_DELTA_Y	"mouse_wheel_delta_y"

class Event
{
public:
	Event();
	Event(const QString& type, const QVariantMap& data);
	virtual ~Event();

	bool isKeyEvent() const;
	bool isMouseEvent() const;

	const QString& getEventType() const { return m_eventType; }
	const QVariantMap& getEventData() const { return m_eventData; }

	static Event* createEvent(JNIEnv *env, jobject eventObj);
	static void loadJavaClasses(JNIEnv *env);

public:
	static jobject classEvent;
	static jmethodID eventInitMID;
	static jfieldID eventTypeFID;

	static jobject classKeyEvent;
	static jmethodID keyEventInitMID;
	static jfieldID keyEventKeyCodeFID;
	static jfieldID keyEventCharCodeFID;
	static jfieldID keyEventModifiersFID;

	static jobject classMouseEvent;
	static jmethodID mouseEventInitMID;
	static jfieldID mouseEventButtonFID;
	static jfieldID mouseEventButtonsFID;
	static jfieldID mouseEventKeyModifiersFID;
	static jfieldID mouseEventPositionFID;

	static jobject classMouseWheelEvent;
	static jmethodID mouseWheelEventInitFID;
	static jfieldID mouseWheelEventDeltaXFID;
	static jfieldID mouseWheelEventDeltaYFID;
	static jfieldID mouseWheelEventButtonsFID;
	static jfieldID mouseWheelEventKeyModifiersFID;

private:
	QString m_eventType;
	QVariantMap m_eventData;
};

#endif /* EVENT_H_ */
