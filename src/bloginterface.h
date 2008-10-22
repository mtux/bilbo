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

/**
Abstract base class, can be implemented by plugins to support a blog API.

	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
*/
class BlogInterface{
public:

    virtual ~BlogInterface() {}
	
	virtual QString getPluginName() = 0;
	virtual bool publishPost(QString blogid, QString username, QString password, Post &newPost) = 0;
	virtual Post *getPost(QString blogid, QString postid, QString username, QString password, QString appkey="") = 0;
	virtual bool editPost(QString blogid, QString username, QString password, Post &editedPost) = 0;
	virtual bool deletePost(QString blogid, QString username, QString password, QString postid, QString appkey="") = 0;
	virtual QList<Post> getRecentPosts(QString blogid, QString username, QString password, QString appkey="", int numberOfPosts=10) = 0;
	virtual QList<Category> getCategoryList(QString blogid, QString username, QString password, QString appkey="") = 0;
	virtual QString uploadMediaFile(QString blogid, QString username, QString password, MediaFile &file, QString appkey="") = 0;
};

Q_DECLARE_INTERFACE(BlogInterface, "org.bilbo.BilboEngine.BlogInterface/0.1")

#endif
