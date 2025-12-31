/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Mock WGC Backend for Testing
 *  Story 1.1 - Test Double
 */

#include "WGCCaptureMock.h"
#include <QThread>
#include <QDebug>

WGCCaptureMock* WGCCaptureMock::mockInstance()
{
    static WGCCaptureMock instance;
    return &instance;
}

WGCCaptureMock::WGCCaptureMock(QObject *parent)
    : WGCCapture(parent)
    , m_simulatedLatency(50) // Default to 50ms (well under 1s requirement)
    , m_simulateFailure(false)
{
}

void WGCCaptureMock::setSimulatedLatency(qint64 latencyMs)
{
    m_simulatedLatency = latencyMs;
}

void WGCCaptureMock::setSimulateFailure(bool shouldFail)
{
    m_simulateFailure = shouldFail;
}

QImage WGCCaptureMock::createTestImage(int width, int height, const QColor &color)
{
    QImage image(width, height, QImage::Format_RGB32);
    image.fill(color);
    return image;
}

QImage WGCCaptureMock::captureFrameWithLatency(qint64 *latencyMs)
{
    if (m_simulateFailure) {
        qWarning("Mock WGC: Simulating capture failure");
        if (latencyMs) *latencyMs = -1;
        Q_EMIT error(u"Simulated capture failure"_s);
        return QImage();
    }

    // Simulate capture latency
    if (m_simulatedLatency > 0) {
        QThread::msleep(m_simulatedLatency);
    }

    // Create test image (1920x1080 red image)
    QImage testImage = createTestImage(1920, 1080, Qt::red);

    qint64 actualLatency = m_simulatedLatency;
    if (latencyMs) *latencyMs = actualLatency;

    qDebug("Mock WGC: Captured test image (%dx%d) in %lld ms",
           testImage.width(), testImage.height(), actualLatency);

    // Verify performance requirement
    if (actualLatency >= 1000) {
        qWarning("Mock WGC: Latency exceeds 1 second: %lld ms", actualLatency);
        Q_EMIT error(u"Capture latency exceeds 1 second"_s);
    }

    Q_EMIT frameCaptured(testImage);
    return testImage;
}

#include "moc_WGCCaptureMock.cpp"
