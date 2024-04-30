#pragma once
#include <QLineEdit>
#include <QPushButton>

class QLineEditUnit : public QLineEdit
{
	Q_OBJECT
public:
	QLineEditUnit(QWidget* parent = Q_NULLPTR);
	~QLineEditUnit();
	//��ȡ�༭��λ
	QString UnitText()const;

	//���ñ༭��λ
	void SetUnitText(const QString& unitText);

private:
	QPushButton* m_unitButton = nullptr;//��λ�ı���ť
};