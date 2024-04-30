#pragma once
#include <QLineEdit>
#include <QPushButton>

class QLineEditUnit : public QLineEdit
{
	Q_OBJECT
public:
	QLineEditUnit(QWidget* parent = Q_NULLPTR);
	~QLineEditUnit();
	//获取编辑框单位
	QString UnitText()const;

	//设置编辑框单位
	void SetUnitText(const QString& unitText);

private:
	QPushButton* m_unitButton = nullptr;//单位文本按钮
};