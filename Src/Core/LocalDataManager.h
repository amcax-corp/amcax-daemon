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

#include <vector>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <memory>
#include <QByteArray>


namespace acamcad
{
	class LocalDataManager
	{
	public:
		LocalDataManager();
		~LocalDataManager();

		struct DataModelHistory
		{
			int step;
			int persistentId;
			int type;
			QString data;
		};

		void execStore(const std::vector<DataModelHistory>& models);
		//bool execGet(QSqlQuery& query);


		struct DataModelRecent
		{
			QString file;
			long long time;
		};

		void execStore(const std::vector<DataModelRecent>& models);
		void execGet(std::vector<DataModelRecent>& models);

		struct DataModelPosition
		{
			QByteArray array;

		};
		void execStore(const DataModelPosition& position);
		void execGet(DataModelPosition& models);

	protected:

		bool initData(QSqlDatabase& database, QString db_name);

	protected:
		QSqlDatabase database_;
		///QString db_name_;
	};
}