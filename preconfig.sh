#!/usr/bin/env bash
pushd qtbase

COMPILE_JOBS=4

QT_CFG=''
QT_CFG+=' -opensource'          # Use the open-source license
QT_CFG+=' -confirm-license'     # Silently acknowledge the license confirmation
QT_CFG+=' -v'                   # Makes it easier to see what header dependencies are missing
QT_CFG+=' -static'

if [[ $OSTYPE = darwin* ]]; then
	QT_CFG+=' -no-c++11'            # Build fails on mac right now with C++11
else
	QT_CFG+=' -icu'
    QT_CFG+=' -system-freetype' # Freetype for text rendering
    QT_CFG+=' -fontconfig'      # Fontconfig for better font matching
fi

QT_CFG+=' -release'             # Build only for release (no debugging support)
QT_CFG+=' -nomake examples'     # Don't build any examples
QT_CFG+=' -nomake tools'        # Don't built the tools

# Unnecessary Qt modules
QT_CFG+=' -no-opengl'
QT_CFG+=' -no-openvg'
QT_CFG+=' -no-eglfs'
QT_CFG+=' -no-opengl'

# Unnecessary Qt features
QT_CFG+=' -D QT_NO_GRAPHICSVIEW'
QT_CFG+=' -D QT_NO_GRAPHICSEFFECT'
QT_CFG+=' -no-qml-debug'

# Unix
QT_CFG+=' -no-dbus'             # Disable D-Bus feature
QT_CFG+=' -no-gtkstyle'         # Disable theming integration with Gtk+
QT_CFG+=' -no-cups'             # Disable CUPs support
QT_CFG+=' -no-sm'
QT_CFG+=' -no-xinerama'
QT_CFG+=' -no-xkb'
QT_CFG+=' -no-kms'
QT_CFG+=' -no-linuxfb'
QT_CFG+=' -no-directfb'
QT_CFG+=' -no-mtdev'
QT_CFG+=' -no-libudev'
QT_CFG+=' -no-egl'
QT_CFG+=' -no-evdev'
QT_CFG+=' -no-gtkstyle'
QT_CFG+=' -no-alsa'
QT_CFG+=' -no-pulseaudio'
QT_CFG+=' -no-feature-PRINTPREVIEWWIDGET'

# Use the bundled libraries, vs system-installed
QT_CFG+=' -qt-libjpeg'
QT_CFG+=' -qt-libpng'
QT_CFG+=' -qt-zlib'

# Explicitly compile with SSL support, so build will fail if headers are missing
QT_CFG+=' -openssl'

# Useless styles
QT_CFG+=' -D QT_NO_STYLESHEET'
QT_CFG+=' -D QT_NO_STYLE_CDE'
QT_CFG+=' -D QT_NO_STYLE_CLEANLOOKS'
QT_CFG+=' -D QT_NO_STYLE_MOTIF'
QT_CFG+=' -D QT_NO_STYLE_PLASTIQUE'

SILENT=''

until [ -z "$1" ]; do
    case $1 in
        "--qt-config")
            shift
            QT_CFG+=" $1"
            shift;;
        "--jobs")
            shift
            COMPILE_JOBS=$1
            shift;;
        "--silent")
            SILENT='-s'
            QT_CFG+=" -silent"
            shift;;
        "--help")
            echo "Usage: $0 [--qt-config CONFIG] [--jobs NUM]"
            echo
            echo "  --qt-config CONFIG          Specify extra config options to be used when configuring Qt"
            echo "  --jobs NUM                  How many parallel compile jobs to use. Defaults to 4."
            echo
            exit 0
            ;;
        *)
            echo "Unrecognised option: $1"
            exit 1;;
    esac
done


# For parallelizing the bootstrapping process, e.g. qmake and friends.
export MAKEFLAGS=-j$COMPILE_JOBS

if [ -z "$SILENT" ]; then
    ./configure -prefix $PWD $QT_CFG
else
    echo "Setting up Qt. Please wait..."
    ./configure -prefix $PWD $QT_CFG &> /dev/null
fi

echo
echo "Building Qt and WebKit. Please wait..."
make -j$COMPILE_JOBS $SILENT
