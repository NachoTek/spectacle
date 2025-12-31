/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Windows Graphics Capture Backend
 *  Story 1.1 - Screen Capture Implementation
 */

#pragma once

#include <QObject>
#include <QImage>
#include <QElapsedTimer>
#include <windows.graphics.capture.h>

/**
 * @brief Windows Graphics Capture (WGC) backend for screen capture
 *
 * This class wraps the Windows.Graphics.Capture API for high-performance
 * screen capture on Windows 11.
 */
class WGCCapture : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get singleton instance
     * @return WGCCapture instance
     */
    static WGCCapture* instance();

    /**
     * @brief Initialize WGC capture session
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Capture frame from screen
     * @return Captured image (null if failed)
     */
    QImage captureFrame();

    /**
     * @brief Capture frame with latency tracking
     * @param latencyMs Output parameter for capture latency in milliseconds
     * @return Captured image (null if failed)
     */
    QImage captureFrameWithLatency(qint64 *latencyMs);

    /**
     * @brief Get last capture latency
     * @return Latency in milliseconds
     */
    qint64 lastLatency() const { return m_lastLatency; }

    /**
     * @brief Check if WGC is available on this system
     * @return true if WGC API is available
     */
    bool isAvailable() const { return m_available; }

    /**
     * @brief Check if session is initialized
     * @return true if initialized
     */
    bool isInitialized() const { return m_initialized; }

Q_SIGNALS:
    /**
     * @brief Emitted when frame is captured
     * @param image Captured image
     */
    void frameCaptured(const QImage &image);

    /**
     * @brief Emitted on capture error
     * @param errorMessage Error description
     */
    void error(const QString &errorMessage);

private:
    explicit WGCCapture(QObject *parent = nullptr);
    ~WGCCapture();
    Q_DISABLE_COPY(WGCCapture)

    bool m_available;
    bool m_initialized;
    qint64 m_lastLatency;
    QElapsedTimer m_latencyTimer;

    /**
     * @brief Check WGC API availability
     * @return true if available
     */
    bool checkAvailability();

    /**
     * @brief Create WGC capture session
     * @return true if successful
     */
    bool createCaptureSession();

    /**
     * @brief Cache the capture session for reuse
     */
    void cacheSession();
};

