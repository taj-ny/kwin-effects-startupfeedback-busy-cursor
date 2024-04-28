add_subdirectory(kcm)

set(startupfeedback_busy_cursor_SOURCES
    startupfeedback.cpp
    main.cpp
)

kconfig_add_kcfg_files(startupfeedback_busy_cursor_SOURCES
    startupfeedbackconfig.kcfgc
)

add_library(startupfeedback_busy_cursor MODULE ${startupfeedback_busy_cursor_SOURCES})
target_link_libraries(startupfeedback_busy_cursor PRIVATE
    KWin::kwin

    KF6::ConfigGui

    KDecoration2::KDecoration
)

install(TARGETS startupfeedback_busy_cursor DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin/effects/plugins)
