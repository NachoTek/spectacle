/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Captured Image Provider
 *  Story 1.4 - CRITICAL #3 Fix: QML Image Provider
 */

#ifndef CAPTUREDIMAGEPROVIDER_H
#define CAPTUREDIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>
#include <QMap>

/**
 * @brief Image provider for QML to access captured screenshots
 *
 * This QQuickImageProvider allows QML components to display
 * captured images via the "image://capture/" URL scheme.
 */
class CapturedImageProvider : public QQuickImageProvider
{
public:
    /**
     * @brief Constructor
     */
    CapturedImageProvider();

    /**
     * @brief Request image by ID
     * @param id Unique identifier for the image
     * @param size Output parameter for image size
     * @param requestedSize Optional size to scale the image to
     * @return The requested image, or null image if not found
     */
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

    /**
     * @brief Store an image with a unique ID
     * @param id Unique identifier for this image
     * @param image The image to store
     */
    void setImage(const QString &id, const QImage &image);

private:
    QMap<QString, QImage> m_images;
};

#endif // CAPTUREDIMAGEPROVIDER_H
