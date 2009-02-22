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

#include <QtGui>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <klineedit.h>
#include <KMessageBox>
#include "postentry.h"
#include "bilboeditor.h"
#include "bilbomedia.h"
#include "backend.h"
#include "dbman.h"
#include "global.h"
#include "sendtoblogdialog.h"
#include <qt4/QtCore/QMap>
#include <kio/job.h>
#include "settings.h"
#include <KApplication>

#define MINUTE 60000

PostEntry::PostEntry( QWidget *parent )
        : QFrame( parent )
{
    kDebug();
    createUi();
    editPostWidget = new BilboEditor( this );
    editPostWidget->setMediaList( &mMediaList );
    this->layout()->addWidget( editPostWidget );
    mTimer = new QTimer(this);
    mTimer->start(Settings::autosaveInterval() * MINUTE);
    connect( mTimer, SIGNAL(timeout()), this, SLOT(saveTemporary()) );
    connect( qApp, SIGNAL(aboutToQuit()), this, SLOT(saveTemporary()) );
    progress = 0;
    mCurrentPostBlogId = -1;
}

void PostEntry::settingsChanged()
{
    kDebug();
    mTimer->setInterval(Settings::autosaveInterval() * MINUTE);
    if(Settings::autosaveInterval())
        mTimer->start();
    else
        mTimer->stop();
}

void PostEntry::createUi()
{
    this->resize( 626, 307 );
    gridLayout = new QGridLayout( this );

    horizontalLayout = new QHBoxLayout( this );
    horizontalLayout->setSizeConstraint( QLayout::SetDefaultConstraint );

    labelTitle = new QLabel( this );
    labelTitle->setText( i18nc( "noun, the post title", "Title:" ) );
    horizontalLayout->addWidget( labelTitle );

    txtTitle = new KLineEdit( this );
    horizontalLayout->addWidget( txtTitle );
    labelTitle->setBuddy( txtTitle );
    connect( txtTitle, SIGNAL( textChanged( const QString& ) ), this,
             SLOT( sltTitleChanged( const QString& ) ) );

    gridLayout->addLayout( horizontalLayout, 0, 0, 1, 1 );

}

void PostEntry::sltTitleChanged( const QString& title )
{
//     kDebug();
    mCurrentPost.setTitle( title );
    Q_EMIT sigTitleChanged( title );
}

QString PostEntry::postTitle() const
{
    return mCurrentPost.title();
}

const QString& PostEntry::postBody()
{
    kDebug();
    const QString& str = this->editPostWidget->htmlContent();
//     if ( !mCurrentPost ) {
//         mCurrentPost = new BilboPost;
//     }
    mCurrentPost.setContent( str );
    return str;
}

void PostEntry::setPostTitle( const QString & title )
{
    kDebug();
    this->txtTitle->setText( title );
    mCurrentPost.setTitle( title );
}

void PostEntry::setPostBody( const QString & body )
{
    kDebug();
    mCurrentPost.setContent( body );
    this->editPostWidget->setHtmlContent( body );
}

int PostEntry::currentPostBlogId()
{
    return mCurrentPostBlogId;
}

void PostEntry::setCurrentPostBlogId( int blog_id )
{
    kDebug();
    mCurrentPostBlogId = blog_id;
}

BilboPost* PostEntry::currentPost()
{
    kDebug();
    mCurrentPost.setContent( QString( postBody() ) );
    return &mCurrentPost;
}

void PostEntry::setCurrentPost( const BilboPost &post )
{
    kDebug();
//     if(mCurrentPost)
//         delete mCurrentPost;
    mCurrentPost = post;
//     kDebug()<<"postId: "<<mCurrentPost.postId();
    this->setPostBody( mCurrentPost.content() );
    this->setPostTitle( mCurrentPost.title() );
}

Qt::LayoutDirection PostEntry::defaultLayoutDirection()
{
    return this->txtTitle->layoutDirection();
}

void PostEntry::setDefaultLayoutDirection( Qt::LayoutDirection direction )
{
    kDebug();
    this->editPostWidget->setLayoutDirection( direction );
    this->txtTitle->setLayoutDirection( direction );
}

PostEntry::~PostEntry()
{
    kDebug();
//     delete mCurrentPost;
}

void PostEntry::setCurrentPostProperties( const BilboPost &post )
{
    kDebug();
    mCurrentPost.setProperties( post );
}

QMap< QString, BilboMedia * > & PostEntry::mediaList()
{
    return mMediaList;
}

bool PostEntry::uploadMediaFiles()
{
    kDebug();
    bool result = false;
    int numOfFilesToBeUploaded = 0;
    Backend *b = new Backend( mCurrentPostBlogId, this );
    QMap <QString, BilboMedia*>::iterator it = mMediaList.begin();
    QMap <QString, BilboMedia*>::iterator endIt = mMediaList.end();
    for ( ; it != endIt; ++it ) {
        if( !it.value()->isUploaded() ){
//         if ( it.value()->isLocal() ) {
            result = true;
            connect( b, SIGNAL( sigMediaUploaded( BilboMedia* ) ), this, SLOT( sltMediaFileUploaded( BilboMedia* ) ) );
            connect( b, SIGNAL( sigError( const QString& ) ), this, SLOT( sltError( const QString& ) ) );
            connect( b, SIGNAL( sigMediaError( const QString&, BilboMedia* ) ),
                     this, SLOT( sltMediaError( const QString&, BilboMedia* ) ) );
            b->uploadMedia( it.value() );
            ++numOfFilesToBeUploaded;
        }
    }
    if ( result ) {
        progress = new QProgressBar( this );
        this->layout()->addWidget( progress );
        progress->setMaximum( numOfFilesToBeUploaded );
        progress->setValue( 0 );
    }
    isUploadingMediaFilesFailed = false;
    return result;
}

void PostEntry::sltMediaFileUploaded( BilboMedia * media )
{
    kDebug()<<media->name();
    progress->setValue( progress->value() + 1 );
    if ( progress->value() >= progress->maximum() ) {
        QTimer::singleShot( 800, this, SLOT( sltDeleteProgressBar() ) );
        if ( !isUploadingMediaFilesFailed ) {
            if ( editPostWidget->updateMediaPaths() ) {
                mCurrentPost.setContent( this->editPostWidget->htmlContent() );
                publishPostAfterUploadMediaFiles();
            } else {
                kDebug() << "Updateing media pathes failed!";
            }
        }
        sender()->deleteLater();
    }
}

void PostEntry::sltError( const QString & errMsg )
{
    kDebug();
    QString err = i18n( "An Error occurred on latest transaction.\n%1", errMsg );
    sltDeleteProgressBar();
    emit postPublishingDone( true, err );
    sender()->deleteLater();
}

void PostEntry::sltMediaError( const QString & errorMessage, BilboMedia * media )
{
    kDebug();
    isUploadingMediaFilesFailed = true;
    kDebug() << " AN ERROR OCCURRED ON UPLOADING,\tError message is: " << errorMessage;
    QString err = i18n( "Uploading media file %1 failed.\n%3", media->name(), media->localUrl().prettyUrl(), errorMessage);
    sltDeleteProgressBar();
    emit postPublishingDone( true, err );
    sender()->deleteLater();
}

void PostEntry::publishPost( int blogId, const BilboPost &postData )
{
    kDebug();
    bool isNew = false;
    if(mCurrentPost.status() == BilboPost::New)
        isNew = true;
    SendToBlogDialog *dia = new SendToBlogDialog( isNew, mCurrentPost.isPrivate(), this);
    dia->setAttribute(Qt::WA_DeleteOnClose, false);
    if( dia->exec() == KDialog::Accepted ) {
        mCurrentPost.setProperties( postData );
        mCurrentPostBlogId = blogId;

        if(dia->isNew())
            isNewPost = true;
        else
            isNewPost = false;

        if(dia->isPrivate())
            mCurrentPost.setPrivate(true);
        else
            mCurrentPost.setPrivate(false);

        if ( !this->uploadMediaFiles() )
            publishPostAfterUploadMediaFiles();
    }
}

void PostEntry::publishPostAfterUploadMediaFiles()
{
    kDebug();
    progress = new QProgressBar( this );
    this->layout()->addWidget( progress );
    progress->setMaximum( 0 );
    progress->setMinimum( 0 );

    Backend *b = new Backend( mCurrentPostBlogId );
    connect( b, SIGNAL( sigPostPublished( int, BilboPost* ) ), this, SLOT( sltPostPublished( int, BilboPost* ) ) );
    connect( b, SIGNAL( sigError( const QString& ) ), this, SLOT( sltError( const QString& ) ) );
    if(isNewPost)
        b->publishPost( &mCurrentPost );
    else
        b->modifyPost( &mCurrentPost );
}

void PostEntry::sltPostPublished( int blog_id, BilboPost *post )
{
    kDebug() << "BlogId: " << blog_id << "Post Id on server: " << post->postId();
    ///FIXME This DB communication is un necessary! fix it
//  BilboBlog *b = DBMan::self()->getBlogInfo(blog_id);
    QString blog_name = "NOT SET";// = b->title();
//  delete b;
    QString msg;
    mCurrentPost = (*post);
    if ( mCurrentPost.isPrivate() ) {
        msg = i18n( "Draft with title \"%1\" saved successfully.", post->title() );
    } else if(mCurrentPost.status() == BilboPost::Modified){
        msg = i18n( "Post with title \"%1\" modified successfully.", post->title() );
    } else {
        msg = i18n( "Post with title \"%1\" published successfully.", post->title() );
    }
//     KMessageBox::information( this, msg, "Successful" );
    if ( progress ) {
//         this->layout()->removeWidget( progress );
        progress->deleteLater();
    }
    emit postPublishingDone( false, msg );
    sender()->deleteLater(); //FIXME Check if this command needed or NOT -Mehrdad
}

void PostEntry::sltDeleteProgressBar()
{
    kDebug();
    if(progress){
        this->layout()->removeWidget( progress );
        progress->deleteLater();
    }
    progress = 0;
}

void PostEntry::saveLocally()
{
    kDebug();
    if(currentPost()->content().isEmpty()) {
        if( KMessageBox::warningYesNo(this, i18n("Current post content is empty, \
are you sure of saving an empty post?")) == KMessageBox::NoExec )
            return;
    }
//     QMap <QString, BilboMedia*>::const_iterator it = this->mediaList().constBegin();
//     while ( it != this->mediaList().constEnd() ) {
//         if ( !( it.value()->isUploaded() ) && !( it.key().startsWith( MEDIA_DIR ) ) ) {//"file://" + CACHED_MEDIA_DIR
//             QString baseName = it.value()->name();
//             QString desiredFileName = MEDIA_DIR + baseName;
//             int indexOfDot = baseName.indexOf('.', 0, Qt::CaseInsensitive);
//             QString newFileName = desiredFileName;
//             int i = 1;
//             while( KStandardDirs::exists( newFileName ) ){
//                 baseName.insert(indexOfDot, QString::number(i));
//                 newFileName = MEDIA_DIR + baseName;
//                 baseName = it.value()->name();
//                 ++i;
//             }
//             KIO::file_copy(it.value()->localUrl(), KUrl(newFileName));
//         }
//         ++it;
//     }
    ///^ Removed, so from now the media files aren't moved to anywhere. -Mehrdad
    mCurrentPost.setId( DBMan::self()->saveTemp_LocalEntry( mCurrentPost, mCurrentPostBlogId, DBMan::Local ) );
}

void PostEntry::saveTemporary()
{
    kDebug();
    if( !currentPost()->content().isEmpty() ) {
//         kDebug()<<"postId: "<<mCurrentPost.postId();
        mCurrentPost.setId( DBMan::self()->saveTemp_LocalEntry(mCurrentPost, mCurrentPostBlogId, DBMan::Temp) );
        kDebug()<<"Temporary saved";
    }
}

#include "postentry.moc"
