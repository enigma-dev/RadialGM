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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/Dialogs")

# Include ENIGMA things
set(ENIGMA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Submodules/enigma-dev)
include_directories("${CMAKE_BINARY_DIR}/Submodules/enigma-dev/CommandLine/libEGM/libProtocols" "${ENIGMA_DIR}/CommandLine/libEGM")

# Hack to get msys path
if(WIN32 AND NOT MSVC)
  get_filename_component(MINGW_BIN ${CMAKE_C_COMPILER} DIRECTORY)
endif()

# Populate a CMake variable with the sources
set(RGM_SOURCES
    Plugins/RGMPlugin.cpp
    Plugins/ServerPlugin.cpp
    main.cpp
    MainWindow.cpp
    Dialogs/PreferencesDialog.cpp
    Dialogs/TimelineChangeMoment.cpp
    Components/ArtManager.cpp
    Components/Utility.cpp
    Components/QMenuView.cpp
    Components/RecentFiles.cpp
    Models/TreeSortFilterProxyModel.cpp
    Models/MessageModel.cpp
    Models/RepeatedImageModel.cpp
    Models/RepeatedStringModel.cpp
    Models/ModelMapper.cpp
    Models/ProtoModel.cpp
    Models/RepeatedMessageModel.cpp
    Models/ImmediateMapper.cpp
    Models/ResourceModelMap.cpp
    Models/TreeModel.cpp
    Editors/ObjectEditor.cpp
    Editors/PathEditor.cpp
    Editors/CodeEditor.cpp
    Editors/SoundEditor.cpp
    Editors/FontEditor.cpp
    Editors/BackgroundEditor.cpp
    Editors/TimelineEditor.cpp
    Editors/ShaderEditor.cpp
    Editors/ScriptEditor.cpp
    Editors/SettingsEditor.cpp
    Editors/SpriteEditor.cpp
    Editors/BaseEditor.cpp
    Editors/RoomEditor.cpp
    Widgets/CodeWidgetPlain.cpp
    Widgets/AssetView.cpp
    Widgets/CodeWidget.cpp
    Widgets/BackgroundView.cpp
    Widgets/StackedCodeWidget.cpp
    Widgets/RoomView.cpp
    Widgets/PathView.cpp
    Widgets/AssetScrollAreaBackground.cpp
    Widgets/SpriteSubimageListView.cpp
    Widgets/ColorPicker.cpp
    Widgets/SpriteView.cpp
)

if (MSVC)
  set(RGM_SOURCES ${RGM_SOURCES} qtplugins.cpp)
endif()

set(RGM_HEADERS
    Plugins/ServerPlugin.h
    Plugins/RGMPlugin.h
    MainWindow.h
    main.h
    Dialogs/PreferencesDialog.h
    Dialogs/PreferencesKeys.h
    Dialogs/TimelineChangeMoment.h
    Components/Logger.h
    Components/RecentFiles.h
    Components/QMenuView.h
    Components/ArtManager.h
    Components/QMenuView_p.h
    Components/Utility.h
    Models/TreeModel.h
    Models/RepeatedStringModel.h
    Models/ResourceModelMap.h
    Models/ModelMapper.h
    Models/TreeSortFilterProxyModel.h
    Models/RepeatedImageModel.h
    Models/MessageModel.h
    Models/ImmediateMapper.h
    Models/RepeatedMessageModel.h
    Models/ProtoModel.h
    Models/RepeatedModel.h
    Editors/FontEditor.h
    Editors/PathEditor.h
    Editors/SpriteEditor.h
    Editors/SettingsEditor.h
    Editors/SoundEditor.h
    Editors/TimelineEditor.h
    Editors/BackgroundEditor.h
    Editors/ObjectEditor.h
    Editors/RoomEditor.h
    Editors/BaseEditor.h
    Editors/ScriptEditor.h
    Editors/CodeEditor.h
    Editors/ShaderEditor.h
    Widgets/AssetScrollArea.h
    Widgets/StackedCodeWidget.h
    Widgets/AssetScrollAreaBackground.h
    Widgets/PathView.h
    Widgets/ResourceSelector.h
    Widgets/RoomView.h
    Widgets/AssetView.h
    Widgets/ColorPicker.h
    Widgets/SpriteSubimageListView.h
    Widgets/CodeWidget.h
    Widgets/SpriteView.h
    Widgets/BackgroundView.h

)

set(RGM_UI
    MainWindow.ui
    Dialogs/TimelineChangeMoment.ui
    Dialogs/PreferencesDialog.ui
    Dialogs/AddImageDialog.ui
    Editors/CodeEditor.ui
    Editors/SoundEditor.ui
    Editors/BackgroundEditor.ui
    Editors/SpriteEditor.ui
    Editors/PathEditor.ui
    Editors/SettingsEditor.ui
    Editors/RoomEditor.ui
    Editors/FontEditor.ui
    Editors/TimelineEditor.ui
    Editors/ObjectEditor.ui
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

# we do this even in release mode for "Editor Diagnostics"
target_compile_definitions(${EXE} PUBLIC QT_MESSAGELOGCONTEXT)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT WIN32)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
  set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

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

# Find GRPC
find_library(LIB_GRPC_UNSECURE NAMES grpc++_unsecure)
find_library(LIB_GRPC NAMES grpc)
find_library(LIB_GPR NAMES gpr)
find_library(LIB_CARES NAMES cares)
find_library(LIB_ADDRESS_SORTING NAMES address_sorting)
target_link_libraries(${EXE} PRIVATE ${LIB_CARES} ${LIB_ADDRESS_SORTING} ${LIB_GPR} ${LIB_GRPC} ${LIB_GRPC_UNSECURE})

# Find Protobuf
include(FindProtobuf)
include_directories(${Protobuf_INCLUDE_DIRS})
target_link_libraries(${EXE} PRIVATE ${Protobuf_LIBRARIES})

# Find OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(${EXE} PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Find Qt
find_package(Qt5 COMPONENTS Core Widgets Gui PrintSupport Multimedia REQUIRED)
target_link_libraries(${EXE} PRIVATE Qt5::Core Qt5::Widgets Qt5::Gui Qt5::PrintSupport Qt5::Multimedia)

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

# LibProto
add_subdirectory(Submodules/enigma-dev/CommandLine/libEGM)
add_dependencies(${EXE} "EGM")
target_link_libraries(${EXE} PRIVATE "EGM" "Protocols")

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
	target_link_libraries(${EXE} PRIVATE Ws2_32 Wtsapi32 Wldap32 Crypt32 Winmm Userenv Netapi32 version Dwmapi Imm32)
endif(WIN32)

install(TARGETS ${EXE} DESTINATION "${ENIGMA_DIR}")

if(WIN32 AND NOT MSVC)

  find_file(LIB_GCC NAMES libgcc_s_dw2-1.dll libgcc_s_seh-1.dll PATHS "${MINGW_BIN}")
  install(FILES
            "${LIB_GCC}"
            "${MINGW_BIN}/libwinpthread-1.dll"
            "${MINGW_BIN}/libstdc++-6.dll" 
            "${MINGW_BIN}/libgrpc++_unsecure.dll"
            "${MINGW_BIN}/libprotobuf.dll"
            "${MINGW_BIN}/zlib1.dll"
            "${MINGW_BIN}/libpng16-16.dll"
            "${MINGW_BIN}/libpugixml.dll"
            "${MINGW_BIN}/libgpr.dll"
            "${MINGW_BIN}/libgrpc_unsecure.dll"
            "${MINGW_BIN}/yaml-cpp.dll"
            "${MINGW_BIN}/libdouble-conversion.dll"
            "${MINGW_BIN}/libicuuc65.dll"
            "${MINGW_BIN}/libicuin65.dll"
            "${MINGW_BIN}/libpcre2-16-0.dll"
            "${MINGW_BIN}/libcares-3.dll"
            "${MINGW_BIN}/libaddress_sorting.dll"
            "${MINGW_BIN}/libharfbuzz-0.dll"
            "${MINGW_BIN}/libzstd.dll"
            "${MINGW_BIN}/libicudt65.dll"
            "${MINGW_BIN}/libglib-2.0-0.dll"
            "${MINGW_BIN}/libfreetype-6.dll"
            "${MINGW_BIN}/libgraphite2.dll"
            "${MINGW_BIN}/libbz2-1.dll"
            "${MINGW_BIN}/libintl-8.dll"
            "${MINGW_BIN}/libpcre-1.dll"
            "${MINGW_BIN}/libiconv-2.dll"
          DESTINATION "${ENIGMA_DIR}")

    install(CODE "execute_process(COMMAND windeployqt.exe --no-translations ${ENIGMA_DIR}/RadialGM.exe)")
endif() 