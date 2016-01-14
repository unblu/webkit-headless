/*
 * unbluwebkitplugin.h
 *
 *  Created on: Oct 24, 2012
 *      Author: mike
 */

#ifndef UNBLUSELECTPOPUP_H_
#define UNBLUSELECTPOPUP_H_

#include "jqobject.h"

class QtFallbackWebPopupWrapper;

class UnbluWebKitPlugin : public JQObject, public QWebKitPlatformPlugin
{
	Q_OBJECT
	Q_INTERFACES(QWebKitPlatformPlugin)
    Q_PLUGIN_METADATA(IID "org.qt-project.QtWebKit.PlatformPluginInterface")

public:
	UnbluWebKitPlugin();
	virtual ~UnbluWebKitPlugin();

    virtual bool supportsExtension(Extension extension) const;
    virtual QObject* createExtension(Extension extension) const;
};

#endif /* UNBLUSELECTPOPUP_H_ */
