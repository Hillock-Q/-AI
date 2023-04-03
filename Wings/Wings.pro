QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AttrViewMgr.cpp \
    BtnCtrl.cpp \
    CompileMgr.cpp \
    Delegate.cpp \
    DesignMgr.cpp \
    DialogAbout.cpp \
    DialogAiParamsView.cpp \
    DialogGen.cpp \
    DialogInputConfig.cpp \
    DialogMessageInfo.cpp \
    DialogNew.cpp \
    DialogNumberEdit.cpp \
    DialogOutputConfig.cpp \
    DialogParamEdit.cpp \
    DialogTextEdit.cpp \
    DirFileMgr.cpp \
    GraphicsArrowItem.cpp \
    GraphicsEllipseItem.cpp \
    GraphicsRectItem.cpp \
    GraphicsScene.cpp \
    GraphicsView.cpp \
    InUseCheck.cpp \
    ItemInfo.cpp \
    ListViewMgr.cpp \
    MenuCustomList.cpp \
    MenuItem.cpp \
    MenuProjectList.cpp \
    MenuText.cpp \
    NetViewMgr.cpp \
    ProjectMgr.cpp \
    RunMgr.cpp \
    SystemTray.cpp \
    WgAttrList.cpp \
    WgCtrlList.cpp \
    WgFloatEditor.cpp \
    WgFloatListEditor.cpp \
    WgIntEditor.cpp \
    WgIntListEditor.cpp \
    WgIntTableEditor.cpp \
    WgStrSelector.cpp \
    WgTextEditor.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    AttrViewMgr.h \
    BtnCtrl.h \
    CompileMgr.h \
    Delegate.h \
    DesignMgr.h \
    DialogAbout.h \
    DialogAiParamsView.h \
    DialogGen.h \
    DialogInputConfig.h \
    DialogMessageInfo.h \
    DialogNew.h \
    DialogNumberEdit.h \
    DialogOutputConfig.h \
    DialogParamEdit.h \
    DialogTextEdit.h \
    DirFileMgr.h \
    GraphicsArrowItem.h \
    GraphicsEllipseItem.h \
    GraphicsRectItem.h \
    GraphicsScene.h \
    GraphicsView.h \
    InUseCheck.h \
    ItemInfo.h \
    ListViewMgr.h \
    MenuCustomList.h \
    MenuItem.h \
    MenuProjectList.h \
    MenuText.h \
    NetAiParamsDefine.h \
    NetMark.h \
    NetViewMgr.h \
    ProjectInfo.h \
    ProjectMgr.h \
    RunMgr.h \
    SystemTray.h \
    WgAttrList.h \
    WgCtrlList.h \
    WgFloatEditor.h \
    WgFloatListEditor.h \
    WgIntEditor.h \
    WgIntListEditor.h \
    WgIntTableEditor.h \
    WgStrSelector.h \
    WgTextEditor.h \
    mainwindow.h

FORMS += \
    DialogAbout.ui \
    DialogAiParamsView.ui \
    DialogGen.ui \
    DialogInputConfig.ui \
    DialogMessageInfo.ui \
    DialogNew.ui \
    DialogNumberEdit.ui \
    DialogOutputConfig.ui \
    DialogParamEdit.ui \
    DialogTextEdit.ui \
    InUseCheck.ui \
    WgAttrList.ui \
    WgCtrlList.ui \
    WgFloatEditor.ui \
    WgFloatListEditor.ui \
    WgIntEditor.ui \
    WgIntListEditor.ui \
    WgIntTableEditor.ui \
    WgStrSelector.ui \
    WgTextEditor.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Images.qrc \
    Images.qrc
