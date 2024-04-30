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
#pragma once

#include <QLabel>
#include <QWidget>
#include <QDialog>
#include <QString>
#include <vector>
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>

//#include "../Core/CoreDefine.h"
#include <common/CoordT.hpp>

namespace acamcad {

	class DialogAuto : public QDialog {
		Q_OBJECT

	public:

		enum class DataType {
			NONE,
			BOOL,
			INT,
			DOUBLE,
			COORD2,
			COORD3,
			COORD3VEC
		};

		struct DataAuto
		{
			bool b;
			size_t i;
			double d;
			QString unit;
			AMCAX::Coord2 c2;
			AMCAX::Coord3 c3;

			std::vector<AMCAX::Coord3> c3s;

			///DataAuto() {}

			///~DataAuto() {}
		};

		struct Data {
			DataType type;
			QString name;
			DataAuto value;


			Data(QString n) :type(DataType::NONE), name(n) {}

			Data(QString n, bool b) : type(DataType::BOOL), name(n)
			{
				value.b = b;
			}

			Data(QString n, size_t i) :type(DataType::INT), name(n)
			{
				value.i = i;
			}

			Data(QString n, double d, QString u) : type(DataType::DOUBLE), name(n)
			{
				value.d = d;
				value.unit = u;
			}

			Data(QString n, AMCAX::Coord2 co2) :type(DataType::COORD2), name(n)
			{
				value.c2 = co2;
			}

			Data(QString n, AMCAX::Coord3 co3) :type(DataType::COORD3), name(n)
			{
				value.c3 = co3;
			}

			Data(QString n, std::vector<AMCAX::Coord3> co3s, QString u) :type(DataType::COORD3VEC), name(n)
			{
				value.c3s = co3s;
				value.unit = u;
			}

			~Data() {}
		};

		DialogAuto(QWidget* parent = nullptr);
		void SetData(QString title, std::vector<Data> value);
		std::vector<Data>& GetData();


	public slots: // public slots
		void accept() override;

	protected:

		void Refresh(QVBoxLayout* layout);
		//QCheckBox

		void addDataUI(Data& data, QVBoxLayout* layout, int index);

		void saveData();
	protected:
		std::vector<Data> vec_;

		struct DataUI
		{
			QCheckBox* box;
			std::vector<QLineEdit*> line;
			std::vector<QBoxLayout*> layout;
			int pos;
			//QLayout* layout;

			DataUI(QCheckBox* b) :box(b) {}
			DataUI(std::vector<QLineEdit*> l) :box(nullptr), line(l) {}
			//DataUI(DataUI& data) : line(data.line) {}
			//DataUI(DataUI&& data) :box(data.box), line(data.line) {}
			DataUI(std::vector<QLineEdit*> li, std::vector<QBoxLayout*> la, int p) :
				line(li), layout(la), pos(p)
			{}
		};

		std::vector<DataUI> ui_;
		//QLineEdit
	};

}