#include "DialogAuto.h"
#include <QVBoxLayout>
#include <QIntValidator>
#include <QPushButton>
#include <QFont>
#include <QPalette>
#include "QLineEditUnit.h"
#include <iostream>
//#include <QRegExpValidator>
#include "IntValidator.h"

#define WIDTH_LINEEDIT_3 80
#define WIDTH_LINEEDIT_2 80
#define WIDTH_LINEEDIT_1 182

namespace acamcad {

	DialogAuto::DialogAuto(QWidget* parent) :
		QDialog(parent)
	{
	}

	void DialogAuto::SetData(QString title, std::vector<Data> value)
	{
		setWindowTitle(title);
		setMinimumWidth(400);
		QVBoxLayout* layout = new QVBoxLayout(this);
		setLayout(layout);

		vec_ = value;
		Refresh(layout);
	}

	std::vector<DialogAuto::Data>& DialogAuto::GetData()
	{
		return vec_;
	}

	void DialogAuto::Refresh(QVBoxLayout* layout)
	{
		for (int i = 0; i < vec_.size(); i++)
		{
			addDataUI(vec_[i], layout, i);
		}

		layout->addSpacing(40);
		layout->addStretch();

		QHBoxLayout* layout2 = new QHBoxLayout;
		layout->addLayout(layout2);
		//layout->addStretch();
		layout2->addStretch();
		QPushButton* button = new QPushButton(this);
		button->setText(tr("确定"));
		button->setStyleSheet("QPushButton{"
			"color: white;"
			"background-color: #464681;"
			"border-radius: 4px;"
			"}");
		button->setFixedSize(QSize(72, 24));
		connect(button, &QPushButton::clicked, this, &DialogAuto::accept);
		layout2->addWidget(button);

		button = new QPushButton(this);
		button->setFixedSize(QSize(72, 24));
		button->setText(tr("取消"));
		connect(button, &QPushButton::clicked, this, &DialogAuto::reject);
		layout2->addWidget(button);


	}

	void DialogAuto::addDataUI(DialogAuto::Data& data, QVBoxLayout* layout, int index)
	{
		QHBoxLayout* layout2 = new QHBoxLayout;


		//layout->addStretch();

		switch (data.type)
		{
		case DataType::NONE:
		{

			QLabel* label = new QLabel(this);
			label->setText("▼" + data.name);
			QFont font;
			font.setPointSizeF(12);
			font.setBold(true);

			label->setFont(font);

			label->setStyleSheet("color: #464681");

			layout2->addWidget(label);

			layout->addSpacing(10);
			layout->addLayout(layout2);
			layout->addSpacing(10);
			//layout->addSpacing(10);

			break;
		}
		case DataType::BOOL:
		{
			QCheckBox* box = new QCheckBox(this);
			box->setText(data.name);
			layout2->addWidget(box);
			box->setChecked(data.value.b);

			ui_.push_back(box);

			layout2->addStretch();
			layout->addLayout(layout2);
		}
		break;
		case DataType::INT:
		{
			QLabel* label = new QLabel(this);
			label->setText(data.name);
			layout2->addWidget(label);

			layout2->addStretch();

			QLineEdit* line = new QLineEdit(this);

			///QRegExp reg("^[1-9][0-9]*$");
			///line->setInputMask("9");
			//line->setValidator(new QRegExpValidator(reg, this));
			line->setValidator(new IntValidator(1, INT_MAX, this));
			line->setText(QString::number(data.value.i));
			line->setFixedWidth(WIDTH_LINEEDIT_1);
			layout2->addWidget(line);

			//DataUI d = { line };
			ui_.push_back(DataUI({ line }));
			layout->addLayout(layout2);
		}
		break;
		case DataType::DOUBLE:
		{
			QLabel* label = new QLabel(this);
			label->setText(data.name);
			layout2->addWidget(label);

			layout2->addStretch();

			QLineEditUnit* line = new QLineEditUnit(this);
			line->setValidator(new QDoubleValidator(this));
			line->setFixedWidth(WIDTH_LINEEDIT_1);
			line->SetUnitText(data.value.unit);
			line->setText(QString::number(data.value.d, 'f', 2));
			layout2->addWidget(line);

			//DataUI d = { line };
			ui_.push_back(DataUI({ line }));
			layout->addLayout(layout2);
		}
		break;
		case DataType::COORD2:
		{
			QLabel* label = new QLabel(this);
			label->setText(data.name);
			layout2->addWidget(label);

			layout2->addStretch();

			label = new QLabel(this);
			label->setText("X");
			layout2->addWidget(label);

			std::vector<QLineEdit*> vec;

			QLineEdit* line = new QLineEdit(this);
			line->setValidator(new QDoubleValidator(this));
			line->setText(QString::number(data.value.c2.X(), 'f', 2));
			line->setFixedWidth(WIDTH_LINEEDIT_2);
			layout2->addWidget(line);
			vec.push_back(line);

			label = new QLabel(this);
			label->setText("Y");
			layout2->addWidget(label);

			line = new QLineEdit(this);
			line->setText(QString::number(data.value.c2.Y(), 'f', 2));
			line->setFixedWidth(WIDTH_LINEEDIT_2);
			line->setValidator(new QDoubleValidator(this));
			layout2->addWidget(line);
			//ui_.push_back(DataUI({ line }));
			vec.push_back(line);

			ui_.push_back(vec);
			layout->addLayout(layout2);
		}
		break;
		case DataType::COORD3:
		{
			QLabel* label = new QLabel(this);
			label->setText(data.name);
			layout2->addWidget(label);

			layout2->addStretch();


			std::vector<QLineEdit*> vec;
			label = new QLabel(this);
			label->setText("X");

			layout2->addWidget(label);

			QLineEdit* line = new QLineEdit(this);
			line->setFixedWidth(WIDTH_LINEEDIT_3);
			line->setText(QString::number(data.value.c3.X(), 'f', 2));
			line->setValidator(new QDoubleValidator(this));
			layout2->addWidget(line);
			vec.push_back(line);

			label = new QLabel(this);
			label->setText("Y");
			layout2->addWidget(label);

			line = new QLineEdit(this);
			line->setFixedWidth(WIDTH_LINEEDIT_3);
			line->setText(QString::number(data.value.c3.Y(), 'f', 2));
			line->setValidator(new QDoubleValidator(this));
			layout2->addWidget(line);
			vec.push_back(line);

			label = new QLabel(this);
			label->setText("Z");
			layout2->addWidget(label);

			line = new QLineEdit(this);
			line->setFixedWidth(WIDTH_LINEEDIT_3);
			line->setText(QString::number(data.value.c3.Z(), 'f', 2));
			line->setValidator(new QDoubleValidator(this));
			layout2->addWidget(line);
			vec.push_back(line);

			ui_.push_back(vec);

			layout->addLayout(layout2);
		}
		break;
		case DataType::COORD3VEC:
		{
			QLabel* label = new QLabel(this);
			label->setText("▼" + data.name);
			QFont font;
			font.setPointSizeF(12);
			font.setBold(true);

			label->setFont(font);

			label->setStyleSheet("color: #464681");

			layout2->addWidget(label);
			layout2->addStretch();

			layout->addSpacing(10);
			layout->addLayout(layout2);
			layout->addSpacing(10);

			QPushButton* button = new QPushButton(this);
			button->setText(tr("+"));
			button->setFixedSize(QSize(30, 30));
			connect(button, &QPushButton::clicked, this, [this, layout, index]() {

				saveData();
				vec_[index].value.c3s.push_back({ 0.0,0.0,0.0 });

				auto c = children();

				for (auto iter : c)
				{
					iter->deleteLater();
				}
				delete this->layout();

				ui_.clear();

				QVBoxLayout* layout = new QVBoxLayout(this);
				setLayout(layout);

				Refresh(layout);
				});
			layout2->addWidget(button);

			button = new QPushButton(this);
			button->setText("-");
			button->setFixedSize(QSize(30, 30));
			connect(button, &QPushButton::clicked, this, [this, layout, index]() {

				auto& data = vec_[index].value.c3s;

				if (data.empty())
					return;

				saveData();
				data.erase(data.begin() + (data.size() - 1));

				auto c = children();

				for (auto iter : c)
				{
					iter->deleteLater();
				}
				delete this->layout();

				ui_.clear();

				QVBoxLayout* layout = new QVBoxLayout(this);
				setLayout(layout);

				Refresh(layout);
				//ui_.erase()
				});
			layout2->addWidget(button);

			int pos = layout2->children().size();
			std::vector<QLineEdit*> vec;
			std::vector<QBoxLayout*> lay;
			for (int i = 0; i < data.value.c3s.size(); i++)
			{
				QHBoxLayout* layout3 = new QHBoxLayout(this);
				layout->addLayout(layout3);
				lay.push_back(layout3);

				QLabel* label = new QLabel(this);
				label->setText(data.value.unit.arg(i + 1));
				layout3->addWidget(label);
				layout3->addStretch();

				QLineEdit* line = new QLineEdit(this);
				line->setFixedWidth(WIDTH_LINEEDIT_3);
				line->setText(QString::number(data.value.c3s[i].X(), 'f', 2));
				line->setValidator(new QDoubleValidator(this));
				layout3->addWidget(line);
				vec.push_back(line);

				line = new QLineEdit(this);
				line->setFixedWidth(WIDTH_LINEEDIT_3);
				line->setText(QString::number(data.value.c3s[i].Y(), 'f', 2));
				line->setValidator(new QDoubleValidator(this));
				layout3->addWidget(line);
				vec.push_back(line);

				line = new QLineEdit(this);
				line->setFixedWidth(WIDTH_LINEEDIT_3);
				line->setText(QString::number(data.value.c3s[i].Z(), 'f', 2));
				line->setValidator(new QDoubleValidator(this));
				layout3->addWidget(line);
				vec.push_back(line);
			}

			ui_.push_back({ vec,lay,pos });
		}
		break;
		default:
			break;
		}


		//layout->addSpacing(10);
	}

	void DialogAuto::accept()
	{

		saveData();

		QDialog::accept();
	}

	void DialogAuto::saveData()
	{
		for (int i = 0, j = 0; i < vec_.size(); i++)
		{
			//vec_[i];
			auto& data = vec_[i];

			switch (data.type)
			{
			case DataType::BOOL:
				data.value.b = ui_[j].box->isChecked();
				j++;
				break;
			case DataType::INT:
				data.value.i = ui_[j].line[0]->text().toInt();
				j++;
				break;
			case DataType::DOUBLE:
				data.value.d = ui_[j].line[0]->text().toDouble();
				j++;
				break;
			case DataType::COORD2:
				data.value.c2.SetCoord(ui_[j].line[0]->text().toDouble(), ui_[j].line[1]->text().toDouble());
				j++;
				break;
			case DataType::COORD3:
				data.value.c3.SetCoord(
					ui_[j].line[0]->text().toDouble(),
					ui_[j].line[1]->text().toDouble(),
					ui_[j].line[2]->text().toDouble());
				j++;
				break;
			case DataType::COORD3VEC:
			{
				data.value.c3s.clear();
				int size = ui_[j].line.size() / 3;
				for (int k = 0; k < size; k++)
				{
					data.value.c3s.push_back(AMCAX::Coord3(
						ui_[j].line[3 * k]->text().toDouble(),
						ui_[j].line[3 * k + 1]->text().toDouble(),
						ui_[j].line[3 * k + 2]->text().toDouble()
					));
				}
				j++;
			}
			break;
			default:
				break;
			}
		}
	}

}
