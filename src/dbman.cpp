/***************************************************************************
 *   Copyright (C) 2008 by Mehrdad Momeny, Golnaz Nilieh   *
 *   mehrdad.momeny@gmail.com, g382nilieh@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "dbman.h"
#include <QMessageBox>
#include <QDebug>

DBMan::DBMan()
{
	qDebug("DBMan::DBMan");
	
	if(!QFile::exists(CONF_DB)){
		if(!this->createDB()){
			QMessageBox::critical(0, "DB Error", "cannot create configuration database");
			qDebug()<<"DBMan::DBMan : cannot create configuration database, SQL error: "<<db.lastError().text()<<endl;
		}
	}else if (!connectDB())
		exit(1);
}

bool DBMan::connectDB()
{
	qDebug("DBMan::connectDB");
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(CONF_DB);
	
	if(!db.open()){
		QMessageBox::critical(0, "DB Error", "cannot connect to configuration database");
		qDebug()<<"DBMan::connectDB : cannot connect to configuration database, SQL error: "<<db.lastError().text()<<endl;
		return false;
	}
	return true;
}

DBMan::~DBMan()
{
}

bool DBMan::createDB()
{
	qDebug("DBMan::createDB");
	bool ret=true;
	if(!connectDB())
		exit(1);
	
	QSqlQuery q;
	///Blog table!
	if(!q.exec("CREATE TABLE blog (blogid INTEGER PRIMARY KEY, blog_url TEXT, username TEXT, password TEXT, style_url TEXT, api_type TEXT)"))
		ret=false;
	
	///posts table!
	if(!q.exec("CREATE TABLE post (id INTEGER PRIMARY KEY, postid NUMERIC, blogid NUMERIC, author TEXT, title TEXT, content TEXT, c_time TEXT, m_time TEXT, is_private NUMERIC, is_comment_allowed NUMERIC, is_trackback_allowed NUMERIC, link TEXT, perma_link TEXT, summary TEXT, tags TEXT);"))
		ret=false;
	
	///categories table!
	if(!q.exec("CREATE TABLE category (catid INTEGER PRIMARY KEY, name TEXT, blogid NUMERIC);"))
		ret=false;
	
	///files table
	if(!q.exec("CREATE TABLE file (fileid INTEGER PRIMARY KEY, name TEXT, blogid NUMERIC, is_uploaded NUMERIC, local_url TEXT, remote_url TEXT);"))
		ret=false;
	
	///connection bethween posts and categories
	if(!q.exec("CREATE TABLE post_cat (postid INTEGER, catid INTEGER, PRIMARY KEY(postid, catid));"))
		ret=false;
	
	
	///this will implement using clearPosts() , clearCategories() and clearFiles() in application level!
// 	q.exec("CREATE TRIGGER delete_post AFTER DELETE ON post BEGIN delete from post_cat WHERE postid=OLD.postid; END");
// 	q.exec("CREATE TRIGGER delete_blog AFTER DELETE ON blog BEGIN DELETE from category WHERE blogid=OLD.blogid; DELETE from file WHERE blogid=OLD.blogid; END");
	
	return ret;
}

bool DBMan::addBlog(int blogid, QString blog_url, QString username, QString password, QString style_url, int api)
{
	QSqlQuery q;
	q.prepare("INSERT INTO blog (blogid, blog_url, username, password, style_url, api_type) VALUES(?, ?, ?, ?, ?, ?)");
	q.addBindValue(blogid);
	q.addBindValue(blog_url);
	q.addBindValue(username);
	q.addBindValue(password);
	q.addBindValue(style_url);
	q.addBindValue(api);
	
	return q.exec();
}

/**
 * Edit a previously added blog
 * @param blogid it's not changed! but just for refer to blog!
 * @param username 
 * @param password 
 * @param style_url 
 * @param api 
 * @return 
 */
bool DBMan::editBlog(int blogid, QString username, QString password, QString style_url, int api)
{
	QSqlQuery q;
	q.prepare("UPDATE blog SET username=? , password=? , style_url=? , api_type=? WHERE blogid=?");
	q.addBindValue(username);
	q.addBindValue(password);
	q.addBindValue(style_url);
	q.addBindValue(api);
	q.addBindValue(blogid);
	
	return q.exec();
}

bool DBMan::removeBlog(int blogid)
{
	QSqlQuery q;
	q.prepare("DELETE FROM blog WHERE blogid=?");
	q.addBindValue(blogid);
	return q.exec();
}

int DBMan::addPost(int postid, int blogid, QString author, QString title, QString & content, QString c_time, bool is_private, bool is_comment_allowed, bool is_trackback_allowed, QString link, QString perma_link, QString summary, QString tags)
{
	QSqlQuery q;
	q.prepare("INSERT INTO post (postid, blogid, author, title, content, c_time, m_time, is_private, is_comment_allowed, is_trackback_allowed, link, perma_link, summary, tags) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	q.addBindValue(postid);
	q.addBindValue(blogid);
	q.addBindValue(author);
	q.addBindValue(title);
	q.addBindValue(content);
	q.addBindValue(c_time);
	q.addBindValue(c_time);
	q.addBindValue(is_private);
	q.addBindValue(is_comment_allowed);
	q.addBindValue(is_trackback_allowed);
	q.addBindValue(link);
	q.addBindValue(perma_link);
	q.addBindValue(summary);
	q.addBindValue(tags);
	
	if(q.exec())
		return q.lastInsertId().toInt();
	else
		return -1;
}

bool DBMan::editPost(int id, int blogid, int postid, QString author, QString title, QString & content, QString c_time, QString m_time, bool is_private, bool is_comment_allowed, bool is_trackback_allowed, QString link, QString perma_link, QString summary, QString tags)
{
	QSqlQuery q;
	q.prepare("UPDATE post SET blogid=?, postid=?, author=?, title=?, content=?, c_time=?, m_time=?, is_private=?, is_comment_allowed=?, is_trackback_allowed=?, link=?, perma_link=?, summary=?, tags=? WHERE id=?");
	q.addBindValue(blogid);
	q.addBindValue(postid);
	q.addBindValue(author);
	q.addBindValue(title);
	q.addBindValue(content);
	q.addBindValue(c_time);
	q.addBindValue(m_time);
	q.addBindValue(is_private);
	q.addBindValue(is_comment_allowed);
	q.addBindValue(is_trackback_allowed);
	q.addBindValue(link);
	q.addBindValue(perma_link);
	q.addBindValue(summary);
	q.addBindValue(tags);
	q.addBindValue(id);
	
	return q.exec();
}

bool DBMan::removePost(int id)
{
	QSqlQuery q;
	q.prepare("DELETE FROM post WHERE id=?");
	q.addBindValue(id);
	return q.exec();
}

bool DBMan::clearPosts(int blogid)
{
	QSqlQuery q;
	q.prepare("DELETE FROM post WHERE blogid=?");
	q.addBindValue(blogid);
	return q.exec();
}

bool DBMan::addCategory(int catid, QString name, int blogid)
{
	QSqlQuery q;
	q.prepare("INSERT INTO category (catid, name, blogid) VALUES(?, ?, ?)");
	q.addBindValue(catid);
	q.addBindValue(name);
	q.addBindValue(blogid);
	
	return q.exec();
}

bool DBMan::clearCategories(int blogid)
{
	QSqlQuery q;
	q.prepare("DELETE FROM category WHERE blogid=?");
	q.addBindValue(blogid);
	return q.exec();
}

int DBMan::addFile(QString name, int blogid, bool isUploaded, QString localUrl, QString remoteUrl)
{
	QSqlQuery q;
	q.prepare("INSERT INTO file(name, blogid, is_uploaded, local_url, remote_url) VALUES(?, ?, ?, ?, ?)");
	q.addBindValue(name);
	q.addBindValue(blogid);
	q.addBindValue(isUploaded);
	q.addBindValue(localUrl);
	q.addBindValue(remoteUrl);
	
	if(q.exec())
		return q.lastInsertId().toInt();
	else
		return -1;
}

bool DBMan::removeFile(int fileid)
{
	QSqlQuery q;
	q.prepare("DELETE FROM file WHERE fileid=?");
	q.addBindValue(fileid);
	return q.exec();
}

bool DBMan::clearFiles(int blogid)
{
	QSqlQuery q;
	q.prepare("DELETE FROM file WHERE blogid=?");
	q.addBindValue(blogid);
	return q.exec();
}

