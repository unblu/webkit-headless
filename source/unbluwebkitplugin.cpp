/*
 * unbluwebkitplugin.cpp
 *
 *  Created on: Oct 24, 2012
 *      Author: mike
 */

#include "unbluwebkitplugin.h"
#include "unbluwebpopup.h"

UnbluWebKitPlugin::UnbluWebKitPlugin() : JQObject()
{
	qWarning() << Q_FUNC_INFO;
}

UnbluWebKitPlugin::~UnbluWebKitPlugin()
{

}

bool UnbluWebKitPlugin::supportsExtension(QWebKitPlatformPlugin::Extension extension) const
{
	return extension == QWebKitPlatformPlugin::MultipleSelections;
}

QObject* UnbluWebKitPlugin::createExtension(QWebKitPlatformPlugin::Extension extension) const
{
	if(extension == QWebKitPlatformPlugin::MultipleSelections) {
		UnbluWebPopup *popup = new UnbluWebPopup();
		return reinterpret_cast<QObject*>(popup->handle());
	}
	return NULL;
}
