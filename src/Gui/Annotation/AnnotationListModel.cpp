/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Annotation List Model for QML
 *  Story 1.3 - Pre-Capture Annotation Tools
 */

#include "AnnotationListModel.h"

#ifdef Q_OS_WIN

AnnotationListModel::AnnotationListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

AnnotationListModel::~AnnotationListModel() = default;

int AnnotationListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_annotations.count();
}

QVariant AnnotationListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_annotations.count()) {
        return QVariant();
    }

    const Annotation &annotation = m_annotations.at(index.row());

    switch (role) {
    case TypeRole:
        return QVariant::fromValue(static_cast<int>(annotation.type()));
    case ColorRole:
        return annotation.color();
    case StrokeWidthRole:
        return annotation.strokeWidth();
    case PointsRole:
        return QVariant::fromValue(annotation.points());
    case BoundingBoxRole:
        return annotation.boundingBox();
    case UuidRole:
        return annotation.uuid().toString();
    case SelectedRole:  // Task 7: Selection state
        return annotation.isSelected();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> AnnotationListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TypeRole] = "type";
    roles[ColorRole] = "color";
    roles[StrokeWidthRole] = "strokeWidth";
    roles[PointsRole] = "points";
    roles[BoundingBoxRole] = "boundingBox";
    roles[UuidRole] = "uuid";
    roles[SelectedRole] = "selected";  // Task 7: Selection state
    return roles;
}

void AnnotationListModel::addAnnotation(const Annotation &annotation)
{
    beginInsertRows(QModelIndex(), m_annotations.count(), m_annotations.count());
    m_annotations.append(annotation);
    endInsertRows();
}

void AnnotationListModel::removeAnnotation(const QUuid &uuid)
{
    for (int i = 0; i < m_annotations.count(); ++i) {
        if (m_annotations.at(i).uuid() == uuid) {
            beginRemoveRows(QModelIndex(), i, i);
            m_annotations.removeAt(i);
            endRemoveRows();
            return;
        }
    }
}

void AnnotationListModel::undoLast()
{
    if (m_annotations.isEmpty()) {
        return;
    }

    beginRemoveRows(QModelIndex(), m_annotations.count() - 1, m_annotations.count() - 1);
    m_annotations.removeLast();
    endRemoveRows();
}

void AnnotationListModel::clear()
{
    beginResetModel();
    m_annotations.clear();
    endResetModel();
}

Annotation AnnotationListModel::getAnnotation(int index) const
{
    if (index < 0 || index >= m_annotations.count()) {
        return Annotation(AnnotationTool::None);
    }
    return m_annotations.at(index);
}

// Task 7: Selection and editing methods

void AnnotationListModel::selectAnnotation(const QUuid &uuid)
{
    for (int i = 0; i < m_annotations.count(); ++i) {
        if (m_annotations.at(i).uuid() == uuid) {
            // Deselect previous
            if (!m_selectedUuid.isNull()) {
                for (int j = 0; j < m_annotations.count(); ++j) {
                    if (m_annotations[j].uuid() == m_selectedUuid) {
                        m_annotations[j].setSelected(false);
                        QModelIndex idx = index(j);
                        Q_EMIT dataChanged(idx, idx, {SelectedRole});
                        break;
                    }
                }
            }

            // Select new
            m_annotations[i].setSelected(true);
            m_selectedUuid = uuid;
            QModelIndex idx = index(i);
            Q_EMIT dataChanged(idx, idx, {SelectedRole});
            qDebug("Selected annotation: %s", qUtf8Printable(uuid.toString()));
            return;
        }
    }
}

void AnnotationListModel::clearSelection()
{
    if (m_selectedUuid.isNull()) {
        return;
    }

    for (int i = 0; i < m_annotations.count(); ++i) {
        if (m_annotations.at(i).isSelected()) {
            m_annotations[i].setSelected(false);
            QModelIndex idx = index(i);
            Q_EMIT dataChanged(idx, idx, {SelectedRole});
        }
    }

    m_selectedUuid = QUuid();
    qDebug("Cleared annotation selection");
}

void AnnotationListModel::deleteSelected()
{
    if (m_selectedUuid.isNull()) {
        qWarning("No annotation selected for deletion");
        return;
    }

    for (int i = 0; i < m_annotations.count(); ++i) {
        if (m_annotations.at(i).uuid() == m_selectedUuid) {
            beginRemoveRows(QModelIndex(), i, i);
            m_annotations.removeAt(i);
            endRemoveRows();
            m_selectedUuid = QUuid();
            qDebug("Deleted selected annotation at index %d", i);
            return;
        }
    }

    qWarning("Selected annotation not found for deletion");
}

void AnnotationListModel::moveSelected(int dx, int dy)
{
    if (m_selectedUuid.isNull()) {
        qWarning("No annotation selected for moving");
        return;
    }

    for (int i = 0; i < m_annotations.count(); ++i) {
        if (m_annotations.at(i).uuid() == m_selectedUuid) {
            m_annotations[i].translate(dx, dy);
            QModelIndex idx = index(i);
            Q_EMIT dataChanged(idx, idx, {PointsRole, BoundingBoxRole});
            qDebug("Moved selected annotation by (%d, %d)", dx, dy);
            return;
        }
    }

    qWarning("Selected annotation not found for moving");
}

#endif // Q_OS_WIN
