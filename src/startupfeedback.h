/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2010 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "effect/effect.h"
#include <KConfigWatcher>
#include <KStartupInfo>
#include <QObject>

#include <chrono>

class KSelectionOwner;
namespace KWin
{

class GLShader;
class GLTexture;

class StartupFeedbackEffect
    : public Effect
{
    Q_OBJECT
public:
    StartupFeedbackEffect();

    void reconfigure(ReconfigureFlags flags) override;
    void prePaintScreen(ScreenPrePaintData &data, std::chrono::milliseconds presentTime) override;
    bool isActive() const override;

    int requestedEffectChainPosition() const override
    {
        return 90;
    }

    static bool supported();

private Q_SLOTS:
    void gotNewStartup(const QString &id, const QIcon &icon);
    void gotRemoveStartup(const QString &id);
    void gotStartupChange(const QString &id, const QIcon &icon);

private:
    struct Startup
    {
        std::shared_ptr<QTimer> expiredTimer;
    };

    void start(const Startup &startup);
    void stop();
    void setCursorShape(Qt::CursorShape shape);

    bool m_noFeedback;
    bool m_useWaitCursor;
    KStartupInfo *m_startupInfo;
    KSelectionOwner *m_selection;
    QString m_currentStartup;
    QMap<QString, Startup> m_startups;
    bool m_active;
    KConfigWatcher::Ptr m_configWatcher;
    bool m_splashVisible;
    std::chrono::seconds m_timeout;
};
} // namespace
