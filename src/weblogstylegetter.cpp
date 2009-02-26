/***************************************************************************

   Copyright (C) 2007-2008-2008 by Christian Weilbach <christian_weilbach@web.de>
   Copyright (C) 2007-2008 Antonio Aloisio <gnuton@gnuton.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "weblogstylegetter.h"

#include <kio/job.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
// #include <kdatetime.h>
#include <klocalizedstring.h>
#include <kdebug.h>

#include <QFile>

#include "bilbopost.h"
#include "bilboblog.h"
#include "backend.h"
#include "dbman.h"
// #include "media/media.h"

#define POST_TITLE "Temporary-Post-Used-For-Style-Detection-Title-"
#define POST_CONTENT "Temporary-Post-Used-For-Style-Detection-Content-"
// #define DIR_TO_SAVE_ENTRIES "styles/"
// #define BLOGS_STYLE_DIR KStandardDirs::locateLocal("cache", "bilbo/styles/", true)

// namespace KBlogger
// {

// WeblogStyleGetter::WeblogStyleGetter(const QString &blog, QObject *parent): QObject(parent)
// {
//     // sets cachePath to ~/.kde4/share/apps/kblogger/styles/my_blog_name
//     mCachePath = KStandardDirs::locateLocal("appdata", DIR_TO_SAVE_ENTRIES + blog + '/' , true);
//     generateRandomPostStrings();
//     mParent = qobject_cast<QWidget*>(parent);
//     Q_ASSERT(mParent);
// 
//     // create temp post
//     kDebug();
//     QStringList categories("");
//     mTempPost = new Post(blog,
//                                 mPostTitle,
//                                 mPostContent,
//                                 categories,
//                                 false,
//                                 KDateTime(QDateTime(QDate(2000,1,1),QTime(0,0)))//datetime
//                                );
//     Backend::self()->disconnect(SIGNAL(jobsFinished()));
//     connect ( Backend::self() , SIGNAL(jobsFinished()), SLOT(slotPostSent()) );
// 
//     // send post
//     Backend::self()->sendPost(mTempPost, mParent);
//     Backend::self()->runQueuedJobs();
// }

WeblogStyleGetter::WeblogStyleGetter( const int blogid, QObject *parent ): QObject( parent )
{
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
    mParent = qobject_cast<QWidget*>( parent );
    Q_ASSERT( mParent );

    // create temp post
    kDebug();
//     QStringList categories("");
    mTempPost = new BilboPost();
    mTempPost->setTitle( mPostTitle );
    mTempPost->setContent( mPostContent );
    mTempPost->setPrivate( false );
//     mTempPost = new BilboPost(blog,
//                                 mPostTitle,
//                                 mPostContent,
//                                 categories,
//                                 false,
//                                 KDateTime(QDateTime(QDate(2000,1,1),QTime(0,0)))//datetime
//                                );
    b = new Backend( blogid );
    connect( b, SIGNAL( sigPostPublished( int, BilboPost* ) ), this, 
             SLOT( sltTempPostPublished( int, BilboPost* ) ) );
    connect( b, SIGNAL( sigError( const QString& ) ), this, SLOT( sltError( const QString& ) ) );

    b->publishPost( mTempPost );

//     Backend::self()->disconnect(SIGNAL(jobsFinished()));
//     connect ( Backend::self() , SIGNAL(jobsFinished()), SLOT(slotPostSent()) );
// 
//     // send post
//     Backend::self()->sendPost(mTempPost, mParent);
//     Backend::self()->runQueuedJobs();
}

WeblogStyleGetter::~WeblogStyleGetter()
{
    kDebug();
}

// QString WeblogStyleGetter::styledHtml(const QString& blogname,
//                                       const QString &title,
//                                       const QString &content)
// {
//     kDebug();
//     QString url = QString("%1%2/index.html").arg(DIR_TO_SAVE_ENTRIES).arg(blogname);
//     QString basePath = Media::cachePath();
//     KUrl dest = KStandardDirs::locate("appdata", url);
//     kDebug() << dest << url;
//     QString buffer;
//     if ( !dest.isValid() ) {
//         return "<html><head><base href=\"" + basePath + "\" /></head><body><b>" + title.toUtf8() + "</b><br>" + content.toUtf8() + "</html>";
//     }
//     QFile file( dest.path() );
//     if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//         return "<html><body><b>" + title + "</b><br>" + content + "</html>";
//     }
//     while (!file.atEnd()) {
//         QByteArray line = file.readLine();
//         buffer.append(line);
//     }
// 
//     QRegExp titleRx(QString("%1[\\d]*").arg(POST_TITLE));
//     QRegExp contentRx(QString("%1[\\d]*").arg(POST_CONTENT));
// 
//     buffer.replace(titleRx, title.toUtf8() );
//     buffer.replace(contentRx, content.toUtf8() );
// 
//     return buffer;
// }

QString WeblogStyleGetter::styledHtml( const int blogid,
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
    kDebug() << blogDir;
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
        buffer.append( line );
    }

    QRegExp titleRx( QString( "%1[\\d]*" ).arg( POST_TITLE ) );
    QRegExp contentRx( QString( "%1[\\d]*" ).arg( POST_CONTENT ) );

    buffer.replace( titleRx, title );
    buffer.replace( contentRx, content );

    return buffer;
}

// void WeblogStyleGetter::slotPostSent()
// {
//     kDebug();
//     Backend::self()->disconnect(SIGNAL(jobsFinished()));
//     connect ( Backend::self() , SIGNAL(jobsFinished()), SLOT(slotPostFetched()) );
//     Backend::self()->fetchPost(mTempPost, mParent);
//     Backend::self()->runQueuedJobs();
// }

void WeblogStyleGetter::sltTempPostPublished( int blogId, BilboPost* post )
{
    kDebug();
    
//     Backend *b = qobject_cast< Backend* >( sender() );
    b = qobject_cast< Backend* >( sender() );
    connect( b, SIGNAL( sigPostFetched( BilboPost * ) ), this, SLOT( sltTempPostFetched( BilboPost * ) ) );
    b->fetchPost( *post );
}

void WeblogStyleGetter::sltTempPostFetched( BilboPost* post )
{
    KUrl postUrl;
    postUrl = post->permaLink();
    if ( postUrl.url().isEmpty() ) {
        kDebug() << "permalink was empty";
        kDebug() << post->permaLink().url();
        postUrl = post->link();
        if ( postUrl.url().isEmpty() ) {
            KMessageBox::error( mParent,
                            i18n( "Can not fetch style from the blog" ) );
            kDebug() << post->link().url();
            return;
        }
    }

    mTempPost = post;
    mJob = KIO::storedGet( postUrl, KIO::NoReload, KIO::HideProgressInfo );

    connect( mJob, SIGNAL( result( KJob* ) ),
             this, SLOT( sltHtmlCopied( KJob* ) ) );
    
//     sender()->deleteLater();
}

// void WeblogStyleGetter::slotPostFetched()
// {
//     kDebug();
// 
//     Backend::self()->disconnect(SIGNAL(jobsFinished()));
//     // read post url
//     KUrl src(mTempPost->link());
// 
//     kDebug() << "Retrieving" << src;
//     // copy post in the cache
//     mJob = KIO::storedGet(src, KIO::NoReload, KIO::HideProgressInfo);
// 
//     connect( mJob, SIGNAL( result( KJob* ) ),
//              this, SLOT( slotHtmlCopied( KJob* ) ) );
// }

void WeblogStyleGetter::sltHtmlCopied( KJob *job )
{
    kDebug();
    if ( job->error() ) {
        KMessageBox::error( mParent, i18n( "Impossible to get html file,%1", 
                            job->errorString() ) );
        sender()->deleteLater();
        return;
    }
    QByteArray httpData( qobject_cast<KIO::StoredTransferJob*>( job )->data() );

    // KUrl directory() hasn't worked for me, so i do it myself:
    QString href( mTempPost->permaLink().url() );
    int filenameOffset = href.lastIndexOf( "/" );
    href = href.remove( filenameOffset + 1, 255 );
    QString base( "<base href=\""+href+"\"/>" );
    kDebug() << "base: " << base;

    QRegExp rxBase( "(<base\\shref=[^>]+>)" ); // TODO check if that works
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
//         KMessageBox::error( mParent,
//                             i18n("Impossible to write data to file %1", dest.path()));
        file.close();
        return;
    }
    file.close();
//     Q_EMIT sigStyleFetched();
    

    //Remove temp post from the server.
    b->removePost( *mTempPost );
//     Backend::self()->runQueuedJobs();
    connect( b, SIGNAL( sigPostRemoved( int, const BilboPost &) ), this, 
             SLOT( sltTempPostRemoved( int, const BilboPost & ) ) );
//     delete mTempPost;
//     b->deleteLater();
    
//     Q_EMIT sigStyleFetched();
}

void WeblogStyleGetter::sltTempPostRemoved( int blog_id, const BilboPost &post)
{
    delete mTempPost;
    b->deleteLater();

    Q_EMIT sigStyleFetched();
}

void WeblogStyleGetter::generateRandomPostStrings()
{
    kDebug();
    srand( time( 0 ) );
    int postRandomNumber = rand();
    mPostTitle = QString( "%1%2" ).arg( POST_TITLE ).arg( postRandomNumber );
    mPostContent = QString( "%1%2" ).arg( POST_CONTENT ).arg( postRandomNumber );
}

void WeblogStyleGetter::sltError( const QString & errMsg )
{
    kDebug();
    QString err = i18n( "An Error occurred on latest transaction.\n%1", errMsg );
//     emit postPublishingDone( true, err );
    KMessageBox::error( mParent, err );
    b->deleteLater();
}
// } //namespace
#include "weblogstylegetter.moc"
