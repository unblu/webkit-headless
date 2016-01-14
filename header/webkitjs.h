/*
 * webkitjs.h
 *
 *  Created on: Jul 16, 2014
 *      Author: mike
 */

#ifndef WEBKITJS_H_
#define WEBKITJS_H_

#include "jqobject.h"

class WebKitJS : public JQObject
{
	Q_OBJECT

public:
	WebKitJS(QObject *parent);
	virtual ~WebKitJS();

	Q_INVOKABLE void log(const QVariantList& arguments);

signals:
	void onscroll();
};

#endif /* WEBKITJS_H_ */
