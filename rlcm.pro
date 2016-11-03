QT += widgets
QT += concurrent

TARGET = rlcm

TEMPLATE = app

static
{
    CONFIG += static
    DEFINES += STATIC
}

CONFIG += c++14

QMAKE_CXXFLAGS += -Wall -Wextra -pedantic -pedantic-errors
QMAKE_CXXFLAGS += -Wmain -Wswitch-enum -Wmissing-include-dirs
QMAKE_CXXFLAGS += -Wunreachable-code -Wundef -Wcast-align -Wredundant-decls
QMAKE_CXXFLAGS += -Winit-self -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual
QMAKE_CXXFLAGS += -Wwrite-strings -Wpointer-arith -Wcast-qual -Wlogical-op
QMAKE_CXXFLAGS += -Wuninitialized -fexceptions

LIBS += -lpsapi

SOURCES += src/Bundle.cpp \
    src/Challenge.cpp \
    src/Clock.cpp \
    src/main.cpp \
    src/MainFrame.cpp \
    src/OutputStream.cpp \
    src/Process.cpp \
    src/SpinBox.cpp

HEADERS += src/Bundle.hpp \
    src/Challenge.hpp \
    src/Clock.hpp \
    src/MainFrame.hpp \
    src/OutputStream.hpp \
    src/Process.hpp \
    src/SpinBox.hpp

RESOURCES += data/rsrc.qrc
