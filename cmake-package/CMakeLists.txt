cmake_minimum_required(VERSION 3.11)

project(RadialGM-Package)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/../Submodules/enigma-dev" DESTINATION .)
install(FILES "${CMAKE_SOURCE_DIR}/../LICENSE.txt" DESTINATION .)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/ DESTINATION .)

########################################################################
# CPack configuration
########################################################################

set(CPACK_PACKAGE_NAME "ENIGMA-Installer")
set(CPACK_PACKAGE_VENDOR "ENIGMA Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RadialGM & ENIGMA installer")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "enigma-dev")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/../LICENSE.txt")
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/../Submodules/enigma-dev/Resources/icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/../Submodules/enigma-dev/Resources/icon.ico")

if (MSVC) # MSVC requires seperate dirs as dll names for debug and release can be same
  set(CPACK_NSIS_MENU_LINKS "Release/RadialGM${CMAKE_EXECUTABLE_SUFFIX}" "RadialGM"
                            "Debug/RadialGM-Debug${CMAKE_EXECUTABLE_SUFFIX}" "RadialGM-Debug")
else()
  set(CPACK_NSIS_MENU_LINKS "RadialGM${CMAKE_EXECUTABLE_SUFFIX}" "RadialGM"
                            "RadialGM-Debug${CMAKE_EXECUTABLE_SUFFIX}" "RadialGM-Debug")
endif()

include(CPack)