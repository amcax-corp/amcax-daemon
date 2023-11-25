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
#ifndef PARAMETERWIDGET_H
#define PARAMETERWIDGET_H

#include <QLabel>
#include <QWidget>
#include <QDialog>
#include <QVector>
#include <QList>
#include <QBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <QSize>

#include <QDebug>

#include <utility>
#include <cstdint>
#include <cstring>
#include <vector>
#include <tuple>

#include "../Core/CoreDefine.h"

#include <common/CoordT.hpp>

namespace acamcad {

void clearLayout(QLayout *layout);

// Overriding the default size hint so that it won't be as wide at default
// not declaring as nested class because it's an Q_Object
class ShortLineEdit: public QLineEdit 
{
    Q_OBJECT
    friend class ParamGBox;
private:
    static int counter; // Debuging purpose making sure there's no leakage
    QSize sizeHint() const override { return QSize(72, QLineEdit::sizeHint().height()); }
    ShortLineEdit(): QLineEdit(), suffix(nullptr) { ++counter; }
    ~ShortLineEdit() { --counter; };
    void setSuffix(QLabel *l);
protected:
    void resizeEvent(QResizeEvent *re) override { updateSuffixPos(); QLineEdit::resizeEvent(re); }
    void showEvent(QShowEvent *se) override { updateSuffixPos(); QLineEdit::showEvent(se); }
private:
    QLabel *suffix;
private slots:
    void updateSuffixPos();

};

struct PDEditAttr {
    enum LType {
        TYPE_NONE,
        TYPE_SIZE_T,
        TYPE_DOUBLE,
        TYPE_STRING,
    } type;
    bool hasBottom;
    bool hasTop;
    union {
        size_t s;
        double d;
        operator size_t() const { return s; }
        operator double() const { return d; }
        operator size_t&() { return s; }
        operator double&() { return d; }
    } bottom;
    union {
        size_t s;
        double d;
        operator size_t() const { return s; }
        operator double() const { return d; }
        operator size_t&() { return s; }
        operator double&() { return d; }
    } top;
    QString extra;

    PDEditAttr(): type(TYPE_STRING), hasBottom(false), hasTop(false) { }
    PDEditAttr(const double b, const double t): 
        type(TYPE_DOUBLE), hasBottom(std::isinf(b)), hasTop(std::isinf(t))
    {
        bottom.d = b;
        top.d = t;
    }
    PDEditAttr(const double b, const double t, const QString e): 
        type(TYPE_DOUBLE), hasBottom(std::isinf(b)), hasTop(std::isinf(t)), extra(e)
    {
        bottom.d = b;
        top.d = t;
    }
    PDEditAttr(const bool isbottom, const size_t l): 
        type(TYPE_SIZE_T), hasBottom(isbottom), hasTop(!isbottom) 
    {
        bottom.s = l;
        top.s = l;
    }
    PDEditAttr(const bool hasBottom, const size_t bottom,
               const bool hasTop, const size_t top):
        type(TYPE_SIZE_T), hasBottom(hasBottom), hasTop(hasTop) 
    {
        this->bottom.s = bottom;
        this->top.s = top;
    }
};

// typedef std::pair<QString, QLineEdit *> PDRowUnit;
typedef std::pair<QString, PDEditAttr> PDRowUnit;


class ParamGBox;

class ParamDialog: public QDialog 
{
    Q_OBJECT
public: // type define
    enum ParamType 
    {
        /* TYPE_MPOINT2, */ TYPE_MPOINT, TYPE_MPOINT3,
        TYPE_MPOINT3_2, // /* TYPE_POINT3_2 is MPoint3 but only shows 2 Params in the dialog */
        /* TYPE_MVEC2, */ TYPE_MVEC, TYPE_MVEC3, 
        TYPE_MVEC3_2, // /* TYPE_MVEC3_2 is MVec3 but only shows 2 Params in the dialog */
        /* TYPE_FLOAT, */ TYPE_DOUBLE, TYPE_DOUBLE_PI, TYPE_BOOL,
        /* TYPE_INT8, TYPE_INT16, TYPE_INT32, TYPE_INT64, TYPE_INT128, */
        /* TYPE_UINT8, TYPE_UINT16, TYPE_UINT32, TYPE_UINT64, TYPE_UINT128, */
        TYPE_SIZE_T, TYPE_STRING,
        // TYPE_ARR_MPOINT3,
        TYPE_ARR_MPOINT3,
    }; // ParamType

    /*
        capsule data so that different types of values can be stored in
        a single container.
    */
    struct DataUnit 
    {
        struct Size_t 
        { // defining Size_t so that DataUnit can handle top and bottom too
            size_t value;
            size_t bottom, top;
            bool hasBottom, hasTop;

            Size_t(): value(0), hasBottom(false), hasTop(false) {}
            Size_t(const size_t &s,
                   bool hasBottom, size_t bottom,
                   bool hasTop=false, bool top=0) :
                value(s), bottom(bottom), top(top),
                hasBottom(hasBottom), hasTop(hasTop) { }

            operator size_t() const { return value; }
            size_t &operator=(const size_t &a) { return value = a; }
        } s; // Size_t
        struct Double 
        { // define Double so that DataUnit can handle top and bottom too
            double value;
            double bottom, top;

            Double(): value(.0), bottom(-INFINITY), top(INFINITY) {};
            Double(const double &d,
                   double bottom, double top):
                value(d), bottom(bottom), top(top) { }

            operator double() const { return value; }
            double &operator=(const double &a) { return value = a; }
        } d; // Double
        bool b;
        AMCAX::Coord3 point;
        AMCAX::Coord3 vec;
        QString str;
        std::vector<AMCAX::Coord3d> pointarr;
        // float f;
        /* int8_t i8; int16_t i16; int32_t i32; int64_t i64; */ // int
        /* uint8_t u8; uint16_t u16; uint32_t u32; uint64_t u64; */ // uint

        // no need to apply The Rule of Five
        DataUnit(const size_t &s,
                 bool hasBottom=true, size_t bottom=1,
                 bool hasTop=false, size_t top=0):
            s(s, hasBottom, bottom, hasTop, top) { }
        DataUnit(const double &d, const double &bottom=INFINITY, const double &top=INFINITY):
            d(d, bottom, top) { }
        DataUnit(const bool &b): b(b) {  }
        DataUnit(const AMCAX::Coord3& value, ParamType type) {
            if (type == ParamType::TYPE_MPOINT3) {
                point = value;
            }
            else if (type == ParamType::TYPE_MVEC3) {
                vec = value;
            }
        }
        DataUnit(const QString &str): str(str) { }
        DataUnit(const std::vector<AMCAX::Coord3> &pointarr) = delete;

        DataUnit() { };
        DataUnit(const DataUnit &rd):
            s(rd.s, rd.s.hasBottom, rd.s.bottom, rd.s.hasTop, rd.s.top),
            d(rd.d, rd.d.bottom, rd.d.top),
            point(rd.point),
            vec(rd.vec),
            str(rd.str),
            pointarr(rd.pointarr) { }
        DataUnit(DataUnit &&) = delete;
        ~DataUnit() { }; // required by std::pair

        DataUnit &operator=(DataUnit &&) = delete;
        DataUnit &operator=(const DataUnit &) = delete;

        void updatePoint(const AMCAX::Coord3d &otherp) { point = otherp; }

        QString toString(const ParamType t, const int i) const {
            switch (t) {
            case TYPE_MPOINT:
            case TYPE_MPOINT3:
            case TYPE_MPOINT3_2: 
            case TYPE_ARR_MPOINT3: {
                return QString::number(point[i], 'f', 2);
            }
            break;
            case TYPE_MVEC:
            case TYPE_MVEC3:
            case TYPE_MVEC3_2: {
                return QString::number(vec[i], 'f', 2);
            }
            break;
            case TYPE_DOUBLE:
            case TYPE_DOUBLE_PI: {
                return QString::number(d, 'g', 12);
            }
            break;
            case TYPE_SIZE_T: {
                return QString::number(s);
            }
            break;
            case TYPE_STRING: {
                return str;
            }
            break;
            default:
                break;
            }
            return QString("");
        }
    }; // DataUnit

    // ParameterType, title, default value
    struct RequestUnit 
    {
        ParamType type;
        QString title;
        DataUnit def;

        RequestUnit() { }
        RequestUnit(const ParamType &type, const QString &title, const DataUnit &def): 
            type(type), title(title), def(def) { }
        RequestUnit(const std::tuple<ParamType, QString, DataUnit> &oth):
            type(std::get<0>(oth)), title(std::get<1>(oth)), def(std::get<2>(oth)) {}
        RequestUnit(const RequestUnit &oth):
            type(oth.type), title(oth.title), def(oth.def) {}

        operator ParamType() const { return type; }
        operator QString() const { return title; }
        operator DataUnit() const { return def; }
    };

    // redundant ParameterType, payload, isValid
    typedef QList<std::pair<ParamType, DataUnit>> Response;

private: // type define
    union InputWidget 
    {
        QLineEdit *edit;
        QCheckBox *check;

        InputWidget() = delete;
        InputWidget(QLineEdit *edit) { this->edit = edit; }
        InputWidget(QCheckBox *check) { this->check = check; }
        ~InputWidget() { edit = nullptr; check = nullptr; } 
    }; // InputWidget

public: // constructors
    ParamDialog(QWidget *parent = nullptr);

public: // public methods
    void addElements(const QVector<RequestUnit> &param);

public slots: // public slots
    void accept() override;
    void reject() override;

private slots: // private slots

private: // private methods
    void addElement(const RequestUnit &param);
    void generateResponse();
    QSize sizeHint() const override { return QSize(72, QDialog::sizeHint().height()); }

public: // public members
    void open() override;

private: // private members
    QVBoxLayout *inputLayout;
    Response response;
    // QList<InputWidget> inputs;
    QList<QCheckBox *> tunables;
    QList<ParamGBox *> gboxes;

public: // public static methods

private: // private static methods
    // static 
    
private: // private constant members
    const static QMap<ParamType, QList<QString>> TypeName;

signals: //
    void dataReady(const ParamDialog::Response &data);

protected:
    void closeEvent(QCloseEvent *ce) override;

};

// not declaring as nested class because it's an Q_Object
class ParamGBoxTitleButton: public QLabel
{
    Q_OBJECT
    friend class ParamGBoxTitle;
private:
    ParamGBoxTitleButton(const QString &text, QWidget *parent=nullptr):
        QLabel(text, parent) {}
    ~ParamGBoxTitleButton() {}
    Qt::MouseButtons btns;
protected:
    void mousePressEvent(QMouseEvent *me) override;
    void mouseReleaseEvent(QMouseEvent *me) override;
signals:
    void negative();
    void positive();
}; // ParamGBoxTitleButton

// not declaring as nested class because it's an Q_Object
class ParamGBoxTitleToggle: public QLabel
{
    Q_OBJECT
    friend class ParamGBoxTitle;
private:
    ParamGBoxTitleToggle(const QString &text, QWidget *parent=nullptr):
        QLabel(text, parent) {}
    ~ParamGBoxTitleToggle() {}
    Qt::MouseButtons btns;
protected:
    void mousePressEvent(QMouseEvent *me) override;
    void mouseReleaseEvent(QMouseEvent *me) override;
signals:
    void clicked();
}; // ParamGBoxTitleToggle

// not declaring as nested class because it's an Q_Object
class ParamGBoxTitle: public QWidget
{
    Q_OBJECT
    friend class ParamGBox;
private:
    ParamGBoxTitle(const QString &title, QWidget *parent = nullptr);
    ~ParamGBoxTitle() { };
    void setCheckable(bool cable);
    void setCountable();
    void setCount(const int count);
    const QString title;
    bool countable;
    ParamGBoxTitleToggle *label;
    ParamGBoxTitleButton *button;
    
signals:
    void negative();
    void positive();
    void clicked();

private slots:
    void onNegative() { emit negative(); }
    void onPositive() { emit positive(); }
    void onClicked() { emit clicked(); }
}; // ParamGBoxTitle

// not declaring as nested class because it's an Q_Object
class ParamGBox: public QWidget
{
    Q_OBJECT
    friend class ParamDialog;
private:
    ParamGBox(const QString &title,
              const ParamDialog::ParamType &type,
              const QVector<PDRowUnit> &row,
              const ParamDialog::DataUnit &defaultValue,
              const bool checkable = false,
              const bool checked = true,
              QWidget *parent = nullptr);
    ~ParamGBox() {};
private:
    void updateTitlePos();
    void setCheckable(bool checkable);
    void setChecked(bool checked);
    void setCountable();
    bool isCheckable() const;
    bool isChecked() const;

    void addRow(const QVector<PDRowUnit> &row, const ParamDialog::DataUnit &defaultValue);

    ShortLineEdit *createLineEdit(QValidator *validator);
    // INFINITY if has no bottom/top limit
    ShortLineEdit *createDoubleLineEdit(const double bottom, const double top);
    ShortLineEdit *createDoubleLineEdit(const double bottom, const double top, const QString &suffix);
    ShortLineEdit *createIntLineEdit(const bool hasBottom, const int bottom,
                               const bool hasTop = false, const int top = 0);
    ShortLineEdit *createLineEdit(const PDEditAttr &attr);
    
protected:
    void resizeEvent(QResizeEvent *re) override;
    void showEvent(QShowEvent *se) override;
private:
    std::vector<QLineEdit *> inputs;

    ParamDialog::ParamType type;
    QVector<PDRowUnit> templateRow;
    ParamDialog::DataUnit templateDU;

    QVBoxLayout *basel;
    ParamGBoxTitle *twidget;
    QFrame *hlinetop;
    
    bool checkable;
    bool checked;

private:
    std::vector<QLineEdit *> getInputs() { return inputs; }
    
private slots:
    void onNegative();
    void onPositive();
    void toggle();
};

}

#endif // PARAMETERWIDGET_H
