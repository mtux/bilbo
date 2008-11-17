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
#include "backend.h"
#include "bilboblog.h"
#include "bilbopost.h"
#include "global.h"

#include <kblog/blogger1.h>
#include <kblog/gdata.h>
#include <kblog/metaweblog.h>
#include <kblog/movabletype.h>
#include <kblog/wordpressbuggy.h>
Backend::Backend(int blog_id, QObject* parent): QObject(parent)
{
	qDebug("Backend::Backend: with blog id: %d", blog_id);
	bBlog = db->getBlogInfo(blog_id);
	switch( bBlog->api() ){
	case BilboBlog::BLOGGER1_API:
		mBlog = new KBlog::Blogger1(KUrl());
		break;
	case BilboBlog::GDATA_API:
		mBlog = new KBlog::GData(KUrl());
		break;
	case BilboBlog::METAWEBLOG_API:
		mBlog = new KBlog::MetaWeblog(KUrl());
		break;
	case BilboBlog::MOVABLETYPE_API:
        mBlog = new KBlog::MovableType(KUrl());
        break;
    case BilboBlog::WORDPRESSBUGGY_API:
        mBlog = new KBlog::WordpressBuggy(KUrl());
	}
	
	mBlog->setUsername(bBlog->username());
	mBlog->setPassword(bBlog->password());
	mBlog->setUrl(KUrl(bBlog->blogUrl()));
	mBlog->setBlogId(bBlog->blogid());
}

Backend::~Backend()
{
}

void Backend::getCategoryListFromServer()
{
	qDebug("Backend::getCategoryListFromServer: Blog Id: %d", bBlog->id());
	if(bBlog->api()==1 || bBlog->api()==2 || bBlog->api()==3){
		KBlog::MetaWeblog *tmp = dynamic_cast<KBlog::MetaWeblog*>(mBlog);
		connect(tmp, SIGNAL(listedCategories(const QList< QMap< QString, QString > > &)),
				this, SLOT(categoriesListed(const QList< QMap< QString, QString > > &)));
		tmp->listCategories();
    } else {
		qDebug("Backend::getCategoryListFromServer: Blog API doesn't support getting Category list.");
    }
}

void Backend::categoriesListed(const QList< QMap < QString , QString > > & categories)
{
	qDebug("Backend::categoriesListed: Blog Id: %d", bBlog->id());
	db->clearCategories(bBlog->id());
	QString name, description, htmlUrl, rssUrl;
	const QMap<QString, QString> *category;

	for ( int i = 0; i < categories.count(); ++i) {
		category = &(categories.at(i));

		name = category->value("name");
		description = category->value("description"); //TODO UNUSED
		htmlUrl = category->value("htmlUrl"); //TODO UNUSED
		rssUrl = category->value("rssUrl"); //TODO UNUSED
		
		db->addCategory(name, bBlog->id());
	}
	emit sigCategoryListFetched(bBlog->id());
}

void Backend::getEntriesListFromServer(int count)
{
	qDebug("Backend::getEntriesListFromServer: Blog Id: %d", bBlog->id());
	mBlog->listRecentPosts(count);
	connect(mBlog, SIGNAL(listedRecentPosts(const QList<KBlog::BlogPost> & )), this, SLOT(entriesListed(const QList<KBlog::BlogPost >&)));
}

void Backend::entriesListed(const QList< KBlog::BlogPost > & posts)
{
	qDebug("Backend::entriesListed: Blog Id: %d", bBlog->id());
	db->clearPosts(bBlog->id());
	
	for(int i=0; i<posts.count(); i++){
		db->addPost(BilboPost(posts[i]), bBlog->id());
	}
	emit sigEntriesListFetched(bBlog->id());
}

void Backend::publishPost(BilboPost * post)
{
	///FIXME: Categories problem!
	qDebug("Backend::publishPost");
	
	KBlog::BlogPost *bp = post->toKBlogPost();
	qDebug(post->toString().toLatin1().data());
	
	int api = bBlog->api();
	if(api==0 || api==1 || api==2){
		KBlog::Blogger1 *b1 = dynamic_cast<KBlog::Blogger1*>(mBlog);
		connect(b1, SIGNAL(createdPost( KBlog::BlogPost * )), this, SLOT(postPublished( KBlog::BlogPost * )));
		b1->createPost(bp);
	} else if(api==3){
		KBlog::WordpressBuggy *wp = dynamic_cast<KBlog::WordpressBuggy*>(mBlog);
		connect(wp, SIGNAL(createdPost( KBlog::BlogPost * )), this, SLOT(postPublished( KBlog::BlogPost * )));
		wp->createPost(bp);
	} else if(api==4){
		KBlog::GData *gd = dynamic_cast<KBlog::GData*>(mBlog);
		connect(gd, SIGNAL(createdPost( KBlog::BlogPost * )), this, SLOT(postPublished( KBlog::BlogPost * )));
		gd->createPost(bp);
	}
}

void Backend::postPublished(KBlog::BlogPost *post)
{
	qDebug("Backend::postPublished");
	BilboPost pp((*post));
	int post_id = db->addPost(pp, bBlog->id());
	if(post_id!=-1){
		emit sigPostPublished(bBlog->id(), post_id);
		qDebug("Backend::sigPostPublished emited!");
	}
}

void Backend::UploadMedia(BilboMedia * media)
{
}

void Backend::mediaUploaded(KBlog::BlogMedia * media)
{
}
