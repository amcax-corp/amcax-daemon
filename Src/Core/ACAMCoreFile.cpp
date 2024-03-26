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
#include "ACAMCore.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QTextStream>

#include <fstream>
#include <iostream>
#include <topology/TopoBuilder.hpp>
#include <topology/TopoCompound.hpp>
#include <topology/TopoExplorer.hpp>
#include <io/ShapeTool.hpp>
#include <step/StepReader.hpp>
#include <step/StepWriter.hpp>
#include <step/StepData.hpp>

#include "DataManager.h"

namespace acamcad
{

void AMCore::addNewDataTypeFile(const QString& str, const DataType& dt)
{
	file_allowed_type_[str] = dt;
}

void AMCore::loadObjects()
{
	static QString defaultDir = ".";
	QString filter = "AMCAX BRep files (" "*." + FILE_EXT_AMCAX_BREP + ")";
	QString stepFilter = "STEP files (" "*." + FILE_EXT_STP + " " "*." + FILE_EXT_STEP + ")";
	QString finalFilter = filter + ";;" + stepFilter;
	QString fileName = QFileDialog::getOpenFileName(NULL, "", defaultDir, finalFilter);

	if (!fileName.isEmpty())
	{
		auto lpos = std::max(fileName.lastIndexOf("/"), fileName.lastIndexOf("\\"));
		defaultDir = fileName.left(lpos + 1);
		loadObjectsFromFile(fileName);
	}
}

void AMCore::saveObjects()
{
	static QString defaultDir = ".";
	QString filter = "AMCAX BRep files (" "*." + FILE_EXT_AMCAX_BREP + ")";
	QString stepFilter = "STEP files (" "*." + FILE_EXT_STP + " " "*." + FILE_EXT_STEP + ")";
	QString finalFilter = filter + ";;" + stepFilter;
	QString fileName = QFileDialog::getSaveFileName(NULL, "", defaultDir, finalFilter);

	saveObjects(fileName);
}

void AMCore::saveObjects(const QString& filename)
{
	if (!filename.isEmpty())
	{
		CObjectIter it_begin = dataManager_->objects_begin();
		CObjectIter it_end = dataManager_->objects_end();

		saveObjectsToFile(filename, it_begin, it_end);
	}
}

void AMCore::saveSelectedObjects()
{
	static QString defaultDir = ".";
	QString filter = "AMCAX BRep files (" "*." + FILE_EXT_AMCAX_BREP + ")";
	QString stepFilter = "STEP files (" "*." + FILE_EXT_STP + " " "*." + FILE_EXT_STEP + ")";
	QString finalFilter = filter + ";;" + stepFilter;
	QString filename = QFileDialog::getSaveFileName(NULL, "", defaultDir, finalFilter);

	if (!filename.isEmpty())
	{
		std::vector<BaseObject*> selected; 
		dataManager_->getSelectedList(selected);

		CObjectIter it_begin = selected.begin(); 
		CObjectIter it_end = selected.end(); 
		saveObjectsToFile(filename, it_begin,  it_end);
	}
}

void AMCore::loadObjectsFromFile(const QString& filename)
{
	QFileInfo fi(filename);
	QString filepath = fi.absoluteFilePath();
	QFile file(filepath);
	QString ext1 = fi.suffix();

	if (fi.isDir() || !file.exists())
		return; 

	if (auto lext1 = ext1.toLower(); lext1 == FILE_EXT_STP || lext1 == FILE_EXT_STEP)
	{  //stp file
		auto fin = std::ifstream(filename.toStdString(), std::ifstream::in);
		AMCAX::STEP::StepReader stepReader;
		std::vector<AMCAX::STEP::StepData> shapes;
		stepReader.Read(fin, shapes);
		for (auto& shapItem : shapes)
		{
			BRepObject* brep_obj = new BRepObject();
			brep_obj->SetShape(shapItem.Shape());
			brep_obj->setVisible(true);
			dataManager_->addObject(brep_obj);
			brep_obj->updateDraw();
		}
	}
	else if (ext1 == FILE_EXT_AMCAX_BREP)  //abr file
	{
		AMCAX::TopoShape shapeCmp;
		AMCAX::ShapeTool::Read(shapeCmp, filename.toStdString());

		AMCAX::TopoExplorer exp;
		for (exp.Init(shapeCmp, AMCAX::ShapeType::Shell); exp.More(); exp.Next()) {
			const auto& shape = static_cast<const AMCAX::TopoShape&>(exp.Current());
			BRepObject* brep_obj = new BRepObject();
			brep_obj->SetShape(shape);
			brep_obj->setVisible(true);
			dataManager_->addObject(brep_obj);
			brep_obj->updateDraw();
		}
	}
}


void AMCore::saveObjectsToFile(const QString& filename, std::vector<acamcad::BaseObject*>::const_iterator& it_begin, std::vector<acamcad::BaseObject*>::const_iterator& it_end)
{
	if (it_begin == it_end) {
		return; 
	}

	DataType file_type;

	QString ext = QFileInfo(filename).suffix();
	QFileInfo f_info = QFileInfo(filename);

	if (file_allowed_type_.find(ext) != file_allowed_type_.end())
	{
		file_type = file_allowed_type_[ext];
	}
	else
	{
		file_type = DataType::UNKNOW_TYPE;
		return; 
	}

	if (auto lext = ext.toLower(); lext == FILE_EXT_STP || lext == FILE_EXT_STEP) //stp file
	{
		AMCAX::STEP::StepWriter stepWriter(filename.toStdString());
		std::vector<AMCAX::STEP::StepData> shapes;
		for (CObjectIter o_it = it_begin; o_it != it_end; o_it++)
		{
			BRepObject* brep_object = dynamic_cast<BRepObject*> (*o_it);
			shapes.push_back(AMCAX::STEP::StepData(brep_object->getShape()));
		}
		stepWriter.Init();
		stepWriter.WriteShapes(shapes);
		stepWriter.Done();
	}
	else if (ext == FILE_EXT_AMCAX_BREP)  //abr file
	{
		AMCAX::TopoCompound comp;
		AMCAX::TopoBuilder build;
		build.MakeCompound(comp);
		for (CObjectIter o_it = it_begin; o_it != it_end; o_it++)
		{
			BRepObject* brep_object = dynamic_cast<BRepObject*> (*o_it);
			build.Add(comp, brep_object->getShape());
		}
		AMCAX::ShapeTool::Write(comp, filename.toStdString());
	}
}

}
