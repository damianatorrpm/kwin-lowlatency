install(DIRECTORY contents DESTINATION ${DATA_INSTALL_DIR}/${KWIN_NAME}/effects/kwin4_effect_translucency)
install(FILES metadata.desktop DESTINATION ${DATA_INSTALL_DIR}/${KWIN_NAME}/effects/kwin4_effect_translucency)

install(FILES metadata.desktop
        DESTINATION ${SERVICES_INSTALL_DIR}/${KWIN_NAME}
        RENAME kwin4_effect_translucency.desktop)

file(COPY contents metadata.desktop DESTINATION ${CMAKE_BINARY_DIR}/bin/kwin/effects/kwin4_effect_translucency)
