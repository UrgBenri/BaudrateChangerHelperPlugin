TEMPLATE    = lib
CONFIG      += plugin
DESTDIR     = $$PWD/../bin/plugins
CONFIG      += c++11
TARGET      = BaudrateChangerHelperPlugin

!include(BaudrateChangerHelperPlugin.pri) {
        error("Unable to include File Connector Plugin.")
}

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
