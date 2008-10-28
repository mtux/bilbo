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
	if(!q.exec("CREATE TABLE blog (id INTEGER PRIMARY KEY, blogid TEXT, blog_url TEXT, username TEXT, password TEXT, style_url TEXT, api_type TEXT)"))
		ret=false;
	
	///posts table!
	if(!q.exec("CREATE TABLE post (id INTEGER PRIMARY KEY, postid TEXT, blog_id NUMERIC, author TEXT, title TEXT, content TEXT, c_time TEXT, m_time TEXT, is_private NUMERIC, is_comment_allowed NUMERIC, is_trackback_allowed NUMERIC, link TEXT, perma_link TEXT, summary TEXT, tags TEXT);"))
		ret=false;
	
	///categories table!
	if(!q.exec("CREATE TABLE category (catid INTEGER PRIMARY KEY, name TEXT, blog_id NUMERIC);"))
		ret=false;
	
	///files table
	if(!q.exec("CREATE TABLE file (fileid INTEGER PRIMARY KEY, name TEXT, blog_id NUMERIC, is_uploaded NUMERIC, local_url TEXT, remote_url TEXT);"))
		ret=false;
	
	///connection bethween posts and categories
	if(!q.exec("CREATE TABLE post_cat (post_id NUMERIC, cat_id INTEGER, PRIMARY KEY(post_id, cat_id));"))
		ret=false;
	
	
	///this will implement using clearPosts() , clearCategories() and clearFiles() in application level!
// 	q.exec("CREATE TRIGGER delete_post AFTER DELETE ON post BEGIN delete from post_cat WHERE postid=OLD.postid; END");
// 	q.exec("CREATE TRIGGER delete_blog AFTER DELETE ON blog BEGIN DELETE from category WHERE blogid=OLD.blogid; DELETE from file WHERE blogid=OLD.blogid; END");
	
	return ret;
}

int DBMan::addBlog(QString blogid, QString blog_url, QString username, QString password, QString style_url, QString api)
{
	QSqlQuery q;
	q.prepare("INSERT INTO blog (blogid, blog_url, username, password, style_url, api_type) VALUES(?, ?, ?, ?, ?, ?)");
	q.addBindValue(blogid);
	q.addBindValue(blog_url);
	q.addBindValue(username);
	q.addBindValue(password);
	q.addBindValue(style_url);
	q.addBindValue(api);
	
	if(q.exec())
		return q.lastInsertId().toInt();
	else
		return -1;
}

int DBMan::addBlog(BilboBlog & blog)
{
	QSqlQuery q;
	q.prepare("INSERT INTO blog (blogid, blog_url, username, password, style_url, api_type) VALUES(?, ?, ?, ?, ?, ?)");
	q.addBindValue(blog.blogId());
	q.addBindValue(blog.blogUrl().toString());
	q.addBindValue(blog.username());
	q.addBindValue(blog.password());
	q.addBindValue(blog.styleUrl);
	q.addBindValue(blog.api);
	
	if(q.exec())
		return q.lastInsertId().toInt();
	else
		return -1;
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
bool DBMan::editBlog(int id, QString username, QString password, QString style_url, QString api)
{
	QSqlQuery q;
	q.prepare("UPDATE blog SET username=? , password=? , style_url=? , api_type=? WHERE id=?");
	q.addBindValue(username);
	q.addBindValue(password);
	q.addBindValue(style_url);
	q.addBindValue(api);
	q.addBindValue(id);
	
	return q.exec();
}

bool DBMan::removeBlog(int blog_id)
{
	QSqlQuery q;
	q.prepare("DELETE FROM blog WHERE id=?");
	q.addBindValue(blog_id);
	return q.exec();
}

/**
 * 
 * @param postid 
 * @param blogid 
 * @param author 
 * @param title 
 * @param content 
 * @param c_time 
 * @param is_private 
 * @param is_comment_allowed 
 * @param is_trackback_allowed 
 * @param link 
 * @param perma_link 
 * @param summary 
 * @param tags 
 * @return return post id in database (deffer with postid)
 */
int DBMan::addPost(QString postid, int blog_id, QString author, QString title, QString & content, QString c_time, bool is_private, bool is_comment_allowed, bool is_trackback_allowed, QString link, QString perma_link, QString summary, QString tags)
{
	QSqlQuery q;
	q.prepare("INSERT INTO post (postid, blog_id, author, title, content, c_time, m_time, is_private, is_comment_allowed, is_trackback_allowed, link, perma_link, summary, tags) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	q.addBindValue(postid);
	q.addBindValue(blog_id);
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

int DBMan::addPost(BilboPost & post, int blog_id)
{
	QSqlQuery q;
	q.prepare("INSERT INTO post (postid, blog_id, author, title, content, c_time, m_time, is_private, is_comment_allowed, is_trackback_allowed, link, perma_link, summary, tags) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	q.addBindValue(post.postId());
	q.addBindValue(blog_id);
	q.addBindValue(post.author);
	q.addBindValue(post.author);
	q.addBindValue(post.content());
	q.addBindValue(post.cTime().toString(Qt::ISODate));
	q.addBindValue(post.cTime().toString(Qt::ISODate));
	q.addBindValue(post.isPrivate());
	q.addBindValue(post.isCommentAllowed());
	q.addBindValue(post.isTrackBackAllowed());
	q.addBindValue(post.postLink().toString());
	q.addBindValue(post.postPermaLink().toString());
	q.addBindValue(post.summary());
	QString tags="";
	int i=0;
	while(i<post.tags().count())
		tags = post.tags()[i]+",";
	tags.remove(tags.length()-1, 1);
	q.addBindValue(tags);
	
	if(q.exec())
		return q.lastInsertId().toInt();
	else
		return -1;
}

bool DBMan::editPost(int id, int blog_id, QString postid, QString author, QString title, QString & content, QString c_time, QString m_time, bool is_private, bool is_comment_allowed, bool is_trackback_allowed, QString link, QString perma_link, QString summary, QString tags)
{
	QSqlQuery q;
	q.prepare("UPDATE post SET blog_id=?, postid=?, author=?, title=?, content=?, c_time=?, m_time=?, is_private=?, is_comment_allowed=?, is_trackback_allowed=?, link=?, perma_link=?, summary=?, tags=? WHERE id=?");
	q.addBindValue(blog_id);
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

bool DBMan::clearPosts(int blog_id)
{
	QSqlQuery q;
	q.prepare("DELETE FROM post WHERE blog_id=?");
	q.addBindValue(blog_id);
	return q.exec();
}

int DBMan::addCategory(QString name, int blog_id)
{
	QSqlQuery q;
	q.prepare("INSERT INTO category (name, blog_id) VALUES(?, ?)");
	q.addBindValue(name);
	q.addBindValue(blog_id);
	
	if(q.exec())
		return q.lastInsertId().toInt();
	else
		return -1;
}

bool DBMan::clearCategories(int blog_id)
{
	QSqlQuery q;
	q.prepare("DELETE FROM category WHERE blog_id=?");
	q.addBindValue(blog_id);
	return q.exec();
}

int DBMan::addFile(QString name, int blog_id, bool isUploaded, QString localUrl, QString remoteUrl)
{
	QSqlQuery q;
	q.prepare("INSERT INTO file(name, blog_id, is_uploaded, local_url, remote_url) VALUES(?, ?, ?, ?, ?)");
	q.addBindValue(name);
	q.addBindValue(blog_id);
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

bool DBMan::clearFiles(int blog_id)
{
	QSqlQuery q;
	q.prepare("DELETE FROM file WHERE blog_id=?");
	q.addBindValue(blog_id);
	return q.exec();
}

// QList< BilboBlog > DBMan::listBlogs()
// {
// 	QList<BilboBlog> list;
// // 	BilboBlog tmp(QUrl(""));
// 	QSqlQuery q;
// 	q.exec("SELECT id, blogid, blog_url, username, password, style_url, api_type FROM blog");
// 	while( q.next() ){
// // 		tmp= new BilboBlog(QUrl(q.value(2).toString()));
// 	}
// 	
// // 	return list;
// }


