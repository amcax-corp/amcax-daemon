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
#include "ParamDialog.h"

#include <cmath>

#include <QGroupBox>
#include <QLineEdit>
#include <QValidator>
#include <QPushButton>
#include <QMouseEvent>

#include <algorithm>

using namespace acamcad;

void clearLayout(QLayout *layout) 
{
    if (layout) 
    {
        QLayoutItem *item;
        while ((item = layout->takeAt(0))) 
        {
            if (item->layout()) 
            {
                ::clearLayout(item->layout());
                delete item->layout();
            }
            if (item->widget()) 
            {
                delete item->widget();
            }
            delete item;
        }
    }
}

int ShortLineEdit::counter = 0;

void ShortLineEdit::setSuffix(QLabel *label) 
{
    suffix = label;
    suffix->setParent(this);
    connect(this, &ShortLineEdit::textChanged, this, &ShortLineEdit::updateSuffixPos);
}

void ShortLineEdit::updateSuffixPos() 
{
    if (suffix) {
        auto left = rect().right() - suffix->width();
        left = left < cursorRect().left() + 3 ? 0 : left;
        auto top = rect().center().y() - suffix->height() / 2;
        suffix->move(left, top);
    }
}

void ParamGBoxTitleButton::mousePressEvent(QMouseEvent *me)
{
    btns = me->buttons();
    QLabel::mousePressEvent(me);
}

void ParamGBoxTitleButton::mouseReleaseEvent(QMouseEvent *me)
{
    if (btns && !(btns & ~Qt::LeftButton)) // only left button was pressed
    {
        auto pos = me->localPos();
        if (pos.x() < width() / 2 + 1)
        {
            emit negative();
        } else
        {
            emit positive();
        }
    }
    QLabel::mouseReleaseEvent(me);
}

void ParamGBoxTitleToggle::mousePressEvent(QMouseEvent *me)
{
    btns = me->buttons();
    QLabel::mousePressEvent(me);
}

void ParamGBoxTitleToggle::mouseReleaseEvent(QMouseEvent *me)
{
    if (btns && !(btns & ~Qt::LeftButton)) // only left button was pressed
    {
        emit clicked();
    }
    QLabel::mouseReleaseEvent(me);
}

void ParamGBoxTitle::setCheckable(bool checkable)
{
    button->setHidden(!checkable);
    // setFixedWidth(label->width() + (checkable ? button->width() : 0) + 6);
}

ParamGBoxTitle::ParamGBoxTitle(const QString &title, QWidget *parent):
    QWidget(parent), title(title), countable(false)
{
    setStyleSheet("font-size: 0.6875em; background-color: #F0F0F0; "
                  "padding: 0em; margin: 0em; border: 0em;"
                  "padding-left: 0.25em; padding-right: 0.25em;");
    
    auto *hbox = new QHBoxLayout(this);
    setLayout(hbox);
    label = new ParamGBoxTitleToggle(title, this);
    button = new ParamGBoxTitleButton("( - | + )", this);
    button->setStyleSheet("font-family: monospace;");

    hbox->addWidget(label, 0);
    hbox->addStretch(1);
    hbox->addWidget(button, 0);
    // setMinimumWidth(label->width());

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

    connect(button, &ParamGBoxTitleButton::positive, this, &ParamGBoxTitle::onPositive);
    connect(button, &ParamGBoxTitleButton::negative, this, &ParamGBoxTitle::onNegative);
    connect(label, &ParamGBoxTitleToggle::clicked, this, &ParamGBoxTitle::onClicked);
}

void ParamGBoxTitle::setCountable()
{
    countable = true;
    label->setText(title + " (1)");
}

void ParamGBoxTitle::setCount(const int count)
{
    if (countable) 
    { 
        label->setText(title + " (" + QString::number(count) + ")"); 
    }
}

ParamGBox::ParamGBox(const QString &title,
                     const ParamDialog::ParamType &type,
                     const QVector<PDRowUnit> &row,
                     const ParamDialog::DataUnit &defaultValue,
                     const bool checkable,
                     const bool checked,
                     QWidget *parent) : 
                     QWidget(parent), templateRow(row), templateDU(defaultValue),
                     type(type), checkable(checkable), checked(checked)
{
    basel = new QVBoxLayout(this);
    setLayout(basel);

    hlinetop = new QFrame(this);
    hlinetop->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    twidget = new ParamGBoxTitle(title, this);
    twidget->setCheckable(checkable);
    basel->addWidget(twidget);

    hlinetop->stackUnder(twidget);

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    addRow(row, defaultValue);

    setChecked(checked);

    connect(twidget, &ParamGBoxTitle::negative, this, &ParamGBox::onNegative);
    connect(twidget, &ParamGBoxTitle::positive, this, &ParamGBox::onPositive);
    connect(twidget, &ParamGBoxTitle::clicked, this, &ParamGBox::toggle);
}

void ParamGBox::addRow(const QVector<PDRowUnit> &row, 
                       const ParamDialog::DataUnit &defaultValue)
{
    auto *hbox = new QHBoxLayout;
    for (auto i = 0; i < row.size(); ++i)
    {
        if (row[i].first.size())
        { // only add a name label if it has a name
            auto *name = new QLabel(row[i].first);
            hbox->addWidget(name, 0);
        } 

        auto *input = createLineEdit(row[i].second);
        auto defText = defaultValue.toString(type, i);

        input->setPlaceholderText(defText);
        input->setText(defText);
        input->setEnabled(checked);
        
        hbox->addWidget(input, 0);
    }
    basel->addLayout(hbox);
}

void ParamGBox::updateTitlePos()
{
    hlinetop->setFixedWidth(width());
    hlinetop->move(0, twidget->height() / 2 - 1);
    // twidget->update();
}

void ParamGBox::setCheckable(bool checkable)
{
    this->checkable = checkable;
    twidget->setCheckable(checkable);
}

void ParamGBox::setChecked(bool checked)
{
    for (auto * input: inputs)
    {
        input->setEnabled(checked);
    }
}

void ParamGBox::setCountable()
{
    twidget->setCountable();
}

bool ParamGBox::isCheckable() const
{
    return checkable; 
}

bool ParamGBox::isChecked() const
{
    return checked;
}

void ParamGBox::resizeEvent(QResizeEvent *re) 
{
    updateTitlePos(); 
    QWidget::resizeEvent(re);
}

void ParamGBox::showEvent(QShowEvent *se) 
{
    updateTitlePos(); 
    QWidget::showEvent(se);
}

void ParamGBox::onNegative()
{
    if (inputs.size() > templateRow.size())
    {
        auto *lastrow = basel->takeAt(basel->count() - 1)->layout();
        while (lastrow->count())
        {
            auto w = lastrow->takeAt(0)->widget();
            lastrow->removeWidget(w);
            w->deleteLater();
        }
        basel->removeItem(lastrow);
        lastrow->deleteLater();
        for (auto i = 0; i < templateRow.size(); ++i)
        {
            inputs.pop_back();
        }
        static_cast<ParamDialog *>(parent())->adjustSize();
        twidget->setCount(inputs.size() / templateRow.size());
    }
}

void ParamGBox::onPositive()
{
    addRow(templateRow, templateDU);
    twidget->setCount(inputs.size() / templateRow.size());
}

void ParamGBox::toggle()
{
    setChecked((checked = !checked));
}

const QMap<ParamDialog::ParamType, QList<QString>> ParamDialog::TypeName {
    { ParamDialog::TYPE_MPOINT, QList<QString>{tr("MPoint_X:"), tr("MPoint_Y:"), tr("MPoint_Z:")} },
    { ParamDialog::TYPE_MPOINT3, QList<QString>{tr("MPoint_X:"), tr("MPoint_Y:"), tr("MPoint_Z:")} },
    { ParamDialog::TYPE_MVEC, QList<QString>{tr("MVec_X:"), tr("MVec_Y:"), tr("MVec_Z:")} },
    { ParamDialog::TYPE_MVEC3, QList<QString>{tr("MVec_X:"), tr("MVec_Y:"), tr("MVec_Z:")} },
};

void ParamDialog::closeEvent(QCloseEvent *ce)
{
    ::clearLayout(inputLayout);
    gboxes.clear();
    tunables.clear();
    QDialog::closeEvent(ce);
}

ParamDialog::ParamDialog(QWidget *parent): QDialog(parent) {
    setWindowTitle(tr("Parameters"));

    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    auto *bLayout = new QVBoxLayout;
    setLayout(bLayout);

    inputLayout = new QVBoxLayout;
    bLayout->addLayout(inputLayout);

    auto *confirm = new QPushButton(tr("&Confirm"));
    auto *cancel = new QPushButton(tr("C&ancel"));

    connect(confirm, &QPushButton::clicked, this, &ParamDialog::accept);
    connect(cancel, &QPushButton::clicked, this, &ParamDialog::reject);
    
    auto *butLayout = new QHBoxLayout;
    bLayout->addStretch(1);
    bLayout->addLayout(butLayout);

    butLayout->addStretch(1);
    butLayout->addWidget(confirm, 0);
    butLayout->addStretch(0);
    butLayout->addWidget(cancel, 0);
    butLayout->addStretch(1);

}

void ParamDialog::addElement(const RequestUnit &param) {
    // Stores default value in the response package and update them later;
    // Make sure correspounding amount of "input" widgets are created and
    // are added to the "inputs" QVector;
    response.append(std::make_pair(param.type, param.def));

    switch (param.type) {
        case TYPE_MPOINT:
        case TYPE_MPOINT3: {
            auto gBox = new ParamGBox(param.title, TYPE_MPOINT3, 
            QVector<PDRowUnit> {
                {TypeName[TYPE_MPOINT3][0], PDEditAttr(INFINITY, INFINITY)},
                {TypeName[TYPE_MPOINT3][1], PDEditAttr(INFINITY, INFINITY)},
                {TypeName[TYPE_MPOINT3][2], PDEditAttr(INFINITY, INFINITY)},
            }, param.def);
            inputLayout->addWidget(gBox, 0);
            gboxes.push_back(gBox);
        }
        break;
        case TYPE_MPOINT3_2: {
            auto gBox = new ParamGBox(param.title, TYPE_MPOINT3_2,
            QVector<PDRowUnit> {
                {TypeName[TYPE_MPOINT3][0], PDEditAttr(INFINITY, INFINITY)},
                {TypeName[TYPE_MPOINT3][1], PDEditAttr(INFINITY, INFINITY)},
            }, param.def);
            inputLayout->addWidget(gBox, 0);
            gboxes.push_back(gBox);
        }
        break;
        case TYPE_ARR_MPOINT3: {
            auto gBox = new ParamGBox(param.title, param.type,
            QVector<PDRowUnit> {
                {TypeName[TYPE_MPOINT3][0], PDEditAttr(INFINITY, INFINITY)},
                {TypeName[TYPE_MPOINT3][1], PDEditAttr(INFINITY, INFINITY)},
                {TypeName[TYPE_MPOINT3][2], PDEditAttr(INFINITY, INFINITY)},
            }, param.def, true, true);
            gBox->setCountable();
            inputLayout->addWidget(gBox, 0);
            gboxes.push_back(gBox);
        }
        break;
        case TYPE_MVEC:
        case TYPE_MVEC3: {
            auto gBox = new ParamGBox(param.title, TYPE_MVEC3,
            QVector<PDRowUnit> {
                {TypeName[TYPE_MVEC3][0], PDEditAttr(INFINITY, INFINITY)},
                {TypeName[TYPE_MVEC3][1], PDEditAttr(INFINITY, INFINITY)},
                {TypeName[TYPE_MVEC3][2], PDEditAttr(INFINITY, INFINITY)},
            }, param.def);
            inputLayout->addWidget(gBox, 0);
            gboxes.push_back(gBox);
        }
        break;
        case TYPE_MVEC3_2: {
            auto gBox = new ParamGBox(param.title, param,
            QVector<PDRowUnit> {
                {TypeName[TYPE_MVEC3][0], PDEditAttr(INFINITY, INFINITY)},
                {TypeName[TYPE_MVEC3][1], PDEditAttr(INFINITY, INFINITY)},
            }, param.def);
            inputLayout->addWidget(gBox, 0);
            gboxes.push_back(gBox);
        }
        break;
        case TYPE_DOUBLE: {
            auto gBox = new ParamGBox(param.title, param,
            QVector<PDRowUnit> {
                {"", PDEditAttr(param.def.d.bottom, param.def.d.top)},
            }, param.def);
            inputLayout->addWidget(gBox, 0);
            gboxes.push_back(gBox);
        }
        break;
        case TYPE_DOUBLE_PI: {
            auto gBox = new ParamGBox(param.title, param,
            QVector<PDRowUnit> {
                {"", PDEditAttr(param.def.d.bottom, param.def.d.top, " Pi Rad ")},
            }, param.def);
            inputLayout->addWidget(gBox, 0);
            gboxes.push_back(gBox);
        }
        break;
        case TYPE_BOOL: {
            // Create all the widgets but only add to the layout
            // when exec() the dialog
            auto *cBox = new QCheckBox(param.title, this);
            // inputs.append(cBox);
            cBox->setCheckable(true);
            cBox->setChecked(param.def.b);
            
            tunables.append(cBox);
        }
        break;
        case TYPE_SIZE_T: {
            auto gBox = new ParamGBox(param.title, TYPE_SIZE_T,
            QVector<PDRowUnit> {
                {"", PDEditAttr(param.def.s.hasBottom, param.def.s.bottom, 
                                param.def.s.hasTop, param.def.s.top)},
            }, param.def);
            inputLayout->addWidget(gBox, 0);
            gboxes.push_back(gBox);
        }
        break;
        case TYPE_STRING: {
            auto gBox = new ParamGBox(param.title, param,
            QVector<PDRowUnit> {
                {"", PDEditAttr()}
            }, param.def);
            inputLayout->addWidget(gBox, 0);
            gboxes.push_back(gBox);
        }
        break;
        default:
        break;
    }
}

void ParamDialog::accept() 
{
    generateResponse();
    emit dataReady(response);
    QDialog::accept();
}

void ParamDialog::reject() 
{
    // Doing nothing
    QDialog::reject();
}

void ParamDialog::open()
{
    resize(1, 1);
    QDialog::open();
}

ShortLineEdit * ParamGBox::createLineEdit(QValidator *validator) {
    auto *input = new ShortLineEdit;
    if (validator) { input->setValidator(validator); }
    
    inputs.push_back(input);
    input->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

    return input;
}

ShortLineEdit * ParamGBox::createDoubleLineEdit(const double bottom,
                                                const double top) {
    auto *v = new QDoubleValidator;
    if (!std::isinf(bottom)) {
        v->setBottom(bottom);
    }
    if (!std::isinf(top)) {
        v->setTop(top);
    }
    return createLineEdit(v);
}

ShortLineEdit * ParamGBox::createDoubleLineEdit(const double bottom,
                                                const double top,
                                                const QString &suffix) {
    auto *edit = createDoubleLineEdit(bottom, top);
    QLabel* label = new QLabel(suffix);
    label->setStyleSheet("font-size:1em;color:red;");
    label->setAutoFillBackground(true);
    edit->setSuffix(label);

    return edit;
}

ShortLineEdit * ParamGBox::createIntLineEdit(const bool hasBottom, const int bottom,
                                             const bool hasTop, const int top) {
    auto *v = new QIntValidator;
    if (hasBottom) {
        v->setBottom(bottom);
    }
    if (hasTop) {
        v->setTop(top);
    }
    return createLineEdit(v);
}

ShortLineEdit * ParamGBox::createLineEdit(const PDEditAttr &unit)
{
    switch (unit.type)
    {
        case PDEditAttr::TYPE_SIZE_T:
        {
            return createIntLineEdit(unit.hasBottom, unit.bottom.s,
                                     unit.hasTop, unit.top.s);
        } break;
        case PDEditAttr::TYPE_DOUBLE:
        {
            if (unit.extra.size())
            {
                return createDoubleLineEdit(unit.bottom.d, 
                                            unit.top.d, 
                                            unit.extra);
            } else
            {
                return createDoubleLineEdit(unit.bottom.d, 
                                            unit.top.d);
            }
        } break;
        case PDEditAttr::TYPE_STRING:
        {
            return createLineEdit(nullptr);
        }
        default:;
    }
    return nullptr;
}


void ParamDialog::addElements(const QVector<RequestUnit> &paramList) {
    ::clearLayout(inputLayout); // just in case
    tunables.clear(); // just in case
    response.clear(); // just in case
    gboxes.clear(); // just in case

    std::for_each(paramList.begin(), paramList.end(), [&](const RequestUnit &param) {
        addElement(param);
    });

    if (tunables.size()) {
        // create groupBox for all the tunables
        auto *tunableBox = new QGroupBox(tr("Tunables"));
        auto *tunableLayout = new QVBoxLayout();
        tunableBox->setLayout(tunableLayout);
        inputLayout->addWidget(tunableBox);
        //do {
        //    auto *cBox = tunables.takeFirst();
        //    tunableLayout->addWidget(cBox);
        //} while (tunables.size());
        for (auto* cBox : tunables) {
            tunableLayout->addWidget(cBox);
        }
    }
}

void ParamDialog::generateResponse() {
    const auto getOne = [](const QLineEdit *input) -> QString {
        int _ = 0; // Required argument for validators but is not actually used
        if (input)
        {
            auto text = input->text();
            auto *vor = input->validator();
            auto valid = vor ? 
                            vor->validate(text, _) == QValidator::Acceptable :
                            true;
            if (valid) { return text; }
            /*else { return input->placeholderText(); } // assume the placeholder is also the default value */
        }
        return QString("");
    };
    const auto getOneDouble = [&getOne](const QLineEdit *input) -> double {
        return getOne(input).toDouble();
    };
    const auto getOneSizeT = [&getOne](const QLineEdit *input) -> size_t {
        return getOne(input).toULongLong();
    };
    const auto getThreeDouble = [&getOneDouble](const std::vector<QLineEdit *> &inputs3) {
        double v[3]{.0};
        for (auto i = 0; i < 3 && i < inputs3.size(); ++i)
        {
            v[i] = getOneDouble(inputs3[i]);
        }
        return std::make_tuple(v[0], v[1], v[2]);
    };
    std::for_each(response.begin(), response.end(), [&, this](auto &rspu) {
        switch (rspu.first)
        {
        case TYPE_MPOINT: [[fallthrough]];
        case TYPE_MPOINT3: {
            double x, y, z;
            std::tie(x, y, z) = getThreeDouble(gboxes.takeFirst()->getInputs());
            auto &rspdu = rspu.second.point;
            if (!std::isnan(x)) rspdu[0] = x;
            if (!std::isnan(y)) rspdu[1] = y;
            if (!std::isnan(z)) rspdu[2] = z;
        }
        break;
        case TYPE_MPOINT3_2: {
            double x, y, _;
            std::tie(x, y, _) = getThreeDouble(gboxes.takeFirst()->getInputs());
            auto &rspdu = rspu.second.point;
            if (!std::isnan(x)) rspdu[0] = x;
            if (!std::isnan(y)) rspdu[1] = y;
        }
        break;
        case TYPE_ARR_MPOINT3: {
            auto inputs = gboxes.takeFirst()->getInputs();
            std::vector<AMCAX::Coord3d> results;
            for (int i = 0; i < inputs.size(); i += 3)
            {
                double x, y, z;
                std::tie(x, y, z) = getThreeDouble({inputs[i], inputs[i + 1], inputs[i + 2]});
                if (!std::isnan(x) && !std::isnan(y))
                {
                    results.push_back({x, y, z});
                }
            }
            rspu.second.pointarr = results;
        }
        break;
        case TYPE_MVEC: [[fallthrough]];
        case TYPE_MVEC3: {
            double x, y, z;
            std::tie(x, y, z) = getThreeDouble(gboxes.takeFirst()->getInputs());
            auto &rspdu = rspu.second.vec;
            if (!std::isnan(x)) rspdu[0] = x;
            if (!std::isnan(y)) rspdu[1] = y;
            if (!std::isnan(z)) rspdu[2] = z;
        }
        break;
        case TYPE_MVEC3_2: {
            double x, y, _;
            std::tie(x, y, _) = getThreeDouble(gboxes.takeFirst()->getInputs());
            auto &rspdu = rspu.second.vec;
            if (!std::isnan(x)) rspdu[0] = x;
            if (!std::isnan(y)) rspdu[1] = y;
        }
        break;
        case TYPE_BOOL: {
            auto *input = tunables.takeFirst();
            auto value = input->isChecked();
            auto &rspdu = rspu.second.b;
            rspdu = value;
        }
        break;
        case TYPE_DOUBLE: [[fallthrough]];
        case TYPE_DOUBLE_PI: {
            auto value = getOneDouble(gboxes.takeFirst()->getInputs()[0]);
            auto &rspdu = rspu.second.d;
            if (!std::isnan(value)) rspdu = value;
        }
        break;
        case TYPE_SIZE_T: {
            auto value = getOneSizeT(gboxes.takeFirst()->getInputs()[0]);
            auto &rspdu = rspu.second.s;
            rspdu = value;
        } 
        break;
        case TYPE_STRING: {
            auto text = getOne(gboxes.takeFirst()->getInputs()[0]);
            rspu.second.str = text;
        }
        break;
        default:
            break;
        }
    });
}
