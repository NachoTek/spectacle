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
        UuidRole,
        SelectedRole  // Task 7: Selection state for editing
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

    /**
     * @brief Select annotation by UUID
     */
    Q_INVOKABLE void selectAnnotation(const QUuid &uuid);

    /**
     * @brief Deselect all annotations
     */
    Q_INVOKABLE void clearSelection();

    /**
     * @brief Get selected annotation UUID
     */
    Q_INVOKABLE QUuid selectedAnnotation() const { return m_selectedUuid; }

    /**
     * @brief Delete selected annotation
     */
    Q_INVOKABLE void deleteSelected();

    /**
     * @brief Move selected annotation by delta
     */
    Q_INVOKABLE void moveSelected(int dx, int dy);

private:
    QVector<Annotation> m_annotations;
    QUuid m_selectedUuid;  // Task 7: Track selected annotation
};

#endif // Q_OS_WIN

#endif // ANNOTATIONLISTMODEL_H
