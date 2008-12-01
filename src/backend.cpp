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
#include "global.h"
#include "bilbopost.h"
#include "bilbomedia.h"

#include <kurl.h>
#include <kblog/blogger1.h>
#include <kblog/gdata.h>
#include <kblog/metaweblog.h>
#include <kblog/movabletype.h>
#include <kblog/wordpressbuggy.h>
#include <kblog/blogmedia.h>
#include <kdebug.h>
#include <KDE/KLocale>
// #include <QMimeData>

Backend::Backend(int blog_id, QObject* parent): QObject(parent)
{
	kDebug()<<"with blog id: "<< blog_id;
	bBlog = __db->getBlogInfo(blog_id);
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
	mBlog->setUrl(KUrl(bBlog->url()));
    mBlog->setBlogId(bBlog->blogid());
    mChecksum = 0;
    mediaLocalUrl= "";
    
    connect( mBlog, SIGNAL( error( KBlog::Blog::ErrorType, const QString& ) ),
            this, SLOT( void error( KBlog::Blog::ErrorType, const QString& ) ) );
    connect( mBlog, SIGNAL( errorPost( KBlog::Blog::ErrorType, const QString &, KBlog::BlogPost* ) ),
            this, SLOT( void error( KBlog::Blog::ErrorType, const QString& ) ) );
    connect( mBlog, SIGNAL( errorComment( KBlog::Blog::ErrorType, const QString &, KBlog::BlogPost*, KBlog::BlogComment* ) ),
            this, SLOT( void error( KBlog::Blog::ErrorType, const QString& ) ) );
    connect( mBlog, SIGNAL( errorMedia( KBlog::Blog::ErrorType, const QString &, KBlog::BlogMedia* ) ),
             this, SLOT( void error( KBlog::Blog::ErrorType, const QString& ) ) );
}

Backend::~Backend()
{
    kDebug();
    delete mBlog;
    delete bBlog;
}

void Backend::getCategoryListFromServer()
{
	kDebug()<<"Blog Id: "<< bBlog->id();
	if(bBlog->api()==1 || bBlog->api()==2 || bBlog->api()==3){
		KBlog::MetaWeblog *tmp = dynamic_cast<KBlog::MetaWeblog*>(mBlog);
		connect(tmp, SIGNAL(listedCategories(const QList< QMap< QString, QString > > &)),
				this, SLOT(categoriesListed(const QList< QMap< QString, QString > > &)));
		tmp->listCategories();
    } else {
		kDebug()<<"Blog API doesn't support getting Category list.";
    }
}

void Backend::categoriesListed(const QList< QMap < QString , QString > > & categories)
{
	kDebug()<<"Blog Id: "<< bBlog->id();
	__db->clearCategories(bBlog->id());
	QString name, description, htmlUrl, rssUrl;
	const QMap<QString, QString> *category;

	for ( int i = 0; i < categories.count(); ++i) {
		category = &(categories.at(i));

		name = category->value("name");
		description = category->value("description"); //TODO UNUSED
		htmlUrl = category->value("htmlUrl"); //TODO UNUSED
		rssUrl = category->value("rssUrl"); //TODO UNUSED
		
		__db->addCategory(name, bBlog->id());
	}
	Q_EMIT sigCategoryListFetched(bBlog->id());
}

void Backend::getEntriesListFromServer(int count)
{
	kDebug()<<"Blog Id: "<< bBlog->id();
	mBlog->listRecentPosts(count);
	connect(mBlog, SIGNAL(listedRecentPosts(const QList<KBlog::BlogPost> & )), this, SLOT(entriesListed(const QList<KBlog::BlogPost >&)));
}

void Backend::entriesListed(const QList< KBlog::BlogPost > & posts)
{
    kDebug()<<"Blog Id: "<< bBlog->id();
	__db->clearPosts(bBlog->id());
	
	for(int i=0; i<posts.count(); i++){
		__db->addPost(BilboPost(posts[i]), bBlog->id());
	}
	Q_EMIT sigEntriesListFetched(bBlog->id());
}

void Backend::publishPost(BilboPost * post)
{
	///FIXME: Categories problem!
    kDebug()<<"Blog Id: "<< bBlog->id();
	
	KBlog::BlogPost *bp = post->toKBlogPost();
// 	qDebug(post->toString().toLatin1().data());
	
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
    kDebug()<<"Blog Id: "<< bBlog->id();
    if(post->status()==KBlog::BlogPost::Error){
        kDebug()<<"Publishing Failed";
        QString tmp(i18n("Publishing Post failed : "));
        tmp += post->error();
        Q_EMIT sigError(tmp);
        return;
    }
	BilboPost pp((*post));
	int post_id = __db->addPost(pp, bBlog->id());
	if(post_id!=-1){
        kDebug()<<"Emiteding sigPostPublished...";
		Q_EMIT sigPostPublished(bBlog->id(), post_id, post->isPrivate());
	}
}

void Backend::uploadMedia(BilboMedia * media)
{
    kDebug()<<"Blog Id: "<< bBlog->id();
    
    switch(bBlog->api())
    {
    case BilboBlog::BLOGGER1_API:
    case BilboBlog::GDATA_API:
        kDebug()<<"The Blogger1 and GData API type doesn't support uploading Media files.";
        return;
        break;
    case BilboBlog::METAWEBLOG_API:
    case BilboBlog::MOVABLETYPE_API:
    case BilboBlog::WORDPRESSBUGGY_API:
        KBlog::BlogMedia *m = new KBlog::BlogMedia() ;
        KBlog::MetaWeblog *MWBlog = qobject_cast<KBlog::MetaWeblog*>(mBlog);
        
        m->setMimetype( media->mimeType() );
        
        QByteArray data;
        QFile file(media->localUrl());
        
        if( !file.open(QIODevice::ReadOnly) ) {
            kError() << "Cannot open file " << media->localUrl();
            QString tmp(i18n("Uploading media failed : Cannot open file %1", media->localUrl()));
            Q_EMIT sigError(tmp);
            return;
        }
        
        data = file.readAll();
        
        Q_ASSERT( data.count() );
        
        m->setData(data);
        m->setName(media->name());
        this->mediaLocalUrl = media->localUrl();
        
        mChecksum = qChecksum(data.data(), data.count());
        
        if (mChecksum==0) {
            kError() << "Media file checksum is zero";
            QString tmp(i18n("Uploading media failed : Media file checksum is zero, please check file path. path: %1", media->localUrl()));
            Q_EMIT sigError(tmp);
            return;
        }
        
        if(!MWBlog) { // FIXME do not crash on GDATA for the moment
            kError() << "MWBlog is NULL: casting has not worked, this should NEVER happen, has the gui allowed using GDATA?" << endl;
            QString tmp(i18n("MWBlog is NULL: casting has not worked, this should NEVER happen, has the gui allowed using GDATA?"));
            Q_EMIT sigError(tmp);
            return;
        }
        connect (MWBlog, SIGNAL(createdMedia( KBlog::BlogMedia* )), this, SLOT(mediaUploaded( KBlog::BlogMedia* )));
        MWBlog->createMedia(m);
        return;
        break;
    }
    kError() <<"Api type does not sets correctly!";
    QString tmp(i18n("Api type does not sets correctly!"));
    Q_EMIT sigError(tmp);
}

void Backend::mediaUploaded(KBlog::BlogMedia * media)
{
    kDebug()<<"Blog Id: "<< bBlog->id();
    if(media->status() == KBlog::BlogMedia::Error){
        kError()<<"Upload error! with this message: "<< media->error();
        QString tmp(i18n("Uploading Media failed : %1", media->error()));
        Q_EMIT sigError(tmp);
        return;
    }
    quint16 newChecksum = qChecksum(media->data().data(), media->data().count());
    if( newChecksum != mChecksum ){
        kError()<<"Check sum error: checksum of sent file: "<<mChecksum<<" Checksum of recived file: "<<newChecksum<<
                "Error: "<<media->error()<<endl;
        QString tmp(i18n("Uploading Media failed : Check sum Error. returned error: %1", media->error()));
        Q_EMIT sigError(tmp);
        return;
    }
    BilboMedia * m = new BilboMedia();
    m->setUploded(true);
    m->setLocalUrl(mediaLocalUrl);
    m->setBlogId(bBlog->id());//TODO un comment this.
    m->setRemoteUrl(QUrl(media->url().url()).toString());
//     m->setMimeData(new QMimeData(media->mimetype()));
    m->setName(media->name());
    kDebug()<<"Emitting sigMediaUploaded...";
    Q_EMIT sigMediaUploaded(m);
}

void Backend::modifyPost(BilboPost * post)
{
}

void Backend::postModified(KBlog::BlogPost * post)
{
}

void Backend::error(KBlog::Blog::ErrorType type, const QString & errorMessage)
{
    kDebug()<<"Blog Id: "<< bBlog->id();
    QString errType;
    switch (type) {
        case KBlog::Blog::XmlRpc:
            errType = i18n("XML RPC Error: ");
            break;
        case KBlog::Blog::Atom:
            errType = i18n("Atom API Error: ");
            break;
        case KBlog::Blog::ParsingError:
            errType = i18n("KBlog Parsing Error: ");
            break;
        case KBlog::Blog::AuthenticationError:
            errType = i18n("Authentication Error: ");
            break;
        case KBlog::Blog::NotSupported:
            errType = i18n("Not Supported Error: ");
            break;
        default:
            errType = i18n("Unknown Error type: ");
    };
    errType += errorMessage;
    kDebug()<<errType;
    kDebug()<<"Emitting sigError";
    Q_EMIT sigError( errType );
}

#include "backend.moc"