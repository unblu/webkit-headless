/*
 * jvmcontext.cpp
 *
 *  Created on: Oct 5, 2012
 *      Author: mike
 */

#include "jvmcontext.h"

jobject JavaVMContext::classString;
jmethodID JavaVMContext::stringInitMID;

jobject JavaVMContext::classInteger;
jmethodID JavaVMContext::integerInitMID;

jobject JavaVMContext::classBoolean;
jobject JavaVMContext::classPrimitiveBoolean;
jmethodID JavaVMContext::booleanInitMID;
jfieldID JavaVMContext::booleanValueFID;

jobject JavaVMContext::classPoint;
jmethodID JavaVMContext::pointInitMID;
jfieldID JavaVMContext::pointXFID;
jfieldID JavaVMContext::pointYFID;

jobject JavaVMContext::classRectangle;
jmethodID JavaVMContext::rectangleInitMID;
jfieldID JavaVMContext::rectangleXFID;
jfieldID JavaVMContext::rectangleYFID;
jfieldID JavaVMContext::rectangleWidthFID;
jfieldID JavaVMContext::rectangleHeightFID;

jobject JavaVMContext::classDimension;
jmethodID JavaVMContext::dimensionInitMID;
jfieldID JavaVMContext::dimensionWidthFID;
jfieldID JavaVMContext::dimensionHeightFID;

JavaVMContext* JavaVMContext::contextInstance = NULL;

JavaVMContext::JavaVMContext(JavaVM *jvm) : m_pJVM(jvm)
{
}

JavaVMContext::~JavaVMContext()
{
}

bool JavaVMContext::getEnvironment(JNIEnv*& env)
{
	bool attached = m_pJVM->GetEnv((void**)&env, JNI_VERSION_1_4) == JNI_OK;
	if(!attached) {
		if(m_pJVM->AttachCurrentThread((void**)&env, NULL) != JNI_OK) {
			fprintf(stderr, "Can't attach native thread to VM on load\n");
			env = NULL;
		}
	}
	return attached;
}

void JavaVMContext::releaseEnvironment(JNIEnv *env, bool attached)
{
	if(env != NULL && !attached) {
		m_pJVM->DetachCurrentThread();
	}
}

bool JavaVMContext::initialize(JavaVM *jvm)
{
	JavaVMContext *context = new JavaVMContext(jvm);

	if(context != NULL) {
		if(!context->init()) {
			delete context;
			return false;
		}

		contextInstance = context;
		return true;
	}
	return false;
}

bool JavaVMContext::init()
{
	JNIEnv *env;
	const char *err;
	bool result = true;
	bool attached = m_pJVM->GetEnv((void**)&env, JNI_VERSION_1_4) == JNI_OK;

	if(!attached) {
		if(m_pJVM->AttachCurrentThread((void**)&env, NULL) != JNI_OK) {
			fprintf(stderr, "Can't attach native thread to VM on load\n");
			return false;
		}
	}

	if((err = initCoreClasses(env)) != NULL) {
		fprintf(stderr, "Problems loading core IDs: %s\n", err);
		result = false;
	}

	if(!attached) {
		m_pJVM->DetachCurrentThread();
	}

	return result;
}

const char* JavaVMContext::initCoreClasses(JNIEnv *env)
{
	if(!LOAD_CREF(env, Point, "java/awt/Point")) return "java.awt.Point";
	if(!LOAD_CREF(env, Dimension, "java/awt/Dimension")) return "java.awt.Dimension";
	if(!LOAD_CREF(env, Rectangle, "java/awt/Rectangle")) return "java.awt.Rectangle";
	if(!LOAD_CREF(env, String, "java/lang/String")) return "java.lang.String";
	if(!LOAD_CREF(env, Integer, "java/lang/Integer")) return "java.lang.Integer";
	if(!LOAD_PCREF(env, Boolean, "java/lang/Boolean")) return "java.lang.Boolean";

	if(!LOAD_MID(env, pointInitMID, classPoint, "<init>", "(II)V"))
		return "java.awt.Point<init>(II)V";
	if(!LOAD_FID(env, pointXFID, classPoint, "x", "I"))
		return "java.awt.Point.x";
	if(!LOAD_FID(env, pointYFID, classPoint, "y", "I"))
		return "java.awt.Point.y";

	if(!LOAD_MID(env, dimensionInitMID, classDimension, "<init>", "(II)V"))
		return "java.awt.Dimension<init>(II)V";
	if(!LOAD_FID(env, dimensionWidthFID, classDimension, "width", "I"))
		return "java.awt.Dimension.width";
	if(!LOAD_FID(env, dimensionHeightFID, classDimension, "height", "I"))
		return "java.awt.Dimension.height";

	if(!LOAD_MID(env, rectangleInitMID, classRectangle, "<init>", "(IIII)V"))
		return "java.awt.Rectangle<init>(IIII)V";
	if(!LOAD_FID(env, rectangleXFID, classRectangle, "x", "I"))
		return "java.awt.Rectangle.x";
	if(!LOAD_FID(env, rectangleYFID, classRectangle, "y", "I"))
		return "java.awt.Rectangle.y";
	if(!LOAD_FID(env, rectangleWidthFID, classRectangle, "width", "I"))
		return "java.awt.Rectangle.width";
	if(!LOAD_FID(env, rectangleHeightFID, classRectangle, "height", "I"))
		return "java.awt.Rectangle.height";

	if(!LOAD_MID(env, booleanInitMID, classBoolean, "<init>", "(Z)V"))
		return "java.lang.Boolean.<init>(Z)V";
	if(!LOAD_FID(env, booleanValueFID, classBoolean, "value", "Z"))
		return "java.lang.Boolean.value";

	if(!LOAD_MID(env, stringInitMID, classString, "<init>", "(Ljava/lang/String;)V"))
		return "java.lang.String.<init>(Ljava/lang/String;)V";

	if(!LOAD_MID(env, integerInitMID, classInteger, "<init>", "(I)V"))
		return "java.lang.Integer.<init>(I)V";

	return NULL;
}
