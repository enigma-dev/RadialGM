cmake_minimum_required(VERSION 3.11)

project(RadialGM-Package)

install(DIRECTORY ${CMAKE_SOURCE_DIR} DESTINATION .)

########################################################################
# CPack configuration
########################################################################

set(CPACK_PACKAGE_NAME "ENIGMA-Installer")
set(CPACK_PACKAGE_VENDOR "ENIGMA Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RadialGM & ENIGMA installer")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "enigma-dev")
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/license.txt")
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/enigma-dev/Resources/icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/enigma-dev/Resources/icon.ico")
set(CPACK_NSIS_MENU_LINKS "RadialGM${CMAKE_EXECUTABLE_SUFFIX}" "RadialGM"
                          "RadialGM-Debug${CMAKE_EXECUTABLE_SUFFIX}" "RadialGM-Debug")

include(CPack)