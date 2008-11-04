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
#include "bilbopost.h"

BilboPost::BilboPost()
 : KBlog::BlogPost()
{
}


BilboPost::~BilboPost()
{
}

QUrl BilboPost::postLink()
{
	return QUrl(this->link().url());
}

QUrl BilboPost::postPermaLink()
{
	return QUrl(this->permaLink().url());
}

void BilboPost::setPostLink(const QUrl &link)
{
	this->setLink(KUrl(link));
}

void BilboPost::setPostPermaLink(const QUrl &link)
{
	this->setPermaLink(KUrl(link));
}

QDateTime BilboPost::cTime()
{
	return QDateTime::fromString(this->creationDateTime().toString(KDateTime::ISODate), Qt::ISODate);
}

void BilboPost::setMTime(const QDateTime &t)
{
	this->setModificationDateTime(KDateTime(t));
}

QDateTime BilboPost::mTime()
{
	return QDateTime::fromString(this->modificationDateTime().toString(KDateTime::ISODate), Qt::ISODate);
}

void BilboPost::setCTime(const QDateTime &t)
{
	this->setCreationDateTime(KDateTime(t));
}

QString BilboPost::author()
{
	return mAuthor;
}

void BilboPost::setId(int id)
{
	mId = id;
}

int BilboPost::id()
{
	return mId;
}

void BilboPost::setAuthor(const QString &author)
{
	mAuthor = author;
}


