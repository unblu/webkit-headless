/*
 * unbluwebpopupcombo.h
 *
 *  Created on: Oct 25, 2012
 *      Author: mike
 */

#ifndef UNBLUWEBPOPUPCOMBO_H_
#define UNBLUWEBPOPUPCOMBO_H_

#include "jqobject.h"
#include "unbluwebpopup.h"

#include <QStandardItemModel>

class UnbluWebPopupCombo : public JQObject
{
	Q_OBJECT

public:
	UnbluWebPopupCombo(UnbluWebPopup *ownerPopup);

	int count() const;

	QVariant itemData(int index, int role) const;
	bool itemIsSeparator(int index) const;
	bool itemIsLabel(int index) const;
	bool itemIsEnabled(int index) const;

	void insertItem(int index, const QString& text);
	void insertLabel(int index, const QString& text);
	void insertSeparator(int index);

	void setMaxCount(int max);

	void setCurrentIndex(int index);
	int currentIndex() const;

	QAbstractItemModel* model() const;
	void setModel(QAbstractItemModel *m);

	jobject createSelectItemData(JNIEnv *env);

	static void loadJavaClasses(JNIEnv *env);

public slots:
	void showPopup();
	void hidePopup();
    virtual bool eventFilter(QObject *sender, QEvent* event);

private:
    static void setSeparator(QAbstractItemModel *model, const QModelIndex& index);
    static bool isSeparator(QAbstractItemModel *model, const QModelIndex& index);

    static void setLabel(QAbstractItemModel *model, const QModelIndex& index);
    static bool isLabel(QAbstractItemModel *model, const QModelIndex& index);

    void setCurrentIndex(const QModelIndex& index);

    int m_nMaxCount;
    int m_nColumn;

    QPersistentModelIndex m_currentIdx;
    QPersistentModelIndex m_rootIdx;
    QAbstractItemModel *m_model;
    UnbluWebPopup *m_ownerPopup;

    static jobject classSelectItemData;
    static jmethodID selectItemDataInitMID;

    static jobject classSelectItem;
    static jmethodID selectItemInitMID;
};

#endif /* UNBLUWEBPOPUPCOMBO_H_ */
