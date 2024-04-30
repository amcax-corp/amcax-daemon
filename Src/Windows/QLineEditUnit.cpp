#include "QLineEditUnit.h"
#include <QLayout>
#include <QStyle>
QLineEditUnit::QLineEditUnit(QWidget* parent) : QLineEdit(parent)
{
	m_unitButton = new QPushButton(this);
	//��λ��ť��ƽ�ޱ߿�
	m_unitButton->setStyleSheet("border-style:solid;color: black;");
	m_unitButton->setFlat(true);
	//��λ���벼��
	QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
	layout->addStretch();
	layout->setSpacing(0);
	//��λ��ť��Margins
	//�ұ�����13px��������λ(��ť)
	layout->setContentsMargins(0, 0, 13, 0);
	layout->addWidget(m_unitButton);
	this->setLayout(layout);
}

QLineEditUnit::~QLineEditUnit()
{
}

QString QLineEditUnit::UnitText() const
{
	return   this->m_unitButton->text();
}

void QLineEditUnit::SetUnitText(const QString& unitText)
{
	if (unitText.isEmpty() || this->m_unitButton->text() == unitText)
	{
		return;
	}


	m_unitButton->setFont(this->font());
	this->m_unitButton->setText(unitText);
	// this->m_unitButton->setVisible(true);
	//������ֵĿ��
	int width = this->fontMetrics().width(m_unitButton->text());
	m_unitButton->setFixedWidth(width);
	//����λ��ͷ�˾�������ʾ����ֹ���������������һ�����ڰ�ť����
	QBoxLayout* layout = (QBoxLayout*)this->layout();
	QMargins margins = layout->contentsMargins();
	setTextMargins(margins.left(), margins.top(), width + margins.right(), margins.bottom());
}

