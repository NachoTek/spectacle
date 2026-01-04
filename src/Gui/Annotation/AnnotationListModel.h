/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Annotation List Model for QML
 *  Story 1.3 - Pre-Capture Annotation Tools
 */

#ifndef ANNOTATIONLISTMODEL_H
#define ANNOTATIONLISTMODEL_H

#include "Annotation.h"

#include <QAbstractListModel>
#include <QVector>

#ifdef Q_OS_WIN

/**
 * @brief QAbstractListModel for managing annotations in QML
 *
 * Exposes annotation list to QML with roles for type, color, strokeWidth,
 * points, and bounding box. Supports add, remove, undo operations.
 */
class AnnotationListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        TypeRole = Qt::UserRole + 1,
        ColorRole,
        StrokeWidthRole,
        PointsRole,
        BoundingBoxRole,
        UuidRole
    };

    explicit AnnotationListModel(QObject *parent = nullptr);
    ~AnnotationListModel() override;

    /**
     * @brief Get number of annotations
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Get data for specific role
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Get role names for QML
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Add annotation to model
     */
    Q_INVOKABLE void addAnnotation(const Annotation &annotation);

    /**
     * @brief Remove annotation by UUID
     */
    Q_INVOKABLE void removeAnnotation(const QUuid &uuid);

    /**
     * @brief Remove last annotation (undo)
     */
    Q_INVOKABLE void undoLast();

    /**
     * @brief Clear all annotations
     */
    Q_INVOKABLE void clear();

    /**
     * @brief Get annotation at index
     */
    Q_INVOKABLE Annotation getAnnotation(int index) const;

private:
    QVector<Annotation> m_annotations;
};

#endif // Q_OS_WIN

#endif // ANNOTATIONLISTMODEL_H
