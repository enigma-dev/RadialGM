###################################################################################
##                                                                               ##
##  Copyright (C) 2024 Saif Kandil (k0T0z)                                       ##
##                                                                               ##
##  This file is a part of the ENIGMA Development Environment.                   ##
##                                                                               ##
##                                                                               ##
##  ENIGMA is free software: you can redistribute it and/or modify it under the  ##
##  terms of the GNU General Public License as published by the Free Software    ##
##  Foundation, version 3 of the license or any later version.                   ##
##                                                                               ##
##  This application and its source code is distributed AS-IS, WITHOUT ANY       ##
##  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS    ##
##  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more        ##
##  details.                                                                     ##
##                                                                               ##
##  You should have recieved a copy of the GNU General Public License along      ##
##  with this code. If not, see <http://www.gnu.org/licenses/>                   ##
##                                                                               ##
##  ENIGMA is an environment designed to create games and other programs with a  ##
##  high-level, fully compilable language. Developers of ENIGMA or anything      ##
##  associated with ENIGMA are in no way responsible for its users or            ##
##  applications created by its users, or damages caused by the environment      ##
##  or programs made in the environment.                                         ##
##                                                                               ##
###################################################################################

set(RGM_TESTS "RGM-Tests" CACHE STRING "RGM Tests")

project(${RGM_TESTS} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS "${RGM_ROOTDIR}/" "${RGM_ROOTDIR}/Dialogs" "${RGM_ROOTDIR}/Editors")

file(GLOB RGM_TESTS_SRC_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/tests_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MainWindowTests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Editors/VisualShaderEditorTests.cpp
)

file(GLOB RGM_TESTS_H_FILES 
    ${RGM_ROOTDIR}/MainWindow.h
    ${RGM_ROOTDIR}/Models/MessageModel.h
    ${RGM_ROOTDIR}/Models/ProtoModel.h
    ${RGM_ROOTDIR}/Models/TreeModel.h
    ${RGM_ROOTDIR}/Models/PrimitiveModel.h
    ${RGM_ROOTDIR}/Models/RepeatedMessageModel.h
    ${RGM_ROOTDIR}/Models/EventTypesListSortFilterProxyModel.h
    ${RGM_ROOTDIR}/Models/ResourceModelMap.h
    ${RGM_ROOTDIR}/Models/EventTypesListModel.h
    ${RGM_ROOTDIR}/Models/ImmediateMapper.h
    ${RGM_ROOTDIR}/Models/RepeatedModel.h
    ${RGM_ROOTDIR}/Models/EventsListModel.h
    ${RGM_ROOTDIR}/Models/ModelMapper.h
    ${RGM_ROOTDIR}/Models/RepeatedSortFilterProxyModel.h
    ${RGM_ROOTDIR}/Models/TreeSortFilterProxyModel.h
    ${RGM_ROOTDIR}/Components/RecentFiles.h
    ${RGM_ROOTDIR}/Components/QMenuView_p.h
    ${RGM_ROOTDIR}/Components/Utility.h
    ${RGM_ROOTDIR}/Components/QMenuView.h
    ${RGM_ROOTDIR}/Components/Logger.h
    ${RGM_ROOTDIR}/Components/ArtManager.h
    ${RGM_ROOTDIR}/Editors/ObjectEditor.h
    ${RGM_ROOTDIR}/Editors/PathEditor.h
    ${RGM_ROOTDIR}/Editors/ScriptEditor.h
    ${RGM_ROOTDIR}/Editors/CodeEditor.h
    ${RGM_ROOTDIR}/Editors/SoundEditor.h
    ${RGM_ROOTDIR}/Editors/IncludeEditor.h
    ${RGM_ROOTDIR}/Editors/InformationEditor.h
    ${RGM_ROOTDIR}/Editors/TimelineEditor.h
    ${RGM_ROOTDIR}/Editors/SettingsEditor.h
    ${RGM_ROOTDIR}/Editors/ShaderEditor.h
    ${RGM_ROOTDIR}/Editors/RoomEditor.h
    ${RGM_ROOTDIR}/Editors/BaseEditor.h
    ${RGM_ROOTDIR}/Editors/FontEditor.h
    ${RGM_ROOTDIR}/Editors/SpriteEditor.h
    ${RGM_ROOTDIR}/Editors/BackgroundEditor.h
    ${RGM_ROOTDIR}/Editors/VisualShaderEditor.h
    ${RGM_ROOTDIR}/Plugins/RGMPlugin.h
    ${RGM_ROOTDIR}/Plugins/ServerPlugin.h
    ${RGM_ROOTDIR}/Dialogs/EventArgumentsDialog.h
    ${RGM_ROOTDIR}/Dialogs/PreferencesDialog.h
    ${RGM_ROOTDIR}/Dialogs/PreferencesKeys.h
    ${RGM_ROOTDIR}/Dialogs/TimelineChangeMoment.h
    ${RGM_ROOTDIR}/Dialogs/KeyBindingPreferences.h
    ${RGM_ROOTDIR}/Utils/SafeCasts.h
    ${RGM_ROOTDIR}/Utils/ProtoManip.h
    ${RGM_ROOTDIR}/Utils/FieldPath.h
    ${RGM_ROOTDIR}/Utils/QBoilerplate.h
    ${RGM_ROOTDIR}/Widgets/BackgroundView.h
    ${RGM_ROOTDIR}/Widgets/CodeWidget.h
    ${RGM_ROOTDIR}/Widgets/ResourceSelector.h
    ${RGM_ROOTDIR}/Widgets/ColorPicker.h
    ${RGM_ROOTDIR}/Widgets/AssetScrollArea.h
    ${RGM_ROOTDIR}/Widgets/SpriteView.h
    ${RGM_ROOTDIR}/Widgets/AssetView.h
    ${RGM_ROOTDIR}/Widgets/PathView.h
    ${RGM_ROOTDIR}/Widgets/StackedCodeWidget.h
    ${RGM_ROOTDIR}/Widgets/SpriteSubimageListView.h
    ${RGM_ROOTDIR}/Widgets/AssetScrollAreaBackground.h
    ${RGM_ROOTDIR}/Widgets/RoomView.h
)

file(GLOB RGM_TESTS_DEPS_FILES 
    ${RGM_ROOTDIR}/MainWindow.cpp
    ${RGM_ROOTDIR}/Models/RepeatedMessageModel.cpp
    ${RGM_ROOTDIR}/Models/TreeSortFilterProxyModel.cpp
    ${RGM_ROOTDIR}/Models/PrimitiveModel.cpp
    ${RGM_ROOTDIR}/Models/EventsListModel.cpp
    ${RGM_ROOTDIR}/Models/ModelMapper.cpp
    ${RGM_ROOTDIR}/Models/RepeatedModel.cpp
    ${RGM_ROOTDIR}/Models/MessageModel.cpp
    ${RGM_ROOTDIR}/Models/TreeModel.cpp
    ${RGM_ROOTDIR}/Models/EventTypesListModel.cpp
    ${RGM_ROOTDIR}/Models/ResourceModelMap.cpp
    ${RGM_ROOTDIR}/Models/ImmediateMapper.cpp
    ${RGM_ROOTDIR}/Models/ProtoModel.cpp
    ${RGM_ROOTDIR}/Models/EventTypesListSortFilterProxyModel.cpp
    ${RGM_ROOTDIR}/Models/RepeatedSortFilterProxyModel.cpp
    ${RGM_ROOTDIR}/Components/Utility.cpp
    ${RGM_ROOTDIR}/Components/RecentFiles.cpp
    ${RGM_ROOTDIR}/Components/QMenuView.cpp
    ${RGM_ROOTDIR}/Components/ArtManager.cpp
    ${RGM_ROOTDIR}/Editors/VisualShaderEditor.cpp
    ${RGM_ROOTDIR}/Editors/PathEditor.cpp
    ${RGM_ROOTDIR}/Editors/RoomEditor.cpp
    ${RGM_ROOTDIR}/Editors/ObjectEditor.cpp
    ${RGM_ROOTDIR}/Editors/FontEditor.cpp
    ${RGM_ROOTDIR}/Editors/SpriteEditor.cpp
    ${RGM_ROOTDIR}/Editors/BackgroundEditor.cpp
    ${RGM_ROOTDIR}/Editors/SettingsEditor.cpp
    ${RGM_ROOTDIR}/Editors/ShaderEditor.cpp
    ${RGM_ROOTDIR}/Editors/SoundEditor.cpp
    ${RGM_ROOTDIR}/Editors/IncludeEditor.cpp
    ${RGM_ROOTDIR}/Editors/InformationEditor.cpp
    ${RGM_ROOTDIR}/Editors/BaseEditor.cpp
    ${RGM_ROOTDIR}/Editors/ScriptEditor.cpp
    ${RGM_ROOTDIR}/Editors/CodeEditor.cpp
    ${RGM_ROOTDIR}/Editors/TimelineEditor.cpp
    ${RGM_ROOTDIR}/Editors/VisualShaderEditor.cpp
    ${RGM_ROOTDIR}/Plugins/RGMPlugin.cpp
    ${RGM_ROOTDIR}/Plugins/ServerPlugin.cpp
    ${RGM_ROOTDIR}/Dialogs/EventArgumentsDialog.cpp
    ${RGM_ROOTDIR}/Dialogs/TimelineChangeMoment.cpp
    ${RGM_ROOTDIR}/Dialogs/PreferencesDialog.cpp
    ${RGM_ROOTDIR}/Dialogs/PreferencesKeys.cpp
    ${RGM_ROOTDIR}/Dialogs/KeyBindingPreferences.cpp
    ${RGM_ROOTDIR}/Utils/ProtoManip.cpp
    ${RGM_ROOTDIR}/Utils/FieldPath.cpp
    ${RGM_ROOTDIR}/Widgets/BackgroundView.cpp
    ${RGM_ROOTDIR}/Widgets/ColorPicker.cpp
    ${RGM_ROOTDIR}/Widgets/AssetView.cpp
    ${RGM_ROOTDIR}/Widgets/PathView.cpp
    ${RGM_ROOTDIR}/Widgets/RoomView.cpp
    ${RGM_ROOTDIR}/Widgets/SpriteView.cpp
    ${RGM_ROOTDIR}/Widgets/CodeWidget.cpp
    ${RGM_ROOTDIR}/Widgets/CodeWidgetPlain.cpp # Ignore checking for QScintilla
    ${RGM_ROOTDIR}/Widgets/SpriteSubimageListView.cpp
    ${RGM_ROOTDIR}/Widgets/StackedCodeWidget.cpp
    ${RGM_ROOTDIR}/Widgets/AssetScrollAreaBackground.cpp
)

file(GLOB RGM_TESTS_UI_FILES 
    ${RGM_ROOTDIR}/MainWindow.ui
    ${RGM_ROOTDIR}/Dialogs/TimelineChangeMoment.ui
    ${RGM_ROOTDIR}/Dialogs/PreferencesDialog.ui
    ${RGM_ROOTDIR}/Dialogs/AddImageDialog.ui
    ${RGM_ROOTDIR}/Editors/CodeEditor.ui
    ${RGM_ROOTDIR}/Editors/SoundEditor.ui
    ${RGM_ROOTDIR}/Editors/BackgroundEditor.ui
    ${RGM_ROOTDIR}/Editors/SpriteEditor.ui
    ${RGM_ROOTDIR}/Editors/PathEditor.ui
    ${RGM_ROOTDIR}/Editors/SettingsEditor.ui
    ${RGM_ROOTDIR}/Editors/RoomEditor.ui
    ${RGM_ROOTDIR}/Editors/FontEditor.ui
    ${RGM_ROOTDIR}/Editors/TimelineEditor.ui
    ${RGM_ROOTDIR}/Editors/ObjectEditor.ui
    ${RGM_ROOTDIR}/Editors/InformationEditor.ui
    ${RGM_ROOTDIR}/Editors/IncludeEditor.ui
)

file(GLOB RGM_TESTS_QRC_FILES 
    ${RGM_ROOTDIR}/images.qrc
    ${RGM_ROOTDIR}/resources.rc
)

enable_testing()

add_executable(${RGM_TESTS} ${RGM_TESTS_H_FILES} ${RGM_TESTS_SRC_FILES} ${RGM_TESTS_DEPS_FILES} ${RGM_TESTS_UI_FILES} ${RGM_TESTS_QRC_FILES})

# Find Protobuf
find_package(Protobuf CONFIG REQUIRED)
target_link_libraries(${RGM_TESTS} PRIVATE protobuf::libprotobuf)

# Find gRPC
find_package(gRPC CONFIG REQUIRED)
target_link_libraries(${RGM_TESTS} PRIVATE gRPC::gpr gRPC::grpc gRPC::grpc++)

find_package(Qt5 COMPONENTS Core Widgets Gui PrintSupport Multimedia REQUIRED)
target_link_libraries(${RGM_TESTS} PRIVATE Qt5::Core Qt5::Widgets Qt5::Gui Qt5::PrintSupport Qt5::Multimedia)

find_package(Qt5Test REQUIRED)
target_link_libraries(${RGM_TESTS} PRIVATE Qt5::Test)

target_include_directories(${RGM_TESTS} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/" 
    "${RGM_ROOTDIR}/" 
)

target_compile_definitions(${RGM_TESTS} PRIVATE QT_MESSAGELOGCONTEXT ENIGMA_DIR="${ENIGMA_DIR}")

target_link_libraries(${RGM_TESTS} PRIVATE ${LIB_EGM} ${LIB_PROTO} ${SHARED_LIB})

add_test(NAME ${RGM_TESTS} COMMAND ${RGM_TESTS})
