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

    BilboBlog(QUrl& server);
	BilboBlog();
    ~BilboBlog();
	
	/**
	 * returns blog xmlrpc Url!
     * For http://bilbo.wordpress.com :
     * it's url() is http://bilbo.wordpress.com/xmlrpc.php
     * and it's blogUrl() is http://bilbo.wordpress.com/
	 * @return url usable for xmlrpc!
	 */
	QUrl url() const;
	void setUrl(const QUrl&);
	QString blogid() const;
	void setBlogId(const QString&);
	QString username() const;
	void setUsername(const QString&);
	QString password() const;
	void setPassword(const QString&);
	QString title() const;
	void setTitle(const QString&);
	QString stylePath() const;
	void setStylePath(const QString&);
	ApiType api() const;
	void setApi( const ApiType );
	int id() const;//id in DB
	void setId(const int);
	Qt::LayoutDirection direction() const;
	void setDirection( const Qt::LayoutDirection );
    
    /**
     * return Blog Actual Url!
     * For http://bilbo.wordpress.com :
     * it's url() is http://bilbo.wordpress.com/xmlrpc.php
     * and it's blogUrl() is http://bilbo.wordpress.com/
     * @return Blog actual url.
     */
    QString blogUrl();
	
private:
	QUrl mUrl;
	QString mBlogid;
	QString mUsername;
	QString mPassword;
	QString mTitle;
	QString mStylePath;
	ApiType mApi;
	int mId;//id in DB
	Qt::LayoutDirection mDir;
};

#endif
