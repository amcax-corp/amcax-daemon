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

#include <common/CoordT.hpp>
#include <set>
#include <vector>

namespace acamcad
{

enum class SelectModel
{
	VERTEX_MODEL, 
	EDGE_MODEL, 
	FACE_MODEL, 
	OBJECT_MODEL, 
	CLICK_MODEL		//click model, used in add face to object, the click will in work plane;
};

struct PyramidInf
{
	AMCAX::Coord3 ori_;
	std::vector<AMCAX::Coord3> dir_;
};

struct SelectInfoWithCamera
{
	int object_id_ = -1;

	int object_subpart_id_ = -1;			//选中的可绘制的子物体，对于网格，就是鼠标点击的面的id
	int object_subselect_id_ = -1;			//选中的真实的id，对于网格就是，顶点，边的id

	AMCAX::Coord3 c_begin_;
	AMCAX::Coord3 c_end_;

	bool operator < (const SelectInfoWithCamera& rhs)
	{
		if (object_id_ < rhs.object_id_)
			return true;
		else if (object_id_ == rhs.object_id_)
		{
			if (object_subselect_id_ < rhs.object_subselect_id_)
				return true;
			else
				return false;
		}
		else
			return false;
	}

	bool operator == (const SelectInfoWithCamera& rhs)
	{
		if (object_id_ == rhs.object_id_)
		{
			if (object_subselect_id_ == rhs.object_subselect_id_)
				return true;
			else
				return false;
		}
		else
			return false;
	}
};

struct SelectInfo
{
	int object_id_;
	int object_subselect_id_;		//选中的真实的id，对于网格就是，顶点，边的id

	SelectInfo()
	{
		object_id_ = -1;
		object_subselect_id_ = -1;
	}

	SelectInfo(int o_id, int os_id)
	{
		object_id_ = o_id;
		object_subselect_id_ = os_id;
	}

	bool operator < (const SelectInfo& rhs)
	{
		if (object_id_ < rhs.object_id_)
			return true;
		else if (object_id_ == rhs.object_id_)
		{
			if (object_subselect_id_ < rhs.object_subselect_id_)
				return true;
			else
				return false;
		}
		else
			return false;
	}

	bool operator == (const SelectInfo& rhs) const 
	{
		if (object_id_ == rhs.object_id_)
		{
			if (object_subselect_id_ == rhs.object_subselect_id_)
				return true;
			else
				return false;
		}
		else
			return false;
	}
};


inline void extractSelectInfoList(const std::vector<SelectInfoWithCamera>& sc_info_list, std::vector<SelectInfo>& s_info_list)
{
	s_info_list.resize(sc_info_list.size());
	for (int i = 0; i < sc_info_list.size(); i++)
	{
		s_info_list[i].object_id_ = sc_info_list[i].object_id_;
		s_info_list[i].object_subselect_id_ = sc_info_list[i].object_subselect_id_;
	}
}

inline std::vector<int> getSelectObjectList(const std::vector<SelectInfoWithCamera>& s_info_list)
{
	std::set<int> s_id_list;
	for (int i = 0; i < s_info_list.size(); i++)
	{
		s_id_list.insert(s_info_list[i].object_id_);
	}
	std::vector<int> so_id_list(s_id_list.begin(), s_id_list.end());
	return so_id_list;
}

inline std::vector<int> getSelectSubList(const std::vector<SelectInfoWithCamera>& s_info_list)
{
	std::vector<int> s_id_list(s_info_list.size());
	for (int i = 0; i < s_info_list.size(); i++)
	{
		s_id_list[i] = s_info_list[i].object_subselect_id_;
	}
	return s_id_list;
}

inline std::vector<int> getSelectSubListWithObject(const std::vector<SelectInfoWithCamera>& s_info_list, const int& object_id)
{
	std::vector<int> s_id_list; s_id_list.reserve(s_info_list.size());
	for (int i = 0; i < s_info_list.size(); i++)
	{
		if (s_info_list[i].object_id_ == object_id)
		{
			s_id_list.push_back(s_info_list[i].object_subselect_id_);
		}
	}
	return s_id_list;
}


inline bool isSelectOneObject(const std::vector<SelectInfo>& s_info_list)
{
	int object_id = s_info_list[0].object_id_;
	for (int i = 0; i < s_info_list.size(); i++)
	{
		if (s_info_list[i].object_id_ != object_id)
		{
			return false;
		}
	}
	return true;
}

inline std::vector<int> getSelectObjectList(const std::vector<SelectInfo>& s_info_list)
{
	std::set<int> s_id_list;
	for (int i = 0; i < s_info_list.size(); i++)
	{
		s_id_list.insert(s_info_list[i].object_id_);
	}
	std::vector<int> so_id_list(s_id_list.begin(), s_id_list.end());
	return so_id_list;
}

inline std::vector<int> getSelectSubList(const std::vector<SelectInfo>& s_info_list)
{
	std::vector<int> s_id_list(s_info_list.size());
	for (int i = 0; i < s_info_list.size(); i++)
	{
		s_id_list[i] = s_info_list[i].object_subselect_id_;
	}
	return s_id_list;
}

inline std::vector<int> getSelectSubListWithObject(const std::vector<SelectInfo>& s_info_list, const int& object_id)
{
	std::vector<int> s_id_list; s_id_list.reserve(s_info_list.size());
	for (int i = 0; i < s_info_list.size(); i++)
	{
		if (s_info_list[i].object_id_ == object_id)
		{
			s_id_list.push_back(s_info_list[i].object_subselect_id_);
		}
	}
	return s_id_list;
}

inline int findSelect(const SelectModel& s_model, const std::vector<SelectInfo>& s_info_list, const SelectInfo& s_info)
{
	for (int i = 0; i < s_info_list.size(); i++)
	{
		if (s_info_list[i] == s_info)
		{
			return i;
		}
	}

	return -1;
}


//===================================================================

struct ClickInfo
{
	AMCAX::Coord3 point_;
};

inline std::vector<AMCAX::Coord3> getClickPointlist(const std::vector<ClickInfo>& c_info_list)
{
	std::vector<AMCAX::Coord3> point_list(c_info_list.size());
	for (int i = 0; i < c_info_list.size(); i++)
	{
		const ClickInfo& c_info = c_info_list[i];
		point_list[i] = c_info.point_;
	}
	return point_list;
}; 

}// namespace acamcad