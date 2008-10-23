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
#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QtPlugin>
#include <QStringList>
#include <QDateTime>
// #include <QUrl>

struct Post {
	QString appkey;
	QString postid;
	QString blogid;
	QString userid;
	QString title;
	QString link;
	QString permaLink;
	QString content;
	QString author;
	QStringList categories;
	QDateTime pubDate;
	bool isPrivate;
	bool allowComments;
	bool allowPings;
	QString keywords;
	QString textMore;
	QStringList tags;
};

struct Category {
	QString id;
	QString name;
};

struct MediaFile {
	QString name;
	QByteArray file;//.toBase64()
	QString type;
};

struct BlogInfo {
	QString blogId;
	QString blogName;
	QString blogUrl;
};

/**
Abstract base class, can be implemented by plugins to support a blog API.

	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
*/
class BlogInterface{
public:

    virtual ~BlogInterface() {}
	
	virtual QString getPluginName() = 0;
	
	virtual bool publishPost(const QString &blogUrl, const QString &blogid, const QString &username, const QString &password, const Post &newPost) = 0;
	
	virtual Post *getPost(const QString &blogUrl, const QString &blogid, const QString &postid, const QString &username, const QString &password, const QString &appkey=QString("")) = 0;
	
	virtual bool editPost(const QString &blogUrl, const QString &blogid, const QString &username, const QString &password, const Post &editedPost) = 0;
	
	virtual bool deletePost(const QString &blogUrl, const QString &blogid, const QString &username, const QString &password, const QString &postid, const QString &appkey="") = 0;
	
	virtual QList<Post> getRecentPosts(const QString &blogUrl, const QString &blogid, const QString &username, const QString &password, const QString &appkey="", const int numberOfPosts=10) = 0;
	
	virtual QList<Category> getCategoryList(const QString &blogUrl, const QString &blogid, const QString &username, const QString &password, const QString &appkey="") = 0;
	
	virtual QString uploadMediaFile(const QString &blogUrl, const QString &blogid, const QString &username, const QString &password, const MediaFile &file, const QString &appkey="") = 0;
	
	virtual QList<BlogInfo> listblogs(const QString &blogUrl, const QString &username, const QString &password) = 0;
};

Q_DECLARE_INTERFACE(BlogInterface, "org.bilbo.BilboEngine.BlogInterface/0.3")

#endif
