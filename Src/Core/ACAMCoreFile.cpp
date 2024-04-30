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
#include <step/StepDataTool.hpp>
#include <private/qzipwriter_p.h>
#include <private/qzipreader_p.h>
#include <polymesh/PolyMeshIO.hpp>
#include <tmeshSpline/TMSplineIO.hpp>
#include "DataManager.h"
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonvalue.h>

namespace
{
	QString defaultDir = ".";
}


namespace acamcad
{

	//void AMCore::addNewDataTypeFile(const QString& str, const DataType& dt)
	//{
	//	file_allowed_type_[str] = dt;
	//}

	void AMCore::loadObjects()
	{
		///QZipReader zipreader("");
		//static QString defaultDir = ".";
		QString filterAM = "AMCAX files (" "*."  FILE_EXT_AMCAX  ")";
		QString filter = "AMCAX BRep files (" "*."  FILE_EXT_AMCAX_BREP  ")";
		QString stepFilter = "STEP files (" "*."  FILE_EXT_STP  " " "*."  FILE_EXT_STEP  ")";
		QString finalFilter = filterAM + ";;" + filter + ";;" + stepFilter;
		QString fileName = QFileDialog::getOpenFileName(NULL, "", defaultDir, finalFilter);

		loadObjectsFromFile(fileName);
	}

	void AMCore::loadObjectsFromFile(const QString& fileName)
	{
		if (!fileName.isEmpty())
		{
			bool result = loadObjectsFromFile1(fileName);

			if (result && file_.isEmpty())
			{
				file_ = fileName;

				if (listener_)
					listener_->refresh_title(file_);
			}

			if (listener_)
				listener_->refresh_file(fileName);
		}
	}

	void AMCore::saveObjects()
	{
		//std::cout << "AMCore::saveObjects " << QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toStdString() << std::endl;
		if (file_.isEmpty())
		{
			//static QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
			QString filterAM = "AMCAX files (" "*."  FILE_EXT_AMCAX  ")";
			//QString filter = "AMCAX BRep files (" "*."  FILE_EXT_AMCAX_BREP  ")";
			//QString stepFilter = "STEP files (" "*."  FILE_EXT_STP  " " "*."  FILE_EXT_STEP  ")";
			QString finalFilter = filterAM;
			QString fileName = QFileDialog::getSaveFileName(NULL, "", defaultDir, finalFilter);

			if (saveObjects(fileName))
			{
				file_ = fileName;

				if (listener_)
					listener_->refresh_title(file_);
			}

		}
		else
		{
			saveObjects(file_);
		}


	}

	bool AMCore::newObjects()
	{
		//QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
		QString filterAM = "AMCAX files (" "*."  FILE_EXT_AMCAX  ")";
		//QString filter = "AMCAX BRep files (" "*."  FILE_EXT_AMCAX_BREP  ")";
		//QString stepFilter = "STEP files (" "*."  FILE_EXT_STP  " " "*."  FILE_EXT_STEP  ")";
		QString finalFilter = filterAM;
		QString fileName = QFileDialog::getSaveFileName(NULL, "", defaultDir, finalFilter);

		if (saveObjects(fileName))
		{
			file_ = fileName;

			if (listener_)
				listener_->refresh_title(file_);

			return true;
		}
		else
			return false;
	}


	void AMCore::saveAsObjects()
	{
		///QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
		QString filterAM = "AMCAX files (" "*."  FILE_EXT_AMCAX  ")";
		//QString filter = "AMCAX BRep files (" "*."  FILE_EXT_AMCAX_BREP  ")";
		QString stepFilter = "STEP files (" "*."  FILE_EXT_STP  " " "*."  FILE_EXT_STEP  ")";
		QString finalFilter = filterAM + ";;" + stepFilter;
		QString fileName = QFileDialog::getSaveFileName(NULL, "", defaultDir, finalFilter);

		if (saveObjects(fileName))
		{
			file_ = fileName;

			if (listener_)
				listener_->refresh_title(file_);
		}

	}

	bool AMCore::saveObjects(const QString& filename)
	{
		if (!filename.isEmpty())
		{
			CObjectIter it_begin = dataManager_->objects_begin();
			CObjectIter it_end = dataManager_->objects_end();



			if (listener_)
				listener_->refresh_file(filename);

			return saveObjectsToFile(filename, it_begin, it_end);
		}
	}

	void AMCore::saveSelectedObjects()
	{
		//static QString defaultDir = ".";
		QString filterAM = "AMCAX files (" "*."  FILE_EXT_AMCAX  ")";
		//QString filter = "AMCAX BRep files (" "*."  FILE_EXT_AMCAX_BREP  ")";
		QString stepFilter = "STEP files (" "*."  FILE_EXT_STP  " " "*." FILE_EXT_STEP  ")";
		QString finalFilter = filterAM + ";;" + stepFilter;
		QString filename = QFileDialog::getSaveFileName(NULL, "", defaultDir, finalFilter);

		if (!filename.isEmpty())
		{
			std::vector<AdapterObject*> selected;
			dataManager_->getSelectedList(selected);

			CObjectIter it_begin = selected.begin();
			CObjectIter it_end = selected.end();
			saveObjectsToFile(filename, it_begin, it_end);
		}
	}

	bool AMCore::loadObjectsFromFile1(const QString& filename)
	{
		auto lpos = std::max(filename.lastIndexOf("/"), filename.lastIndexOf("\\"));
		defaultDir = filename.left(lpos + 1);


		QFileInfo fi(filename);
		QString filepath = fi.absoluteFilePath();
		QFile file(filepath);
		QString ext1 = fi.suffix();

		if (fi.isDir() || !file.exists())
			return false;
		if (!file.open(QIODevice::ReadOnly))
			return false;

		std::vector<AdapterObject*> vec;
		if (auto lext1 = ext1.toLower(); lext1 == FILE_EXT_STP || lext1 == FILE_EXT_STEP)
		{  //stp file
			QTextStream in(&file);
			QString content = in.readAll();
			std::istringstream fin(content.toStdString(), std::istream::in);
			AMCAX::STEP::StepReader stepReader(fin);
			stepReader.Read();
			AMCAX::STEP::StepDataList shapes = stepReader.GetShapes();
			shapes = AMCAX::STEP::StepDataTool::Flatten(shapes);
			for (auto& shapItem : shapes)
			{
				AdapterObject* adapter_object = new AdapterObject;
				adapter_object->setDataType(acamcad::DataType::BREP_TYPE);
				//adapter_object->bRep = std::make_unique<BRepObject>(adapter_object);
				//BRepObject* brep_obj = new BRepObject();
				adapter_object->bRep->SetShape(shapItem->Shape());
				adapter_object->setVisible(true);
				dataManager_->addObject(adapter_object);
				adapter_object->updateDraw();
				vec.push_back(adapter_object);

			}
		}
		else if (ext1 == FILE_EXT_AMCAX_BREP)  //abr file
		{
			QTextStream in(&file);
			QString content = in.readAll();
			std::istringstream fin(content.toStdString(), std::istream::in);
			AMCAX::TopoShape shapeCmp;
			AMCAX::ShapeTool::Read(shapeCmp, fin);

			AMCAX::TopoExplorer exp;
			for (exp.Init(shapeCmp, AMCAX::ShapeType::Shell); exp.More(); exp.Next()) {
				const auto& shape = static_cast<const AMCAX::TopoShape&>(exp.Current());
				AdapterObject* adapter_object = new AdapterObject;
				adapter_object->setDataType(acamcad::DataType::BREP_TYPE);
				//adapter_object->bRep = std::make_unique<BRepObject>(adapter_object);
				//BRepObject* brep_obj = new BRepObject();
				adapter_object->bRep->SetShape(shape);
				adapter_object->setVisible(true);
				dataManager_->addObject(adapter_object);
				adapter_object->updateDraw();
				vec.push_back(adapter_object);
			}
		}
		else if (ext1 == FILE_EXT_AMCAX)
		{
			file.close();
			QZipReader zip(filename);

			if (zip.isReadable())
			{
				{
					QByteArray array = zip.fileData(FILE_AMCAX_CONTROL);

					QJsonParseError jsonError;
					QJsonDocument doc = QJsonDocument::fromJson(array, &jsonError);

					if (jsonError.error != QJsonParseError::NoError && !doc.isNull())
					{
						return false;
					}

					if (doc["version"].toInt() != 1)
					{
						return false;
					}

				}

				for (int i = 0; i < zip.fileInfoList().size(); i++)
				{
					QByteArray dt = zip.fileInfoList().at(i).filePath.toUtf8();
					QString strtemp = QString::fromLocal8Bit(dt);

					if (strtemp == FILE_AMCAX_CONTROL)
						continue;

					QByteArray array = zip.fileData(strtemp);

					QFileInfo info(strtemp);

					std::istringstream context(array.toStdString(), std::istream::in);

					AdapterObject* adapter_object = new AdapterObject;
					if (info.suffix() == FILE_EXT_AMCAX_BREP)
					{
						adapter_object->setDataType(acamcad::DataType::BREP_TYPE);
					}
					else if (info.suffix() == FILE_EXT_AMCAX_SUBD)
					{
						adapter_object->setDataType(acamcad::DataType::MESH_TYPE);
					}
					else if (info.suffix() == FILE_EXT_AMCAX_TMS)
					{
						adapter_object->setDataType(acamcad::DataType::TSPLINEU_TYPE);
					}

					if (adapter_object->readStream(context))
					{
						adapter_object->setVisible(true);
						adapter_object->updateDraw();
						vec.push_back(adapter_object);
						dataManager_->addObject(adapter_object);
					}
					else
					{
						delete adapter_object;
					}
				}
				dataManager_->RecordAddObject(vec);
				return true;
			}

		}

		dataManager_->RecordAddObject(vec);

		return false;
	}


	bool AMCore::saveObjectsToFile(const QString& filename, std::vector<acamcad::AdapterObject*>::const_iterator& it_begin, std::vector<acamcad::AdapterObject*>::const_iterator& it_end)
	{
		auto lpos = std::max(filename.lastIndexOf("/"), filename.lastIndexOf("\\"));
		defaultDir = filename.left(lpos + 1);

		DataType file_type;

		QString ext = QFileInfo(filename).suffix();
		QFileInfo f_info = QFileInfo(filename);

		if (auto lext = ext.toLower(); lext == FILE_EXT_STP || lext == FILE_EXT_STEP) // stp file
		{
			if (it_begin == it_end) {
				return false;
			}

			std::ostringstream fout;
			AMCAX::STEP::StepWriter stepWriter(fout);
			stepWriter.Init();
			for (CObjectIter o_it = it_begin; o_it != it_end; o_it++)
			{
				AdapterObject* object = *o_it;
				if (object->dataType() == DataType::BREP_TYPE)
					stepWriter.WriteShape(std::make_shared<AMCAX::STEP::StepData>(object->bRep->getShape()));
			}
			stepWriter.Done();

			QFile file(filename);
			if (file.open(QIODevice::WriteOnly | QIODevice::Text))
			{
				QTextStream out(&file);
				out << fout.str().c_str();
				file.close();
			}
		}
		else if (ext == FILE_EXT_AMCAX) // cmcax file
		{
			QZipWriter write(filename);

			if (!write.isWritable())
				return false;
			{
				QJsonObject json;
				json.insert("version", 1);

				QJsonDocument doc;
				doc.setObject(json);

				write.addFile(QString(FILE_AMCAX_CONTROL), doc.toJson());

			}

			std::vector<AdapterObject*> brep, obj, tms;

			for (CObjectIter o_it = it_begin; o_it != it_end; o_it++)
			{
				AdapterObject* object = *o_it;

				switch (object->dataType())
				{
				case acamcad::DataType::BREP_TYPE:
					brep.push_back(object);
					break;
				case acamcad::DataType::MESH_TYPE:
					obj.push_back(object);
					break;
				case acamcad::DataType::TSPLINEU_TYPE:
					tms.push_back(object);
					break;
				default:
					break;
				}

			}

			for (int i = 0; i < brep.size(); i++)
			{
				std::ostringstream fout;
				AMCAX::ShapeTool::Write(brep[i]->bRep->getShape(), fout);
				QByteArray arr;
				arr.append(fout.str().c_str());
				write.addFile(QString("B%1.%2").arg(i).arg(FILE_EXT_AMCAX_BREP), arr);
			}


			for (int i = 0; i < obj.size(); i++)
			{
				std::ostringstream fout;

				if (AMCAX::SubD::PolyMeshIO::WriteMeshStreamOBJ(fout, obj[i]->mesh->getShape())) {
					QByteArray arr;
					arr.append(fout.str().c_str());

					write.addFile(QString("M%1.%2").arg(i).arg(FILE_EXT_AMCAX_SUBD), arr);
				}

			}

			for (int i = 0; i < tms.size(); i++)
			{
				std::ostringstream fout;
				AMCAX::TMS::TMSplineIO io;

				if (io.WriteTMSplineStream(fout, tms[i]->tSpline->getShape()))
				{
					QByteArray arr;
					arr.append(fout.str().c_str());

					write.addFile(QString("T%1.%2").arg(i).arg(FILE_EXT_AMCAX_TMS), arr);
				}

			}

			write.close();

			return true;

		}


		return false;
	}

}
