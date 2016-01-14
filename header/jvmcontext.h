/*
 * jvmcontext.h
 *
 *  Created on: Oct 5, 2012
 *      Author: mike
 */

#ifndef JVMCONTEXT_H_
#define JVMCONTEXT_H_

#include "core.h"

class JavaVMContext
{
private:
	JavaVMContext(JavaVM *jvm);
	bool init();

public:
	virtual ~JavaVMContext();

	bool getEnvironment(JNIEnv*& env);
	void releaseEnvironment(JNIEnv *env, bool attached);

	JavaVM* getJavaVM() { return m_pJVM; }

	static bool initialize(JavaVM *jvm);
	static JavaVMContext* instance() { return contextInstance; }

private:
	const char* initCoreClasses(JNIEnv *env);

public:
	static jobject classString;
	static jmethodID stringInitMID;

	static jobject classBoolean;
	static jobject classPrimitiveBoolean;
	static jmethodID booleanInitMID;
	static jfieldID booleanValueFID;

	static jobject classInteger;
	static jmethodID integerInitMID;

	static jobject classPoint;
	static jmethodID pointInitMID;
	static jfieldID pointXFID;
	static jfieldID pointYFID;

	static jobject classRectangle;
	static jmethodID rectangleInitMID;
	static jfieldID rectangleXFID;
	static jfieldID rectangleYFID;
	static jfieldID rectangleWidthFID;
	static jfieldID rectangleHeightFID;

	static jobject classDimension;
	static jmethodID dimensionInitMID;
	static jfieldID dimensionWidthFID;
	static jfieldID dimensionHeightFID;

private:
	JavaVM *m_pJVM;

	static JavaVMContext *contextInstance;
};

#endif /* JVMCONTEXT_H_ */
