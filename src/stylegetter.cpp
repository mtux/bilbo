/***************************************************************************

   Copyright (C) 2007-2008-2008 by Christian Weilbach <christian_weilbach@web.de>
   Copyright (C) 2007-2008 Antonio Aloisio <gnuton@gnuton.org>

 ***************************************************************************/
/*   Copyright (C) 2008-2009 Mehrdad Momeny <mehrdad.momeny@gmail.com>     *
 *   Copyright (C) 2008-2009 Golnaz Nilieh <g382nilieh@gmail.com>          *
 *                                                                         *
 *   This file is part of the Bilbo Blogger.                               *
 *   It is a modified version of "weblogstylegetter.cpp" from              *
 *   KBlogger project. it has been modified for use in Bilbo Blogger, at   *
 *   February 2009.                                                        *
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

#include "stylegetter.h"

#include <kio/job.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <klocalizedstring.h>
#include <kdebug.h>

#include <QFile>

#include "bilbopost.h"
#include "bilboblog.h"
#include "backend.h"
#include "dbman.h"

#define POST_TITLE "Temporary-Post-Used-For-Style-Detection-Title-"
#define POST_CONTENT "Temporary-Post-Used-For-Style-Detection-Content-"

StyleGetter::StyleGetter( const int blogid, QObject *parent ): QObject( parent )
{
    kDebug();
    BilboBlog tempBlog = DBMan::self()->getBlogInfo( blogid );
    if ( tempBlog.isError() ) {
        KMessageBox::error( mParent, i18n( "Can not fetch the selected blog style.\n\n,%1", 
                            DBMan::self()->lastErrorText() ) );
        return;
    }
    // sets cachePath to ~/.kde4/share/apps/bilbo/blog_host_name/
    QString blogDir = DBMan::self()->getBlogInfo( blogid ).url().host();
    kDebug() << blogDir;
    mCachePath = KStandardDirs::locateLocal( "data", "bilbo/" + blogDir + '/' , true );
    generateRandomPostStrings();
    mParent = qobject_cast< QWidget* >( parent );
    Q_ASSERT( mParent );

    // create temp post

    mTempPost = new BilboPost();
    mTempPost->setTitle( mPostTitle );
    mTempPost->setContent( mPostContent );
    mTempPost->setPrivate( false );

    b = new Backend( blogid );
    connect( b, SIGNAL( sigPostPublished( int, BilboPost* ) ), this, 
             SLOT( sltTempPostPublished( int, BilboPost* ) ) );
//     connect( b, SIGNAL( sigPostFetched( BilboPost * ) ), this, SLOT( sltTempPostFetched( BilboPost * ) ) );
    connect( b, SIGNAL( sigError( const QString& ) ), this, SLOT( sltError( const QString& ) ) );

    b->publishPost( mTempPost );
}

StyleGetter::~StyleGetter()
{
    kDebug();
}

QString StyleGetter::styledHtml( const int blogid,
                                      const QString &title,
                                      const QString &content )
{
    kDebug();
    BilboBlog tempBlog = DBMan::self()->getBlogInfo( blogid );
    if ( tempBlog.isError() ) {
        kDebug() << DBMan::self()->lastErrorText();
        return "<html><body><b>" + title + "</b><br>" + content + "</html>";
    }

    QString blogDir = tempBlog.url().host();
    QString url = QString( "bilbo/%1/style.html" ).arg( blogDir );
    KUrl dest = KStandardDirs::locate( "data", url );
    kDebug() <<  url;

    QString buffer;
    if ( !dest.isValid() ) {
        return "<html><body><b>" + title + "</b><br>" + content + "</html>";
    }
    QFile file( dest.path() );
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        return "<html><body><b>" + title + "</b><br>" + content + "</html>";
    }
    while ( !file.atEnd() ) {
        QByteArray line = file.readLine();
        buffer.append( QString::fromUtf8( line ) );
    }

    QRegExp typeRx ( QString( "(TYPE[^>]+>)" ) );
    buffer.remove( typeRx );

    QRegExp titleRx( QString( "%1[\\d]*" ).arg( POST_TITLE ) );
    QRegExp contentRx( QString( "%1[\\d]*" ).arg( POST_CONTENT ) );

    buffer.replace( titleRx, title );
    buffer.replace( contentRx, content );

    return buffer;
}

void StyleGetter::sltTempPostPublished( int blogId, BilboPost* post )
{
    kDebug();

    connect( b, SIGNAL( sigPostFetched( BilboPost * ) ), this, SLOT( sltTempPostFetched( BilboPost * ) ) );
    b->fetchPost( *post );
}

void StyleGetter::sltTempPostFetched( BilboPost* post )
{
    KUrl postUrl;
    postUrl = post->permaLink();
    if ( postUrl.url().isEmpty() ) {
        kDebug() << "permalink was empty";
        postUrl = post->link();
        if ( postUrl.url().isEmpty() ) {
            KMessageBox::error( mParent,
                            i18n( "Can not fetch style from the blog" ) );
            return;
        }
    }

    mTempPost = post;
    mJob = KIO::storedGet( postUrl, KIO::NoReload, KIO::HideProgressInfo );

    connect( mJob, SIGNAL( result( KJob* ) ),
             this, SLOT( sltHtmlCopied( KJob* ) ) );
    
//     sender()->deleteLater();
}

void StyleGetter::sltHtmlCopied( KJob *job )
{
    kDebug();
    if ( job->error() ) {
        KMessageBox::error( mParent, i18n( "Impossible to get html file,%1", 
                            job->errorString() ) );
        sender()->deleteLater();
        return;
    }
    QByteArray httpData( qobject_cast<KIO::StoredTransferJob*>( job )->data() );

    QString href( mTempPost->permaLink().url() );
    int filenameOffset = href.lastIndexOf( "/" );
    href = href.remove( filenameOffset + 1, 255 );
    QString base( "<base href=\""+href+"\"/>" );
    kDebug() << "base: " << base;

    QRegExp rxBase( "(<base\\shref=[^>]+>)" );
    if ( rxBase.indexIn( httpData ) != -1 ) {
         httpData.replace( rxBase.cap( 1 ).toLatin1(), base.toLatin1() );
    }
    else {
        int headOffset = httpData.indexOf( "<head>" );
        httpData.insert( headOffset + 6, base.toLatin1() );
    }

    KUrl dest(mCachePath + "style.html");
    Q_ASSERT( dest.isValid() );
    QFile file( dest.path() );
    file.open( QIODevice::WriteOnly );
    if ( file.write( httpData ) == -1 ) {
        KMessageBox::error( mParent,
                            i18n( "Impossible to write data to file %1", dest.path() ) );

        file.close();
        return;
    }
    file.close();
//     Q_EMIT sigStyleFetched();
    

    //Remove temp post from the server.
    b->removePost( *mTempPost );
    connect( b, SIGNAL( sigPostRemoved( int, const BilboPost &) ), this, 
             SLOT( sltTempPostRemoved( int, const BilboPost & ) ) );
}

void StyleGetter::sltTempPostRemoved( int blog_id, const BilboPost &post)
{
    delete mTempPost;
    b->deleteLater();

    Q_EMIT sigStyleFetched();
}

void StyleGetter::generateRandomPostStrings()
{
    kDebug();
    srand( time( 0 ) );
    int postRandomNumber = rand();
    mPostTitle = QString( "%1%2" ).arg( POST_TITLE ).arg( postRandomNumber );
    mPostContent = QString( "%1%2" ).arg( POST_CONTENT ).arg( postRandomNumber );
}

void StyleGetter::sltError( const QString & errMsg )
{
    kDebug();
    QString err = i18n( "An Error occurred on latest transaction.\n%1", errMsg );

    KMessageBox::error( mParent, err );
    b->deleteLater();
}

#include "stylegetter.moc"
