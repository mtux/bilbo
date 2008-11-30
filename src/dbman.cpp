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
#include <kmessagebox.h>
#include "bilboblog.h"
#include "bilbopost.h"
#include <kdebug.h>
#include <KDE/KLocale>

DBMan::DBMan()
{
	kDebug();
	
	if(!QFile::exists(CONF_DB)){
		if(!this->createDB()){
            KMessageBox::detailedError(0, i18n("Cannot create configuration database"), i18n(db.lastError().text().toUtf8().data()));
			kDebug()<<"Cannot create configuration database, SQL error: "<<db.lastError().text()<<endl;
		}
	}else if (!connectDB())
		exit(1);
}

bool DBMan::connectDB()
{
	kDebug();
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(CONF_DB);
	
	if(!db.open()){
        KMessageBox::detailedError(0, i18n("Cannot connect to configuration database"), i18n(db.lastError().text().toUtf8().data()));
		kDebug()<<"Cannot connect to configuration database, SQL error: "<<db.lastError().text()<<endl;
		return false;
	}
	return true;
}

DBMan::~DBMan()
{
}

bool DBMan::createDB()
{
	kDebug();
	bool ret=true;
	if(!connectDB())
		exit(1);
	
	QSqlQuery q;
	///Blog table!
	if(!q.exec("CREATE TABLE blog (id INTEGER PRIMARY KEY, blogid TEXT, blog_url TEXT, username TEXT, password TEXT, style_url TEXT, api_type TEXT, title TEXT, direction TEXT)"))
		ret=false;
	
	///posts table!
	if(!q.exec("CREATE TABLE post (id INTEGER PRIMARY KEY, postid TEXT, blog_id NUMERIC, author TEXT, title TEXT, content TEXT, c_time TEXT, m_time TEXT, is_private NUMERIC, is_comment_allowed NUMERIC, is_trackback_allowed NUMERIC, link TEXT, perma_link TEXT, summary TEXT, tags TEXT, position int);"))
		ret=false;
	
	///categories table!
	if(!q.exec("CREATE TABLE category (catid INTEGER PRIMARY KEY, name TEXT, blog_id NUMERIC);"))
		ret=false;
	
	///files table
	if(!q.exec("CREATE TABLE file (fileid INTEGER PRIMARY KEY, name TEXT, blog_id NUMERIC, is_uploaded NUMERIC, local_url TEXT, remote_url TEXT);"))
		ret=false;
	
	///connection bethween posts and categories
	if(!q.exec("CREATE TABLE post_cat (post_id INTEGER, cat_id INTEGER);"))
		ret=false;
	
	
	///this will implement using clearPosts() , clearCategories() and clearFiles() in application level!
	q.exec("CREATE TRIGGER delete_post AFTER DELETE ON post BEGIN DELETE from post_cat WHERE post_id=OLD.id; END");
	q.exec("CREATE TRIGGER delete_blog AFTER DELETE ON blog BEGIN DELETE from category WHERE category.blog_id=OLD.id; DELETE from file WHERE file.blog_id=OLD.id; DELETE from post WHERE post.blog_id=OLD.id; END");
	
	return ret;
}

int DBMan::addBlog(QString blogid, QString blog_url, QString username, QString password, QString style_url, QString api, QString title, int direction)
{
	QSqlQuery q;
	q.prepare("INSERT INTO blog (blogid, blog_url, username, password, style_url, api_type, title, direction) VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
	q.addBindValue(blogid);
	q.addBindValue(blog_url);
	q.addBindValue(username);
	q.addBindValue(password);
	q.addBindValue(style_url);
	q.addBindValue(api);
	q.addBindValue(title);
	q.addBindValue(direction);
	
	if(q.exec())
		return q.lastInsertId().toInt();
	else
		return -1;
}

int DBMan::addBlog(BilboBlog & blog)
{
	QSqlQuery q;
	q.prepare("INSERT INTO blog (blogid, blog_url, username, password, style_url, api_type, title, direction) VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
	q.addBindValue(blog.blogid());
	//q.addBindValue(blog.url().toString());
	q.addBindValue(blog.url().url());
	q.addBindValue(blog.username());
	q.addBindValue(blog.password());
	q.addBindValue(blog.stylePath());
	q.addBindValue(blog.api());
	q.addBindValue(blog.title());
	q.addBindValue(blog.direction());
	
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
bool DBMan::editBlog(int id, QString username, QString password, QString style_url, QString api, QString title, int direction)
{
	QSqlQuery q;
	q.prepare("UPDATE blog SET username=? , password=? , style_url=? , api_type=?, title=?, direction=? WHERE id=?");
	q.addBindValue(username);
	q.addBindValue(password);
	q.addBindValue(style_url);
	q.addBindValue(api);
	q.addBindValue(title);
	q.addBindValue(direction);
	q.addBindValue(id);
	
	return q.exec();
}

bool DBMan::editBlog(BilboBlog & blog)
{
	QSqlQuery q;
	q.prepare("UPDATE blog SET username=? , password=? , style_url=? , api_type=?, title=?, direction=? WHERE id=?");
	q.addBindValue(blog.username());
	q.addBindValue(blog.password());
	q.addBindValue(blog.stylePath());
	q.addBindValue(blog.api());
	q.addBindValue(blog.title());
	q.addBindValue(blog.direction());
	q.addBindValue(blog.id());
	
	return q.exec();
}

bool DBMan::removeBlog(int blog_id)
{
	QSqlQuery q;
	q.prepare("DELETE FROM blog WHERE id=?");
	q.addBindValue(blog_id);
	return q.exec();
}

int DBMan::addPost(QString postid, int blog_id, QString author, QString title, QString & content, QString c_time, bool is_private, bool is_comment_allowed, bool is_trackback_allowed, QString link, QString perma_link, QString summary, QString tags, QStringList categories, int position)
{
	QSqlQuery q;
	q.prepare("INSERT INTO post (postid, blog_id, author, title, content, c_time, m_time, is_private, is_comment_allowed, is_trackback_allowed, link, perma_link, summary, tags, position) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
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
	q.addBindValue(position);
	int ret;
	if(q.exec()){
		ret = q.lastInsertId().toInt();
		int cat_count = categories.count();
		int i=0;
		QSqlQuery q, q2;
		q.prepare("SELECT catid FROM category WHERE name = ? AND blog_id= ?");
		q2.prepare("INSERT INTO post_cat (post_id, cat_id) VALUES(?, ?)");
		while(i<cat_count){
			q.addBindValue(categories[i]);
			q.addBindValue(blog_id);
			if(!q.exec())
				kDebug()<<"Cannot get category id for category "<< categories[i];
			else
				if(q.next()){
					q2.addBindValue(ret);
					q2.addBindValue(q.value(0).toInt());
					if(q2.exec())
						kDebug()<<"Category "<< categories[i] <<"added to post.";
				}
		}
	}
	else
		ret = -1;
	
	return ret;
}

int DBMan::addPost(const BilboPost & post, int blog_id)
{
	kDebug()<<"Adding post with title: "<< post.title() <<" to Blog "<< blog_id;
	QSqlQuery q;
	q.prepare("INSERT INTO post (postid, blog_id, author, title, content, c_time, m_time, is_private, is_comment_allowed, is_trackback_allowed, link, perma_link, summary, tags, position) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	q.addBindValue(post.postId());
	q.addBindValue(blog_id);
	q.addBindValue(post.author());
	q.addBindValue(post.title());
	q.addBindValue(post.content());
	q.addBindValue(post.cTime().toString(Qt::ISODate));
	q.addBindValue(post.cTime().toString(Qt::ISODate));
	q.addBindValue(post.isPrivate());
	q.addBindValue(post.isCommentAllowed());
	q.addBindValue(post.isTrackBackAllowed());
	q.addBindValue(post.postLink().toString());
	q.addBindValue(post.postPermaLink().toString());
	q.addBindValue(post.summary());
	QString tags=QString("");
	
	if(post.tags().count()>0){
		int i=0;
		while(i<post.tags().count()){
			tags = post.tags()[i]+",";
			i++;
		}
		tags.remove(tags.length()-1, 1);
		q.addBindValue(tags);
	} else 
		q.addBindValue(QString("Untagged"));
	q.addBindValue(post.position());
	
	int ret;
	if(q.exec()){
		ret = q.lastInsertId().toInt();
		int cat_count = post.categories().count();
		int i=0;
		QSqlQuery q, q2;int catid;
		q.prepare("SELECT catid FROM category WHERE name = ? AND blog_id= ?");
		q2.prepare("INSERT INTO post_cat (post_id, cat_id) VALUES(?, ?)");
		while(i<cat_count){
			q.addBindValue(post.categories()[i]);
			q.addBindValue(blog_id);
			if(!q.exec())
				kDebug()<<"Cannot get category id for category "<< post.categories()[i];
			else
				if(q.next()){
					catid = q.value(0).toInt();
					q2.addBindValue(ret);
					q2.addBindValue(catid);
					if(!q2.exec()){
                        kDebug()<<"Cannot add Category "<< catid <<" to Post, SQL Error: "<< q2.lastError().text();
					}
				}
			i++;
		}
	}
	else{
		kDebug()<<"Cannot Add post to database!\n\tSQL Error: "<< q.lastError().text();
		ret = -1;
	}
	
	return ret;
}

bool DBMan::editPost(int id, int blog_id, QString postid, QString author, QString title, QString & content, QString c_time, QString m_time, bool is_private, bool is_comment_allowed, bool is_trackback_allowed, QString link, QString perma_link, QString summary, QString tags, QStringList categories, int position)
{
	QSqlQuery q;
	q.prepare("UPDATE post SET blog_id=?, postid=?, author=?, title=?, content=?, c_time=?, m_time=?, is_private=?, is_comment_allowed=?, is_trackback_allowed=?, link=?, perma_link=?, summary=?, tags=?, position=? WHERE id=?");
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
	q.addBindValue(position);
	q.addBindValue(id);
	
	if(!q.exec())
		return false;
	
	///Delete previouse Categories:
	QSqlQuery qd;
	qd.prepare("DELETE FROM post_cat WHERE post_id=?");
	qd.addBindValue(id);
	if(!qd.exec())
		kDebug()<< "Cannot delete previouse categories.";
	
	///Add new Categories:
	int cat_count = categories.count();
	int i=0;
	QSqlQuery q1, q2;
	q1.prepare("SELECT catid FROM category WHERE name = ? AND blog_id= ?");
	q2.prepare("INSERT INTO post_cat (post_id, cat_id) VALUES(?, ?)");
	while(i<cat_count){
		q1.addBindValue(categories[i]);
		q1.addBindValue(blog_id);
		if(!q1.exec())
			kDebug()<<"Cannot get category id for category "<< categories[i];
		else
			if(q1.next()){
			q2.addBindValue(id);
			q2.addBindValue(q.value(0).toInt());
			if(!q2.exec())
                kDebug()<<"Cannot add Category "<< categories[i] <<" to Post, SQL Error: "<< q2.lastError().text();
			}
	}
	
	return true;
}

bool DBMan::editPost(BilboPost & post, int blog_id)
{
	QSqlQuery q;
	q.prepare("UPDATE post SET blog_id=?, postid=?, author=?, title=?, content=?, c_time=?, m_time=?, is_private=?, is_comment_allowed=?, is_trackback_allowed=?, link=?, perma_link=?, summary=?, tags=?, position=? WHERE id=?");
	q.addBindValue(blog_id);
	q.addBindValue(post.postId());
	q.addBindValue(post.author());
	q.addBindValue(post.title());
	q.addBindValue(post.content());
	q.addBindValue(post.cTime().toString(Qt::ISODate));
	q.addBindValue(post.mTime().toString(Qt::ISODate));
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
	q.addBindValue(post.position());
	
	q.addBindValue(post.id());
	
	if(!q.exec())
		return false;
	
	///Delete previouse Categories:
	QSqlQuery qd;
	qd.prepare("DELETE FROM post_cat WHERE post_id=?");
	qd.addBindValue(post.id());
	if(!qd.exec())
		kDebug()<<"Cannot delete previouse categories.";
	
	///Add new Categories:
	if(q.exec()){
		int cat_count = post.categories().count();
		int i=0;
		QSqlQuery q, q2;
		q.prepare("SELECT catid FROM category WHERE name = ? AND blog_id= ?");
		q2.prepare("INSERT INTO post_cat (post_id, cat_id) VALUES(?, ?)");
		while(i<cat_count){
			q.addBindValue(post.categories()[i]);
			q.addBindValue(blog_id);
			if(!q.exec())
				kDebug()<<"Cannot get category id for category "<< post.categories()[i];
			else
				if(q.next()){
				q2.addBindValue(post.id());
				q2.addBindValue(q.value(0).toInt());
                if(!q2.exec())
                    kDebug()<<"Cannot add Category "<< post.categories()[i] <<" to Post, SQL Error: "<< q2.lastError().text();
				}
		}
	}
	else
		kDebug()<<"Cannot edit categories.";
	
	return true;
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

QList< BilboBlog *> DBMan::listBlogs()
{
	QList<BilboBlog *> list;
	QSqlQuery q;
	q.exec("SELECT id, blogid, blog_url, username, password, style_url, api_type, title FROM blog");
	while( q.next() ){
		BilboBlog *tmp;
		tmp->setId(q.value(0).toInt());
		tmp->setBlogId( q.value(1).toString());
		tmp->setUrl (QUrl(q.value(2).toString()));
		tmp->setUsername (q.value(3).toString());
		tmp->setPassword (q.value(4).toString());
		tmp->setStylePath(q.value(5).toString());
		tmp->setTitle(q.value(6).toString());
		list.append(tmp);
	}
	
	return list;
}

QMap< QString, int > DBMan::listBlogsTitle()
{
	QMap< QString, int > list;
	QSqlQuery q;
	q.exec("SELECT title, id FROM blog");
	while( q.next() ){
		list[q.value(0).toString()] = q.value(1).toInt();
	}
	return list;
}

BilboBlog * DBMan::getBlogInfo(QString title)
{
	BilboBlog *b = new BilboBlog();
	QSqlQuery q;
	q.prepare("SELECT id, blogid, blog_url, username, password, style_url, api_type, title, direction FROM blog WHERE title = ?");
	q.addBindValue(title);
	if(q.exec())
		if( q.next() ){
			b->setId(q.value(0).toInt());
			b->setBlogId (q.value(1).toString());
			b->setUrl( QUrl(q.value(2).toString()));
			b->setUsername (q.value(3).toString());
			b->setPassword (q.value(4).toString());
			b->setStylePath( q.value(5).toString());
			b->setApi((BilboBlog::ApiType)q.value(6).toInt());
			b->setTitle( q.value(7).toString());
			b->setDirection((Qt::LayoutDirection)q.value(8).toInt());
			return b;
		}
	return 0;
}

BilboBlog * DBMan::getBlogInfo(int blog_id)
{
	BilboBlog *b = new BilboBlog();
	QSqlQuery q;
	q.prepare("SELECT id, blogid, blog_url, username, password, style_url, api_type, title, direction FROM blog WHERE id = ?");
	q.addBindValue(blog_id);
	if(q.exec()){
		if( q.next() ){
			b->setId(q.value(0).toInt());
			b->setBlogId( q.value(1).toString());
			b->setUrl(QUrl(q.value(2).toString()));
			b->setUsername (q.value(3).toString());
			b->setPassword (q.value(4).toString());
			b->setStylePath(q.value(5).toString());
			b->setApi((BilboBlog::ApiType)q.value(6).toInt());
			b->setTitle( q.value(7).toString());
			b->setDirection( (Qt::LayoutDirection) q.value(8).toInt());
			return b;
		}
	}
	return 0;
}

QList< BilboPost* > DBMan::listPosts(int blog_id)
{
	QList<BilboPost *> list;
	QSqlQuery q;
	q.prepare("SELECT id, postid, author, title, content, c_time, m_time, is_private, is_comment_allowed, is_trackback_allowed, link, perma_link, summary, tags FROM post WHERE blog_id = ? ORDER BY m_time DESC");
	q.addBindValue(blog_id);
	if(q.exec()){
		while( q.next() ){
			BilboPost *tmp;
			tmp->setId( q.value(0).toInt());
			tmp->setAuthor( q.value(2).toString());
			tmp->setPostId(q.value(1).toString());
			tmp->setTitle(q.value(3).toString());
			tmp->setContent(q.value(4).toString());
			QDateTime ct = QDateTime::fromString(q.value(5).toString(), Qt::ISODate);
			tmp->setCTime(ct);
			QDateTime mt = QDateTime::fromString(q.value(6).toString(), Qt::ISODate);
			tmp->setMTime(mt);
			tmp->setPrivate(q.value(7).toBool());
			tmp->setCommentAllowed(q.value(8).toBool());
			tmp->setTrackBackAllowed(q.value(9).toBool());
			QUrl u(q.value(10).toString());
			tmp->setPostLink(u);
			QUrl pu(q.value(11).toString());
			tmp->setPostPermaLink(pu);
			tmp->setSummary(q.value(12).toString());
			tmp->setTags(q.value(13).toString().split(',', QString::SkipEmptyParts ));
			
			///get Category list:
			QStringList catList;
			QSqlQuery q2;
			q2.prepare("SELECT category.name FROM category JOIN post_cat ON category.catid=post_cat.cat_id WHERE post_cat.post_id = ?");
			q2.addBindValue(tmp->id());
			if(q2.exec())
				while(q2.next())
					catList.append(q2.value(0).toString());
			tmp->setCategories(catList);
			list.append(tmp);
		}
	} else
		kDebug()<<"Cannot get list of posts for blog with id "<< blog_id;
	
	return list;
}

BilboPost * DBMan::getPostInfo(int post_id)
{
	QSqlQuery q;
	BilboPost *tmp = new BilboPost();
	q.prepare("SELECT id, postid, author, title, content, c_time, m_time, is_private, is_comment_allowed, is_trackback_allowed, link, perma_link, summary, tags FROM post WHERE id = ?");
	q.addBindValue(post_id);
	if(q.exec()){
		if( q.next() ){
			tmp->setId( q.value(0).toInt() );
			tmp->setAuthor( q.value(2).toString() );
			tmp->setPostId(q.value(1).toString());
			tmp->setTitle(q.value(3).toString());
			tmp->setContent(q.value(4).toString());
			QDateTime ct = QDateTime::fromString(q.value(5).toString(), Qt::ISODate);
			tmp->setCTime(ct);
			QDateTime mt = QDateTime::fromString(q.value(6).toString(), Qt::ISODate);
			tmp->setMTime(mt);
			tmp->setPrivate(q.value(7).toBool());
			tmp->setCommentAllowed(q.value(8).toBool());
			tmp->setTrackBackAllowed(q.value(9).toBool());
			QUrl u(q.value(10).toString());
			tmp->setPostLink(u);
			QUrl pu(q.value(11).toString());
			tmp->setPostPermaLink(pu);
			tmp->setSummary(q.value(12).toString());
			tmp->setTags(q.value(13).toString().split(',', QString::SkipEmptyParts ));
			
			///get Category list:
			QStringList catList;
			QSqlQuery q2;
			q2.prepare("SELECT category.name FROM category JOIN post_cat ON category.catid=post_cat.cat_id WHERE post_cat.post_id = ?");
			q2.addBindValue(tmp->id());
			if(q2.exec())
				while(q2.next())
					catList.append(q2.value(0).toString());
			tmp->setCategories(catList);
		}
	} else
		kDebug()<<"Cannot get post with id "<< post_id;
	
	return tmp;
}

QMap< QString, int > DBMan::listPostsTitle(int blog_id)
{
	QMap< QString, int > list;
	QSqlQuery q;
	q.prepare("SELECT title, id FROM post WHERE blog_id = ? ORDER BY m_time DESC");
	q.addBindValue(blog_id);
	if(q.exec()){
		while( q.next() ){
			list.insert(q.value(0).toString(), q.value(1).toInt());
		}
	} else
		kDebug()<<"Cannot get list of posts for blog with id "<< blog_id;
	
	return list;
}

QMap< QString, int > DBMan::listCategories(int blog_id)
{
	QMap< QString, int > list;
	QSqlQuery q;
	q.prepare("SELECT name, catid FROM category WHERE blog_id = ?");
	q.addBindValue(blog_id);
	if(q.exec()){
		while( q.next() ){
			list[q.value(0).toString()] = q.value(1).toInt();
		}
	} else
		kDebug()<<"Cannot get list of categories for blog with id "<< blog_id;
	
	return list;
}


