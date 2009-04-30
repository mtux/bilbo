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

#include "composer/bilbobrowser.h"

#include <QtGui>

#include <khtml_part.h>
#include <khtmlview.h>
#include <kstatusbar.h>
#include <kmessagebox.h>
#include <kseparator.h>
#include <kpushbutton.h>
#include <kjob.h>
#include <kio/jobclasses.h>

#include "composer/stylegetter.h"
#include "global.h"

BilboBrowser::BilboBrowser( QWidget *parent ) : QWidget( parent )
{
    browserPart = new KHTMLPart( parent );
    browserPart->setStatusMessagesEnabled( true );
    browserPart->setOnlyLocalReferences( false );

    createUi();

    if ( browserPart->browserExtension() ) {
        kDebug() << "extention isn't null.";
        connect( browserPart->browserExtension(), SIGNAL( loadingProgress( int ) ), 
                browserProgress, SLOT( setValue( int ) ) );
    }
    connect( browserPart, SIGNAL( completed() ), this, SLOT( sltCompleted() ) );
    connect( browserPart, SIGNAL( canceled( const QString& ) ), this, SLOT( 
            sltCanceled( const QString& ) ) );
    connect( browserPart, SIGNAL( setStatusBarText( const QString& ) ), this,
            SLOT( sltSetStatusBarText( const QString& ) ) );
    
}

BilboBrowser::~BilboBrowser()
{
}

void BilboBrowser::createUi()
{
    btnGetStyle = new KPushButton ( this );
    btnGetStyle->setText( i18n( "Get blog style" ) );
    connect( btnGetStyle, SIGNAL( clicked( bool ) ), this, SLOT( sltGetBlogStyle() ) );

    QSpacerItem *horizontalSpacer = new QSpacerItem( 40, 20,
                    QSizePolicy::Expanding, QSizePolicy::Minimum );
    KSeparator *separator = new KSeparator( this );

    QVBoxLayout *vlayout = new QVBoxLayout( this );
    QHBoxLayout *hlayout = new QHBoxLayout( this );

    hlayout->addItem( horizontalSpacer );
    hlayout->addWidget( btnGetStyle );
    vlayout->addLayout( hlayout );
    vlayout->addWidget( separator );
    vlayout->addWidget( browserPart->view() );

    browserProgress = new QProgressBar( this );
    browserProgress->setFixedSize(120, 17);

    browserStatus = new KStatusBar( this );
    browserStatus->setFixedHeight( 20 );
    browserStatus->addPermanentWidget( browserProgress );
    vlayout->addWidget( browserStatus );
}

void BilboBrowser::setHtml( const QString& title, const QString& content )
{
    if ( browserProgress->isHidden() ) {
        browserProgress->show();
    }
    browserProgress->reset();
    browserStatus->showMessage( i18n( "loading page items..." ) );

    browserPart->begin();
    browserPart->write( StyleGetter::styledHtml( __currentBlogId, title, content ) );
    browserPart->end();
}

void BilboBrowser::sltGetBlogStyle()
{
    int blogid = __currentBlogId;
    if ( blogid < 0 ) {
        KMessageBox::information( this,
               i18n( "Please select a blog, then try again." ), 
               i18n( "Select a blog" ) );
        return;
    }

    browserPart->setStatusMessagesEnabled( false );
    browserStatus->showMessage( i18n( "Fetching blog style from the web..." ) );
    if ( browserProgress->isHidden() ) {
        browserProgress->show();
    }
    browserProgress->reset();

    StyleGetter *styleGetter = new StyleGetter( __currentBlogId, this );
    connect( styleGetter, SIGNAL( sigGetStyleProgress( int ) ), browserProgress,
            SLOT( setValue( int ) ) );
    connect( styleGetter, SIGNAL( sigStyleFetched() ), this, SLOT( sltSetBlogStyle() ) );
}

void BilboBrowser::sltSetBlogStyle()
{
    browserStatus->showMessage( i18n( "Blog style fetched." ), 2000 );
    browserPart->setStatusMessagesEnabled( true );
    Q_EMIT sigSetBlogStyle();

    if ( qobject_cast< StyleGetter* >( sender() ) ) {
        sender()->deleteLater();
    }
}

void BilboBrowser::sltCompleted()
{
    kDebug() << "actionCompleted";
    QTimer::singleShot( 1500, browserProgress, SLOT( hide() ) );
}

void BilboBrowser::sltCanceled( const QString& errMsg )
{
    if ( !errMsg.isEmpty() ) {
        KMessageBox::detailedError( this,
               i18n( "An error occurred in the latest transaction." ), errMsg );
    }
    browserStatus->showMessage( i18n( "Operation canceled." ) );
    QTimer::singleShot( 2000, browserProgress, SLOT( hide() ) );
}

void BilboBrowser::sltSetStatusBarText( const QString& text )
{
    QString statusText = text;
    statusText.remove( "<qt>" );
    browserStatus->showMessage( statusText );
}

#include "composer/bilbobrowser.moc"
