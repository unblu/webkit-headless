/*
 * jqobject.cpp
 *
 *  Created on: Oct 7, 2012
 *      Author: mike
 */

#include "jqobject.h"

JQObject::JQObject(QObject *parent) : QObject(parent)
{
	m_pJVMContext = JavaVMContext::instance();
}

JQObject::~JQObject()
{
	m_pJVMContext = NULL;
}
