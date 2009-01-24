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

#include "multilinetextedit.h"
#include "global.h"

MultiLineTextEdit::MultiLineTextEdit( QWidget *parent ) : KRichTextEdit( parent )
{
//     netManager = new QNetworkAccessManager( this );
//     connect( manager, SIGNAL( finished( QNetworkReply* ) ), this, 
//              SLOT( sltReplyFinished( QNetworkReply* ) ) );
}

MultiLineTextEdit::~MultiLineTextEdit()
{
}

void MultiLineTextEdit::keyPressEvent( QKeyEvent *event )
{
    //qDebug("MultiLineTextEdit::keyPressEvent");
    int tempKey = event->key();
    if ( tempKey == Qt::Key_Return && event->modifiers() == Qt::ShiftModifier ) {
        this->textCursor().insertText( QString( QChar::LineSeparator ) );
        //qDebug() << "Enter Pressed" ;

    } else {
        //dynamic_cast <QTextEdit*>(this) ->keyPressEvent(event);
        //dynamic_cast <QWidget*>(this) ->keyPressEvent(event);
        KRichTextEdit::keyPressEvent( event );

    }
}

QVariant MultiLineTextEdit::loadResource( int type, const QUrl & name )
{
    kDebug() << "loadResource called for " << name.path();
    if ( type == QTextDocument::ImageResource ) {

        QByteArray data;
        KUrl imageUrl = KUrl( name );
        KUrl localUrl = KUrl( "file://" + __tempMediaDir + imageUrl.fileName() );
        QFile file( localUrl.toLocalFile() );
        
        if ( !file.exists() ) {
            KIO::Job*  copyJob = KIO::file_copy( imageUrl, localUrl, -1, KIO::Overwrite );
            if ( !KIO::NetAccess::synchronousRun( copyJob, 0 ) ) {
                kDebug() << "Copy job failed";
                return QVariant();
            }
        }
        
        if ( file.open( QIODevice::ReadOnly ) ) {
            data = file.readAll();
        } else {
            kDebug() << "Can not read data.";
        }
        return QVariant( data );

    } else {
        return KRichTextEdit::loadResource( type, name );
    }
}


#include <multilinetextedit.moc>
