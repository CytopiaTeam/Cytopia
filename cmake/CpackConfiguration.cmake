# CPack Configuration

# General Package Generator settings
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/JimmySnails/Cytopia")

set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A pixel-art city building game")
set(CPACK_PACKAGE_VENDOR "Cytopia")

set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/build/msvc/cytopia.ico")

set(CPACK_STRIP_FILES ON)
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})

if ( UNIX AND NOT ENV{CI} )
  if ( (linux_distribution MATCHES Debian) OR (linux_distribution MATCHES Ubuntu) )
    configure_file(${PROJECT_SOURCE_DIR}/build/setup/cytopia.desktop setup/cytopia.desktop)
    configure_file(${PROJECT_SOURCE_DIR}/build/setup/cytopia-tiledata-editor.desktop setup/cytopia-tiledata-editor.desktop)

    install(FILES ${PROJECT_BINARY_DIR}/setup/cytopia.desktop DESTINATION /usr/share/applications)
    install(FILES ${PROJECT_BINARY_DIR}/setup/cytopia-tiledata-editor.desktop DESTINATION /usr/share/applications)

    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/games/${PROJECT_NAME}")
    set(CPACK_GENERATOR "DEB")
	
  	set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
  	set(CPACK_DEBIAN_PACKAGE_SECTION games)
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jimmy Snails")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${CPACK_PACKAGE_HOMEPAGE_URL})
	
  elseif ( (linux_distribution MATCHES CentOS) OR (linux_distribution MATCHES SUSE))
    set(CPACK_GENERATOR "RPM")
    # todo: 
	  # - add more debian / rpm specific generator settings
  elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}")
  else ()
    set(CPACK_GENERATOR "TGZ")
  endif()
elseif ( WIN32 )
  # packaging needs WiX Tooset installed 
  set(CPACK_GENERATOR "WIX;ZIP")
  # Upgrade Code must always be the same, so the setup can replace the existing installation
  set(CPACK_WIX_UPGRADE_GUID "F56FA877-A127-415C-9D04-43023C5459B0")
  set(CPACK_WIX_PRODUCT_ICON "${CPACK_PACKAGE_ICON}")
  set(CPACK_WIX_CMAKE_PACKAGE_REGISTRY ${PROJECT_NAME})
  set(CPACK_WIX_UI_BANNER "${PROJECT_SOURCE_DIR}/build/setup/banner.png")
  set(CPACK_WIX_UI_DIALOG "${PROJECT_SOURCE_DIR}/build/setup/logoSide.png")
  set(CPACK_WIX_PROGRAM_MENU_FOLDER "${PROJECT_NAME}")

  #For Windows Start Menu entries
  set(CPACK_PACKAGE_EXECUTABLES
    "${PROJECT_NAME}" "${PROJECT_NAME}"
    "TileDataEditor" "TileDataEditor"
  )

  #For Windows Desktop shortcuts
  set(CPACK_CREATE_DESKTOP_LINKS
    "${PROJECT_NAME}" "${PROJECT_NAME}"
  )

  # Windows Add or Remove Program properties
  set(CPACK_WIX_PROPERTY_ARPCOMMENTS "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
  set(CPACK_WIX_PROPERTY_ARPHELPLINK "https://github.com/JimmySnails/Cytopia/wiki")
  set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://github.com/JimmySnails/Cytopia")
  set(CPACK_WIX_PROPERTY_URLUPDATEINFO "https://github.com/JimmySnails/Cytopia/releases/")
endif()

include(CPack)
