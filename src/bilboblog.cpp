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
#include "bilboblog.h"

BilboBlog::BilboBlog(QUrl &server)
{
	this->setUrl( server);
}

BilboBlog::BilboBlog()
{
}

BilboBlog::~BilboBlog()
{
}

QUrl BilboBlog::url() const
{
	return mUrl;
}

void BilboBlog::setUrl(const QUrl &url)
{
	mUrl = url;
}

QString BilboBlog::blogid() const
{
	return mBlogid;
}

void BilboBlog::setBlogId(const QString &url)
{
	mBlogid = url;
}

QString BilboBlog::username() const
{
	return mUsername;
}

void BilboBlog::setUsername(const QString &username)
{
	mUsername = username;
}

QString BilboBlog::password() const
{
	return mPassword;
}

void BilboBlog::setPassword(const QString &password)
{
	mPassword = password;
}

QString BilboBlog::title() const
{
	return mTitle;
}

void BilboBlog::setTitle(const QString &title)
{
	mTitle = title;
}

QString BilboBlog::stylePath() const
{
	return mStylePath;
}

void BilboBlog::setStylePath(const QString &path)
{
	mStylePath = path;
}

BilboBlog::ApiType BilboBlog::api() const
{
	return mApi;
}

void BilboBlog::setApi(const ApiType api)
{
	mApi = api;
}

int BilboBlog::id() const
{
	return mId;
}

void BilboBlog::setId(const int id)
{
	mId = id;
}

Qt::LayoutDirection BilboBlog::direction() const
{
	return mDir;
}

void BilboBlog::setDirection(const Qt::LayoutDirection dir)
{
	mDir = dir;
}

QString BilboBlog::blogUrl()
{
    QString url=this->url().toString();
    switch( this->api() ){
    case BLOGGER1_API:
        break;
    case METAWEBLOG_API:
    case MOVABLETYPE_API:
    case WORDPRESSBUGGY_API:
        url = url.remove("xmlrpc.php", Qt::CaseInsensitive);
        break;
    case GDATA_API:
        break;
    }
    return url;
}




