/* ===================================================================
* Copyright (C) 2023 Hefei Jiushao Intelligent Technology Co., Ltd. 
* All rights reserved.
*
* This software is licensed under the GNU Affero General Public License 
* v3.0 (AGPLv3.0) or a commercial license. You may choose to use this 
* software under the terms of either license.
*
* For more information about the AGPLv3.0 license, please visit: 
* https://www.gnu.org/licenses/agpl-3.0.html
* For licensing inquiries or to obtain a commercial license, please 
* contact Hefei Jiushao Intelligent Technology Co., Ltd.
* ===================================================================
* Author: 
*/
#ifndef OPERATETOOLBAR_H
#define OPERATETOOLBAR_H

#include <utility>

// QWidget
#include <QTabWidget>
#include <QWidget>
#include <QToolBar>

// QObject
#include <QVector>
#include <QAction>
#include <QString>
#include <QList>
#include <QDebug>

#include "../Core/CoreDefine.h"

namespace acamcad
{
class RenderViewer;
class AMCore;
class OperateToolBar;

class ToolBarTabAction {
private:
    QString actionText;
    QString actionStatusTip;
    QString actionIcon;

public:
    void (RenderViewer::*actionSlot)();
    bool checkable;
    SelectModel selectModel;

public:
    ToolBarTabAction(const QString &aText, const QString &aStatusTip = QString(""), 
                const QString &aIcon = QString(""), void (RenderViewer::*aSlot)() = nullptr, 
                bool checkable = false, SelectModel selectModel=SelectModel::VERTEX_MODEL):
        actionText(aText), actionStatusTip(aStatusTip), 
        actionIcon(aIcon), actionSlot(aSlot), checkable(checkable),
        selectModel(selectModel) {}

public:
    QAction *toQAction() const {
        auto *action = actionText.length() ? new QAction(actionText) : nullptr;
        if (action) {
            if (actionStatusTip.length()) {
                action->setStatusTip(actionStatusTip);
            }
            if (actionIcon.length()) {
                action->setIcon(QIcon(actionIcon));
            }
            action->setCheckable(checkable);
        }
        return action;
    }
}; // ToolBarTabAction

class ToolBarTabFillOut
{
public:
    const QString name;
    const QString title;

    const QVector<ToolBarTabAction> actions;

public:
    ToolBarTabFillOut(const QString &name, const QString &title, const QVector<ToolBarTabAction> &actions): 
        name(name), title(title), actions(actions) {}
};



// class Toolbar : public QTabWidget
// {
//     Q_OBJECT

    

// };

class OperateToolBar : public QTabWidget
{
    Q_OBJECT
public:
    OperateToolBar(RenderViewer *viewer, QWidget *parent=nullptr);

private:
    QVector<QToolBar *> toolBars;

private:
    const static QVector<ToolBarTabFillOut> tabs;
    
};
}
#endif // OPERATETOOLBAR_H
