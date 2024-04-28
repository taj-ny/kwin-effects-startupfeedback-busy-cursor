set(kwin_startupfeedback_busy_cursor_config_SRCS startupfeedback_config.cpp startupfeedback_config.h)

ki18n_wrap_ui(kwin_startupfeedback_busy_cursor_config_SRCS startupfeedback_config.ui)
kconfig_add_kcfg_files(kwin_startupfeedback_busy_cursor_config_SRCS ../startupfeedbackconfig.kcfgc)

qt_add_dbus_interface(kwin_startupfeedback_busy_cursor_config_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)

add_library(kwin_startupfeedback_busy_cursor_config MODULE ${kwin_startupfeedback_busy_cursor_config_SRCS})
target_link_libraries(kwin_startupfeedback_busy_cursor_config
    KF6::KCMUtils
    KF6::CoreAddons
    KF6::I18n
    Qt6::DBus
)

install(TARGETS kwin_startupfeedback_busy_cursor_config DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin/effects/configs)
