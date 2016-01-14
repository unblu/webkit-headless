/*
 * unbluwebpopup.h
 *
 *  Created on: Oct 24, 2012
 *      Author: mike
 */

#ifndef UNBLUWEBPOPUP_H_
#define UNBLUWEBPOPUP_H_

#include "jqobject.h"

#include <QStandardItemModel>

class UnbluWebPopupCombo;

class UnbluWebPopup : public QWebPopup
{
	Q_OBJECT

public:
	UnbluWebPopup();
	virtual ~UnbluWebPopup();

    void destroyPopup();

    void populate(const QWebSelectData& data);

    jobject createPopupDialog(JNIEnv *env);

    static void loadJavaClasses(JNIEnv *env);

    virtual void show(const QWebSelectData& data);
    virtual void hide();

 public slots:
	void hidePopup();
	void activeChanged(int index);

private:
    bool m_bPopupVisible;

    friend class UnbluWebPopupCombo;

	static jobject classPopupDialog;
	static jmethodID popupDialogInitMID;
};

#endif /* UNBLUWEBPOPUP_H_ */
