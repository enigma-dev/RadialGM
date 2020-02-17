cmake_minimum_required(VERSION 3.11)

if (CMAKE_BUILD_TYPE EQUAL "Debug")
  set(EXE "RadialGM-Debug")
else()
  set(EXE "RadialGM")
endif()

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

if(WIN32)
  if(MSVC)
    file(TO_CMAKE_PATH ${VCPKG_ROOT} VCPKG_ROOT)
    if(CMAKE_BUILD_TYPE  MATCHES Debug)
      set (DEBUG_POSTFIX d)
      set(DLL_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/debug/bin")
    else()
      set(DLL_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/bin")
    endif()
  else()
    # Hack to get msys path
    get_filename_component(DLL_DIR ${CMAKE_C_COMPILER} DIRECTORY)
  endif()
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

set(CMAKE_INSTALL_RPATH "${ENIGMA_DIR}")

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

#Find gRPC
if (WIN32) # Windows actually does something right for once
  find_package(gRPC CONFIG REQUIRED)
  target_link_libraries(${EXE} PRIVATE gRPC::gpr gRPC::grpc gRPC::grpc++)
else() # https://tinyurl.com/arch-grpc-bug
  find_library(LIB_GRPC_UNSECURE NAMES grpc++_unsecure)
  find_library(LIB_GRPC NAMES grpc)
  find_library(LIB_GPR NAMES gpr)
  find_library(LIB_CARES NAMES cares)
  find_library(LIB_ADDRESS_SORTING NAMES address_sorting)
  target_link_libraries(${EXE} PRIVATE ${LIB_CARES} ${LIB_ADDRESS_SORTING} ${LIB_GPR} ${LIB_GRPC} ${LIB_GRPC_UNSECURE})
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

if(WIN32)
  if (MSVC)
    # Default MSVC warnings are overkill. Set to Lvl 2
    target_compile_options(${EXE} PRIVATE /W1)
    target_compile_options("EGM" PRIVATE /W1)
    target_compile_options("Protocols" PRIVATE /W1)

    install(FILES
      "${DLL_DIR}/libprotobuf${DEBUG_POSTFIX}.dll"
      "${DLL_DIR}/Qt5PrintSupport${DEBUG_POSTFIX}.dll"
      "${DLL_DIR}/Qt5MultiMedia${DEBUG_POSTFIX}.dll"
      "${DLL_DIR}/Qt5Widgets${DEBUG_POSTFIX}.dll"
      "${DLL_DIR}/Qt5Gui${DEBUG_POSTFIX}.dll"
      "${DLL_DIR}/Qt5Core${DEBUG_POSTFIX}.dll"
      "${DLL_DIR}/Qt5Network${DEBUG_POSTFIX}.dll"
      "${DLL_DIR}/zlib${DEBUG_POSTFIX}1.dll"
      "${DLL_DIR}/libpng16${DEBUG_POSTFIX}.dll"
      "${DLL_DIR}/pugixml.dll"
      "${DLL_DIR}/libssl-1_1-x64.dll"
      "${DLL_DIR}/libcrypto-1_1-x64.dll"
      "${DLL_DIR}/cares.dll"
      "${DLL_DIR}/harfbuzz.dll"
      "${DLL_DIR}/icuin${DEBUG_POSTFIX}65.dll"
      "${DLL_DIR}/icuuc${DEBUG_POSTFIX}65.dll"
      "${DLL_DIR}/pcre2-16${DEBUG_POSTFIX}.dll"
      "${DLL_DIR}/freetype${DEBUG_POSTFIX}.dll"
      "${DLL_DIR}/icudt${DEBUG_POSTFIX}65.dll"
      "${DLL_DIR}/bz2${DEBUG_POSTFIX}.dll"
    DESTINATION "${ENIGMA_DIR}")
  else()
    find_file(LIB_GCC NAMES libgcc_s_dw2-1.dll libgcc_s_seh-1.dll PATHS "${DLL_DIR}")
    find_file(LIB_SSL NAMES libssl-1_1.dll libssl-1_1-x64.dll PATHS "${DLL_DIR}")
    find_file(LIB_CRYPTO NAMES libcrypto-1_1.dll libcrypto-1_1-x64.dll PATHS "${DLL_DIR}")
    install(CODE "execute_process(COMMAND windeployqt.exe --no-translations ${ENIGMA_DIR}/${EXE}.exe)")
    install(FILES
      "${LIB_GCC}"
      "${LIB_SSL}"
      "${LIB_CRYPTO}"
      "${DLL_DIR}/libwinpthread-1.dll"
      "${DLL_DIR}/libstdc++-6.dll" 
      "${DLL_DIR}/libgrpc++_unsecure.dll"
      "${DLL_DIR}/libprotobuf.dll"
      "${DLL_DIR}/zlib1.dll"
      "${DLL_DIR}/libpng16-16.dll"
      "${DLL_DIR}/libpugixml.dll"
      "${DLL_DIR}/libgpr.dll"
      "${DLL_DIR}/libgrpc_unsecure.dll"
      "${DLL_DIR}/libgrpc.dll"
      "${DLL_DIR}/libgrpc++.dll"
      "${DLL_DIR}/yaml-cpp.dll"
      "${DLL_DIR}/libdouble-conversion.dll"
      "${DLL_DIR}/libicuuc65.dll"
      "${DLL_DIR}/libicuin65.dll"
      "${DLL_DIR}/libpcre2-16-0.dll"
      "${DLL_DIR}/libcares-3.dll"
      "${DLL_DIR}/libaddress_sorting.dll"
      "${DLL_DIR}/libharfbuzz-0.dll"
      "${DLL_DIR}/libzstd.dll"
      "${DLL_DIR}/libicudt65.dll"
      "${DLL_DIR}/libglib-2.0-0.dll"
      "${DLL_DIR}/libfreetype-6.dll"
      "${DLL_DIR}/libgraphite2.dll"
      "${DLL_DIR}/libbz2-1.dll"
      "${DLL_DIR}/libintl-8.dll"
      "${DLL_DIR}/libpcre-1.dll"
      "${DLL_DIR}/libiconv-2.dll"
    DESTINATION "${ENIGMA_DIR}")
  endif()
endif() 
