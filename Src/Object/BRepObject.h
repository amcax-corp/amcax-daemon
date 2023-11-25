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

namespace acamcad
{
	class BRepObject : public BaseObject
	{
		friend class ObjectDraw;
		friend class BRepObjectDraw;

	public:
		BRepObject();
		BRepObject(const BRepObject& object);

		virtual ~BRepObject();

	public:
		virtual void doOperate(MOperation* operate) override;
        virtual void updateDraw() override;

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

	public:
		void mesh_OPvert_Backup();
		void mesh_Clear_OPvertBackup();

		void meshRotation(const AMCAX::Coord3& center, const AMCAX::Coord3& axis, double angle);		//rotation
		void meshMove(const AMCAX::Coord3& v, double dis);
		
		BRepObject* mirrorMeshObject(const AMCAX::Coord3& center, const AMCAX::Coord3& normal);

	public:
        const virtual ObjectDraw* getDrawTool() const override { return draw_tool_; }

	private:
		void updateShapeInfo();

		AMCAX::TopoShape shape_;
		AMCAX::IndexSet<AMCAX::TopoShape> shapeVertices_;
		AMCAX::IndexSet<AMCAX::TopoShape> shapeEdges_;
		AMCAX::IndexSet<AMCAX::TopoShape> shapeFaces_;

		AMCAX::TopoShape brep_trans_back_;

		ObjectDraw* draw_tool_;

	public:
		/// Get all Info for the Object as a string
        QString getObjectinfo() override;
	};

} //namespace acamcad
