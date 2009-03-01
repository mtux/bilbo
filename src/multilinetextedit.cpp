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

//#include <QDebug>
#include <QtGui>
// #include <QNetworkAccessManager>
// #include <QNetworkRequest>
// #include <QNetworkReply>
#include <kdebug.h>
#include <kurl.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <kio/jobuidelegate.h>
#include <kmimetype.h>

#include "multilinetextedit.h"
#include "constants.h"
#include "bilbomedia.h"

QMap <QString, bool> MultiLineTextEdit::downloadFinished;
MultiLineTextEdit::MultiLineTextEdit( QWidget *parent ) : KRichTextEdit( parent )
{
//     netManager = new QNetworkAccessManager( this );
//     connect( manager, SIGNAL( finished( QNetworkReply* ) ), this, 
//              SLOT( sltReplyFinished( QNetworkReply* ) ) );
}

MultiLineTextEdit::~MultiLineTextEdit()
{
}

void MultiLineTextEdit::clearCache()
{
    downloadFinished.clear();
}

void MultiLineTextEdit::keyPressEvent( QKeyEvent *event )
{
    int tempKey = event->key();
    if ( tempKey == Qt::Key_Return && event->modifiers() == Qt::ShiftModifier ) {
        this->textCursor().insertText( QString( QChar::LineSeparator ) );

    } else {
        KRichTextEdit::keyPressEvent( event );
    }
}

QVariant MultiLineTextEdit::loadResource( int type, const QUrl & name )
{
    kDebug() << "loadResource called for " << name.path();
    if ( type == QTextDocument::ImageResource ) {

        QByteArray data;
        KUrl imageUrl = KUrl( name );
        QString imageUrlString = imageUrl.url();

        if ( name.scheme() != "file" ) {
//             KUrl localUrl = KUrl( "file://" + CACHED_MEDIA_DIR + imageUrl.fileName() );
            QString cacheFileName = name.host() + name.path();
            cacheFileName.replace( QChar( '/' ), QChar( '.' ) );
            
            KUrl localUrl = KUrl( "file://" + CACHED_MEDIA_DIR + cacheFileName );
//             QFile file( localUrl.toLocalFile() );
            QFile file( CACHED_MEDIA_DIR + cacheFileName );

            if ( !file.exists() ) {
                if ( !downloadFinished.contains( imageUrl.url() ) ) {
                    downloadFinished.insert( imageUrl.url(), false);
                    KIO::Job*  copyJob = KIO::file_copy( imageUrl, localUrl, -1, ( KIO::Overwrite | KIO::HideProgressInfo ) );

                    connect( copyJob, SIGNAL( result( KJob * ) ), this, 
                            SLOT( sltRemoteFileCopied( KJob * ) ) );
                }
                return QVariant();
            }
            if ( file.open( QIODevice::ReadOnly ) ) {
                data = file.readAll();
            } else {
                kDebug() << "Can not read data.";
            }
            
            if ( !mMediaList->contains( imageUrlString ) ) {
                BilboMedia *media = new BilboMedia();
                media->setName( imageUrl.fileName() );
                media->setRemoteUrl( imageUrlString );
                media->setLocalUrl( localUrl );
                media->setUploaded( true );
            
                KMimeType::Ptr typePtr;
                typePtr = KMimeType::findByUrl( localUrl, 0, true, false );
                media->setMimeType( typePtr.data()->name() );
                Q_EMIT sigMediaTypeFound( media );
            }

        } else {
            QFile file( imageUrl.toLocalFile() );
            
            if ( !file.exists() ) {
                kDebug() << "local file doesn't exist" ;
                return QVariant();
            }
            if ( file.open( QIODevice::ReadOnly ) ) {
                data = file.readAll();
            } else {
                kDebug() << "Can not read data.";
            }
            
            if ( !mMediaList->contains( imageUrlString ) ) {
                BilboMedia *media = new BilboMedia();
                media->setName( imageUrl.fileName() );
                media->setRemoteUrl( imageUrlString );
                media->setLocalUrl( imageUrl ); //NOTE may be omitted later.
                media->setUploaded( false );
                
                KMimeType::Ptr typePtr;
                typePtr = KMimeType::findByUrl( imageUrl, 0, true, false );
                media->setMimeType( typePtr.data()->name() );
                Q_EMIT sigMediaTypeFound( media );
            }
        }

        return QVariant( data );

    } else {
        return KRichTextEdit::loadResource( type, name );
    }
}


void MultiLineTextEdit::sltRemoteFileCopied( KJob * job )
{
    KIO::FileCopyJob *copyJob = dynamic_cast <KIO::FileCopyJob*>( job );
    
    if ( job->error() ) {
        copyJob->ui()->setWindow( this );
        copyJob->ui()->showErrorMessage();
    } else {
        QString srcPath = copyJob->srcUrl().url();
        
        if ( !mMediaList->contains( srcPath ) ) {
            BilboMedia *media = new BilboMedia();
            media->setName( copyJob->srcUrl().fileName() );
            media->setRemoteUrl( srcPath );
            media->setLocalUrl( copyJob->destUrl() ); //NOTE may be omitted later.
            media->setUploaded( true );
            
            KMimeType::Ptr typePtr;
            typePtr = KMimeType::findByUrl( copyJob->destUrl(), 0, true, false );
            media->setMimeType( typePtr.data()->name() );
            Q_EMIT sigMediaTypeFound( media );
        } else {
            mMediaList->value( srcPath )->setLocalUrl( copyJob->destUrl() );
        }
        
        downloadFinished[ srcPath ] = true;
        Q_EMIT sigRemoteImageArrived( copyJob->srcUrl() );
        kDebug() << srcPath << " arrived.";
    }
}

void MultiLineTextEdit::setMediaList( QMap <QString, BilboMedia*> * list )
{
    mMediaList = list;
}
// GetImageThread::GetImageThread( KRichTextEdit *parent, const KUrl & image ) : QThread( parent )
// {
//     cursor = parent->textCursor();
//     imageUrl = image;
// }
// 
// GetImageThread::run()
// {
//     KUrl localUrl = KUrl( "file://" + __tempMediaDir + imageUrl.fileName() );
//     KIO::Job*  copyJob = KIO::file_copy( imageUrl, localUrl, -1, KIO::Overwrite | KIO::HideProgressInfo );
//     if ( !KIO::NetAccess::synchronousRun( copyJob, 0 ) ) {
//         kDebug() << "Copy job failed";
//         return;
//     }
//     QTextImageFormat f;
//     f.setImageName( localUrl.url() );
//     cursor
// }

#include <multilinetextedit.moc>
