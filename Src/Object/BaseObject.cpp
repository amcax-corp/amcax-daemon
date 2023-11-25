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
#include "BaseObject.h"
#include <qfileinfo.h>

namespace acamcad
{
BaseObject::BaseObject(QObject* parent)
	: QObject(), id_(-1), persistentId_(-1), label_(""),objectType_(DataType::ALL_TYPE),
	visible_(true), select_(false), select_sub_(false),path_("."), filename_(""), name_("")
{
}

BaseObject::BaseObject(const BaseObject& _object) :
	QObject(),id_(_object.id_),persistentId_(_object.persistentId_),label_(""),objectType_(_object.objectType_),
	visible_(_object.visible_), select_(_object.select_), select_sub_(_object.select_sub_), path_("."), filename_("")
{
	// Generate a usable name based on the copied object
	name_ = "Copy of " + _object.name_;
}

BaseObject::~BaseObject()
{
}

void BaseObject::doOperate(MOperation* operate)
{
}

bool acamcad::BaseObject::isDataType(DataType _type) const
{
	return (objectType_ == _type);
}

DataType acamcad::BaseObject::dataType() const
{
	return BaseObject::objectType_;
}

void acamcad::BaseObject::setDataType(DataType _type)
{
	objectType_ = _type;
}

QString BaseObject::getObjectinfo()
{
	return QString();
}

void BaseObject::printObjectInfo()
{
}

bool BaseObject::isVisible() const
{
	return visible_;
}

void BaseObject::setVisible(bool _visible)
{
	visible_ = _visible;
}

void BaseObject::show()
{
	visible_ = true;
}

void BaseObject::hide()
{
	visible_ = false;
}

bool BaseObject::isSelect() const
{
	return select_;
}

void BaseObject::setSelect(bool _select)
{
	select_ = _select;
}

bool BaseObject::isSelectSub() const
{
	return select_sub_;
}

void BaseObject::setSelectSub(bool _select)
{
	select_sub_ = _select;
}

//==========================================================

void BaseObject::setFromFileName(const QString& _filename) {
	QFileInfo file_info(_filename);
	setPath(file_info.path());
	QString filename = file_info.fileName();
	setFileName(filename);
}

QString BaseObject::path() const
{
	return path_;
}
void BaseObject::setPath(const QString& _path)
{
	path_ = _path;
}

QString BaseObject::name() const
{
	return name_;
}
void BaseObject::setName(QString _name)
{
	name_ = _name;
}

QString BaseObject::filename() const
{
	return filename_;
}
void BaseObject::setFileName(const QString& _filename)
{
	filename_ = _filename;
}

}