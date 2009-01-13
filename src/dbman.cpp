/***************************************************************************
 *   This file is part of the Bilbo Blogger.                               *
 *   Copyright (C) 2008-2009 Mehrdad Momeny <mehrdad.momeny@gmail.com>     *
 *   Copyright (C) 2008-2009 Golnaz Nilieh <g382nilieh@gmail.com>          *
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
#include <kdatetime.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kwallet.h>

DBMan::DBMan()
{
	kDebug();
	mWallet = KWallet::Wallet::openWallet( "kdewallet", 0 );
	if ( mWallet ) {
		if(!mWallet->setFolder( "bilbo" )){
			mWallet->createFolder( "bilbo" );
			mWallet->setFolder( "bilbo" );
		}
		kDebug() << "Wallet successfully opened.";
	}
	
	if(!QFile::exists(CONF_DB)){
		if(!this->createDB()){
            KMessageBox::detailedError(0, i18n("Cannot create configuration database"),
									   i18n(db.lastError().text().toUtf8().data()));
			kDebug()<<"Cannot create configuration database, SQL error: "<<db.lastError().text()<<endl;
		}
	}else if (!connectDB())
		exit(1);
}

DBMan * DBMan::mSelf = 0L;

DBMan * DBMan::self()
{
	if( !mSelf )
		mSelf = new DBMan;
	return mSelf;
}

bool DBMan::connectDB()
{
	kDebug();
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(CONF_DB);
	
	if(!db.open()){
        KMessageBox::detailedError(0, i18n("Cannot connect to configuration database"),
								   i18n(db.lastError().text().toUtf8().data()));
		kDebug()<<"Cannot connect to configuration database, SQL error: "<<db.lastError().text()<<endl;
		return false;
	}
	return true;
}

DBMan::~DBMan()
{
	kDebug();
    db.close();
	mSelf = 0L;
}

bool DBMan::createDB()
{
	kDebug();
	bool ret=true;
	if(!connectDB())
		exit(1);
	
	QSqlQuery q;
	///Blog table!
	if(!q.exec("CREATE TABLE blog (id INTEGER PRIMARY KEY, blogid TEXT, blog_url TEXT, username TEXT,\
		    style_url TEXT, api_type TEXT, title TEXT, direction TEXT, local_directory TEXT)"))
		ret=false;
	
	///posts table!
	if(!q.exec("CREATE TABLE post (id INTEGER PRIMARY KEY, postid TEXT, blog_id NUMERIC,\
		    author TEXT, title TEXT, content TEXT, c_time TEXT, m_time TEXT, is_private NUMERIC,\
		   is_comment_allowed NUMERIC, is_trackback_allowed NUMERIC, link TEXT, perma_link TEXT,\
		   summary TEXT, tags TEXT, status int);"))
		ret=false;
	
	///categories table!
	if(!q.exec("CREATE TABLE category (catid INTEGER PRIMARY KEY, name TEXT, description TEXT,\
		 htmlUrl TEXT, rssUrl TEXT, categoryId TEXT, parentId TEXT, blog_id NUMERIC);"))
		ret=false;
	
	///files table
	if(!q.exec("CREATE TABLE file (fileid INTEGER PRIMARY KEY, name TEXT, blog_id NUMERIC, is_uploaded NUMERIC,\
		    local_url TEXT, remote_url TEXT);"))
		ret=false;
	
	///connection bethween posts and categories
	if(!q.exec("CREATE TABLE post_cat (post_id INTEGER, cat_id INTEGER);"))
		ret=false;
	
	
	///this will implement using clearPosts() , clearCategories() and clearFiles() in application level!
	q.exec("CREATE TRIGGER delete_post AFTER DELETE ON post BEGIN DELETE from post_cat WHERE post_id=OLD.id; END");
	q.exec("CREATE TRIGGER delete_blog AFTER DELETE ON blog \
			BEGIN DELETE from category WHERE category.blog_id=OLD.id; DELETE from file WHERE file.blog_id=OLD.id;\
			DELETE from post WHERE post.blog_id=OLD.id; END");
	
	return ret;
}

int DBMan::addBlog(QString blogid, QString blog_url, QString username, QString password, QString style_url,
                    QString api, QString title, int direction, QString directory)
{
	QSqlQuery q;
	q.prepare("INSERT INTO blog (blogid, blog_url, username, style_url, api_type, title,\
               direction, local_directory) VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
	q.addBindValue(blogid);
	q.addBindValue(blog_url);
	q.addBindValue(username);
// 	q.addBindValue(password);
	q.addBindValue(style_url);
	q.addBindValue(api);
	q.addBindValue(title);
	q.addBindValue(direction);
	q.addBindValue(directory);
	
	if(q.exec()){
		if(mWallet && mWallet->writePassword(blog_url+'_'+username, password)==0)
			kDebug()<<"Password stored to kde wallet";
		return q.lastInsertId().toInt();
	} else
		return -1;
}

int DBMan::addBlog(BilboBlog & blog)
{
	QSqlQuery q;
	q.prepare("INSERT INTO blog (blogid, blog_url, username, style_url, api_type, title,\
             direction, local_directory) VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
	q.addBindValue(blog.blogid());
	//q.addBindValue(blog.url().toString());
	q.addBindValue(blog.url().url());
	q.addBindValue(blog.username());
// 	q.addBindValue(blog.password());
	q.addBindValue(blog.stylePath());
	q.addBindValue(blog.api());
	q.addBindValue(blog.title());
	q.addBindValue(blog.direction());
	q.addBindValue(blog.localDirectory());
	
	if(q.exec()){
		if(mWallet && mWallet->writePassword(blog.url().url()+'_'+blog.username(), blog.password())==0)
			kDebug()<<"Password stored to kde wallet";
		return q.lastInsertId().toInt();
	} else
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
bool DBMan::editBlog(int id, QString username, QString password, QString style_url,
                     QString api, QString title, int direction, QString directory)
{
	QSqlQuery q;
	q.prepare("UPDATE blog SET username=? , style_url=? , api_type=?, \
               title=?, direction=? local_directory=? WHERE id=?");
	q.addBindValue(username);
// 	q.addBindValue(password);
	q.addBindValue(style_url);
	q.addBindValue(api);
	q.addBindValue(title);
	q.addBindValue(direction);
	q.addBindValue(directory);
	q.addBindValue(id);
	BilboBlog *tmp = this->getBlogInfo(id);
	if(mWallet && mWallet->writePassword(tmp->url().url()+'_'+tmp->username(), password)==0)
		kDebug()<<"New password stored to kde wallet";
	tmp->deleteLater();
	return q.exec();
}

bool DBMan::editBlog(BilboBlog & blog)
{
	QSqlQuery q;
	q.prepare("UPDATE blog SET username=? , style_url=? , api_type=?, \
               title=?, direction=? local_directory=? WHERE id=?");
	q.addBindValue(blog.username());
// 	q.addBindValue(blog.password());
	q.addBindValue(blog.stylePath());
	q.addBindValue(blog.api());
	q.addBindValue(blog.title());
	q.addBindValue(blog.direction());
	q.addBindValue(blog.localDirectory());
	q.addBindValue(blog.id());
	if(mWallet && mWallet->writePassword(blog.url().url()+'_'+blog.username(), blog.password())==0)
		kDebug()<<"Password stored to kde wallet";
	return q.exec();
}

bool DBMan::removeBlog(int blog_id)
{
	BilboBlog * tmp = this->getBlogInfo(blog_id);
	if(mWallet && mWallet->removeEntry(tmp->url().url()+'_'+tmp->username())==0)
		kDebug()<<"Password removed to kde wallet";
	tmp->deleteLater();
	QSqlQuery q;
	q.prepare("DELETE FROM blog WHERE id=?");
	q.addBindValue(blog_id);
	return q.exec();
}

int DBMan::addPost(QString postid, int blog_id, QString author, QString title, QString & content,
				    QString c_time, bool is_private, bool is_comment_allowed, bool is_trackback_allowed,
		            QString link, QString perma_link, QString summary, QString tags, QStringList categories,
			        int status)
{
	QSqlQuery q;
	q.prepare("INSERT INTO post (postid, blog_id, author, title, content, c_time, m_time, is_private,\
			 is_comment_allowed, is_trackback_allowed, link, perma_link, summary, tags, status)\
			 VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
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
	q.addBindValue(status);
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
	q.prepare("INSERT INTO post (postid, blog_id, author, title, content, c_time, m_time, is_private,\
			 is_comment_allowed, is_trackback_allowed, link, perma_link, summary, tags, status)\
			VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	q.addBindValue(post.postId());
	q.addBindValue(blog_id);
	q.addBindValue(post.author());
	q.addBindValue(post.title());
	q.addBindValue(post.content());
	q.addBindValue(post.creationDateTime().toString(KDateTime::ISODate));
    q.addBindValue(post.creationDateTime().toString(KDateTime::ISODate));
	q.addBindValue(post.isPrivate());
	q.addBindValue(post.isCommentAllowed());
	q.addBindValue(post.isTrackBackAllowed());
	q.addBindValue(post.link().url());
	q.addBindValue(post.permaLink().url());
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
		q.addBindValue(QString());
	q.addBindValue(post.status());
	
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

bool DBMan::editPost(int id, int blog_id, QString postid, QString author, QString title, QString & content,
					  QString c_time, QString m_time, bool is_private, bool is_comment_allowed, bool is_trackback_allowed,
	                  QString link, QString perma_link, QString summary, QString tags, QStringList categories, int status)
{
	QSqlQuery q;
	q.prepare("UPDATE post SET blog_id=?, postid=?, author=?, title=?, content=?, c_time=?, m_time=?, is_private=?,\
			 is_comment_allowed=?, is_trackback_allowed=?, link=?, perma_link=?, summary=?, tags=?, status=?\
			 WHERE id=?");
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
	q.addBindValue(status);
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
	q.prepare("UPDATE post SET blog_id=?, postid=?, author=?, title=?, content=?, c_time=?, m_time=?,\
			  is_private=?, is_comment_allowed=?, is_trackback_allowed=?, link=?, perma_link=?, summary=?,\
			  tags=?, status=? WHERE id=?");
	q.addBindValue(blog_id);
	q.addBindValue(post.postId());
	q.addBindValue(post.author());
	q.addBindValue(post.title());
	q.addBindValue(post.content());
	q.addBindValue(post.creationDateTime().toString(KDateTime::ISODate));
    q.addBindValue(post.modificationDateTime().toString(KDateTime::ISODate));
	q.addBindValue(post.isPrivate());
	q.addBindValue(post.isCommentAllowed());
	q.addBindValue(post.isTrackBackAllowed());
	q.addBindValue(post.link().url());
	q.addBindValue(post.permaLink().url());
	q.addBindValue(post.summary());
	
	QString tags="";
	int i=0;
	while(i<post.tags().count())
		tags = post.tags()[i]+",";
	tags.remove(tags.length()-1, 1);
	q.addBindValue(tags);
	q.addBindValue(post.status());
	
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
                    kDebug()<<"Cannot add Category "<< post.categories()[i] <<
							" to Post, SQL Error: "<< q2.lastError().text();
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

int DBMan::addCategory(const QString &name, const QString &description, const QString &htmlUrl, 
					   const QString &rssUrl, const QString &categoryId, const QString &parentId, int blog_id)
{
	QSqlQuery q;
	q.prepare("INSERT INTO category (name, description, htmlUrl, rssUrl, categoryId, parentId, blog_id)\
			 VALUES(?, ?, ?, ?, ?, ?, ?)");
	q.addBindValue(name);
	q.addBindValue(description);
	q.addBindValue(htmlUrl);
	q.addBindValue(rssUrl);
	q.addBindValue(categoryId);
	q.addBindValue(parentId);
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
	q.exec("SELECT id, blogid, blog_url, username, style_url, api_type, title,\
			 direction, local_directory FROM blog");
	while( q.next() ){
		BilboBlog *tmp = new BilboBlog;
		tmp->setId(q.value(0).toInt());
		tmp->setBlogId( q.value(1).toString());
		tmp->setUrl (QUrl(q.value(2).toString()));
		tmp->setUsername (q.value(3).toString());
// 		tmp->setPassword (q.value(4).toString());
		tmp->setStylePath(q.value(4).toString());
		tmp->setApi((BilboBlog::ApiType)q.value(5).toInt());
		tmp->setTitle(q.value(6).toString());
		tmp->setDirection((Qt::LayoutDirection)q.value(7).toInt());
		tmp->setLocalDirectory( q.value(8).toString() );
		QString buffer;
		if(mWallet && mWallet->readPassword(tmp->url().url()+'_'+tmp->username() , buffer )==0 && !buffer.isEmpty()){
			tmp->setPassword( buffer );
			kDebug()<<"Password loaded from kde wallet.";
		}
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
	BilboBlog *b = new BilboBlog;
	QSqlQuery q;
	q.prepare("SELECT id, blogid, blog_url, username, style_url, api_type, title, \
            direction, local_directory \
            FROM blog WHERE title = ?");
	q.addBindValue(title);
	if(q.exec())
		if( q.next() ){
			b->setId(q.value(0).toInt());
			b->setBlogId (q.value(1).toString());
			b->setUrl( QUrl(q.value(2).toString()));
			b->setUsername (q.value(3).toString());
// 			b->setPassword (q.value(4).toString());
			b->setStylePath( q.value(4).toString());
			b->setApi((BilboBlog::ApiType)q.value(5).toInt());
			b->setTitle( q.value(6).toString());
			b->setDirection((Qt::LayoutDirection)q.value(7).toInt());
			b->setLocalDirectory(q.value(8).toString());
			QString buffer;
			if(mWallet && mWallet->readPassword( b->url().url()+'_'+b->username(), buffer )==0 && !buffer.isEmpty()){
				b->setPassword( buffer );
				kDebug()<<"Password loaded from kde wallet.";
			}
			return b;
		}
	return 0;
}

BilboBlog * DBMan::getBlogInfo(int blog_id)
{
	BilboBlog *b = new BilboBlog;
	QSqlQuery q;
	q.prepare("SELECT id, blogid, blog_url, username, style_url, api_type, \
            title, direction, local_directory \
            FROM blog WHERE id = ?");
	q.addBindValue(blog_id);
	if(q.exec()){
		if( q.next() ){
			b->setId(q.value(0).toInt());
			b->setBlogId( q.value(1).toString());
			b->setUrl(QUrl(q.value(2).toString()));
			b->setUsername (q.value(3).toString());
// 			b->setPassword (q.value(4).toString());
			b->setStylePath(q.value(4).toString());
			b->setApi((BilboBlog::ApiType)q.value(5).toInt());
			b->setTitle( q.value(6).toString());
			b->setDirection( (Qt::LayoutDirection) q.value(7).toInt());
			b->setLocalDirectory(q.value(8).toString());
			QString buffer;
			if(mWallet && mWallet->readPassword( b->url().url()+'_'+b->username(), buffer )==0 && !buffer.isEmpty()){
				b->setPassword( buffer );
				kDebug()<<"Password loaded from kde wallet.";
			}
			return b;
		}
	}
	return 0;
}

QList< BilboPost* > DBMan::listPosts(int blog_id)
{
	QList<BilboPost *> list;
	QSqlQuery q;
	q.prepare("SELECT id, postid, author, title, content, c_time, m_time, is_private, is_comment_allowed,\
			 is_trackback_allowed, link, perma_link, summary, tags\
			 FROM post WHERE blog_id = ? ORDER BY m_time DESC");
	q.addBindValue(blog_id);
	if(q.exec()){
		while( q.next() ){
			BilboPost *tmp = new BilboPost();
			tmp->setId( q.value(0).toInt());
			tmp->setAuthor( q.value(2).toString());
			tmp->setPostId(q.value(1).toString());
			tmp->setTitle(q.value(3).toString());
			tmp->setContent(q.value(4).toString());
			tmp->setCreationDateTime(KDateTime::fromString(q.value(5).toString(), KDateTime::ISODate));
			tmp->setModificationDateTime(KDateTime::fromString(q.value(6).toString(), KDateTime::ISODate));
			tmp->setPrivate(q.value(7).toBool());
			tmp->setCommentAllowed(q.value(8).toBool());
			tmp->setTrackBackAllowed(q.value(9).toBool());
			tmp->setLink(KUrl(q.value(10).toString()));
			tmp->setPermaLink(KUrl(q.value(11).toString()));
			tmp->setSummary(q.value(12).toString());
			tmp->setTags(q.value(13).toString().split(',', QString::SkipEmptyParts ));
			
			///get Category list:
			QList<Category> catList;
			QSqlQuery q2;
			q2.prepare("SELECT category.name, category.description, category.htmlUrl, category.rssUrl,\
					category.categoryId, category.parentId\
					FROM category JOIN post_cat ON category.catid=post_cat.cat_id\
					WHERE post_cat.post_id = ?");
			q2.addBindValue(tmp->id());
			if(q2.exec())
				while(q2.next()){
					Category cat;
					cat.blog_id = blog_id;
					cat.name = q2.value(0).toString();
					cat.description = q2.value(1).toString();
					cat.htmlUrl = q2.value(2).toString();
					cat.rssUrl = q2.value(3).toString();
					cat.categoryId = q2.value(4).toString();
					cat.parentId = q2.value(5).toString();
					catList.append(cat);
				}
			tmp->setCategoryList(catList);
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
	q.prepare("SELECT id, postid, author, title, content, c_time, m_time, is_private, is_comment_allowed,\
			 is_trackback_allowed, link, perma_link, summary, tags FROM post WHERE id = ?");
	q.addBindValue(post_id);
	if(q.exec()){
		if( q.next() ){
			tmp->setId( q.value(0).toInt() );
			tmp->setAuthor( q.value(2).toString() );
			tmp->setPostId(q.value(1).toString());
			tmp->setTitle(q.value(3).toString());
			tmp->setContent(q.value(4).toString());
			tmp->setCreationDateTime(KDateTime::fromString(q.value(5).toString(), KDateTime::ISODate));
            tmp->setModificationDateTime(KDateTime::fromString(q.value(6).toString(), KDateTime::ISODate));
			tmp->setPrivate(q.value(7).toBool());
			tmp->setCommentAllowed(q.value(8).toBool());
			tmp->setTrackBackAllowed(q.value(9).toBool());
			QUrl u(q.value(10).toString());
			tmp->setLink(u);
			QUrl pu(q.value(11).toString());
			tmp->setPermaLink(pu);
			tmp->setSummary(q.value(12).toString());
			tmp->setTags(q.value(13).toString().split(',', QString::SkipEmptyParts ));
			
			///get Category list:
			QList<Category> catList;
			QSqlQuery q2;
			q2.prepare("SELECT category.name, category.description, category.htmlUrl, category.rssUrl,\
					category.categoryId, category.parentId, category.blog_id\
					FROM category JOIN post_cat ON category.catid=post_cat.cat_id\
					WHERE post_cat.post_id = ?");
			q2.addBindValue(tmp->id());
			if(q2.exec())
				while(q2.next()){
					Category cat;
					cat.blog_id = q2.value(6).toInt();
					cat.name = q2.value(0).toString();
					cat.description = q2.value(1).toString();
					cat.htmlUrl = q2.value(2).toString();
					cat.rssUrl = q2.value(3).toString();
					cat.categoryId = q2.value(4).toString();
					cat.parentId = q2.value(5).toString();
					catList.append(cat);
				}
				tmp->setCategoryList(catList);
		}
	} else
		kDebug()<<"Cannot get post with id "<< post_id;
	
	return tmp;
}

QMap< int, QString > DBMan::listPostsTitle(int blog_id)
{
	QMap< int, QString >list;
	QSqlQuery q;
	q.prepare("SELECT title, id FROM post WHERE blog_id = ? ORDER BY m_time DESC");
	q.addBindValue(blog_id);
	if(q.exec()){
		while( q.next() ){
			list.insert(q.value(1).toInt(), q.value(0).toString());
		}
	} else
		kDebug()<<"Cannot get list of posts for blog with id "<< blog_id;
	
	return list;
}

QMap< QString, int > DBMan::listCategoriesName(int blog_id)
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

QList< Category > DBMan::listCategories(int blog_id)
{
	QList< Category > list;
	QSqlQuery q;
	q.prepare("SELECT catid, name, description, htmlUrl, rssUrl, categoryId, parentId FROM category\
			 WHERE blog_id = ?");
	q.addBindValue(blog_id);
	if(q.exec()){
		while( q.next() ){
			Category c;
			c.blog_id = blog_id;
			c.id = q.value(0).toInt();
			c.name = q.value(1).toString();
			c.description = q.value(2).toString();
			c.htmlUrl = q.value(3).toString();
			c.rssUrl = q.value(4).toString();
			c.categoryId = q.value(5).toString();
			c.parentId = q.value(6).toString();
			list.append(c);
		}
	} else
		kDebug()<<"Cannot get list of categories for blog with id "<< blog_id;
	
		return list;
}

QMap< QString, bool > DBMan::listCategoriesId(int blog_id)
{
	QMap< QString, bool > list;
	QSqlQuery q;
	q.prepare("SELECT categoryId FROM category\
			WHERE blog_id = ?");
	q.addBindValue(blog_id);
	if(q.exec()){
		while( q.next() ){
			list.insert(q.value(0).toString(), false);
		}
	} else
		kDebug()<<"Cannot get list of categories for blog with id "<< blog_id;
	
		return list;
}
