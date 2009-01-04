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
#ifndef DBMAN_H
#define DBMAN_H
#include <QtSql>

#include "constants.h"
class BilboBlog;
class BilboPost;

/**
DB manipulation class. this class implement Low level Database operations. and any object of App.
have to use this API to store or retrive information and settings from Database.

	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
*/

class DBMan{
public:
    DBMan();

    ~DBMan();
	
	///(BEGIN) Data retrieveing Functions:
	
	/**
	 *    return List of blogs in Database.
	 * @return 
	 */
	QList<BilboBlog*> listBlogs();
	QMap<QString, int> listBlogsTitle();///QString as Title, and int as blog_id
	BilboBlog *getBlogInfo(QString title);
	BilboBlog *getBlogInfo(int blog_id);
	
	QList<BilboPost*> listPosts(int blog_id);
	QMap< int, QString > listPostsTitle(int blog_id);///QString as Title, and int as post_id
	BilboPost *getPostInfo(int post_id);
	
	QMap<QString, int> listCategoriesName(int blog_id);
	QList<Category> listCategories(int blog_id);
	
	
	///END
	
	///(BEGIN) Data Manipulation Functions:
	
	///Blog:
	int addBlog(QString blogid, QString blog_url, QString username, QString password, QString style_url, QString api, QString title, int direction, QString directory);
	
	int addBlog(BilboBlog& blog);
	
	bool editBlog(int id, QString username, QString password, QString style_url, QString api, QString title, int direction, QString directory);
	
	bool editBlog(BilboBlog& blog);
	
	bool removeBlog(int blog_id);
	
	///Post:
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
	int addPost(QString postid, int blog_id, QString author, QString title, QString &content,
				 QString c_time, bool is_private, bool is_comment_allowed, bool is_trackback_allowed,
	             QString link, QString perma_link, QString summary, QString tags/*comma(,) separated list*/,
			     QStringList categories, int position);
	
	/**
	 * 
	 * @param post 
	 * @param blog_id 
	 * @return return post id in database (deffer with postid)
	 */
	int addPost(const BilboPost& post, int blog_id);
	
	bool editPost(int id, int blog_id, QString postid, QString author, QString title, QString &content,
				   QString c_time, QString m_time, bool is_private, bool is_comment_allowed,
	    		   bool is_trackback_allowed, QString link, QString perma_link, QString summary, QString tags,
	  			   QStringList categories, int position);
	
	bool editPost(BilboPost& post, int blog_id);
	
	bool removePost(int id);
	
	bool clearPosts(int blog_id);
	
	///Category:
	int addCategory(const QString &name, const QString &description, const QString &htmlUrl, 
					const QString &rssUrl, const QString &categoryId, const QString &parentId, int blog_id);
	bool clearCategories(int blog_id);
	
	///File:
	int addFile(QString name, int blog_id, bool isUploaded, QString localUrl, QString remoteUrl);
	int addFile();
	bool removeFile(int fileid);
	bool clearFiles(int blog_id);

	///END
	
private:
	bool createDB();
	QSqlDatabase db;
	bool connectDB();
};

#endif
