/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Mock WGC Backend for Testing
 *  Story 1.1 - Test Double
 */

#pragma once

#include "WGCCapture.h"
#include <QImage>

/**
 * @brief Mock WGC backend for testing
 *
 * Simulates WGC behavior without actual screen capture.
 * Useful for:
 * - Unit tests without screen dependencies
 * - Performance benchmarking with controlled latency
 * - Error condition testing
 */
class WGCCaptureMock : public WGCCapture
{
    Q_OBJECT

public:
    /**
     * @brief Get mock instance
     * @return Mock instance
     */
    static WGCCaptureMock* mockInstance();

    /**
     * @brief Set simulated capture latency
     * @param latencyMs Latency in milliseconds
     */
    void setSimulatedLatency(qint64 latencyMs);

    /**
     * @brief Set error simulation
     * @param shouldFail true to simulate capture failure
     */
    void setSimulateFailure(bool shouldFail);

    /**
     * @brief Create test image with known properties
     * @param width Image width
     * @param height Image height
     * @param color Fill color
     * @return Test image
     */
    static QImage createTestImage(int width, int height, const QColor &color = Qt::red);

    // Override to use mock behavior
    QImage captureFrameWithLatency(qint64 *latencyMs) override;

private:
    explicit WGCCaptureMock(QObject *parent = nullptr);
    ~WGCCaptureMock() = default;

    qint64 m_simulatedLatency;
    bool m_simulateFailure;
};

