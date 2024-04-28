/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2010 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2020 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "startupfeedback.h"
#include "startupfeedbackconfig.h"
// Qt
#include <QApplication>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QFile>
#include <QPainter>
#include <QSize>
#include <QStandardPaths>
#include <QStyle>
#include <QTimer>
// KDE
#include <KConfigGroup>
#include <KSelectionOwner>
#include <KSharedConfig>
#include <KWindowSystem>
// KWin
#include "cursor.h"
#include "cursorsource.h"
#include "effect/effecthandler.h"
#include "opengl/glutils.h"

// based on StartupId in KRunner by Lubos Lunak
// SPDX-FileCopyrightText: 2001 Lubos Lunak <l.lunak@kde.org>

Q_LOGGING_CATEGORY(KWIN_STARTUPFEEDBACK, "kwin_effect_startupfeedback", QtWarningMsg)

static void ensureResources()
{
    // Must initialize resources manually because the effect is a static lib.
    Q_INIT_RESOURCE(startupfeedback);
}

namespace KWin
{
static const int s_startupDefaultTimeout = 5;

StartupFeedbackEffect::StartupFeedbackEffect()
    : m_noFeedback(false)
    , m_startupInfo(new KStartupInfo(KStartupInfo::CleanOnCantDetect, this))
    , m_selection(nullptr)
    , m_active(false)
    , m_configWatcher(KConfigWatcher::create(KSharedConfig::openConfig("klaunchrc", KConfig::NoGlobals)))
    , m_splashVisible(false)
{
    StartupFeedbackConfig::instance(effects->config());
    //ensureResources();

    // TODO: move somewhere that is x11-specific
    if (KWindowSystem::isPlatformX11()) {
        m_selection = new KSelectionOwner("_KDE_STARTUP_FEEDBACK", effects->xcbConnection(), effects->x11RootWindow(), this);
        m_selection->claim(true);
    }
    connect(m_startupInfo, &KStartupInfo::gotNewStartup, this, [](const KStartupInfoId &id, const KStartupInfoData &data) {
        const auto icon = QIcon::fromTheme(data.findIcon(), QIcon::fromTheme(QStringLiteral("system-run")));
        Q_EMIT effects->startupAdded(id.id(), icon);
    });
    connect(m_startupInfo, &KStartupInfo::gotRemoveStartup, this, [](const KStartupInfoId &id, const KStartupInfoData &data) {
        Q_EMIT effects->startupRemoved(id.id());
    });
    connect(m_startupInfo, &KStartupInfo::gotStartupChange, this, [](const KStartupInfoId &id, const KStartupInfoData &data) {
        const auto icon = QIcon::fromTheme(data.findIcon(), QIcon::fromTheme(QStringLiteral("system-run")));
        Q_EMIT effects->startupChanged(id.id(), icon);
    });

    connect(effects, &EffectsHandler::startupAdded, this, &StartupFeedbackEffect::gotNewStartup);
    connect(effects, &EffectsHandler::startupRemoved, this, &StartupFeedbackEffect::gotRemoveStartup);
    connect(effects, &EffectsHandler::startupChanged, this, &StartupFeedbackEffect::gotStartupChange);

    connect(m_configWatcher.data(), &KConfigWatcher::configChanged, this, [this]() {
        reconfigure(ReconfigureAll);
    });
    reconfigure(ReconfigureAll);

    m_splashVisible = QDBusConnection::sessionBus().interface()->isServiceRegistered(QStringLiteral("org.kde.KSplash"));
    auto serviceWatcher = new QDBusServiceWatcher(QStringLiteral("org.kde.KSplash"), QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange, this);
    connect(serviceWatcher, &QDBusServiceWatcher::serviceRegistered, this, [this] {
        m_splashVisible = true;
        stop();
    });
    connect(serviceWatcher, &QDBusServiceWatcher::serviceUnregistered, this, [this] {
        m_splashVisible = false;
        gotRemoveStartup({}); // Start the next feedback
    });
}

bool StartupFeedbackEffect::supported()
{
    return true;
}

void StartupFeedbackEffect::reconfigure(Effect::ReconfigureFlags flags)
{
    StartupFeedbackConfig::self()->read();
    m_useWaitCursor = StartupFeedbackConfig::useWaitCursor();

    KConfigGroup c = m_configWatcher->config()->group(QStringLiteral("FeedbackStyle"));
    m_noFeedback = !c.readEntry("BusyCursor", true);

    c = m_configWatcher->config()->group(QStringLiteral("BusyCursorSettings"));
    m_timeout = std::chrono::seconds(c.readEntry("Timeout", s_startupDefaultTimeout));
    m_startupInfo->setTimeout(m_timeout.count());

    if (m_active) {
        stop();
        start(m_startups[m_currentStartup]);
    }
}

void StartupFeedbackEffect::prePaintScreen(ScreenPrePaintData &data, std::chrono::milliseconds presentTime)
{
    effects->prePaintScreen(data, presentTime);

    if (m_active) {
        if (effects->isCursorHidden()) {
            stop();
        } else {
            setCursorShape(m_useWaitCursor ? Qt::CursorShape::WaitCursor : Qt::CursorShape::BusyCursor);
        }
    }
}

void StartupFeedbackEffect::gotNewStartup(const QString &id, const QIcon &icon)
{
    Startup &startup = m_startups[id];

    startup.expiredTimer = std::make_unique<QTimer>();
    // Stop the animation if the startup doesn't finish within reasonable interval.
    connect(startup.expiredTimer.get(), &QTimer::timeout, this, [this, id]() {
        gotRemoveStartup(id);
    });
    startup.expiredTimer->setSingleShot(true);
    startup.expiredTimer->start(m_timeout);

    m_currentStartup = id;
    start(startup);
}

void StartupFeedbackEffect::gotRemoveStartup(const QString &id)
{
    m_startups.remove(id);
    if (m_startups.isEmpty()) {
        m_currentStartup.clear();
        stop();
        return;
    }
    m_currentStartup = m_startups.begin().key();
    start(m_startups[m_currentStartup]);
}

void StartupFeedbackEffect::gotStartupChange(const QString &id, const QIcon &icon)
{
    if (m_currentStartup == id) {
        Startup &currentStartup = m_startups[m_currentStartup];
        start(currentStartup);
    }
}

void StartupFeedbackEffect::start(const Startup &startup)
{
    if (!m_noFeedback || m_splashVisible || effects->isCursorHidden()) {
        return;
    }

    const Output *output = effects->screenAt(effects->cursorPos().toPoint());
    if (!output) {
        return;
    }

    m_active = true;
    setCursorShape(m_useWaitCursor ? Qt::CursorShape::WaitCursor : Qt::CursorShape::BusyCursor);
}

void StartupFeedbackEffect::stop()
{
    m_active = false;
    setCursorShape(Qt::CursorShape::ArrowCursor);

}

void StartupFeedbackEffect::setCursorShape(Qt::CursorShape shape)
{
    if (auto source = qobject_cast<ShapeCursorSource *>(Cursors::self()->mouse()->source())) {
        source->setShape(shape);
    }
}

bool StartupFeedbackEffect::isActive() const
{
    return m_active;
}

} // namespace

#include "moc_startupfeedback.cpp"
