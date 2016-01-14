/*
 * networkaccessmanager.h
 *
 *  Created on: Jan 25, 2013
 *      Author: mike
 */

#ifndef NETWORKACCESSMANAGER_H_
#define NETWORKACCESSMANAGER_H_

#include "jqobject.h"

class NetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT

public:
	NetworkAccessManager(QObject *parent = NULL);
	virtual ~NetworkAccessManager();

	void setProxy(const QString& proxyUrl);

protected:
	QNetworkReply *createRequest(Operation op, const QNetworkRequest& request, QIODevice *outgoingData = NULL);
};

#endif /* NETWORKACCESSMANAGER_H_ */
