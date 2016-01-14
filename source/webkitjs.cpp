/*
 * webkitjs.cpp
 *
 *  Created on: Jul 16, 2014
 *      Author: mike
 */

#include "utils.h"
#include "webkit.h"
#include "window_private.h"
#include "webkitjs.h"

WebKitJS::WebKitJS(QObject *parent) : JQObject(parent)
{
	connect(this, SIGNAL(onscroll()), parent, SLOT(handleOnScroll()));
}

WebKitJS::~WebKitJS()
{

}

// javaScript callbacks
void WebKitJS::log(const QVariantList& arguments)
{
	QDateTime now = QDateTime::currentDateTime();

	foreach(QVariant v, arguments) {
		fprintf(stderr, "%s [LOG] %s\n", qPrintable(now.toString(Qt::ISODate)), v.toString().toLatin1().constData());
	}
}
