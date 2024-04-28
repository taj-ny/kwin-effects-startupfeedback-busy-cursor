/*
    SPDX-FileCopyrightText: 2010 Fredrik Höglund <fredrik@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "ui_startupfeedback_config.h"
#include <KCModule>

namespace KWin
{

class StartupFeedbackEffectConfig : public KCModule
{
    Q_OBJECT

public:
    explicit StartupFeedbackEffectConfig(QObject *parent, const KPluginMetaData &data);
    ~StartupFeedbackEffectConfig() override;

    void save() override;

private:
    ::Ui::StartupFeedbackEffectConfig ui;
};

} // namespace KWin
