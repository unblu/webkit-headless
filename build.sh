#!/usr/bin/env bash

COMPILE_JOBS=4

cd qt
if [[ $OSTYPE = darwin* ]]; then
	./preconfig.sh --qt-config "-I/usr/local/include -L/usr/local/lib" --jobs $COMPILE_JOBS
else
	./preconfig.sh --jobs $COMPILE_JOBS
fi
export SQLITE3SRCDIR=$PWD/qtbase/3rdparty/sqlite/
cd qtwebkit
../qtbase/bin/qmake "WEBKIT_CONFIG -= legacy_web_audio web_audio" $QMAKE_ARGS
make -j$COMPILE_JOBS
