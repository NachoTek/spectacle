/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Captured Image Provider
 *  Story 1.4 - CRITICAL #3 Fix: QML Image Provider
 */

#include "CapturedImageProvider.h"

CapturedImageProvider::CapturedImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage CapturedImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    if (m_images.contains(id)) {
        QImage img = m_images.value(id);

        if (size) {
            *size = img.size();
        }

        if (requestedSize.width() > 0 && requestedSize.height() > 0) {
            return img.scaled(requestedSize, Qt::KeepAspectRatio);
        }

        return img;
    }

    return QImage();
}

void CapturedImageProvider::setImage(const QString &id, const QImage &image)
{
    m_images.insert(id, image);
}
