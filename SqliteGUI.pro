HEADERS += \
    mainwindow.h \
    highlighter.h \
    querywidget.h \
    viewdesignwidget.h \
    tabledesignwidget.h \
    tabledesignmodel.h \
    stackedwidget.h \
    sqlscanner.h \
    sqlparse.h \
    showtablewidget.h \
    delegate.h \
    dbtree.h \
    showview.h \
    indexwidget.h \
    triggerwidget.h

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    highlighter.cpp \
    querywidget.cpp \
    viewdesignwidget.cpp \
    tabledesignwidget.cpp \
    stackedwidget.cpp \
    sqlscanner.cpp \
    sqlparse.cpp \
    showtablewidget.cpp \
    tabledesignmodel.cpp \
    removedcode.cpp \
    delegate.cpp \
    dbtree.cpp \
    showview.cpp \
    indexwidget.cpp \
    triggerwidget.cpp

RESOURCES += \
    resource.qrc

QT += sql

OTHER_FILES += \
    keyword.txt \
    SqliteGUI.rc

RC_FILE = SqliteGUI.rc
