/*
 * core.cpp
 *
 *  Created on: Oct 2, 2012
 *      Author: mike
 */

#include "utils.h"
#include "jvmcontext.h"
#include "webkit.h"

#if defined(Q_OS_MAC)
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)
#else
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#endif

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
{
	Q_UNUSED(reserved);

	int result = JNI_VERSION_1_4;
	QCoreApplication::setAttribute(Qt::AA_UseSimpleDnD, true);
#if defined(Q_OS_MAC)
	QCoreApplication::setAttribute(Qt::AA_MacUseOffscreenScreen, true);
#endif
	if(!JavaVMContext::initialize(jvm)) {
		return 0;
	}

	WebKit::initialize();

	return result;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *jvm, void *reserved)
{
	Q_UNUSED(jvm);
	Q_UNUSED(reserved);
}
