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
#include <kblog/blog.h>
#include <QUrl>
#include <kurl.h>

/**
Blog definition class!
it's implemented to decrease dependency to KBlog :)

	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
*/
class BilboBlog : public KBlog::Blog
{
public:
    BilboBlog(QUrl& server, QObject *parent=0, const QString &appName=QString("Bilbo Blogger"), const QString &appVer=QString(VERSION));

    ~BilboBlog();
	
	void setBlogUrl(QUrl &url);//use this instead of setUrl()
	QUrl blogUrl();//use this instead of url()

	QString styleUrl;
	QString api;
	int id;//id in DB
};

#endif
