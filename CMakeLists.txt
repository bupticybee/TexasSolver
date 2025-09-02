cmake_minimum_required(VERSION 3.14)

project(TexasSolver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "E:\\Qt\\6.9.2\\mingw_64\\")

# Automatically run Qt's MOC, UIC, and RCC pre-processors
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt5 and its required components
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Find OpenMP for multi-threading support, matching the .pro file's configuration
find_package(OpenMP)

# --- Source File Definitions ---

# Core library sources (shared between GUI and CLI)
set(CORE_SOURCES
    src/Deck.cpp
    src/Card.cpp
    src/GameTree.cpp
    src/library.cpp
    src/compairer/Dic5Compairer.cpp
    src/experimental/TCfrSolver.cpp
    src/nodes/ActionNode.cpp
    src/nodes/ChanceNode.cpp
    src/nodes/GameActions.cpp
    src/nodes/GameTreeNode.cpp
    src/nodes/ShowdownNode.cpp
    src/nodes/TerminalNode.cpp
    src/pybind/bindSolver.cpp
    src/ranges/PrivateCards.cpp
    src/ranges/PrivateCardsManager.cpp
    src/ranges/RiverCombs.cpp
    src/ranges/RiverRangeManager.cpp
    src/runtime/PokerSolver.cpp
    src/solver/BestResponse.cpp
    src/solver/CfrSolver.cpp
    src/solver/PCfrSolver.cpp
    src/solver/Solver.cpp
    src/tools/GameTreeBuildingSettings.cpp
    src/tools/lookup8.cpp
    src/tools/PrivateRangeConverter.cpp
    src/tools/progressbar.cpp
    src/tools/Rule.cpp
    src/tools/StreetSetting.cpp
    src/tools/utils.cpp
    src/trainable/CfrPlusTrainable.cpp
    src/trainable/DiscountedCfrTrainable.cpp
    src/trainable/DiscountedCfrTrainableHF.cpp
    src/trainable/DiscountedCfrTrainableSF.cpp
    src/trainable/Trainable.cpp
)

# GUI-specific sources
set(GUI_SOURCES
    main.cpp
    mainwindow.cpp
    src/runtime/qsolverjob.cpp
    qstextedit.cpp
    strategyexplorer.cpp
    qstreeview.cpp
    src/ui/treeitem.cpp
    src/ui/treemodel.cpp
    htmltableview.cpp
    src/ui/worditemdelegate.cpp
    src/ui/tablestrategymodel.cpp
    src/ui/strategyitemdelegate.cpp
    src/ui/detailwindowsetting.cpp
    src/ui/detailviewermodel.cpp
    src/ui/detailitemdelegate.cpp
    src/ui/roughstrategyviewermodel.cpp
    src/ui/roughstrategyitemdelegate.cpp
    src/ui/droptextedit.cpp
    src/ui/htmltablerangeview.cpp
    rangeselector.cpp
    src/ui/rangeselectortablemodel.cpp
    src/ui/rangeselectortabledelegate.cpp
    boardselector.cpp
    src/ui/boardselectortablemodel.cpp
    src/ui/boardselectortabledelegate.cpp
    settingeditor.cpp
)

# Define the list of header files (useful for IDEs)
set(HEADERS
    include/tools/half-1-12-0.h
    include/trainable/DiscountedCfrTrainableHF.h
    include/trainable/DiscountedCfrTrainableSF.h
    mainwindow.h
    include/Card.h
    include/GameTree.h
    include/Deck.h
    include/json.hpp
    include/library.h
    include/solver/PCfrSolver.h
    include/solver/Solver.h
    include/solver/BestResponse.h
    include/solver/CfrSolver.h
    include/tools/argparse.hpp
    include/tools/CommandLineTool.h
    include/tools/utils.h
    include/tools/GameTreeBuildingSettings.h
    include/tools/Rule.h
    include/tools/StreetSetting.h
    include/tools/lookup8.h
    include/tools/PrivateRangeConverter.h
    include/tools/progressbar.h
    include/runtime/PokerSolver.h
    include/trainable/CfrPlusTrainable.h
    include/trainable/DiscountedCfrTrainable.h
    include/trainable/Trainable.h
    include/compairer/Compairer.h
    include/compairer/Dic5Compairer.h
    include/experimental/TCfrSolver.h
    include/nodes/ActionNode.h
    include/nodes/ChanceNode.h
    include/nodes/GameActions.h
    include/nodes/GameTreeNode.h
    include/nodes/ShowdownNode.h
    include/nodes/TerminalNode.h
    include/ranges/PrivateCards.h
    include/ranges/PrivateCardsManager.h
    include/ranges/RiverCombs.h
    include/ranges/RiverRangeManager.h
    include/tools/tinyformat.h
    include/tools/qdebugstream.h
    include/runtime/qsolverjob.h
    qstextedit.h
    strategyexplorer.h
    qstreeview.h
    include/ui/treeitem.h
    include/ui/treemodel.h
    htmltableview.h
    include/ui/worditemdelegate.h
    include/ui/tablestrategymodel.h
    include/ui/strategyitemdelegate.h
    include/ui/detailwindowsetting.h
    include/ui/detailviewermodel.h
    include/ui/detailitemdelegate.h
    include/ui/roughstrategyviewermodel.h
    include/ui/roughstrategyitemdelegate.h
    include/ui/droptextedit.h
    include/ui/htmltablerangeview.h
    rangeselector.h
    include/ui/rangeselectortablemodel.h
    include/ui/rangeselectortabledelegate.h
    boardselector.h
    include/ui/boardselectortablemodel.h
    include/ui/boardselectortabledelegate.h
    settingeditor.h
)

# Define UI forms to be processed by UIC
set(FORMS
    mainwindow.ui
    strategyexplorer.ui
    rangeselector.ui
    boardselector.ui
    settingeditor.ui
)

# Define Qt resource files to be processed by RCC
set(GUI_RESOURCES
    translations.qrc
)
set(COMMON_RESOURCES
    compairer.qrc
)

# Handle translation files (.ts -> .qm)
set(TS_FILES
    lang_cn.ts
    lang_en.ts
)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

# --- GUI Application Target ---
add_executable(TexasSolverGui WIN32 MACOSX_BUNDLE
    ${CORE_SOURCES}
    ${GUI_SOURCES}
    ${HEADERS}
    ${FORMS}
    ${GUI_RESOURCES}
    ${COMMON_RESOURCES}
)

# Add include directories for headers and generated files
target_include_directories(TexasSolverGui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Link to required libraries
target_link_libraries(TexasSolverGui PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)

# Add compile definitions from the .pro file
target_compile_definitions(TexasSolverGui PRIVATE QT_DEPRECATED_WARNINGS)

# Set optimization level for release builds
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

# Platform-specific settings for OpenMP and icons
if(OpenMP_FOUND)
    target_link_libraries(TexasSolverGui PRIVATE OpenMP::OpenMP_CXX)
endif()

if(WIN32)
    # Set the application icon for Windows by creating a resource file
    set(RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/TexasSolverGui.rc)
    file(WRITE ${RC_FILE} "IDI_ICON1 ICON \"imgs/texassolver_logo.ico\"\n")
    target_sources(TexasSolverGui PRIVATE ${RC_FILE})
elseif(APPLE)
    # Set the application icon for macOS
    set_target_properties(TexasSolverGui PROPERTIES
        MACOSX_BUNDLE_ICON_FILE imgs/texassolver_logo.icns
    )
endif()

# --- Command-Line Tool Target ---
set(CLI_SOURCES
    src/console.cpp
    src/tools/CommandLineTool.cpp
)

add_executable(TexasSolverCli
    ${CORE_SOURCES}
    ${CLI_SOURCES}
    ${COMMON_RESOURCES}
)

# Add include directories
target_include_directories(TexasSolverCli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Link to required libraries (note: only Qt Core is needed for the CLI)
target_link_libraries(TexasSolverCli PRIVATE Qt5::Core)

# Link OpenMP if found
if(OpenMP_FOUND)
    target_link_libraries(TexasSolverCli PRIVATE OpenMP::OpenMP_CXX)
endif()

# Add compile definitions
target_compile_definitions(TexasSolverCli PRIVATE QT_DEPRECATED_WARNINGS)