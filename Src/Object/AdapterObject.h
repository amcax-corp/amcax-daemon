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

#include "BaseObject.h"

#include <memory>

#include <common/CoordT.hpp>
#include <common/VectorT.hpp>


namespace acamcad
{
	class BRepObject;
	class MeshObject;
	class TSplineObject;
	//class TSplineObject;

	class AdapterObject : public BaseObject
	{
	public:

		AdapterObject();
		AdapterObject(const AdapterObject& object, bool isDraw);

		virtual ~AdapterObject();

		// select vert edge face
		void selectVertWithFaceId(const AMCAX::Vector3& begin, const AMCAX::Vector3& end, int f_id, int& sv_id);
		void selectEdgeWithFaceId(const AMCAX::Vector3& begin, const AMCAX::Vector3& end, int f_id, int& se_id);
		//void selectFaceWithFaceId(const AMCAX::Vector3& begin, const AMCAX::Vector3& end, int f_id, int& sf_id);

		// mesh move rotate scale
		void mesh_OPvert_BackupVertes(const std::vector<int>& vert_id);
		void mesh_OPvert_BackupEdges(const std::vector<int>& edge_id);
		void mesh_OPvert_BackupFaces(const std::vector<int>& face_id);
		void mesh_Clear_OPvertBackup();
		virtual void mesh_OPvert_Backup();
		virtual void meshMove(const AMCAX::Coord3& v, double dis);

		//在有了原来的顶点之后，所有的缩放都是一样的，没有体，边，面，点的区别了。
		void meshScaleSingle(const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double scale);
		void meshScalePlane(const AMCAX::Coord3& center, const AMCAX::Coord3& normal, double scale);
		void meshRotation(const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double angle);

		// BoundingBox
		void GetBoundingBox(AMCAX::Coord3& min, AMCAX::Coord3& max);
		void GetBoundingBoxByFace(int face_id, AMCAX::Coord3& min, AMCAX::Coord3& max);
		void GetBoundingBoxByEdge(int edge_id, AMCAX::Coord3& min, AMCAX::Coord3& max);
		void GetBoundingBoxByVetex(int vetex_id, AMCAX::Coord3& position);

		// mirror
		void mirror(const AMCAX::Coord3& center, const AMCAX::Coord3& axis);


		// BaseObject
		virtual void setDataType(DataType _type) override;
		//virtual void doOperate(MOperation* operate) override;
		virtual void updateDraw() override;
		const virtual ObjectDraw* getDrawTool() const override;

	public:
		std::unique_ptr<BRepObject> bRep;
		std::unique_ptr<MeshObject> mesh;
		std::unique_ptr<TSplineObject> tSpline;
		//std::uni
	};
}