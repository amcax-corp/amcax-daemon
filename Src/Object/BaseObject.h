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

#include <QObject>
#include "ObjectType.h"

namespace acamcad
{
class MOperation;

class BaseObject : public QObject
{
	Q_OBJECT
public:
	BaseObject(const BaseObject& _object);
	explicit BaseObject(QObject* parent = nullptr);
	virtual ~BaseObject();

public:
	int id() const { return id_; }
	int persistentId() const { return persistentId_; }
	std::string label() const { return label_; }

	int id() { return id_; }
	int persistentId() { return persistentId_; }
	std::string label() { return label_; }

	void setId(int id) { id_ = id; }
	void setPersistentId(int pid) { persistentId_ = pid; }
	void setLabel(const std::string &label) { label_ = label; }

private:
	int id_;

	int persistentId_;

	// QString name is not used but i can't decide what is it for.
	// Plus it's better to use std::string in non-gui related code
	// instead of QString, so that they could be reused in future projects.
	// Therefore label_ is added here.
	// It enables the abililty to classify BaseObject(s) by it's label, which
	// could will probably be read from json file, or other up-coming features.
	std::string label_;

public:
	/// check if the object is of the given type
	bool isDataType(DataType _type) const;

	/// return the backObjectType of the object
	DataType dataType() const;

	/// set the object type
	void setDataType(DataType _type);

private:
	DataType objectType_;

public:
	virtual bool isVisible() const;

	virtual void setVisible(bool _visible);

	virtual void show();
	virtual void hide();

protected:
	bool visible_;

public:
	friend class ObjectDraw;
	//virtual const ObjectDraw* getDrawTool() const = 0;

	virtual const ObjectDraw* getDrawTool() const { return nullptr; }

public:
	virtual void doOperate(MOperation* operate);
	virtual void updateDraw() {}

public:
	virtual bool isSelect() const;

	virtual void setSelect(bool _select);

	virtual bool isSelectSub() const;

	virtual void setSelectSub(bool _select);

protected:
	bool select_;
	bool select_sub_;

public:
	virtual void setFromFileName(const QString& _filename);

	QString path() const;
	QString name() const;
	QString filename() const;

	void setPath(const QString& _path);
	void setName(QString _name);
	void setFileName(const QString& _filename);

private:
	QString path_;
	QString filename_;
	QString name_;

public:
	virtual QString getObjectinfo();
	virtual void printObjectInfo();

};

} //acamcad
