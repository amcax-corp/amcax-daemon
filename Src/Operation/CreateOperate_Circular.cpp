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
#include "CreateOperate_Circular.h"
#include <modeling/MakeFace.hpp>

namespace acamcad
{

CreateOperate_Circular::CreateOperate_Circular()
	:radius_(1.0), angle_(M_PI*2), seg_(8)
{
}


void CreateOperate_Circular::operateWithBRep(BRepObject *brep_object) 
{
	// Doing nothing
}


}