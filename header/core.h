/*
 * core.h
 *
 *  Created on: Oct 2, 2012
 *      Author: mike
 */

#ifndef CORE_H_
#define CORE_H_

#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <jni.h>
#include <algorithm>

#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QSemaphore>
#include <QWaitCondition>
#include <QMutex>
#include <QBuffer>
#include <QRect>
#include <QDebug>
#include <QDateTime>
#include <QClipboard>
#include <QMimeData>
#include <QMap>
#include <QUuid>
#include <QRegExp>
#include <QBasicTimer>
#include <QFontDatabase>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QComboBox>
#include <QBitmap>
#include <QtPlugin>
#include <QtWebKitWidgets>

#if defined(SOLARIS2) || defined(__GNUC__)
#if defined(_WIN64)
#define L2A(X) ((void *)(long long)(X))
#define A2L(X) ((jlong)(long long)(X))
#else
#define L2A(X) ((void *)(unsigned long)(X))
#define A2L(X) ((jlong)(unsigned long)(X))
#endif
#endif

#if defined(_MSC_VER)
#define L2A(X) ((void *)(X))
#define A2L(X) ((jlong)(X))
#define snprintf sprintf_s
#else
#if defined(_WIN32_WCE)
#define snprintf _snprintf
#endif
#endif

#define LOAD_WEAKREF(env, var) \
	(((var == NULL) ? NULL : ((var = (env)->NewWeakGlobalRef(var)) == NULL ? NULL : var)) != NULL)
#define FIND_CLASS(env, simple, name) \
	(class ## simple = (env)->FindClass(name))
#define FIND_PRIMITIVE_CLASS(env, simple) \
	(classPrimitive ## simple = (env)->GetStaticObjectField((jclass) class ## simple, (env)->GetStaticFieldID((jclass) class ## simple, "TYPE", "Ljava/lang/Class;")))
#define LOAD_CREF(env, simple, name) \
	(FIND_CLASS(env, simple, name) && LOAD_WEAKREF(env, class ## simple))
#define LOAD_PCREF(env, simple, name) \
	(LOAD_CREF(env, simple, name) && \
	 FIND_PRIMITIVE_CLASS(env, simple) && \
	 LOAD_WEAKREF(env, classPrimitive ## simple))
#define LOAD_MID(env, var, clazz, name, sig) \
	((var = (env)->GetMethodID((jclass)clazz, name, sig)) ? var : 0)
#define LOAD_FID(env, var, clazz, name, sig) \
	((var = (env)->GetFieldID((jclass)clazz, name, sig)) ? var : 0)

#define EIllegalArgument 			"java/lang/IllegalArgumentException"
#define EOutOfMemory 				"java/lang/OutOfMemoryError"
#define EUnsatisfiedLink 			"java/lang/UnsatisfiedLinkError"
#define EIllegalState 				"java/lang/IllegalStateException"
#define EUnsupportedOperation 		"java/lang/UnsupportedOperationException"
#define ERuntime 					"java/lang/RuntimeException"
#define EError 						"java/lang/Error"

Q_DECLARE_METATYPE(jobject)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CORE_H_ */
