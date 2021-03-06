#-------------------------------------------------
#
# Project created by QtCreator 2012-05-24T20:23:24
#
#-------------------------------------------------
CONFIG += VDREAM_LIB_BUILD
#QT    -= gui
#QT    += gui widgets

include (vdream.pri)

TARGET   = $${VDREAM_LIB_NAME}
TEMPLATE = lib
CONFIG  += staticlib
DESTDIR  = ../lib

HEADERS += \
    ../include/common/vapp.h \
    ../include/common/vcommon.h \
    ../include/common/vdebugnew.h \
    ../include/common/vdesign.h \
    ../include/common/verror.h \
    ../include/common/veventhandler.h \
    ../include/common/vexception.h \
    ../include/common/vfactory.h \
    ../include/common/vfile.h \
    ../include/common/vlinklibrary.h \
    ../include/common/vlinux.h \
    ../include/common/vmemoryleak.h \
    ../include/common/vmetaclass.h \
    ../include/common/vobject.h \
    ../include/common/vperformance.h \
    ../include/common/vrwobject.h \
    ../include/common/vsecurelib.h \
    ../include/common/vstring.h \
    ../include/common/vsystem.h \
    ../include/common/vthread.h \
    ../include/common/vtick.h \
    ../include/common/vwindows.h \
    ../include/common/vxml.h \
    ../include/common/vxmldoc.h \
    ../include/graph/vgraph.h \
    ../include/graph/vgraphlink.h \
    ../include/graph/vgraphnode.h \
    ../include/graph/vgraphstream.h \
    ../include/log/vlog.h \
    ../include/log/vlogdbwin32.h \
    ../include/log/vlogfactory.h \
    ../include/log/vlogfile.h \
    ../include/log/vloghttp.h \
    ../include/log/vloglist.h \
    ../include/log/vlognull.h \
    ../include/log/vlogstderr.h \
    ../include/log/vlogstdout.h \
    ../include/log/vlogudp.h \
    ../include/net/vhttpcommon.h \
    ../include/net/vhttpheader.h \
    ../include/net/vhttpproxy.h \
    ../include/net/vhttprequest.h \
    ../include/net/vhttpresponse.h \
    ../include/net/vnet.h \
    ../include/net/vnetclient.h \
    ../include/net/vnetfactory.h \
    ../include/net/vnetserver.h \
    ../include/net/vnetsession.h \
    ../include/net/vsslsession.h \
    ../include/net/vsslserver.h \
    ../include/net/vsslcommon.h \
    ../include/net/vsslclient.h \
    ../include/net/vtcpclient.h \
    ../include/net/vtcpserver.h \
    ../include/net/vtcpsession.h \
    ../include/net/vudpclient.h \
    ../include/net/vudpserver.h \
    ../include/net/vudpsession.h \
    ../include/other/vint.h \
    ../include/other/vmyobject.h

SOURCES += \
    ../include/common/vapp.cpp \
    ../include/common/vcommon.cpp \
    ../include/common/vdebugnew.cpp \
    ../include/common/verror.cpp \
    ../include/common/veventhandler.cpp \
    ../include/common/vexception.cpp \
    ../include/common/vfile.cpp \
    ../include/common/vmemoryleak.cpp \
    ../include/common/vobject.cpp \
    ../include/common/vobjectwidget.cpp \
    ../include/common/vperformance.cpp \
    ../include/common/vrwobject.cpp \
    ../include/common/vsecurelib.cpp \
    ../include/common/vstring.cpp \
    ../include/common/vsystem.cpp \
    ../include/common/vthread.cpp \
    ../include/common/vtick.cpp \
    ../include/common/vxml.cpp \
    ../include/common/vxmldoc.cpp \
    ../include/graph/vgraph.cpp \
    ../include/graph/vgraphlink.cpp \
    ../include/graph/vgraphnode.cpp \
    ../include/graph/vgraphstream.cpp \
    ../include/log/vlog.cpp \
    ../include/log/vlogdbwin32.cpp \
    ../include/log/vlogfactory.cpp \
    ../include/log/vlogfile.cpp \
    ../include/log/vloghttp.cpp \
    ../include/log/vloglist.cpp \
    ../include/log/vlognull.cpp \
    ../include/log/vlogstderr.cpp \
    ../include/log/vlogstdout.cpp \
    ../include/log/vlogudp.cpp \
    ../include/net/vhttpheader.cpp \
    ../include/net/vhttpproxy.cpp \
    ../include/net/vhttprequest.cpp \
    ../include/net/vhttpresponse.cpp \
    ../include/net/vnet.cpp \
    ../include/net/vnetclient.cpp \
    ../include/net/vnetfactory.cpp \
    ../include/net/vnetserver.cpp \
    ../include/net/vsslsession.cpp \
    ../include/net/vsslserver.cpp \
    ../include/net/vsslcommon.cpp \
    ../include/net/vsslclient.cpp \
    ../include/net/vtcpclient.cpp \
    ../include/net/vtcpserver.cpp \
    ../include/net/vtcpsession.cpp \
    ../include/net/vudpclient.cpp \
    ../include/net/vudpserver.cpp \
    ../include/net/vudpsession.cpp \
    ../include/other/vmyobject.cpp
