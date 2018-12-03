cmake_minimum_required(VERSION 3.11)

set(EXE "RadialGM")
set(EXE_VERSION "0.1")
set(EXE_DESCRIPTION "ENIGMA IDE")

project(RadialGM)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Create code from Qt rc files
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/Dialogs")

# Include ENIGMA things
set(ENIGMA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Submodules/enigma-dev)
include_directories("${ENIGMA_DIR}/CommandLine/protos" "${ENIGMA_DIR}/CommandLine/libEGM" "${ENIGMA_DIR}/CommandLine/libAssets")

# Populate a CMake variable with the sources
set(RGM_SOURCES
    main.cpp
    MainWindow.cpp
    Dialogs/PreferencesDialog.cpp
    Editors/BaseEditor.cpp
    Editors/CodeEditor.cpp
    Editors/BackgroundEditor.cpp
    Editors/ObjectEditor.cpp
    Editors/SoundEditor.cpp
    Editors/ScriptEditor.cpp
    Editors/FontEditor.cpp
    Editors/PathEditor.cpp
    Editors/TimelineEditor.cpp
    Editors/RoomEditor.cpp
    Editors/SpriteEditor.cpp
    Editors/SettingsEditor.cpp
    Models/TreeModel.cpp
    Models/SpriteModel.cpp
    Models/ProtoModel.cpp
    Models/ModelMapper.cpp
    Models/ImmediateMapper.cpp
    Models/ResourceModelMap.cpp
    Models/RepeatedProtoModel.cpp
    Models/TreeSortFilterProxyModel.cpp
    Components/ArtManager.cpp
    Components/Utility.cpp
    Components/RecentFiles.cpp
    Components/QMenuView.cpp
    Widgets/BackgroundView.cpp
    Widgets/CodeWidget.cpp
    Widgets/ColorPicker.cpp
    Widgets/RoomView.cpp
    Widgets/AssetView.cpp
    Plugins/RGMPlugin.cpp
    Plugins/ServerPlugin.cpp
    qtplugins.cpp
)

if (MSVC)
  set(RGM_SOURCES ${RGM_SOURCES} qtplugins.cpp)
endif()

set(RGM_HEADERS
    main.h
    MainWindow.h
    Dialogs/PreferencesDialog.h
    Dialogs/PreferencesKeys.h
    Editors/CodeEditor.h
    Editors/BaseEditor.h
    Editors/BackgroundEditor.h
    Editors/ObjectEditor.h
    Editors/SoundEditor.h
    Editors/ScriptEditor.h
    Editors/FontEditor.h
    Editors/PathEditor.h
    Editors/TimelineEditor.h
    Editors/RoomEditor.h
    Editors/SpriteEditor.h
    Editors/SettingsEditor.h
    Models/TreeModel.h
    Models/ProtoModel.h
    Models/ModelMapper.h
    Models/ImmediateMapper.h
    Models/SpriteModel.h
    Models/ResourceModelMap.h
    Models/RepeatedProtoModel.h
    Models/TreeSortFilterProxyModel.h
    Components/ArtManager.h
    Components/RecentFiles.h
    Components/Utility.h
    Components/QMenuView.h
    Widgets/BackgroundView.h
    Widgets/CodeWidget.h
    Widgets/ColorPicker.h
    Widgets/RoomView.h
    Widgets/AssetView.h
    Plugins/RGMPlugin.h
    Plugins/ServerPlugin.h
)

set(RGM_UI
    MainWindow.ui
    Dialogs/PreferencesDialog.ui
    Dialogs/AddImageDialog.ui
    Editors/CodeEditor.ui
    Editors/BackgroundEditor.ui
    Editors/ObjectEditor.ui
    Editors/FontEditor.ui
    Editors/PathEditor.ui
    Editors/TimelineEditor.ui
    Editors/RoomEditor.ui
    Editors/SpriteEditor.ui
    Editors/SettingsEditor.ui
)

set(RGM_RC
    images.qrc
    resources.rc
)

# Check for QScintilla
find_library(LIB_QSCINTILLA NAMES qscintilla2)
if (NOT LIB_QSCINTILLA)
    set(EDITOR_SOURCES Widgets/CodeWidgetPlain.cpp)
else()
    set(EDITOR_SOURCES Widgets/CodeWidgetScintilla.cpp)
endif()

# Tell CMake to create the RadialGM executable
if(WIN32)
  add_executable(${EXE} WIN32 ${RGM_UI} ${RGM_HEADERS} ${RGM_SOURCES} ${EDITOR_SOURCES} ${RGM_RC})
else()
  add_executable(${EXE} ${RGM_UI} ${RGM_HEADERS} ${RGM_SOURCES} ${EDITOR_SOURCES} ${RGM_RC})
endif(WIN32)

message(STATUS "Initial build flags:")
set(CompilerFlags
	  CMAKE_C_FLAGS_DEBUG
	  CMAKE_C_FLAGS_MINSIZEREL
	  CMAKE_C_FLAGS_RELEASE
	  CMAKE_C_FLAGS_RELWITHDEBINFO
	  CMAKE_CXX_FLAGS_DEBUG
	  CMAKE_CXX_FLAGS_MINSIZEREL
	  CMAKE_CXX_FLAGS_RELEASE
	  CMAKE_CXX_FLAGS_RELWITHDEBINFO
	  )
foreach(CompilerFlag ${CompilerFlags})
  if(MSVC)
	string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
  endif(MSVC)
  message(STATUS "  '${CompilerFlag}': ${${CompilerFlag}}")
endforeach()

if (LIB_QSCINTILLA)
    target_link_libraries(${EXE} PRIVATE ${LIB_QSCINTILLA})
endif()

# Find PugiXML
find_library(LIB_PUGIXML NAMES pugixml)
target_link_libraries(${EXE} PRIVATE ${LIB_PUGIXML})

# Find RapidJSON
find_package(RapidJSON REQUIRED)
include_directories(${EXE} PRIVATE ${RAPIDJSON_INCLUDE_DIRS})

# Find yaml-cpp
find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(${EXE} PRIVATE yaml-cpp)

# Find libEGM
# Debug
find_library(LIB_EGM_D NAMES EGMd PATHS ${ENIGMA_DIR})
# Release
find_library(LIB_EGM NAMES EGM PATHS ${ENIGMA_DIR})
target_link_libraries(${EXE} PRIVATE "$<IF:$<CONFIG:Debug>,${LIB_EGM_D},${LIB_EGM}>")

# Find libAssets
# Debug
find_library(LIB_ASSETS_D NAMES Assetsd PATHS ${ENIGMA_DIR})
# Release
find_library(LIB_ASSETS NAMES Assets PATHS ${ENIGMA_DIR})
target_link_libraries(${EXE} PRIVATE "$<IF:$<CONFIG:Debug>,${LIB_ASSETS_D},${LIB_ASSETS}>")

# Find libProtocols
# Debug
find_library(LIB_PROTO_D NAMES Protocolsd PATHS ${ENIGMA_DIR})
# Release
find_library(LIB_PROTO NAMES Protocols PATHS ${ENIGMA_DIR})
target_link_libraries(${EXE} PRIVATE "$<IF:$<CONFIG:Debug>,${LIB_PROTO_D},${LIB_PROTO}>")

# Find GRPC
find_library(LIB_GRPC_UNSECURE NAMES grpc++_unsecure)
find_library(LIB_GRPC NAMES grpc)
find_library(LIB_GPR NAMES gpr)
target_link_libraries(${EXE} PRIVATE ${LIB_GRPC_UNSECURE} ${LIB_GRPC} ${LIB_GPR})

# Find Protobuf
include(FindProtobuf)
include_directories(${Protobuf_INCLUDE_DIRS})
target_link_libraries(${EXE} PRIVATE ${Protobuf_LIBRARIES})

# Find OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(${EXE} PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Find Qt
find_package(Qt5 COMPONENTS Core Widgets Gui PrintSupport REQUIRED)
target_link_libraries(${EXE} PRIVATE Qt5::Core Qt5::Widgets Qt5::Gui Qt5::PrintSupport)


if(MSVC)
    target_link_libraries(${EXE} PRIVATE Qt5::WinMain)

    # === Debug
    find_library(LIB_QT_PLATFORM_D NAMES Qt5PlatformCompositorSupportd)
    find_library(LIB_QT_FONT_D NAMES Qt5FontDatabaseSupportd)
    find_library(LIB_QT_UI_D NAMES Qt5WindowsUIAutomationSupportd)
    find_library(LIB_QT_EVENT_D NAMES Qt5EventDispatcherSupportd)
    find_library(LIB_QT_THEME_D NAMES Qt5ThemeSupportd)
    find_library(LIB_QT_PRINT_D NAMES Qt5PrintSupportd)
    find_library(LIB_QT_ICO_D NAMES qicod PATHS "${QT_DIR}/plugins/imageformats")
    find_library(LIB_QT_GIF_D NAMES qgifd PATHS "${QT_DIR}/plugins/imageformats")
    find_library(LIB_QT_JPEG_D NAMES qjpegd PATHS "${QT_DIR}/plugins/imageformats")
    # Windows 10 SDK doesn't include a debug version of Uxtheme
    find_library(LIB_WIN_UXTHEME_D NAMES Uxtheme)
    find_library(LIB_QT_WINVISTASTYLE_D NAMES qwindowsvistastyled PATHS "${QT_DIR}/plugins/styles")
    find_library(LIB_QT_WIN_D NAMES qwindowsd PATHS "${QT_DIR}/plugins/platforms")

    # === Release
    find_library(LIB_QT_PLATFORM NAMES Qt5PlatformCompositorSupport)
    find_library(LIB_QT_FONT NAMES Qt5FontDatabaseSupport)
    find_library(LIB_QT_UI NAMES Qt5WindowsUIAutomationSupport)
    find_library(LIB_QT_EVENT NAMES Qt5EventDispatcherSupport)
    find_library(LIB_QT_THEME NAMES Qt5ThemeSupport)
    find_library(LIB_QT_PRINT NAMES Qt5PrintSupport)
    # ICO is needed to set Win32 icon on the Window
    # also all of these image plugins are very small about 100kb~ each
    find_library(LIB_QT_ICO NAMES qico PATHS "${QT_DIR}/plugins/imageformats")
    find_library(LIB_QT_GIF NAMES qgif PATHS "${QT_DIR}/plugins/imageformats")
    find_library(LIB_QT_JPEG NAMES qjpeg PATHS "${QT_DIR}/plugins/imageformats")
    find_library(LIB_WIN_UXTHEME NAMES Uxtheme)
    find_library(LIB_QT_WINVISTASTYLE NAMES qwindowsvistastyle PATHS "${QT_DIR}/plugins/styles")
    find_library(LIB_QT_WIN NAMES qwindows PATHS "${QT_DIR}/plugins/platforms")

    target_link_libraries(${EXE} PRIVATE "$<IF:$<CONFIG:Debug>,${LIB_QT_PLATFORM_D},${LIB_QT_PLATFORM}>"
                          "$<IF:$<CONFIG:Debug>,${LIB_QT_FONT_D},${LIB_QT_FONT}>"
                          "$<IF:$<CONFIG:Debug>,${LIB_QT_UI_D},${LIB_QT_UI}>"
                          "$<IF:$<CONFIG:Debug>,${LIB_QT_EVENT_D},${LIB_QT_EVENT}>"
                          "$<IF:$<CONFIG:Debug>,${LIB_QT_THEME_D},${LIB_QT_THEME}>"
                          "$<IF:$<CONFIG:Debug>,${LIB_QT_PRINT_D},${LIB_QT_PRINT}>"
                          "$<IF:$<CONFIG:Debug>,${LIB_QT_ICO_D},${LIB_QT_ICO}>"
                          "$<IF:$<CONFIG:Debug>,${LIB_QT_GIF_D},${LIB_QT_GIF}>"
                          "$<IF:$<CONFIG:Debug>,${LIB_QT_JPEG_D},${LIB_QT_JPEG}>"
                          "$<IF:$<CONFIG:Debug>,${LIB_WIN_UXTHEME_D},${LIB_WIN_UXTHEME}>"
                          "$<IF:$<CONFIG:Debug>,${LIB_QT_WINVISTASTYLE_D},${LIB_QT_WINVISTASTYLE}>"
                          "$<IF:$<CONFIG:Debug>,${LIB_QT_WIN_D},${LIB_QT_WIN}>"
                          )
endif(MSVC)

# Find FreeType
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
target_link_libraries(${EXE} PRIVATE ${FREETYPE_LIBRARIES})

# Find JPEG
find_library(LIB_JPEG NAMES jpeg)
target_link_libraries(${EXE} PRIVATE ${LIB_JPEG})

# Find HarfBuzz
find_library(LIB_HARFBUZZ NAMES harfbuzz)
target_link_libraries(${EXE} PRIVATE ${LIB_HARFBUZZ})

# Find Pcre2
find_library(LIB_PCRE2 NAMES "pcre2-16")
target_link_libraries(${EXE} PRIVATE ${LIB_PCRE2})

# Find double-conversion
find_library(LIB_DOUBLE_CONVERSION NAMES double-conversion)
target_link_libraries(${EXE} PRIVATE ${LIB_DOUBLE_CONVERSION})

if(WIN32)
	# Windows is a turd
	target_link_libraries(${EXE} PRIVATE Ws2_32 Wldap32 Crypt32 Winmm Userenv Netapi32 version Dwmapi Imm32)
endif(WIN32)
