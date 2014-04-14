HEADERS += ./Interpreter.h \
    ./Commands.h \
    ./Constants.h \
    ./Task.h \
    ./Exceptions.h \
    ./TaskWindow.h \
    ./TutorialWidget.h \
    ./AboutWindow.h \
    ./SettingsWindow.h \
    ./InputHighlighter.h \
    ./HotKeyManager.h \
    ./InputWindow.h \
    ./HotKeyThread.h \
    ./SystemTrayWidget.h \
    ./SlidingStackedWidget.h \
    ./TaskEntry.h \
    ./Tasuke.h \
    ./Storage.h \
    ./TooltipWidget.h \
    ./SubheadingEntry.h
SOURCES += ./AboutWindow.cpp \
    ./HotKeyManager.cpp \
    ./InputHighlighter.cpp \
    ./Interpreter.cpp \
    ./Commands.cpp \
    ./HotKeyThread.cpp \
    ./InputWindow.cpp \
    ./SettingsWindow.cpp \
    ./SlidingStackedWidget.cpp \
    ./SystemTrayWidget.cpp \
    ./Task.cpp \
    ./Exceptions.cpp \
    ./main.cpp \
    ./TaskEntry.cpp \
    ./TaskWindow.cpp \
    ./Tasuke.cpp \
    ./Storage.cpp \
    ./TutorialWidget.cpp \
    ./TooltipWidget.cpp \
    ./SubheadingEntry.cpp
FORMS += ./TaskWindow.ui \
    ./InputWindow.ui \
    ./AboutWindow.ui \
    ./TaskEntry.ui \
    ./TutorialWidget.ui \
    ./SettingsWindow.ui \
    ./TooltipWidget.ui \
    ./SubheadingEntry.ui
RESOURCES += ./Resources.qrc

macx {
    HEADERS += ./MacWindowActivator.h
    OBJECTIVE_SOURCES += ./MacWindowActivator.mm
}
