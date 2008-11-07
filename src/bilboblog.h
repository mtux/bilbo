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
#ifndef BILBOBLOG_H
#define BILBOBLOG_H

#include "constants.h"
// #include <kblog/blog.h>
#include <QUrl>
// #include <kurl.h>

/**
Blog definition class!

	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
*/
class BilboBlog
{
public:
	enum ApiType {
		BLOGGER1_API=0, METAWEBLOG_API, MOVABLETYPE_API, WORDPRESSBUGGY_API, GDATA_API
	};
	enum TextDirection{
		LeftToRight=0, RightToLeft
	};
// 	struct API_Type {
// 		QString title;
// 		QString code;
// 		ApiType type;
// 	};
    BilboBlog(QUrl& server);
	BilboBlog();
    ~BilboBlog();
	
	QUrl blogUrl();
	void setBlogUrl(const QUrl&);
	QString blogid();
	void setBlogId(const QString&);
	QString username();
	void setUsername(const QString&);
	QString password();
	void setPassword(const QString&);
	QString title();
	void setTitle(const QString&);
	QString stylePath();
	void setStylePath(const QString&);
	ApiType api();
	void setApi(ApiType);
	int id();//id in DB
	void setId(int);
	TextDirection direction();
	void setDirection(TextDirection);
	
private:
	QUrl mBlogUrl;
	QString mBlogid;
	QString mUsername;
	QString mPassword;
	QString mTitle;
	QString mStylePath;
	ApiType mApi;
	int mId;//id in DB
	TextDirection mDir;
};

#endif