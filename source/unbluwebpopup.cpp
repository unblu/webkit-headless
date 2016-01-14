/*
 * unbluwebpopup.cpp
 *
 *  Created on: Oct 24, 2012
 *      Author: mike
 */

#include "utils.h"
#include "unbluwebpopup.h"
#include "customwebpage.h"
#include "unbluwebpopupcombo.h"

jobject UnbluWebPopup::classPopupDialog;
jmethodID UnbluWebPopup::popupDialogInitMID;

UnbluWebPopup::UnbluWebPopup() : QWebPopup(), m_bPopupVisible(false)
{
	qWarning() << Q_FUNC_INFO;
}

UnbluWebPopup::~UnbluWebPopup()
{
	qWarning() << Q_FUNC_INFO;
	destroyPopup();
}

void UnbluWebPopup::show(const QWebSelectData& data)
{
	qWarning() << Q_FUNC_INFO << data.itemCount() << geometry();

	destroyPopup();

	setContent(new UnbluWebPopupCombo(this));

	populate(data);

	QShowEvent event;
	QApplication::sendEvent(content(), &event);
}

void UnbluWebPopup::hide()
{
	qWarning() << Q_FUNC_INFO;
}

void UnbluWebPopup::hidePopup()
{
	QHideEvent event;
	QApplication::sendEvent(content(), &event);
}

void UnbluWebPopup::destroyPopup()
{
	if(content() != NULL) {
		content()->deleteLater();
		setContent(NULL);
	}
}

void UnbluWebPopup::populate(const QWebSelectData& data)
{
	UnbluWebPopupCombo *combo = qobject_cast<UnbluWebPopupCombo*>(content());
	QStandardItemModel *model = qobject_cast<QStandardItemModel*>(combo->model());

	int currentIndex = -1;
	for(int i = 0;i < data.itemCount(); i++) {
		switch(data.itemType(i)) {
		case QWebSelectData::Separator:
			combo->insertSeparator(i);
			break;
		case QWebSelectData::Group:
			combo->insertLabel(i, data.itemText(i));
			break;
		case QWebSelectData::Option:
			combo->insertItem(i, data.itemText(i));
			model->item(i)->setEnabled(data.itemIsEnabled(i));
			model->item(i)->setToolTip(data.itemToolTip(i));
			model->item(i)->setForeground(data.itemForegroundColor(i));
			model->item(i)->setBackground(data.itemBackgroundColor(i));
			if(data.itemIsSelected(i))
				currentIndex = i;
			break;
		}
	}

	if(currentIndex >= 0)
		combo->setCurrentIndex(currentIndex);
}

void UnbluWebPopup::activeChanged(int index)
{
	qWarning() << Q_FUNC_INFO << index;
	if(index < 0)
		return;

	emit selectItem(index, false, false);
}

jobject UnbluWebPopup::createPopupDialog(JNIEnv *env)
{
	return Utils::newObject(env, (jclass)classPopupDialog, popupDialogInitMID, geometry().x(), geometry().y(), geometry().width(), geometry().height());
}

void UnbluWebPopup::loadJavaClasses(JNIEnv *env)
{
	if(!LOAD_CREF(env, PopupDialog, "com/unblu/headlessbrowser/model/server/core/input/PopupDialog")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain class com.unblu.headlessbrowser.model.server.core.input.PopupDialog");
	} else if(!LOAD_MID(env, popupDialogInitMID, classPopupDialog, "<init>", "(IIII)V")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain constructor for class com.unblu.headlessbrowser.model.server.core.input.PopupDialog");
	}
}
