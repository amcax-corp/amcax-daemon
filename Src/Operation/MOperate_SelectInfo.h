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
#include "MOperate.h"
#include <common/CoordT.hpp>

namespace acamcad
{

class MOperate_SingleObject : public MOperation
{
public:
	MOperate_SingleObject(const SelectModel& smodle);
	virtual ~MOperate_SingleObject() { s_info_list_.clear(); }

public:
	virtual void setOperateId(int object_id, const std::vector<int>& s_info_list );
	virtual void setselectInfo(const std::vector<SelectInfo>& s_info_list );

	virtual std::vector<SelectInfo> getSelectInfoNew() const;
	virtual std::vector<int> getOperateIdNew() const;

public:
	virtual void setParameter() {};
	virtual void setParameter(const AMCAX::Coord3 & direction) {};
	virtual void setParameter(const AMCAX::Coord3 & center, const AMCAX::Coord3 & axis) {};
	virtual void setParameter(const std::vector<AMCAX::Coord3>& related_point) {};

	virtual AMCAX::Coord3 getDirection() { return AMCAX::Coord3(); }
	virtual AMCAX::Coord3 getCoord3Direction() { return AMCAX::Coord3(); }

protected:
	std::vector<SelectInfo> s_info_list_;

};

} //namespace acamcad