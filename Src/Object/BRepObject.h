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
#include <topology/TopoShape.hpp>
#include <common/IndexSet.hpp>
#include <common/CoordT.hpp>
#include "AdapterObjectParent.h"
#include <common/VectorT.hpp>

namespace acamcad
{
	class AdapterObject;
	class AdapterObjectParent;
	class ObjectDraw;
	class BRepObjectDraw;

	class BRepObject : public AdapterObjectParent
	{
		friend class ObjectDraw;
		friend class BRepObjectDraw;


	public:
		BRepObject(AdapterObject* parent);
		BRepObject(AdapterObject* parent, const BRepObject& object, bool isDraw);

		virtual ~BRepObject();

	public:
		void doOperate(MOperation* operate);
		void updateDraw();
		/// Get all Info for the Object as a string
		QString getObjectinfo();
		const virtual ObjectDraw* getDrawTool() const { return draw_tool_.get(); }

	public:
		void assignBRep();
		void SetShape(const AMCAX::TopoShape& shape);	//careful to use it;

		AMCAX::TopoShape& getShape();
		const AMCAX::TopoShape& getShape() const;
		const AMCAX::IndexSet<AMCAX::TopoShape>& getShapeVertices();
		const AMCAX::IndexSet<AMCAX::TopoShape>& getShapeEdges();
		const AMCAX::IndexSet<AMCAX::TopoShape>& getShapeFaces();

	public:
		void backBRep(AMCAX::TopoShape& back_brep);
		void applyBRep(const AMCAX::TopoShape& apply_brep);

	public:
		bool loadBRep(const QString& filename);

		bool saveBRep(const QString& filename);

		bool saveBRepSTL(const QString& filename);
		bool saveBRepOBJ(const QString& filename);

		//AdapterObject* getParent();

	public:
		void mesh_OPvert_Backup();
		void mesh_Clear_OPvertBackup();

		void meshRotation(const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double angle);		//rotation
		void meshMove(const AMCAX::Coord3& v, double dis);

		void mirrorMeshObject(const AMCAX::Coord3& center, const AMCAX::Coord3& normal);


		// select vert edge face
		void selectVertWithFaceId(const AMCAX::Vector3& begin, const AMCAX::Vector3& end, int f_id, int& sv_id);
		void selectEdgeWithFaceId(const AMCAX::Vector3& begin, const AMCAX::Vector3& end, int f_id, int& se_id);
		//void selectFaceWithFaceId(const AMCAX::Vector3& begin, const AMCAX::Vector3& end, int f_id, int& sf_id);


	private:
		void updateShapeInfo();

		AMCAX::TopoShape shape_;
		AMCAX::IndexSet<AMCAX::TopoShape> shapeVertices_;
		AMCAX::IndexSet<AMCAX::TopoShape> shapeEdges_;
		AMCAX::IndexSet<AMCAX::TopoShape> shapeFaces_;

		AMCAX::TopoShape brep_trans_back_;

		std::unique_ptr<ObjectDraw> draw_tool_;

		//AdapterObject* parent_;


	};

} //namespace acamcad
