#-------------------------------------------------
#
# Project created by QtCreator 2021-09-18T11:22:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TexasSolverGui
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


macx: {
QMAKE_CXXFLAGS += -Xpreprocessor -fopenmp -lomp -I/usr/local/include
}

macx: {
QMAKE_LFLAGS += -lomp
}

macx: {
LIBS += -L /usr/local/lib /usr/local/lib/libomp.dylib
}

win32: {
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp
}

win64: {
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp
}

QMAKE_LFLAGS += -v

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/Deck.cpp \
    src/Card.cpp \
    src/console.cpp \
    src/GameTree.cpp \
    src/library.cpp \
    src/compairer/Dic5Compairer.cpp \
    src/experimental/TCfrSolver.cpp \
    src/nodes/ActionNode.cpp \
    src/nodes/ChanceNode.cpp \
    src/nodes/GameActions.cpp \
    src/nodes/GameTreeNode.cpp \
    src/nodes/ShowdownNode.cpp \
    src/nodes/TerminalNode.cpp \
    src/pybind/bindSolver.cpp \
    src/ranges/PrivateCards.cpp \
    src/ranges/PrivateCardsManager.cpp \
    src/ranges/RiverCombs.cpp \
    src/ranges/RiverRangeManager.cpp \
    src/runtime/PokerSolver.cpp \
    src/solver/BestResponse.cpp \
    src/solver/CfrSolver.cpp \
    src/solver/PCfrSolver.cpp \
    src/solver/Solver.cpp \
    src/tools/CommandLineTool.cpp \
    src/tools/GameTreeBuildingSettings.cpp \
    src/tools/lookup8.cpp \
    src/tools/PrivateRangeConverter.cpp \
    src/tools/progressbar.cpp \
    src/tools/Rule.cpp \
    src/tools/StreetSetting.cpp \
    src/tools/utils.cpp \
    src/trainable/CfrPlusTrainable.cpp \
    src/trainable/DiscountedCfrTrainable.cpp \
    src/trainable/Trainable.cpp

HEADERS += \
    mainwindow.h \
    include/Card.h \
    include/GameTree.h \
    include/Deck.h \
    include/json.hpp \
    include/library.h \
    include/solver/PCfrSolver.h \
    include/solver/Solver.h \
    include/solver/BestResponse.h \
    include/solver/CfrSolver.h \
    include/tools/argparse.hpp \
    include/tools/CommandLineTool.h \
    include/tools/utils.h \
    include/tools/GameTreeBuildingSettings.h \
    include/tools/Rule.h \
    include/tools/StreetSetting.h \
    include/tools/lookup8.h \
    include/tools/PrivateRangeConverter.h \
    include/tools/progressbar.h \
    include/runtime/PokerSolver.h \
    include/trainable/CfrPlusTrainable.h \
    include/trainable/DiscountedCfrTrainable.h \
    include/trainable/Trainable.h \
    include/compairer/Compairer.h \
    include/compairer/Dic5Compairer.h \
    include/experimental/TCfrSolver.h \
    include/nodes/ActionNode.h \
    include/nodes/ChanceNode.h \
    include/nodes/GameActions.h \
    include/nodes/GameTreeNode.h \
    include/nodes/ShowdownNode.h \
    include/nodes/TerminalNode.h \
    include/ranges/PrivateCards.h \
    include/ranges/PrivateCardsManager.h \
    include/ranges/RiverCombs.h \
    include/ranges/RiverRangeManager.h \
    include/tools/tinyformat.h

FORMS += \
        mainwindow.ui
