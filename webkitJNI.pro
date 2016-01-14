TEMPLATE = lib
CONFIG += shared thread
QT += network webkitwidgets

unix {
	!macx {
		QTPLUGIN += qxcb
		INCLUDEPATH += $(JAVA_HOME)/include/ $(JAVA_HOME)/include/linux/
	} else {
		QTPLUGIN += qcocoa
		CONFIG -= app_bundle
		INCLUDEPATH += $$system(/usr/libexec/java_home)/include/ $$system(/usr/libexec/java_home)/include/darwin/
	}
}
INCLUDEPATH += $(PWD)/header

DEFINES += QT_STATICPLUGIN

HEADERS = header/core.h \
		  header/webkit.h \
		  header/window.h \
		  header/window_private.h \
		  header/jvmcontext.h \
		  header/jqobject.h \
		  header/event.h \
		  header/utils.h \
		  header/eventsender.h \
		  header/unbluwebkitplugin.h \
		  header/unbluwebpopup.h \
		  header/customwebpage.h \
		  header/unbluwebpopupcombo.h \
		  header/downloadmanager.h \
		  header/cookiejar.h \
		  header/networkaccessmanager.h \
		  header/webkitjs.h
		  
SOURCES = source/core.cpp \
		  source/webkit.cpp \
		  source/window.cpp \
		  source/window_private.cpp \
		  source/jvmcontext.cpp \
		  source/jqobject.cpp \
		  source/event.cpp \
		  source/utils.cpp \
		  source/eventsender.cpp \
		  source/unbluwebkitplugin.cpp \
		  source/unbluwebpopup.cpp \
		  source/customwebpage.cpp \
		  source/unbluwebpopupcombo.cpp \
		  source/downloadmanager.cpp \
		  source/cookiejar.cpp \
		  source/networkaccessmanager.cpp \
		  source/webkitjs.cpp

RESOURCES = resource/certs/ca_certificates.qrc \
			resource/pdf.js/pdf_js.qrc

QTPLUGIN += qico \
			qcur		  
		   