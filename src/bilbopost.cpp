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
#include <QStringList>
#include <QDateTime>
#include <kdatetime.h>
BilboPost::BilboPost()
 : KBlog::BlogPost()
{
    this->setCTime(QDateTime::currentDateTime());
    this->setMTime(QDateTime::currentDateTime());
    this->setPostLink(QUrl());
    this->setPostPermaLink(QUrl());
    this->setCategories(QStringList());
    this->setCommentAllowed(true);
    this->setContent("");
    this->setTags(QStringList());
    this->setMood("");
    this->setMusic("");
    this->setPrivate(false);
    this->setSummary("");
    this->setTrackBackAllowed(true);
    this->setTitle("");
    this->setAuthor("");
    this->setPosition(Published);
    this->setId(-1);
}


BilboPost::~BilboPost()
{
}

QUrl BilboPost::postLink() const
{
	return QUrl(this->link().url());
}

QUrl BilboPost::postPermaLink() const
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

QDateTime BilboPost::cTime() const
{
	return QDateTime::fromString(this->creationDateTime().toString(KDateTime::ISODate), Qt::ISODate);
}

void BilboPost::setMTime(const QDateTime &t)
{
	this->setModificationDateTime(KDateTime(t));
}

QDateTime BilboPost::mTime() const
{
	return QDateTime::fromString(this->modificationDateTime().toString(KDateTime::ISODate), Qt::ISODate);
}

void BilboPost::setCTime(const QDateTime &t)
{
	this->setCreationDateTime(KDateTime(t));
}

QString BilboPost::author() const
{
	return this->mAuthor;
}

void BilboPost::setId(const int id)
{
	this->mId = id;
}

int BilboPost::id() const
{
	return this->mId;
}

void BilboPost::setAuthor(const QString &author)
{
	this->mAuthor = author;
}

BilboPost::BilboPost(const KBlog::BlogPost &post)
    :KBlog::BlogPost(post)
{
// 	this->setCreationDateTime(post.creationDateTime());
// 	this->setModificationDateTime(post.modificationDateTime());
// 	this->setLink(post.link());
// 	this->setPermaLink(post.permaLink());
// 	this->setCategories(post.categories());
// 	this->setCommentAllowed(post.isCommentAllowed());
// 	this->setError(post.error());
// 	this->setContent(post.content());
// 	this->setTags(post.tags());
// 	this->setMood(post.mood());
// 	this->setMusic(post.music());
// 	this->setPrivate(post.isPrivate());
// 	this->setStatus(post.status());
// 	this->setSummary(post.summary());
// 	this->setTrackBackAllowed(post.isTrackBackAllowed());
// 	this->setTitle(post.title());
}

BilboPost::Position BilboPost::position() const
{
	return mPosition;
}

void BilboPost::setPosition(const Position pos)
{
	mPosition = pos;
}

KBlog::BlogPost * BilboPost::toKBlogPost()
{
	KBlog::BlogPost *pp = new KBlog::BlogPost(QString());
	pp->setStatus(this->status());
	pp->setSummary(this->summary());
	pp->setTags(this->tags());
	pp->setTitle(this->title());
	pp->setTrackBackAllowed(this->isTrackBackAllowed());
	pp->setCategories(this->categories());
	pp->setCommentAllowed(this->isCommentAllowed());
	pp->setContent(this->content());
	pp->setPrivate(this->isPrivate());
	pp->setCreationDateTime(this->creationDateTime());
	pp->setError(this->error());
	pp->setModificationDateTime(this->modificationDateTime());
	pp->setMood(this->mood());
	pp->setMusic(this->music());
	pp->setPostId(this->postId());
	pp->setLink(this->link());
	pp->setPermaLink(this->permaLink());
	
	return pp;
}

QString BilboPost::toString() const
{
	qDebug("BilboPost::toString");
// 	if(!title().isEmpty())
// 		qDebug("BilboPost::toString: title is %s", this->title());
	QString ret;
	ret="\n******* Post Info **********";
	ret+=QString("\nID: ")+postId();
	ret+=QString("\nTitle: ")+title();
	ret+=QString("\nContent: ") + content();
	ret+="\nTags: " + tags().join(",");
	ret+="\nCategories: " + categories().join(",");
	ret+="\nCreation Date Time: "+cTime().toString();
	
	ret+="\n******* End Post Info ********\n";
	return ret;
}




BilboPost::BilboPost(const BilboPost &post)
    :KBlog::BlogPost()
{
    this->setCTime(post.cTime());
    this->setMTime(post.mTime());
    this->setPostLink(post.postLink());
    this->setPostPermaLink(post.postPermaLink());
    this->setCategories(post.categories());
    this->setCommentAllowed(post.isCommentAllowed());
    this->setError(post.error());
    this->setContent(post.content());
    this->setTags(post.tags());
    this->setMood(post.mood());
    this->setMusic(post.music());
    this->setPrivate(post.isPrivate());
    this->setStatus(post.status());
    this->setSummary(post.summary());
    this->setTrackBackAllowed(post.isTrackBackAllowed());
    this->setTitle(post.title());
    this->setAuthor(post.author());
    this->setPosition(post.position());
    this->setId(post.id());
}
