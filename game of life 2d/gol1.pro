QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    grid2d.cpp \
    main.cpp \
    mainwindow.cpp \
    animator.cpp

HEADERS += \
    grid2d.h \
    mainwindow.h \
    animator.h

LIBS += -LC:\Qt\5.15.2\mingw81_64\lib\libQt5OpenGL.a -lopengl32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
