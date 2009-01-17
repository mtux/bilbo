/***************************************************************************
 *   This file is part of the Bilbo Blogger.                               *
 *   Copyright (C) 2008-2009 Mehrdad Momeny <mehrdad.momeny@gmail.com>     *
 *   Copyright (C) 2008-2009 Golnaz Nilieh <g382nilieh@gmail.com>          *
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
#include "dbman.h"

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

Backend::Backend( int blog_id, QObject* parent ): QObject( parent )
{
    kDebug() << "with blog id: " << blog_id;
    bBlog = DBMan::self()->getBlogInfo( blog_id );
    switch ( bBlog->api() ) {
        case BilboBlog::BLOGGER1_API:
            mBlog = new KBlog::Blogger1( KUrl(), this );
            break;
        case BilboBlog::GDATA_API:
            mBlog = new KBlog::GData( KUrl(), this );
            break;
        case BilboBlog::METAWEBLOG_API:
            mBlog = new KBlog::MetaWeblog( KUrl(), this );
            break;
        case BilboBlog::MOVABLETYPE_API:
            mBlog = new KBlog::MovableType( KUrl(), this );
            break;
        case BilboBlog::WORDPRESSBUGGY_API:
            mBlog = new KBlog::WordpressBuggy( KUrl(), this );
    }

    mBlog->setUsername( bBlog->username() );
    mBlog->setPassword( bBlog->password() );
    mBlog->setUrl( KUrl( bBlog->url() ) );
    mBlog->setBlogId( bBlog->blogid() );
    categoryListNotSet = false;

    connect( mBlog, SIGNAL( error( KBlog::Blog::ErrorType, const QString& ) ),
             this, SLOT( error( KBlog::Blog::ErrorType, const QString& ) ) );
    connect( mBlog, SIGNAL( errorPost( KBlog::Blog::ErrorType, const QString &, KBlog::BlogPost* ) ),
             this, SLOT( error( KBlog::Blog::ErrorType, const QString& ) ) );
    connect( mBlog, SIGNAL( errorComment( KBlog::Blog::ErrorType, const QString &, KBlog::BlogPost*, KBlog::BlogComment* ) ),
             this, SLOT( error( KBlog::Blog::ErrorType, const QString& ) ) );
    connect( mBlog, SIGNAL( errorMedia( KBlog::Blog::ErrorType, const QString &, KBlog::BlogMedia* ) ),
             this, SLOT( error( KBlog::Blog::ErrorType, const QString& ) ) );
}

Backend::~Backend()
{
    kDebug();
    bBlog->deleteLater();
}

void Backend::getCategoryListFromServer()
{
    kDebug() << "Blog Id: " << bBlog->id();
    if ( bBlog->api() == 1 || bBlog->api() == 2 || bBlog->api() == 3 ) {
        KBlog::MetaWeblog *tmp = dynamic_cast<KBlog::MetaWeblog*>( mBlog );
        connect( tmp, SIGNAL( listedCategories( const QList< QMap< QString, QString > > & ) ),
                 this, SLOT( categoriesListed( const QList< QMap< QString, QString > > & ) ) );
        tmp->listCategories();
    } else {
        char * err = "Blog API doesn't support getting Category list.";
        kDebug() << err;
        QString tmp = i18n( err );
        error( KBlog::Blog::NotSupported, tmp );
    }
}

void Backend::categoriesListed( const QList< QMap < QString , QString > > & categories )
{
    kDebug() << "Blog Id: " << bBlog->id();
    DBMan::self()->clearCategories( bBlog->id() );

    for ( int i = 0; i < categories.count(); ++i ) {
        QString name, description, htmlUrl, rssUrl, categoryId, parentId;
        const QMap<QString, QString> &category = categories.at( i );

        name = category.value( "name", QString() );
        description = category.value( "description", QString() );
        htmlUrl = category.value( "htmlUrl", QString() );
        rssUrl = category.value( "rssUrl", QString() );
        categoryId = category.value( "categoryId", QString() );
        parentId = category.value( "parentId", QString() );

        DBMan::self()->addCategory( name, description, htmlUrl, rssUrl, categoryId, parentId, bBlog->id() );
    }
    kDebug() << "Emitting sigCategoryListFetched...";
    Q_EMIT sigCategoryListFetched( bBlog->id() );
}

void Backend::getEntriesListFromServer( int count )
{
    kDebug() << "Blog Id: " << bBlog->id();
    connect( mBlog, SIGNAL( listedRecentPosts( const QList<KBlog::BlogPost> & ) ), this, SLOT( entriesListed( const QList<KBlog::BlogPost >& ) ) );
    mBlog->listRecentPosts( count );
}

void Backend::entriesListed( const QList< KBlog::BlogPost > & posts )
{
    kDebug() << "Blog Id: " << bBlog->id();
    DBMan::self()->clearPosts( bBlog->id() );

    for ( int i = 0; i < posts.count(); i++ ) {
        DBMan::self()->addPost( BilboPost( posts[i] ), bBlog->id() );
    }
    kDebug() << "Emitting sigEntriesListFetched ...";
    Q_EMIT sigEntriesListFetched( bBlog->id() );
}

void Backend::publishPost( BilboPost * post )
{
    kDebug() << "Blog Id: " << bBlog->id();

    KBlog::BlogPost *bp = post->toKBlogPost();

    int api = bBlog->api();
    if ( api == 0 || api == 1 || api == 2 ) {
        KBlog::Blogger1 *b1 = dynamic_cast<KBlog::Blogger1*>( mBlog );
        connect( b1, SIGNAL( createdPost( KBlog::BlogPost * ) ), this, SLOT( postPublished( KBlog::BlogPost * ) ) );
        b1->createPost( bp );
    } else if ( api == 3 ) {
        KBlog::WordpressBuggy *wp = dynamic_cast<KBlog::WordpressBuggy*>( mBlog );
        connect( wp, SIGNAL( createdPost( KBlog::BlogPost * ) ), this, SLOT( postPublished( KBlog::BlogPost * ) ) );
        if ( post->categories().count() > 1 ) {
            mCreatePostCategories = post->categoryList();
            bp->categories().clear();
            categoryListNotSet = true;
            kDebug() << "Will use setPostCategories Function, for " << mCreatePostCategories.count() << " categories.";
        }
        wp->createPost( bp );
    } else if ( api == 4 ) {
        KBlog::GData *gd = dynamic_cast<KBlog::GData*>( mBlog );
        connect( gd, SIGNAL( createdPost( KBlog::BlogPost * ) ), this, SLOT( postPublished( KBlog::BlogPost * ) ) );
        gd->createPost( bp );
    }
//     delete post;
}

void Backend::postPublished( KBlog::BlogPost *post )
{
    kDebug() << "Blog Id: " << bBlog->id();
    if ( post->status() == KBlog::BlogPost::Error ) {
        kDebug() << "Publishing Failed";
        const QString tmp( i18n( "Publishing post failed : %1" ).arg( post->error() ) );
        kDebug() << "Emitting sigError...";
        Q_EMIT sigError( tmp );
        return;
    }
    if ( categoryListNotSet ) {
        mSetPostCategoriesMap[ post->postId()] = post;
        QMap<QString, bool> cats;
        int count = mCreatePostCategories.count();
        for ( int i = 0; i < count; ++i ) {
            cats.insert( mCreatePostCategories[i].categoryId, false );
        }
//         kDebug()<<"there's "<<count<< " categories to send."<<"\t numbers: "<<cats.count()<<" are:"<<cats.keys();
        setPostCategories( post->postId(), cats );
    } else {
        BilboPost *pp = new BilboPost( *post );
        int post_id = DBMan::self()->addPost( *pp, bBlog->id() );
        if ( post_id != -1 ) {
            pp->setId( post_id );
            pp->setPrivate( post->isPrivate() );
            kDebug() << "Emiteding sigPostPublished...";
            Q_EMIT sigPostPublished( bBlog->id(), pp );
        }
    }
}

void Backend::uploadMedia( BilboMedia * media )
{
    kDebug() << "Blog Id: " << bBlog->id();

    switch ( bBlog->api() ) {
        case BilboBlog::BLOGGER1_API:
        case BilboBlog::GDATA_API:
            kDebug() << "The Blogger1 and GData API type doesn't support uploading Media files.";
            return;
            break;
        case BilboBlog::METAWEBLOG_API:
        case BilboBlog::MOVABLETYPE_API:
        case BilboBlog::WORDPRESSBUGGY_API:
            KBlog::BlogMedia *m = new KBlog::BlogMedia() ;
            KBlog::MetaWeblog *MWBlog = qobject_cast<KBlog::MetaWeblog*>( mBlog );

            m->setMimetype( media->mimeType() );

            QByteArray data;
            QFile file( media->localUrl() );

            if ( !file.open( QIODevice::ReadOnly ) ) {
                kError() << "Cannot open file " << media->localUrl();
                const QString tmp( i18n( "Uploading media failed : Cannot open file %1", media->localUrl() ) );
                kDebug() << "Emitting sigError ...";
                Q_EMIT sigError( tmp );
                return;
            }

            data = file.readAll();

            if ( data.count() == 0 ) {
                kError() << "Cannot read the media file, please check if it exists.";
                const QString tmp( i18n( "Uploading media failed : Cannot read the media file, please check if it exists. path: %1",
                                   media->localUrl() ) );
                kDebug() << "Emitting sigError ...";
                Q_EMIT sigError( tmp );
                return;
            }

            m->setData( data );
            m->setName( media->name() );

            media->setCheckSum( qChecksum( data.data(), data.count() ) );

            if ( media->checksum() == 0 ) {
                kError() << "Media file checksum is zero";
                const QString tmp( i18n( "Uploading media failed : Media file checksum is zero, please check file path. path: %1",
                                   media->localUrl() ) );
                kDebug() << "Emitting sigError ...";
                Q_EMIT sigError( tmp );
                return;
            }

            if ( !MWBlog ) {
                kError() << "MWBlog is NULL: casting has not worked, this should NEVER happen, has the gui allowed using GDATA?";
                const QString tmp( i18n( "INTERNAL ERROR: MWBlog is NULL: casting has not worked, this should NEVER happen." ) );
                kDebug() << "Emitting sigError ...";
                Q_EMIT sigError( tmp );
                return;
            }
            mPublishMediaMap[ m ] = media;
            connect( MWBlog, SIGNAL( createdMedia( KBlog::BlogMedia* ) ), this, SLOT( mediaUploaded( KBlog::BlogMedia* ) ) );
            connect( MWBlog, SIGNAL( errorMedia( KBlog::Blog::ErrorType, const QString &, KBlog::BlogMedia* ) ),
                     this, SLOT( sltMediaError( KBlog::Blog::ErrorType, const QString &, KBlog::BlogMedia* ) ) );
            MWBlog->createMedia( m );
            return;
            break;
    }
    kError() << "Api type does not sets correctly!";
    const QString tmp( i18n( "Api type does not sets correctly!" ) );
    Q_EMIT sigError( tmp );
}

void Backend::mediaUploaded( KBlog::BlogMedia * media )
{
    kDebug() << "Blog Id: " << bBlog->id();
    if(!media){
        kError()<<"ERROR! Media returned from KBlog is NULL!";
        return;
    }
    BilboMedia * m = mPublishMediaMap.value( media );
    kDebug() << "check = 1";
    if(!m){
        kError()<<"ERROR! Media returned from KBlog doesn't exists on the Map!";
        return;
    }
    mPublishMediaMap.remove( media );
    kDebug() << "check = 2";
    if ( media->status() == KBlog::BlogMedia::Error ) {
        kError() << "Upload error! with this message: " << media->error();
        const QString tmp( i18n( "Uploading Media failed : %1", media->error() ) );
        kDebug() << "Emitting sigMediaError ...";
        Q_EMIT sigMediaError( tmp, m );
        return;
    }
    quint16 newChecksum = qChecksum( media->data().data(), media->data().count() );
    kDebug() << "check = 3";
    if ( newChecksum != m->checksum() ) {
        kError() << "Check sum error: checksum of sent file: " << m->checksum() <<
                " Checksum of recived file: " << newChecksum << "Error: " << media->error() << endl;
        const QString tmp( i18n( "Uploading Media failed : Check sum Error. returned error: %1",
                           media->error() ) );
        kDebug() << "Emitting sigMediaError ...";
        Q_EMIT sigMediaError( tmp, m );
        return;
    }
    kDebug() << "check = 4";
    m->setRemoteUrl( QUrl( media->url().url() ).toString() );
    kDebug() << "check = 5";
    kDebug() << "Emitting sigMediaUploaded...";
    Q_EMIT sigMediaUploaded( m );
}

void Backend::modifyPost( BilboPost * post )
{
}

void Backend::postModified( KBlog::BlogPost * post )
{
}

void Backend::error( KBlog::Blog::ErrorType type, const QString & errorMessage )
{
    kDebug() << "Blog Id: " << bBlog->id();
    QString errType = errorTypeToString( type );
    errType += errorMessage;
    kDebug() << errType;
    kDebug() << "Emitting sigError";
    Q_EMIT sigError( errType );
}

void Backend::setPostCategories( const QString postId, const QMap< QString, bool > & categoriesList )
{
    kDebug() << "Categories to be set for post: " << categoriesList.keys();
    int count = categoriesList.count();
    if ( count < 1 ) {
        kDebug() << "Category list is empty.";
        return;
    }
    if ( bBlog->api() == BilboBlog::MOVABLETYPE_API || bBlog->api() == BilboBlog::WORDPRESSBUGGY_API ) {
        KBlog::MovableType *mt = qobject_cast<KBlog::MovableType*>( mBlog );
        connect( mt, SIGNAL( settedPostCategories( const QString & ) ), this, SLOT( postCategoriesSetted( const QString& ) ) );
        mt->setPostCategories( postId, categoriesList );
    } else {
        kDebug() << "Blog API doesn't support setting post categories the api type is: " << bBlog->api();
        QString err = i18n( "The registred blog API doesn't support setting categories for a post." );
        error(KBlog::Blog::NotSupported, err);
    }
}

void Backend::postCategoriesSetted( const QString &postId )
{
    kDebug();
    KBlog::BlogPost *post = mSetPostCategoriesMap[ postId ];
    BilboPost *pp = new BilboPost( *post );
    mSetPostCategoriesMap.remove( postId );
    int post_id = DBMan::self()->addPost( *pp, bBlog->id() );
    if ( post_id != -1 ) {
        pp->setPrivate( post->isPrivate() );
        pp->setId( post_id );
        kDebug() << "Emitting sigPostPublished ...";
        Q_EMIT sigPostPublished( bBlog->id(), pp );
    }
    delete post;
}

void Backend::sltMediaError( KBlog::Blog::ErrorType type, const QString & errorMessage, KBlog::BlogMedia * media )
{
    kDebug();
    QString errType = errorTypeToString( type );
    errType += errorMessage;
    kDebug() << errType;
    kDebug() << "Emitting sigMediaError ...";
    emit sigMediaError( errorMessage, mPublishMediaMap[ media ] );
    mPublishMediaMap.remove( media );
}

QString Backend::errorTypeToString( KBlog::Blog::ErrorType type )
{
    QString errType;
    switch ( type ) {
        case KBlog::Blog::XmlRpc:
            errType = i18n( "XML RPC Error: " );
            break;
        case KBlog::Blog::Atom:
            errType = i18n( "Atom API Error: " );
            break;
        case KBlog::Blog::ParsingError:
            errType = i18n( "KBlog Parsing Error: " );
            break;
        case KBlog::Blog::AuthenticationError:
            errType = i18n( "Authentication Error: " );
            break;
        case KBlog::Blog::NotSupported:
            errType = i18n( "Not Supported Error: " );
            break;
        default:
            errType = i18n( "Unknown Error type: " );
    };
    return errType;
}

#include "backend.moc"
