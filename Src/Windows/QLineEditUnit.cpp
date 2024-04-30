#include "QLineEditUnit.h"
#include <QLayout>
#include <QStyle>
QLineEditUnit::QLineEditUnit(QWidget* parent) : QLineEdit(parent)
{
	m_unitButton = new QPushButton(this);
	//单位按钮扁平无边框
	m_unitButton->setStyleSheet("border-style:solid;color: black;");
	m_unitButton->setFlat(true);
	//单位加入布局
	QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
	layout->addStretch();
	layout->setSpacing(0);
	//单位按钮的Margins
	//右边留了13px，留给单位(按钮)
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
	//获得文字的宽度
	int width = this->fontMetrics().width(m_unitButton->text());
	m_unitButton->setFixedWidth(width);
	//到单位开头了就往下显示，防止往后输入的内容有一部分在按钮下面
	QBoxLayout* layout = (QBoxLayout*)this->layout();
	QMargins margins = layout->contentsMargins();
	setTextMargins(margins.left(), margins.top(), width + margins.right(), margins.bottom());
}

