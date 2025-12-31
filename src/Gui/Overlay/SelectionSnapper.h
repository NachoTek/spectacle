/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Selection Snapper for Bounds Snapping
 *  Story 1.2 - Selection Targeting + Refinement
 */

#pragma once

#include <QObject>
#include <QRect>
#include <QPropertyAnimation>
#include <QTimer>

/**
 * @brief Handles selection snapping with smooth animations
 *
 * Provides click-to-snap functionality:
 * - Snaps selection to window bounds with animation
 * - Debounces rapid clicks (<50ms)
 * - Smooth 150ms transition animation
 */
class SelectionSnapper : public QObject
{
    Q_OBJECT

public:
    explicit SelectionSnapper(QObject *parent = nullptr);
    ~SelectionSnapper();

    /**
     * @brief Snap selection to target bounds with animation
     * @param currentBounds Current selection rectangle
     * @param targetBounds Target window bounds
     */
    void snapTo(const QRect &currentBounds, const QRect &targetBounds);

    /**
     * @brief Cancel in-progress snap animation
     */
    void cancelSnap();

    /**
     * @brief Check if snap is in progress
     * @return true if animating
     */
    bool isSnapping() const;

    /**
     * @brief Get current snapped bounds
     * @return Current bounds (may be mid-animation)
     */
    QRect currentBounds() const;

Q_SIGNALS:
    /**
     * @brief Emitted when snap animation completes
     * @param finalBounds The final snapped bounds
     */
    void snapCompleted(const QRect &finalBounds);

    /**
     * @brief Emitted when bounds change during animation
     * @param newBounds The new bounds
     */
    void boundsChanged(const QRect &newBounds);

private:
    QRect m_currentBounds;
    QPropertyAnimation *m_snapAnimation;
    QTimer *m_debounceTimer;

    /**
     * @brief Initialize animation
     */
    void setupAnimation();
};
