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
/**
DB manipulation class. this class implement Low level Database operations. and any object of App. have to use this API to store or retrive information and settings from Database.

	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
*/

class DBMan{
public:
    DBMan();

    ~DBMan();
	
	bool addBlog(int blogid, QString blog_url, QString username, QString password, QString style_url, int api);
	bool editBlog(int blogid, QString username, QString password, QString style_url, int api);
	bool removeBlog(int blogid);
	
	int addPost(int postid, int blogid, QString author, QString title, QString &content, QString c_time, bool is_private, bool is_comment_allowed, bool is_trackback_allowed, QString link, QString perma_link, QString summary, QString tags/*comma(,) separated list*/);
	bool editPost(int id, int blogid, int postid, QString author, QString title, QString &content, QString c_time, QString m_time, bool is_private, bool is_comment_allowed, bool is_trackback_allowed, QString link, QString perma_link, QString summary, QString tags);
	bool removePost(int id);
	bool clearPosts(int blogid);
	
	bool addCategory(int catid, QString name, int blogid);
	bool clearCategories(int blogid);
	
	int addFile(QString name, int blogid, bool isUploaded, QString localUrl, QString remoteUrl);
	bool removeFile(int fileid);
	bool clearFiles(int blogid);

	
private:
	bool createDB();
	QSqlDatabase db;
	bool connectDB();
};

#endif
