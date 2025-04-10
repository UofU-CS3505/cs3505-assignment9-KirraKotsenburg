QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Box2D direct inclusion - REPLACE WITH YOUR ACTUAL PATH
BOX2D_DIR = /Users/woosukchang/Box2D

# Add Box2D include path
INCLUDEPATH += $$BOX2D_DIR/include

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    $$files($$BOX2D_DIR/src/collision/*.cpp) \
    $$files($$BOX2D_DIR/src/common/*.cpp) \
    $$files($$BOX2D_DIR/src/dynamics/*.cpp) \
    $$files($$BOX2D_DIR/src/rope/*.cpp)

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
