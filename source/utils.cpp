/*
 * utils.cpp
 *
 *  Created on: Oct 3, 2012
 *      Author: mike
 */

#include "utils.h"
#include "jvmcontext.h"

bool Utils::m_bPrintDebugMessages = false;
bool Utils::m_bPrintWarningMessages = false;

void Utils::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	Q_UNUSED(context)

	QDateTime now = QDateTime::currentDateTime();

	switch(type) {
		case QtDebugMsg:
			if(m_bPrintDebugMessages) {
				fprintf(stdout, "%s [DEBUG] %s\n", qPrintable(now.toString(Qt::ISODate)), msg.toLatin1().data());
			}
			break;
		case QtWarningMsg:
			if(m_bPrintWarningMessages) {
				fprintf(stdout, "%s [WARNING] %s\n", qPrintable(now.toString(Qt::ISODate)), msg.toLatin1().data());
			}
			break;
		case QtCriticalMsg:
			fprintf(stderr, "%s [CRITICAL] %s\n", qPrintable(now.toString(Qt::ISODate)), msg.toLatin1().data());
			break;
		case QtFatalMsg:
			fprintf(stderr, "%s [FATAL] %s\n", qPrintable(now.toString(Qt::ISODate)), msg.toLatin1().data());
			break;
		case QtInfoMsg:
			fprintf(stdout, "%s [INFO] %s\n", qPrintable(now.toString(Qt::ISODate)), msg.toLatin1().data());
			break;

	}
}

void Utils::throwByName(JNIEnv *env, const char *name, const char *msg)
{
	jclass cls;

	env->ExceptionClear();

	cls = env->FindClass(name);
	if(cls != NULL) {
		env->ThrowNew(cls, msg);
		env->DeleteLocalRef(cls);
	}
}

jobject Utils::newObject(JNIEnv *env, jclass clazz, jmethodID methodID, ...)
{
	jobject obj = NULL;

	va_list args;
	va_start(args, methodID);
	obj = env->NewObjectV(clazz, methodID, args);
	va_end(args);

	return obj;
}

long Utils::getLongField(JNIEnv *env, jobject clazz, jfieldID fieldID)
{
	if(!isNullObject(env, clazz)) {
		return env->GetLongField(clazz, fieldID);
	}
	env->FatalError("Class object is null");
	return -1L;
}

void Utils::setLongField(JNIEnv *env, jobject clazz, jfieldID fieldID, long value)
{
	if(!isNullObject(env, clazz)) {
		env->SetLongField(clazz, fieldID, (jlong)value);
	}
}

int Utils::getIntField(JNIEnv *env, jobject clazz, jfieldID fieldID)
{
	if(!isNullObject(env, clazz)) {
		return env->GetIntField(clazz, fieldID);
	}
	env->FatalError("Class object is null");
	return -1;
}

void Utils::setIntField(JNIEnv *env, jobject clazz, jfieldID fieldID, int value)
{
	if(!isNullObject(env, clazz)) {
		env->SetIntField(clazz, fieldID, (jint)value);
	}
}

jobject Utils::newGlobalRef(JNIEnv *env, jobject obj)
{
	if(!isNullObject(env, obj)) {
		return env->NewGlobalRef(obj);
	}
	return NULL;
}

void Utils::deleteGlobalRef(JNIEnv *env, jobject obj)
{
	if(!isNullObject(env, obj)) {
		env->DeleteGlobalRef(obj);
	}
}

QVariant Utils::invokeMethod(JNIEnv *env, jobject obj, const char *name, const char *sig, ...)
{
	va_list args;
	int retType = 0;
	jmethodID methodId;
	jclass clazz = env->GetObjectClass(obj);
	QVariant retVal = QVariant::fromValue(false);
	QRegExp matcher("(?:\\(\\S*\\))(.)", Qt::CaseSensitive, QRegExp::RegExp2);

	matcher.indexIn(QString(sig));
	QString type = matcher.cap(1);
	if(!type.isEmpty()) retType = type.at(0).toLatin1();
#ifdef U_UTILS_DEBUG
	qDebug() << Q_FUNC_INFO << name << sig << QString(retType);
#endif
	if(!LOAD_MID(env, methodId, clazz, name, sig)) {
		char msg[256];

		snprintf(msg, 256, "Can't obtain %s method for class", name);
		throwByName(env, EUnsatisfiedLink, msg);
		return retVal;
	}


	va_start(args, sig);
	switch(retType) {
		case 'V':
			env->CallVoidMethodV(obj, methodId, args);
			break;
		case 'Z':
			retVal = QVariant::fromValue(env->CallBooleanMethodV(obj, methodId, args));
			break;
		case 'B':
			retVal = QVariant::fromValue(env->CallByteMethodV(obj, methodId, args));
			break;
		case 'C':
			retVal = QVariant::fromValue(env->CallCharMethodV(obj, methodId, args));
			break;
		case 'S':
			retVal = QVariant::fromValue(env->CallShortMethodV(obj, methodId, args));
			break;
		case 'I':
			retVal = QVariant::fromValue(env->CallIntMethodV(obj, methodId, args));
			break;
		case 'J':
			retVal = QVariant::fromValue(env->CallLongMethodV(obj, methodId, args));
			break;
		case 'F':
			retVal = QVariant::fromValue(env->CallFloatMethodV(obj, methodId, args));
			break;
		case 'D':
			retVal = QVariant::fromValue(env->CallDoubleMethodV(obj, methodId, args));
		case 'L':
		case '[':
			retVal = QVariant::fromValue(env->CallObjectMethodV(obj, methodId, args));
			break;
	}
	va_end(args);

	return retVal;
}

bool Utils::isNullObject(JNIEnv *env, jobject obj)
{
	return (env->IsSameObject(obj, NULL) == JNI_TRUE);
}

QRect Utils::getQRect(JNIEnv *env, jobject obj)
{
	if(!isNullObject(env, obj)) {
		int left = getIntField(env, obj, JavaVMContext::rectangleXFID);
		int top = getIntField(env, obj, JavaVMContext::rectangleYFID);
		int width = getIntField(env, obj, JavaVMContext::rectangleWidthFID);
		int height = getIntField(env, obj, JavaVMContext::rectangleHeightFID);
		return QRect(left, top, width, height);
	}
	return QRect();
}

jobject Utils::getAWTRectangle(JNIEnv *env, const QRect& rect)
{
	int left = rect.x();
	int top = rect.y();
	int width = rect.width();
	int height = rect.height();
	return newObject(env, (jclass)JavaVMContext::classRectangle, JavaVMContext::rectangleInitMID, left, top, width, height);
}

QSize Utils::getQSize(JNIEnv *env, jobject obj)
{
	if(!isNullObject(env, obj)) {
		int width = getIntField(env, obj, JavaVMContext::dimensionWidthFID);
		int height = getIntField(env, obj, JavaVMContext::dimensionHeightFID);
		return QSize(width, height);
	}
	return QSize();
}

jobject Utils::getAWTDimension(JNIEnv *env, const QSize& size)
{
	int width = size.width();
	int height = size.height();
	return newObject(env, (jclass)JavaVMContext::classDimension, JavaVMContext::dimensionInitMID, width, height);
}

QPoint Utils::getQPoint(JNIEnv *env, jobject obj)
{
	if(!isNullObject(env, obj)) {
		int x = getIntField(env, obj, JavaVMContext::pointXFID);
		int y = getIntField(env, obj, JavaVMContext::pointYFID);
		return QPoint(x, y);
	}
	return QPoint();
}

jobject Utils::getAWTPoint(JNIEnv *env, const QPoint& point)
{
	return newObject(env, (jclass)JavaVMContext::classPoint, JavaVMContext::pointInitMID, point.x(), point.y());
}

jbyteArray Utils::getByteArray(JNIEnv *env, const uchar *data, int size)
{
	jbyteArray array = env->NewByteArray(size);

	if(!isNullObject(env, array)) {
		env->SetByteArrayRegion(array, 0, size, (jbyte*)data);
	}

	return array;
}

jintArray Utils::getIntArray(JNIEnv *env, const uchar *data, int size)
{
	jintArray array = env->NewIntArray(size);

	if(!isNullObject(env, array)) {
		env->SetIntArrayRegion(array, 0, size, (jint*)data);
	}

	return array;
}

QString Utils::getQString(JNIEnv *env, jobject jstr)
{
	QString result;
	if(!isNullObject(env, jstr)) {
		const char *pszStr = env->GetStringUTFChars((jstring)jstr, NULL);
		result = QString(pszStr);
		env->ReleaseStringUTFChars((jstring)jstr, pszStr);
	}
	return result;
}

jstring Utils::getJString(JNIEnv *env, const QString& string)
{
	jstring result = NULL;
	if(!string.isNull()) {
		result = env->NewStringUTF(string.toUtf8());
	}
	return result;
}

QStringList Utils::getQStringArray(JNIEnv *env, jobject jstringArray)
{
	QStringList result;
	if(!isNullObject(env, jstringArray)) {
		int len = env->GetArrayLength((jarray) jstringArray);

		for(int i=0;i < len;i++) {
			result.append(Utils::getQString(env, env->GetObjectArrayElement((jobjectArray) jstringArray, (jsize) i)));
		}
	}
	return result;
}

jobject Utils::getJStringArray(JNIEnv *env, const QStringList& stringList)
{
	jobjectArray result = NULL;
	if(stringList.size() > 0) {
		int len = stringList.size();
		result = env->NewObjectArray(len, (jclass) JavaVMContext::classString, env->NewStringUTF(""));

		for(int i=0;i < len;i++) {
			env->SetObjectArrayElement(result, i, Utils::getJString(env, stringList.at(i)));
		}
	}
	return result;
}

jobject Utils::getJIntegerArray(JNIEnv *env, const int *data, int size)
{
	jobjectArray result = NULL;
	if(size > 0) {
		result = env->NewObjectArray(size, (jclass) JavaVMContext::classInteger, newObject(env, (jclass) JavaVMContext::classInteger, JavaVMContext::integerInitMID, 0));

		for(int i=0;i < size;i++) {
			env->SetObjectArrayElement(result, i, newObject(env, (jclass) JavaVMContext::classInteger, JavaVMContext::integerInitMID, data[i]));
		}
	}
	return result;
}

jobject Utils::getJBoolean(JNIEnv *env, bool value)
{
	return newObject(env, (jclass) JavaVMContext::classBoolean, JavaVMContext::booleanInitMID, value);
}

void Utils::deleteLocalRef(JNIEnv *env, jobject obj)
{
	if(!isNullObject(env, obj)) {
		env->DeleteLocalRef(obj);
	}
}

void Utils::pushFrame(JNIEnv *env, jint capacity)
{
	if(env->PushLocalFrame(capacity) < 0) {
		throwByName(env, EOutOfMemory, "push local frame failed");
	}
}

jobject Utils::popFrame(JNIEnv *env, jobject result)
{
	return env->PopLocalFrame(result);
}
