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
#include <fstream>
#include <iostream>
#include <topology/TopoBuilder.hpp>
#include <topology/TopoCompound.hpp>
#include <topology/TopoExplorer.hpp>
#include <io/ShapeTool.hpp>

#include "DataManager.h"

namespace acamcad
{

void AMCore::addNewDataTypeFile(const QString& str, const DataType& dt)
{
	file_allowed_type_[str] = dt;
}

void AMCore::loadObjects()
{
	QString filter = "*." + FILE_EXT_AMCAX_BREP;
	QString fileName = QFileDialog::getOpenFileName(NULL, "", ".", filter);


	if (!fileName.isEmpty())
	{
		loadObjectsFromFile(fileName);
	}
}

void AMCore::saveObjects()
{
	QString filter = "*." + FILE_EXT_AMCAX_BREP;
	QString fileName = QFileDialog::getSaveFileName(NULL, "", ".", filter);

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
	QString filter = "*." + FILE_EXT_AMCAX_BREP; 
	QString filename = QFileDialog::getSaveFileName(NULL, "", ".", filter);

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

	if (fi.isDir() || !file.exists() || ext1 != FILE_EXT_AMCAX_BREP)
		return; 

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
