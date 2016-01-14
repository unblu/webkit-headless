/*
 * unbluwebpopupcombo.cpp
 *
 *  Created on: Oct 25, 2012
 *      Author: mike
 */

#include "utils.h"
#include "customwebpage.h"
#include "unbluwebpopupcombo.h"

jobject UnbluWebPopupCombo::classSelectItemData;
jmethodID UnbluWebPopupCombo::selectItemDataInitMID;

jobject UnbluWebPopupCombo::classSelectItem;
jmethodID UnbluWebPopupCombo::selectItemInitMID;

UnbluWebPopupCombo::UnbluWebPopupCombo(UnbluWebPopup *ownerPopup) : m_nMaxCount(INT_MAX), m_nColumn(0), m_model(NULL), m_ownerPopup(ownerPopup)
{
	installEventFilter(this);
}

void UnbluWebPopupCombo::showPopup()
{
	qCritical() << Q_FUNC_INFO;
	CustomWebPage *page = qobject_cast<CustomWebPage*>(m_ownerPopup->page());

	m_ownerPopup->m_bPopupVisible = true;
	page->showPopupDialog(m_ownerPopup);
}

void UnbluWebPopupCombo::hidePopup()
{
	qWarning() << Q_FUNC_INFO;

	if(!m_ownerPopup->m_bPopupVisible)
		return;

	m_ownerPopup->m_bPopupVisible = false;
	emit m_ownerPopup->didHide();
	m_ownerPopup->destroyPopup();
}

bool UnbluWebPopupCombo::eventFilter(QObject *sender, QEvent *event)
{
	Q_UNUSED(sender);

	if(event->type() == QEvent::Show && !m_ownerPopup->m_bPopupVisible)
		showPopup();
	if(event->type() == QEvent::Hide && m_ownerPopup->m_bPopupVisible)
		hidePopup();

	return false;
}

QAbstractItemModel* UnbluWebPopupCombo::model() const
{
	if(m_model == NULL) {
		UnbluWebPopupCombo *that = const_cast<UnbluWebPopupCombo*>(this);
		that->setModel(new QStandardItemModel(0, 1, that));
	}
	return m_model;
}

void UnbluWebPopupCombo::setModel(QAbstractItemModel *m)
{
	if(m_model) {
		if(m_model->QObject::parent() == this)
			delete m_model;
	}
	m_model = m;
}

void UnbluWebPopupCombo::setMaxCount(int max)
{
	if(max < 0) {
		qWarning("UnbluWebpopupCombo: Invalid count (%d) must be >= 0", max);
		return;
	}

	if(max < count())
		m_model->removeRows(max, count() - max, m_rootIdx);

	m_nMaxCount = max;
}

int UnbluWebPopupCombo::count() const
{
	return m_model->rowCount(m_rootIdx);
}

QVariant UnbluWebPopupCombo::itemData(int index, int role) const
{
	QModelIndex mi = m_model->index(index, m_nColumn, m_rootIdx);
	return mi.isValid() ? m_model->data(mi, role) : QVariant();
}

bool UnbluWebPopupCombo::itemIsSeparator(int index) const
{
	return isSeparator(m_model, m_model->index(index, 0, m_rootIdx));
}

bool UnbluWebPopupCombo::itemIsLabel(int index) const
{
	return isLabel(m_model, m_model->index(index, 0, m_rootIdx));
}

bool UnbluWebPopupCombo::itemIsEnabled(int index) const
{
	return qobject_cast<QStandardItemModel*>(m_model)->item(index)->isEnabled();
}

void UnbluWebPopupCombo::insertItem(int index, const QString& text)
{
	int itemCount = count();

	index = qBound(0, index, itemCount);
	if(index >= m_nMaxCount)
		return;

	QStandardItem *item = new QStandardItem(text);
	QStandardItemModel *m = qobject_cast<QStandardItemModel*>(m_model);

	m->insertRow(index, item);
	itemCount++;

	if(itemCount > m_nMaxCount)
		m_model->removeRows(itemCount - 1, itemCount - m_nMaxCount, m_rootIdx);
}

void UnbluWebPopupCombo::insertLabel(int index, const QString& text)
{
	int itemCount = count();

	index = qBound(0, index, itemCount);
	if(index >= m_nMaxCount)
		return;

	insertItem(index, text);
	setLabel(m_model, m_model->index(index, 0, m_rootIdx));
}

void UnbluWebPopupCombo::insertSeparator(int index)
{
	int itemCount = count();

	index = qBound(0, index, itemCount);
	if(index >= m_nMaxCount)
		return;

	insertItem(index, QString());
	setSeparator(m_model, m_model->index(index, 0, m_rootIdx));
}

void UnbluWebPopupCombo::setCurrentIndex(int index)
{
	QModelIndex mi = m_model->index(index, m_nColumn, m_rootIdx);
	setCurrentIndex(mi);
}

int UnbluWebPopupCombo::currentIndex() const
{
	return m_currentIdx.row();
}

void UnbluWebPopupCombo::setCurrentIndex(const QModelIndex& mi)
{
	QModelIndex normalized;

	if(mi.column() != m_nColumn)
		normalized = m_model->index(mi.row(), m_nColumn, mi.parent());
	if(!normalized.isValid())
		normalized = mi;

	bool indexChanged = (normalized != m_currentIdx);
	if(indexChanged)
		m_currentIdx = QPersistentModelIndex(normalized);
}

void UnbluWebPopupCombo::setSeparator(QAbstractItemModel *model, const QModelIndex& index)
{
	model->setData(index, QString::fromLatin1("separator"), Qt::AccessibleDescriptionRole);
	if(QStandardItemModel *m = qobject_cast<QStandardItemModel*>(model)) {
        if (QStandardItem *item = m->itemFromIndex(index))
            item->setFlags(item->flags() & ~(Qt::ItemIsSelectable|Qt::ItemIsEnabled));
	}
}

void UnbluWebPopupCombo::setLabel(QAbstractItemModel *model, const QModelIndex& index)
{
	model->setData(index, QString::fromLatin1("label"), Qt::AccessibleDescriptionRole);
	if(QStandardItemModel *m = qobject_cast<QStandardItemModel*>(model)) {
		if(QStandardItem *item = m->itemFromIndex(index))
			item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
	}
}

bool UnbluWebPopupCombo::isSeparator(QAbstractItemModel *model, const QModelIndex& index)
{
	QVariant data = model->data(index, Qt::AccessibleDescriptionRole);
	if(data.type() == QVariant::String && data.toString() == "seperator") {
		if(QStandardItemModel *m = qobject_cast<QStandardItemModel*>(model)) {
	        if (QStandardItem *item = m->itemFromIndex(index))
	        	return !(item->flags()&(Qt::ItemIsSelectable|Qt::ItemIsEnabled));
		}
	}
	return false;
}

bool UnbluWebPopupCombo::isLabel(QAbstractItemModel *model, const QModelIndex& index)
{
	QVariant data = model->data(index, Qt::AccessibleDescriptionRole);
	if(data.type() == QVariant::String && data.toString() == "label") {
		if(QStandardItemModel *m = qobject_cast<QStandardItemModel*>(model)) {
	        if (QStandardItem *item = m->itemFromIndex(index))
	        	return !(item->flags()&Qt::ItemIsEnabled);
		}
	}
	return false;
}

void UnbluWebPopupCombo::loadJavaClasses(JNIEnv *env)
{
	if(!LOAD_CREF(env, SelectItemData, "com/unblu/headlessbrowser/model/server/core/input/SelectItemData")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain class com.unblu.headlessbrowser.model.server.core.input.SelectItemData");
	} else if(!LOAD_MID(env, selectItemDataInitMID, classSelectItemData, "<init>", "()V")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain constructor for class com.unblu.headlessbrowser.model.server.core.input.SelectItemData");
	} else if(!LOAD_CREF(env, SelectItem, "com/unblu/headlessbrowser/model/server/core/input/SelectItem")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain class com.unblu.headlessbrowser.model.server.core.input.SelectItem");
	} else if(!LOAD_MID(env, selectItemInitMID, classSelectItem, "<init>", "()V")) {
		Utils::throwByName(env, EUnsatisfiedLink, "Can't obtain constructor for class com.unblu.headlessbrowser.model.server.core.input.SelectItem");
	}
}

jobject UnbluWebPopupCombo::createSelectItemData(JNIEnv *env)
{
	jobject jselect = Utils::newObject(env, (jclass)classSelectItemData, selectItemDataInitMID);

	qWarning() << Q_FUNC_INFO << count();

	for(int i = 0;i < count();i++) {
		jobject jitem = Utils::newObject(env, (jclass)classSelectItem, selectItemInitMID);

		Utils::invokeMethod(env, jitem, "setItemType", "(I)V", (jint)(itemIsSeparator(i) ? QWebSelectData::Separator : (itemIsLabel(i) ? QWebSelectData::Group : QWebSelectData::Option)));
		Utils::invokeMethod(env, jitem, "setItemText", "(Ljava/lang/String;)V", Utils::getJString(env, itemData(i, Qt::DisplayRole).toString()));
		Utils::invokeMethod(env, jitem, "setItemToolTip", "(Ljava/lang/String;)V", Utils::getJString(env, itemData(i, Qt::ToolTipRole).toString()));
		Utils::invokeMethod(env, jitem, "setItemSelected", "(Z)V", (jboolean)(currentIndex() == i));
		Utils::invokeMethod(env, jitem, "setItemEnabled", "(Z)V", (jboolean)itemIsEnabled(i));

		QBrush fgcolor = itemData(i, Qt::ForegroundRole).value<QBrush>();
		Utils::invokeMethod(env, jitem, "setItemForegroundColor", "(Ljava/lang/String;)V", Utils::getJString(env, fgcolor.color().name()));

		QBrush bgcolor = itemData(i, Qt::BackgroundRole).value<QBrush>();
		Utils::invokeMethod(env, jitem, "setItemBackgroundColor", "(Ljava/lang/String;)V", Utils::getJString(env, bgcolor.color().name()));

		Utils::invokeMethod(env, jselect, "insertItem", "(ILcom/unblu/headlessbrowser/model/server/core/input/SelectItem;)V", i, jitem);
	}

	return jselect;
}
