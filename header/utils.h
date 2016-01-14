/*
 * utils.h
 *
 *  Created on: Oct 3, 2012
 *      Author: mike
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "core.h"

class Utils
{
public:
	static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

	static void throwByName(JNIEnv *env, const char *name, const char *msg);
	static jobject newObject(JNIEnv *env, jclass clazz, jmethodID methodID, ...);
	static int getIntField(JNIEnv *env, jobject clazz, jfieldID fieldID);
	static void setIntField(JNIEnv *env, jobject clazz, jfieldID fieldID, int value);
	static long getLongField(JNIEnv *env,jobject clazz, jfieldID fieldID);
	static void setLongField(JNIEnv *env, jobject clazz, jfieldID fieldID, long value);
	static jobject newGlobalRef(JNIEnv *env, jobject obj);
	static void deleteGlobalRef(JNIEnv *env, jobject obj);
	static bool isNullObject(JNIEnv *env, jobject obj);
	static QVariant invokeMethod(JNIEnv *env, jobject obj, const char *name, const char *sig, ...);
	static void deleteLocalRef(JNIEnv *env, jobject obj);

	static QRect getQRect(JNIEnv *env, jobject obj);
	static jobject getAWTRectangle(JNIEnv *env, const QRect& rect);

	static QSize getQSize(JNIEnv *env, jobject obj);
	static jobject getAWTDimension(JNIEnv *env, const QSize& size);

	static QPoint getQPoint(JNIEnv *env, jobject obj);
	static jobject getAWTPoint(JNIEnv *env, const QPoint& point);

	static jbyteArray getByteArray(JNIEnv *env, const uchar *data, int size);
	static jintArray getIntArray(JNIEnv *env, const uchar *data, int size);

	static QString getQString(JNIEnv *env, jobject jstr);
	static jstring getJString(JNIEnv *env, const QString& string);

	static QStringList getQStringArray(JNIEnv *env, jobject jstringArray);
	static jobject getJStringArray(JNIEnv *env, const QStringList& stringList);

	static jobject getJIntegerArray(JNIEnv *env, const int *data, int size);

	static jobject getJBoolean(JNIEnv *env, bool value);

	static void pushFrame(JNIEnv *env, jint capacity = 16);
	static jobject popFrame(JNIEnv *env, jobject result = NULL);

public:
	static bool m_bPrintDebugMessages;
	static bool m_bPrintWarningMessages;
};


#endif /* UTILS_H_ */
