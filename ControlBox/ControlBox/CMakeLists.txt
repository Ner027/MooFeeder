cmake_minimum_required(VERSION 3.14)

project(ControlBox VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/home/andre/Qt/5.15.15/gcc_64/lib/cmake")

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core Quick VirtualKeyboard Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick VirtualKeyboard Widgets)
include_directories(cpp/inc)

set(PROJECT_SOURCES
        cpp/src/main.cpp
        cpp/src/gui/cqmlinterface.cpp
        cpp/src/gui/cguimenu.cpp
        cpp/src/gui/cmainmenu.cpp
        cpp/src/gui/csettingsmenu.cpp
        cpp/src/gui/cloginmenu.cpp
        cpp/src/gui/cmonitormenu.cpp
        cpp/src/gui/cstationmenu.cpp
        cpp/src/entities/ccontrolbox.cpp
        cpp/src/entities/ccalf.cpp
        cpp/src/entities/clog.cpp
        cpp/src/entities/cfeedingstation.cpp
        cpp/src/entities/cserializableobject.cpp
        cpp/src/util/chttpform.cpp
        cpp/src/util/cnetworkmanager.cpp
        cpp/src/util/chttprequest.cpp
        cpp/src/serial/serial.cpp
        cpp/src/util/util.cpp
        cpp/src/LoRa/phy/common/rn2483/rn4283.cpp
        cpp/src/strlib/strlib.cpp
        cpp/src/LoRa/mac/server/cloramac.cpp
        cpp/src/oswrapper/oswrapper.cpp
        cpp/src/ptwrapper/cmutex.cpp
        cpp/src/ptwrapper/cthread.cpp
        cpp/src/ptwrapper/clockguard.cpp
        cpp/inc/util/csafequeue.hpp
        cpp/src/LoRa/network/server/cloranetwork.cpp
        cpp/src/app/cstationmanager.cpp
        cpp/src/util/hexutil.cpp
        qml.qrc
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ControlBox
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ControlBox APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ControlBox SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ControlBox
          ${PROJECT_SOURCES}
                cpp/inc/gui/cqmlinterface.h

        )
    endif()
endif()

target_link_libraries(ControlBox
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ControlBox)
endif()
set_target_properties(ControlBox PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ControlBox
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(ControlBox)
    qt_finalize_executable(ControlBox)
endif()
