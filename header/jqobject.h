/*
 * jqobject.h
 *
 *  Created on: Oct 7, 2012
 *      Author: mike
 */

#ifndef JQOBJECT_H_
#define JQOBJECT_H_

#include "core.h"
#include "jvmcontext.h"

#include <QtWebKit>

#define PAGE_SETTINGS_LOAD_IMAGES           "loadImages"
#define PAGE_SETTINGS_JS_ENABLED            "javascriptEnabled"
#define PAGE_SETTINGS_XSS_AUDITING          "XSSAuditingEnabled"
#define PAGE_SETTINGS_USER_AGENT            "userAgent"
#define PAGE_SETTINGS_LOCAL_ACCESS_REMOTE   "localToRemoteUrlAccessEnabled"
#define PAGE_SETTINGS_USERNAME              "userName"
#define PAGE_SETTINGS_PASSWORD              "password"
#define PAGE_SETTINGS_WEB_SECURITY_ENABLED  "webSecurityEnabled"
#define PAGE_SETTINGS_JS_CAN_OPEN_WINDOWS   "javascriptCanOpenWindows"
#define PAGE_SETTINGS_JS_CAN_CLOSE_WINDOWS  "javascriptCanCloseWindows"
#define PAGE_SETTINGS_PLUGINS_ENABLED		"pluginsEnabled"
#define PAGE_SETTINGS_DEVELOPER_TOOLS		"developerTools"

#define COMPLEX_RESULT_RETURNVALUE			"returnValue"
#define COMPLEX_RESULT_RETURNDATA			"returnData"


#define WEBKIT_JNI_VERSION		"1.0.0"

class JQObject : public QObject
{
public:
	JQObject(QObject *parent = NULL);
	virtual ~JQObject();

	inline bool isMainThread() const { return QThread::currentThread() == QApplication::instance()->thread(); }

protected:
	JavaVMContext *m_pJVMContext;
};


#endif /* JQOBJECT_H_ */
