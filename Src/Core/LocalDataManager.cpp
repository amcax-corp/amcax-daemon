#include "LocalDataManager.h"
#include <qstandardpaths.h>
#include <iostream>
#include <qdir.h>
#include <QVariant>

namespace acamcad
{
	LocalDataManager::LocalDataManager()
	{
		//initData();
		initData(database_, "history.db");
	}

	LocalDataManager::~LocalDataManager()
	{
		QSqlQuery query;
		query.exec("DELETE FROM model;");
		database_.close();
	}



	bool LocalDataManager::initData(QSqlDatabase& database, QString db_name)
	{

		///QSqlDatabase database;
		QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
		if (!dir.exists())
		{
			dir.mkdir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
		}

		if (QSqlDatabase::contains("qt_sql_default_connection"))
		{
			database_ = QSqlDatabase::database("qt_sql_default_connection");
		}
		else
		{
			// 建立和SQlite数据库的连接
			database_ = QSqlDatabase::addDatabase("QSQLITE");
			// 设置数据库文件的名字
			//QString file = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/MyDataBase.db";
			QString file = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + db_name;
			//QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
			//std::cout << "LocalDataManager::initHistory :" << file.toStdString() << std::endl;
			database_.setDatabaseName(file);
			std::cout << "database:" << file.toStdString() << std::endl;
		}

		if (!database_.open())
		{
			std::cout << "Error: Failed to connect database." << database_.lastError().nativeErrorCode().toStdString() << std::endl;
			return false;
		}



		// do something
		// 用于执行sql语句的对象
		QSqlQuery sqlQuery;

		// 执行sql语句
		if (!sqlQuery.exec("CREATE TABLE version (\
                          name TEXT NOT NULL primary key,\
                          ver INT NOT NULL)"))
		{
			std::cout << "Error: Fail to create table version. " << sqlQuery.lastError().nativeErrorCode().toStdString() << std::endl;
		}


		// 执行sql语句
		if (!sqlQuery.exec("CREATE TABLE model (\
                          step INT NOT NULL,\
                          persistentId INT NOT NULL,\
                          type INT NOT NULL,\
                          data TEXT NOT NULL)"))
		{
			std::cout << "Error: Fail to create table model. " << sqlQuery.lastError().nativeErrorCode().toStdString() << std::endl;
		}
		else
			sqlQuery.exec("insert into version(name,ver) values('model',1)");

		sqlQuery.exec("DELETE FROM model;");


		if (!sqlQuery.exec("CREATE TABLE file_recent (\
                          file TEXT NOT NULL,\
                          time INTEGER NOT NULL)"))
		{
			std::cout << "Error: Fail to create table file_recent. " << sqlQuery.lastError().nativeErrorCode().toStdString() << std::endl;
		}
		else
			sqlQuery.exec("insert into version(name,ver) values('file_recent',1)");

		// 执行sql语句
		if (!sqlQuery.exec("CREATE TABLE position (\
                          id INT NOT NULL primary key,\
                          array BLOB NOT NULL)"))
		{
			std::cout << "Error: Fail to create table position. " << sqlQuery.lastError().nativeErrorCode().toStdString() << std::endl;
		}
		else
		{
			sqlQuery.exec("insert into version(name,ver) values('position',1)");
			sqlQuery.exec("insert into position(id,array) values(1,'')");
		}



		//sqlQuery.exec("VACUUM;PRAGMA auto_vacuum = 2;PRAGMA page_size = 4096;");
		return true;
	}

	void LocalDataManager::execStore(const std::vector<DataModelHistory>& models)
	{
		//std::cout << "LocalDataManager::exe!" << std::endl;
		//QString qsQuery;
		QSqlQuery query(database_);

		query.prepare("insert into model(step,persistentId,type,data) values(?,?,?,?)");

		QVariantList steps, persistentIds, types, datas;

		for (int i = 0; i < models.size(); i++)
		{
			auto& data = models[i];

			steps << data.step;
			persistentIds << data.persistentId;
			types << data.type;
			datas << data.data;
		}

		query.addBindValue(steps);
		query.addBindValue(persistentIds);
		query.addBindValue(types);
		query.addBindValue(datas);

		if (!query.execBatch())
		{
			std::cout << "LocalDataManager::exec " << query.lastError().databaseText().toStdString() << std::endl;
		}

	}

	void LocalDataManager::execStore(const std::vector<DataModelRecent>& models)
	{
		QVariantList files, times;
		for (auto& iter : models)
		{
			files << iter.file;
			times << iter.time;
		}

		QSqlQuery query(database_);
		query.prepare("delete  from file_recent where  file = ?");
		query.addBindValue(files);
		query.execBatch();

		query.prepare("insert into file_recent(file,time) values(?,?)");
		query.addBindValue(files);
		query.addBindValue(times);

		if (!query.execBatch())
		{
			std::cout << "LocalDataManager::exec  " << query.lastError().databaseText().toStdString() << std::endl;
		}

		if (query.exec("select file from file_recent order by time DESC  limit 10 offset 20"))
		{
			QVariantList files1;
			while (query.next())
			{
				files1 << query.value(0).toString();
			}

			if (!files1.isEmpty())
			{
				query.prepare("delete from file_recent where file =  ?");
				query.addBindValue(files1);

				query.execBatch();
			}

		}


	}

	void LocalDataManager::execGet(std::vector<DataModelRecent>& models)
	{
		QSqlQuery query(database_);
		if (query.exec("select file from file_recent order by time DESC  limit 20"))
		{
			while (query.next())
			{
				//DataModelRecent data(query.value(0).)
				models.push_back({ query.value(0).toString(),0 });
			}
		}

	}


	void LocalDataManager::execStore(const DataModelPosition& position)
	{

		QSqlQuery sqlQuery;
		sqlQuery.prepare("update position set array  = ? where id = 1");

		///QVariant bFullScreen, x, y, w, h;
		sqlQuery.addBindValue(QVariant::fromValue(position.array));

		sqlQuery.exec();
		//bFullScreen.setValue(position.bFullScreen);


	}

	void LocalDataManager::execGet(DataModelPosition& models)
	{
		QSqlQuery sqlQuery;

		if (sqlQuery.exec("select array from position where id  = 1"))
		{
			while (sqlQuery.next())
			{
				//QArrayData d(data);
				models.array = sqlQuery.value(0).value<QByteArray>();
			}

		}
	}

	//bool LocalDataManager::execGet(QSqlQuery& query)
	//{
	//	//QSqlQuery query(database_);
	//	return query.exec();

	//}
}