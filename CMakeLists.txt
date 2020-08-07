cmake_minimum_required(VERSION 3.14)
project(RadialGM)

include(CMakeDependentOption)

option(RGM_ENABLE_GRPC_SERVER "Enable the GRPC client plugin for compilation and code analysis." ON)
option(RGM_BUILD_EMAKE "Build Emake and the compiler." ON)

# FIXME: MSVC dynamic linking requires US TO DLLEXPORT our funcs
# since we currently don't, I'm force disabling the option on MSVC
cmake_dependent_option(RGM_BUILD_STATIC "Build static libs." ON "MSVC" OFF)

if (RGM_BUILD_STATIC)
 set(LIB_TYPE STATIC)
else()
 set(LIB_TYPE SHARED)
endif()

if (CMAKE_BUILD_TYPE MATCHES "Debug")
  set(EXE "RadialGM-Debug")
  add_definitions(-DRGM_DEBUG)
else()
  set(EXE "RadialGM")
endif()

set(EXE_DESCRIPTION "ENIGMA IDE")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
SET(CMAKE_SKIP_BUILD_RPATH ON)
set(CMAKE_INSTALL_RPATH $ORIGIN)

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
include_directories("${CMAKE_BINARY_DIR}/Submodules/enigma-dev/shared/protos/" "${ENIGMA_DIR}/CommandLine/libEGM" "${ENIGMA_DIR}/shared")

# Populate a CMake variable with the sources
set(RGM_SOURCES
    Plugins/RGMPlugin.cpp
    main.cpp
    MainWindow.cpp
    Dialogs/PreferencesDialog.cpp
    Dialogs/TimelineChangeMoment.cpp
    Components/ArtManager.cpp
    Components/Utility.cpp
    Components/QMenuView.cpp
    Components/RecentFiles.cpp
    Models/TreeSortFilterProxyModel.cpp
    Models/ProtoModel.cpp
    Models/TreeModel.cpp
    Models/DiagnosticModel.cpp
    Models/EditorMapper.cpp
    Models/EditorModel.cpp
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

set(RGM_HEADERS
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
    Models/TreeSortFilterProxyModel.h
    Models/ProtoModel.h
    Models/DiagnosticModel.h
    Models/EditorMapper.h
    Models/EditorModel.h
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
find_library(LIB_QSCINTILLA NAMES qscintilla2 qscintilla2_qt5 qscintilla2-qt5 qt5scintilla2 libqscintilla2_qt5.so libqscintilla2.so libqscintilla2.a qscintilla2.lib)
if (NOT LIB_QSCINTILLA)
    set(EDITOR_SOURCES Widgets/CodeWidgetPlain.cpp)
else()
    set(EDITOR_SOURCES Widgets/CodeWidgetScintilla.cpp)
endif()

if (RGM_ENABLE_GRPC_SERVER)
  add_definitions(-DRGM_SERVER_ENABLED)
  set(RGM_SOURCES ${RGM_SOURCES} Plugins/ServerPlugin.cpp)
  set(RGM_HEADERS ${RGM_HEADERS} Plugins/ServerPlugin.h)
endif()

# Tell CMake to create the RadialGM executable
add_executable(${EXE} WIN32 ${RGM_UI} ${RGM_HEADERS} ${RGM_SOURCES} ${EDITOR_SOURCES} ${RGM_RC})

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
  message(STATUS "  '${CompilerFlag}': ${${CompilerFlag}}")
endforeach()

if (LIB_QSCINTILLA)
    message(STATUS "Found QScintilla: ${LIB_QSCINTILLA}")
    add_definitions(-DQSCINTILLA_DLL)
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

if (RGM_ENABLE_GRPC_SERVER)
  #Find gRPC
  if (MSVC) # Windows actually does something right for once
    find_package(gRPC CONFIG REQUIRED)
    target_link_libraries(${EXE} PRIVATE gRPC::gpr gRPC::grpc gRPC::grpc++)
  else() # https://tinyurl.com/arch-grpc-bug
    find_library(LIB_GRPC_UNSECURE NAMES grpc++_unsecure)
    find_library(LIB_GPR NAMES gpr)
    find_library(LIB_CARES NAMES cares)
    find_library(LIB_ADDRESS_SORTING NAMES address_sorting)
    target_link_libraries(${EXE} PRIVATE ${LIB_CARES} ${LIB_ADDRESS_SORTING} ${LIB_GPR} ${LIB_GRPC_UNSECURE})
  endif()
endif()

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

# LibProto
add_subdirectory(Submodules/enigma-dev/shared)
add_subdirectory(Submodules/enigma-dev/shared/protos)
add_subdirectory(Submodules/enigma-dev/CommandLine/libEGM)
add_dependencies(${EXE} "EGM")
target_link_libraries(${EXE} PRIVATE "EGM" "Protocols" "ENIGMAShared")

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

if(MSVC)
  # Default MSVC warnings are overkill. Set to Lvl 2
  target_compile_options(${EXE} PRIVATE /W1)
endif()

if (RGM_BUILD_EMAKE)
  add_subdirectory(Submodules/enigma-dev/CompilerSource)
  add_subdirectory(Submodules/enigma-dev/CommandLine/emake)
  if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CLI_TARGET "emake-debug")
  else()
    set(CLI_TARGET "emake")
  endif()
  add_dependencies(${EXE} ${CLI_TARGET})
endif()

install(TARGETS ${EXE} RUNTIME DESTINATION .)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${EXE}.dir/Debug/${EXE}.pdb" DESTINATION . OPTIONAL)

set(RGM_APP "${CMAKE_INSTALL_PREFIX}/${EXE}${CMAKE_EXECUTABLE_SUFFIX}")
set(EMAKE_APP "${CMAKE_INSTALL_PREFIX}/${CLI_TARGET}${CMAKE_EXECUTABLE_SUFFIX}")

if (MSVC)
file(TO_CMAKE_PATH ${VCPKG_ROOT} VCPKG_ROOT)
  if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(SEARCH_PATHS "${VCPKG_ROOT}/installed/x64-windows/debug/bin/")
  else()
    set(SEARCH_PATHS "${VCPKG_ROOT}/installed/x64-windows/bin/")
  endif()
else()
  set(LIBS "${CMAKE_INSTALL_PREFIX}/${CMAKE_SHARED_LIBRARY_PREFIX}EGM${CMAKE_SHARED_LIBRARY_SUFFIX}" 
           "${CMAKE_INSTALL_PREFIX}/${CMAKE_SHARED_LIBRARY_PREFIX}Protocols${CMAKE_SHARED_LIBRARY_SUFFIX}"
           "${CMAKE_INSTALL_PREFIX}/${CMAKE_SHARED_LIBRARY_PREFIX}ENIGMAShared${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

if (WIN32)

  set(WINDEPLOY_ARGS "--no-translations --no-angle")

  if (MSVC)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
      set(CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY TRUE)
      set(WINDEPLOY_ARGS "${WINDEPLOY_ARGS} --pdb --no-compiler-runtime")
    else()
      set(WINDEPLOY_ARGS "${WINDEPLOY_ARGS} --no-compiler-runtime")
    endif()
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    include(InstallRequiredSystemLibraries)
    install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION .)
  endif()

  install(CODE "
          include(BundleUtilities)
          fixup_bundle(\"${RGM_APP}\" \"${LIBS}\" \"${SEARCH_PATHS}\")
          fixup_bundle(\"${EMAKE_APP}\" \"${LIBS}\" \"${SEARCH_PATHS}\")
          execute_process(COMMAND windeployqt.exe ${WINDEPLOY_ARGS} ${RGM_APP})
          ")
endif()
