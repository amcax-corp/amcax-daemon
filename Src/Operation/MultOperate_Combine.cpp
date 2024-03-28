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
#include "MultOperate_Combine.h"

#include <topology/TopoBuilder.hpp>
#include <topology/TopoCompound.hpp>

#include <polymesh/PolyMesh.hpp>
#include <polymeshAPI/MeshCopy.hpp>
#include <tmeshSplineAPI/TMSplineCopy.hpp>

acamcad::MultOperate_Combine::MultOperate_Combine(const SelectModel& smodel)
	:MultOperate(smodel)
{
}


acamcad::BaseObject* acamcad::MultOperate_Combine::operate()
{

	switch (ObjectType())
	{
	case acamcad::DataType::BREP_TYPE:
	{
		AdapterObject* adapter_object = new AdapterObject;
		adapter_object->setDataType(acamcad::DataType::BREP_TYPE);
		//adapter_object->bRep = std::make_unique<BRepObject>(adapter_object);

		//BRepObject* newObject = new BRepObject();
		adapter_object->bRep->getShape() = AMCAX::TopoCompound();

		AMCAX::TopoBuilder TB;
		TB.MakeCompound(static_cast<AMCAX::TopoCompound&>(adapter_object->bRep->getShape()));
		for (int i = 0; i < objectList_.size(); ++i)
		{
			AdapterObject* bobj = objectList_[i];
			TB.Add(adapter_object->bRep->getShape(), bobj->bRep->getShape());
		}

		adapter_object->bRep->updateDraw();
		return adapter_object;
	}
	break;
	case acamcad::DataType::MESH_TYPE:
	{
		AdapterObject* adapter_object = new AdapterObject;
		adapter_object->setDataType(acamcad::DataType::MESH_TYPE);
		adapter_object->mesh->assignMesh();

		for (int i = 0; i < objectList_.size(); i++) {
			AdapterObject* bobj = objectList_[i];
			AMCAX::SubD::MeshCopy::CombineMesh(adapter_object->mesh->mesh(), bobj->mesh->mesh());
		}
		adapter_object->updateDraw();
		return adapter_object;
	}
	break;
	case acamcad::DataType::TSPLINEU_TYPE:
	{
		AdapterObject* adapter_object = new AdapterObject;
		adapter_object->setDataType(acamcad::DataType::TSPLINEU_TYPE);
		adapter_object->tSpline->assignTSpline();

		for (int i = 0; i < objectList_.size(); i++) {
			AMCAX::TMS::TMSplineCopy().AppendToTMSpline(adapter_object->tSpline->getTSpline(),
				objectList_[i]->tSpline->getTSpline());
		}
		adapter_object->updateDraw();
		return adapter_object;
	}
	break;
	default:
		break;
	}
}
