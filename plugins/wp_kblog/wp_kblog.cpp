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

#include <kurl.h>
#include <kdatetime.h>

#include "wp_kblog.h"
#include "version.h"

bool WP_KBlog::deletePost(const QString& blogUrl, const QString& blogid, const QString& username, const QString& password, const QString& postid, const QString& appkey)
{
	return true;
}

bool WP_KBlog::editPost(const QString& blogUrl, const QString& blogid, const QString& username, const QString& password, const Post& editedPost)
{
	return true;
}

bool WP_KBlog::publishPost(const QString& blogUrl, const QString& blogid, const QString& username, const QString& password, const Post& newpost)
{
	KBlog::Blog* b = new KBlog::WordpressBuggy(KUrl(blogUrl));
	b->setUsername( username );
	b->setPassword( password );
	b->setBlogId(blogid);
	KBlog::BlogPost *post= new KBlog::BlogPost;
	
	post->setTitle(newpost.title);
	post->setContent(newpost.content);
	post->setCategories(newpost.categories);
	post->setCommentAllowed(newpost.allowComments);
	post->setCreationDateTime(KDateTime(newpost.pubDate));
	post->setPermaLink(KUrl(newpost.permaLink));
	post->setPrivate(newpost.isPrivate);
	post->setTags(newpost.tags);
	post->setTrackBackAllowed(newpost.allowPings);
// 	post.setLink(KUrl(newpost.link));
	connect( b, SIGNAL(createdPost( KBlog::BlogPost* )), this, SLOT(postCreated( KBlog::BlogPost* )));
	b->createPost( post );
	
	return true;
}

Post* WP_KBlog::getPost(const QString& blogUrl, const QString& blogid, const QString& postid, const QString& username, const QString& password, const QString& appkey)
{
}

QList< Category > WP_KBlog::getCategoryList(const QString& blogUrl, const QString& blogid, const QString& username, const QString& password, const QString& appkey)
{
}

QList< Post > WP_KBlog::getRecentPosts(const QString& blogUrl, const QString& blogid, const QString& username, const QString& password, const QString& appkey, const int numberOfPosts)
{
}

QString WP_KBlog::getPluginName()
{
	return "Wordpress Plugin, using KBlog library.";
}

QString WP_KBlog::uploadMediaFile(const QString& blogUrl, const QString& blogid, const QString& username, const QString& password, const MediaFile& file, const QString& appkey)
{
}


QList< BlogInfo > WP_KBlog::listblogs(const QString & blogUrl, const QString & username, const QString & password)
{
	KBlog::WordpressBuggy *b= new KBlog::WordpressBuggy(KUrl(blogUrl));
	b->setUsername(username);
	b->setPassword(password);
	b->listBlogs();
	
}

Q_EXPORT_PLUGIN2(bilbo_wpkblog, WP_KBlog)
