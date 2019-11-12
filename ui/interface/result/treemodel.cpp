/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
    treemodel.cpp

    Provides a simple tree model to show how to create and use hierarchical
    models.
*/
#include "treemodel.h"
#include <QStringList>
#include <QJsonObject>
#include <QDir>
#include <algorithm>
#include "treeitem.h"
#include "erroritem.h"
#include "helper.h"

using namespace CC;
//! [0]
TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_roleNameMapping[static_cast<int>(TreeModelRoles::file)] = CONST_file.toUtf8();
    m_roleNameMapping[static_cast<int>(TreeModelRoles::iconFile)] = CONST_iconFile.toUtf8();
    m_roleNameMapping[static_cast<int>(TreeModelRoles::severity)] = CONST_severity.toUtf8();
    m_roleNameMapping[static_cast<int>(TreeModelRoles::severityStr)] = CONST_severityStr.toUtf8();
    m_roleNameMapping[static_cast<int>(TreeModelRoles::line)] = CONST_line.toUtf8();
    m_roleNameMapping[static_cast<int>(TreeModelRoles::id)] = CONST_id.toUtf8();
    m_roleNameMapping[static_cast<int>(TreeModelRoles::summary)] = CONST_summary.toUtf8();
    m_roleNameMapping[static_cast<int>(TreeModelRoles::hide)] = CONST_hide.toUtf8();
    m_roleNameMapping[static_cast<int>(TreeModelRoles::message)] = CONST_message.toUtf8();
    m_roleNameMapping[static_cast<int>(TreeModelRoles::column)] = CONST_column.toUtf8();
    m_roleNameMapping[static_cast<int>(TreeModelRoles::inconclusive)] = CONST_inconclusive.toUtf8();
    m_roleNameMapping[static_cast<int>(TreeModelRoles::file0)] = CONST_file0.toUtf8();
    m_roleNameMapping[static_cast<int>(TreeModelRoles::sinceDate)] = CONST_sinceDate.toUtf8();
    m_roleNameMapping[static_cast<int>(TreeModelRoles::tags)] = CONST_tags.toUtf8();

    QMap<QString,QVariant> rootData;
    rootData.insert(CONST_file,CONST_file);
    rootData[CONST_iconFile] = QJsonObject({{"file","file1.cpp"},{"icon","qrc:/images/language-cpp.png"}});    rootData.insert(CONST_severity,CONST_severity);
    rootData.insert(CONST_severityStr,CONST_severityStr);
    rootData.insert(CONST_line,CONST_line);
    rootData.insert(CONST_id,CONST_id);
    rootData.insert(CONST_summary,CONST_summary);
    rootData.insert(CONST_hide,CONST_hide);
    rootData.insert(CONST_message,CONST_message);
    rootData.insert(CONST_column,CONST_column);
    rootData.insert(CONST_inconclusive,CONST_inconclusive);
    rootData.insert(CONST_file0,CONST_file0);
    rootData.insert(CONST_sinceDate,CONST_sinceDate);
    rootData.insert(CONST_tags,CONST_tags);
    rootItem = new TreeItem(rootData);
}

void TreeModel::BeginResetModel()
{
    this->beginResetModel();
}

void TreeModel::EndResetModel()
{
    this->endResetModel();

}
//! [0]

//! [1]
TreeModel::~TreeModel()
{
    delete rootItem;
}
//! [1]

//! [2]
int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}
//! [2]

//! [3]
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (!m_roleNameMapping.keys().contains(role)) {
        return QVariant();
    }
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    return item->data(m_roleNameMapping.value(role));
}
//! [3]

//! [4]
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}
//! [4]

//! [5]
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case static_cast<int>(TreeModelColumn::iconFile):
            return rootItem->data(CONST_file);
        case static_cast<int>(TreeModelColumn::severityStr):
            return rootItem->data(CONST_severityStr);
        case static_cast<int>(TreeModelColumn::line):
            return rootItem->data(CONST_line);
        case static_cast<int>(TreeModelColumn::id):
            return rootItem->data(CONST_id);
        case static_cast<int>(TreeModelColumn::summary):
            return rootItem->data(CONST_summary);
        }
    }
    return QVariant();
}
//! [5]

//! [6]
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
//! [6]

//! [7]
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
//! [7]

//! [8]
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}
//! [8]

QHash<int, QByteArray> TreeModel::roleNames() const
{
    return m_roleNameMapping;
}

TreeModel *TreeModel::instance()
{
    static TreeModel model;
    return &model;
}

bool TreeModel::addErrorItem(const ErrorItem &item)
{
    bool ret = addErrorItemExec(item);
    return ret;
}

bool TreeModel::addErrorItemExec(const ErrorItem &item)
{
    if (item.errorPath.isEmpty()) {
        return false;
    }
    const QErrorPathItem &loc = item.errorId.startsWith("clang") ? item.errorPath.front() : item.errorPath.back();
    QString relativeFile = Helper::GetStripPath(loc.file);
    if (relativeFile.isEmpty()) {
        relativeFile = tr("Undefined file");
    }
    bool hide = false;
    if (mHiddenMessageId.contains(item.errorId)) {
        hide = true;
    }
    if (!hide && !mFilter.isEmpty()) {
        if (!item.summary.contains(mFilter, Qt::CaseInsensitive) &&
            !item.message.contains(mFilter, Qt::CaseInsensitive) &&
            !item.errorPath.back().file.contains(mFilter, Qt::CaseInsensitive) &&
            !item.errorId.contains(mFilter, Qt::CaseInsensitive)) {
            hide = true;
        }
    }
    if (!hide) {
        mHasVisibleErrors = true;
    }
    TreeItem *fileItem = nullptr;
    for(const auto & child : rootItem->children()) {
            if(child->data(CONST_file) == relativeFile){
                fileItem =  child;
            }
    }

    if(!fileItem) {
        QMap<QString,QVariant> fileItemData;
        fileItemData.insert(CONST_iconFile, QJsonObject({{"file",relativeFile},{"icon","qrc:/images/language-cpp.png"}}));
        fileItemData.insert(CONST_severityStr,"");
        fileItemData.insert(CONST_line,"");
        fileItemData.insert(CONST_id,"");
        fileItemData.insert(CONST_summary,"");
        fileItem = new TreeItem(fileItemData,rootItem);
        rootItem->appendChild(fileItem);
    }
    QMap<QString,QVariant> errorItemData;
    errorItemData.insert(CONST_file,             relativeFile);
    errorItemData.insert(CONST_severity,     item.severity);
    errorItemData.insert(CONST_line,             loc.line);
    errorItemData.insert(CONST_id,               item.errorId);
    errorItemData.insert(CONST_summary,  item.summary);
    errorItemData.insert(CONST_hide,         hide);
    errorItemData.insert(CONST_message,  item.message);
    errorItemData.insert(CONST_column,       "");
    errorItemData.insert(CONST_inconclusive,     item.inconclusive);
    errorItemData.insert(CONST_file0,            item.file0);
    errorItemData.insert(CONST_sinceDate,    item.sinceDate);
    errorItemData.insert(CONST_tags,             item.tags);
    QString icon;
    QString severityStr;
    Helper::Severity2Icon(item.severity,icon,severityStr);
    errorItemData.insert(CONST_iconFile,QJsonObject({{"file",relativeFile},{"icon",icon}}));
    errorItemData.insert(CONST_severityStr,severityStr);
    TreeItem *errorItem = checkExistingItem(fileItem->children(),errorItemData);
    if(!errorItem) {
        errorItem = new TreeItem(errorItemData, fileItem);
        fileItem->appendChild(errorItem);
    }
    if (item.errorPath.size() > 1) {
        for (int i = 0; i < item.errorPath.size(); i++) {
            const QErrorPathItem &e = item.errorPath[i];
            QMap<QString,QVariant> pathItemData;
            pathItemData[CONST_file] = e.file;
            pathItemData[CONST_line] = e.line;
            pathItemData[CONST_column] = e.column;
            pathItemData[CONST_message] = e.info;
            pathItemData[CONST_summary] = e.info;
            pathItemData[CONST_severityStr] = "note";
            pathItemData.insert(CONST_id,"");
            pathItemData.insert(CONST_iconFile,QJsonObject({{"file",e.file},{"icon","qrc:/old/go-down.png"}}));
            TreeItem *pathItem = checkExistingItem(errorItem->children(),pathItemData);
            if (!pathItem) {
                pathItem = new TreeItem(pathItemData,errorItem);
                errorItem->appendChild(pathItem);
            }
        }
    }
    return true;
}

TreeItem *TreeModel::checkExistingItem(QList<TreeItem *> children, const QMap<QString, QVariant> &data)
{
    TreeItem *item = nullptr;
//    if(children.count()) {
//        std::for_each(children.begin(),children.end(),[&item,data](TreeItem *child) {
//           if((child->data(CONST_line) == data[CONST_line])
//                   && (child->data(CONST_severity) == data[CONST_severity])
//                   && (child->data(CONST_summary) == data[CONST_summary])) {
//               item = child;
//           }
//        });
//    }
    for(const auto & child : children) {
           if((child->data(CONST_line) == data[CONST_line])
                   && (child->data(CONST_severity) == data[CONST_severity])
                   && (child->data(CONST_summary) == data[CONST_summary])) {
               item = child;
           }
    }

    return item;
}
