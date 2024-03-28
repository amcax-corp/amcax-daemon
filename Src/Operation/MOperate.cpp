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
#include "MOperate.h"
#include <exception>


namespace acamcad
{
	MOperation::MOperation() : selmodel_(SelectModel::OBJECT_MODEL)
	{
	}
	MOperation::MOperation(const SelectModel& smodel) : selmodel_(smodel)
	{
	}

	bool MOperation::operate(AdapterObject* object)
	{
		bool result = false;
		try {
			result = DoOperate(object);
		}
		catch (std::exception e) {
			std::cout << "error:" << e.what() << "!!!" << std::endl;
		}

		return result;
	}

	void MOperation::setSelectModel(const SelectModel& smodle)
	{
		selmodel_ = smodle;
	}

}


