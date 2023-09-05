QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    customiconprovider.cpp \
    iconview.cpp \
    main.cpp \
    mainwindow.cpp \
    myfilesystemmodel.cpp \
    mylistview.cpp \
    thumbnailtask.cpp

HEADERS += \
    customiconprovider.h \
    iconview.h \
    mainwindow.h \
    myfilesystemmodel.h \
    mylistview.h \
    thumbnailtask.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    icon/player.jpg

RESOURCES += \
    hh.qrc
