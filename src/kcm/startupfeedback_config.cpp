/*
    SPDX-FileCopyrightText: 2010 Fredrik Höglund <fredrik@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "startupfeedback_config.h"

//#include <config-kwin.h>

// KConfigSkeleton
#include "startupfeedbackconfig.h"

#include <KPluginFactory>
#include "kwineffects_interface.h"

namespace KWin
{

K_PLUGIN_CLASS(StartupFeedbackEffectConfig)

StartupFeedbackEffectConfig::StartupFeedbackEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    ui.setupUi(widget());
    StartupFeedbackConfig::instance("kwinrc");
    addConfig(StartupFeedbackConfig::self(), widget());
}

StartupFeedbackEffectConfig::~StartupFeedbackEffectConfig()
{
}

void StartupFeedbackEffectConfig::save()
{
    KCModule::save();

    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("startupfeedback_busy_cursor"));
}

} // namespace KWin

#include "startupfeedback_config.moc"

#include "moc_startupfeedback_config.cpp"
