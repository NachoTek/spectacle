/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Selection Snapper for Bounds Snapping
 *  Story 1.2 - Selection Targeting + Refinement
 */

#include "SelectionSnapper.h"

#include <QEasingCurve>
#include <QVariantAnimation>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(LOG_SELECTIONSNAPPER, "spectacle.gui.selectionsnapper")

SelectionSnapper::SelectionSnapper(QObject *parent)
    : QObject(parent)
    , m_snapAnimation(nullptr)
    , m_debounceTimer(new QTimer(this))
{
    // Debounce timer: ignore clicks within 50ms
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(50);
}

SelectionSnapper::~SelectionSnapper()
{
    delete m_snapAnimation;
}

void SelectionSnapper::snapTo(const QRect &currentBounds, const QRect &targetBounds)
{
    // Ignore if debounce timer is active (rapid click prevention)
    if (m_debounceTimer->isActive()) {
        qCDebug(LOG_SELECTIONSNAPPER) << "Snap ignored (debounce active)";
        return;
    }

    // Start debounce timer
    m_debounceTimer->start();

    // Cancel any existing animation
    cancelSnap();

    // Create variant animation for smooth transition
    m_snapAnimation = new QVariantAnimation(this);
    m_snapAnimation->setDuration(150); // 150ms smooth transition
    m_snapAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    m_snapAnimation->setStartValue(QVariant::fromValue(currentBounds));
    m_snapAnimation->setEndValue(QVariant::fromValue(targetBounds));

    connect(m_snapAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        m_currentBounds = value.value<QRect>();
        Q_EMIT boundsChanged(m_currentBounds);
    });

    connect(m_snapAnimation, &QVariantAnimation::finished, this, [this]() {
        Q_EMIT snapCompleted(m_currentBounds);
    });

    // Start animation
    m_snapAnimation->start();

    qCDebug(LOG_SELECTIONSNAPPER) << "Snap animation started:" << currentBounds << "->" << targetBounds;
}

void SelectionSnapper::cancelSnap()
{
    if (m_snapAnimation) {
        if (m_snapAnimation->state() == QVariantAnimation::Running) {
            m_snapAnimation->stop();
        }
        delete m_snapAnimation;
        m_snapAnimation = nullptr;
    }
}

bool SelectionSnapper::isSnapping() const
{
    return m_snapAnimation && m_snapAnimation->state() == QVariantAnimation::Running;
}

QRect SelectionSnapper::currentBounds() const
{
    return m_currentBounds;
}
