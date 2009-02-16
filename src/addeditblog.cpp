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


#include <gdata.h>
#include <blogger1.h>
#include <metaweblog.h>
#include <movabletype.h>
#include <wordpressbuggy.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kio/jobclasses.h>
#include <kio/job.h>

#include "bilboblog.h"
#include "addeditblog.h"
#include "dbman.h"
#include "global.h"

#define TIMEOUT 45000

AddEditBlog::AddEditBlog( int blog_id, QWidget *parent, Qt::WFlags flags )
        : KDialog( parent, flags )
{
    kDebug();
    mainW = new KTabWidget( this );
    ui.setupUi( mainW );
    this->setMainWidget( mainW );
    this->setWindowTitle( i18n( "Add a new blog" ) );
    isNewBlog = true;
    if ( blog_id > -1 ) {
        this->setWindowTitle( i18n( "Edit Blog Settings" ) );
        this->enableButtonOk( true );
        ui.btnFetch->setEnabled( true );
        ui.btnAutoConf->setEnabled( true );
        isNewBlog = false;
        bBlog = new BilboBlog( DBMan::self()->getBlogInfo( blog_id ) );
        ui.txtUrl->setText( bBlog->url().url() );
        ui.txtUser->setText( bBlog->username() );
        ui.txtPass->setText( bBlog->password() );
        ui.txtId->setText( bBlog->blogid() );
        ui.lblTitle->setText( bBlog->title() );
        ui.comboApi->setCurrentIndex( bBlog->api() );
        ui.comboDir->setCurrentIndex( bBlog->direction() );
    } else {
        bBlog = new BilboBlog( this );
        bBlog->setBlogId( 0 );
    }
    connect( ui.txtId, SIGNAL( textChanged( const QString& ) ), this, SLOT( enableOkButton( const QString& ) ) );
    connect( ui.txtUrl, SIGNAL( textChanged( const QString & ) ), this, SLOT( enableAutoConfBtn() ) );
    connect( ui.txtUser, SIGNAL( textChanged( const QString & ) ), this, SLOT( enableAutoConfBtn() ) );
    connect( ui.txtPass, SIGNAL( textChanged( const QString & ) ), this, SLOT( enableAutoConfBtn() ) );
    connect( ui.btnAutoConf, SIGNAL( clicked() ), this, SLOT( autoConfigure() ) );
    connect( ui.btnFetch, SIGNAL( clicked() ), this, SLOT( fetchBlogId() ) );
//     connect(this, SIGNAL(accepted()), this, SLOT(sltAccepted()));
    connect( this, SIGNAL( rejected() ), this, SLOT( sltRejected() ) );
    connect( ui.txtUrl, SIGNAL( returnPressed() ), this, SLOT( sltReturnPressed() ) );
    connect( ui.txtUser, SIGNAL( returnPressed() ), this, SLOT( sltReturnPressed() ) );
    connect( ui.txtPass, SIGNAL( returnPressed() ), this, SLOT( sltReturnPressed() ) );
    connect( ui.txtId, SIGNAL( returnPressed() ), this, SLOT( sltReturnPressed() ) );

    ui.txtUrl->setFocus();
}

void AddEditBlog::enableAutoConfBtn()
{
    if ( ui.txtUrl->text().isEmpty() || ui.txtUser->text().isEmpty() || ui.txtPass->text().isEmpty() ) {
        ui.btnAutoConf->setEnabled( false );
        ui.btnFetch->setEnabled( false );
    } else {
        ui.btnAutoConf->setEnabled( true );
        ui.btnFetch->setEnabled( true );
    }
}

void AddEditBlog::autoConfigure()
{
    kDebug();
    if ( ui.txtUrl->text().isEmpty() || ui.txtUser->text().isEmpty() || ui.txtPass->text().isEmpty() ) {
        kDebug() << "Username, Password or Url doesn't set!";
        KMessageBox::sorry( this, i18n( "You have to set the username, password and url of your blog or website." ),
                            i18n( "Incomplete fields" ) );
        return;
    }
    ui.btnAutoConf->setEnabled( false );
    ui.btnFetch->setEnabled( false );

    ///Guess API with Url:
    if ( ui.txtUrl->text().contains( "xmlrpc.php", Qt::CaseInsensitive ) ) {
        ui.comboApi->setCurrentIndex( 3 );
        fetchBlogId();
        return;
    }
    if ( ui.txtUrl->text().contains( "blogspot", Qt::CaseInsensitive ) ) {
        ui.comboApi->setCurrentIndex( 4 );
        fetchBlogId();
        return;
    }
    if ( ui.txtUrl->text().contains( "wordpress", Qt::CaseInsensitive ) ) {
        ui.comboApi->setCurrentIndex( 3 );
        ui.txtUrl->setText( ui.txtUrl->text() + "/xmlrpc.php" );
        fetchBlogId();
        return;
    }
    if ( ui.txtUrl->text().contains( "livejournal", Qt::CaseInsensitive ) ) {
        ui.comboApi->setCurrentIndex( 0 );
        ui.txtUrl->setText( "http://www.livejournal.com/interface/blogger/" );
        ui.txtId->setText( ui.txtUser->text() );
        ui.btnAutoConf->setEnabled( true );
        ui.btnFetch->setEnabled( true );
        return;
    }
    kDebug() << "Trying to guess API type by Homepage contents";
    KIO::StoredTransferJob *httpGetJob = KIO::storedGet( ui.txtUrl->text() , KIO::NoReload, KIO::HideProgressInfo );
    connect( httpGetJob, SIGNAL( result( KJob* ) ), this, SLOT( gotHtml( KJob* ) ) );
    mFetchAPITimer = new QTimer( this );
    mFetchAPITimer->setSingleShot( true );
    connect( mFetchAPITimer, SIGNAL( timeout() ), this, SLOT( handleFetchAPITimeout() ) );
    mFetchAPITimer->start( TIMEOUT );
}

void AddEditBlog::gotHtml( KJob *job )
{
    kDebug();
    if ( !job ) return;
    if ( job->error() ) {
        kDebug() << "Auto configuration failed! Error: " << job->errorString();
        ui.btnAutoConf->setEnabled( true );
        ui.btnFetch->setEnabled( true );
        return;
    }
    QString httpData( dynamic_cast<KIO::StoredTransferJob*>( job )->data() );
    delete job;

    QRegExp rxGData( QString( "content='blogger' name='generator'" ) );
    if ( rxGData.indexIn( httpData ) != -1 ) {
        kDebug() << "content='blogger' name='generator' matched";
        mFetchAPITimer->deleteLater();
        ui.comboApi->setCurrentIndex( 4 );
        QRegExp rxBlogId( QString( "BlogID=(\\d+)" ) );
        ui.txtId->setText( rxBlogId.cap( 1 ) );
        enableAutoConfBtn();
        return;
    }

    QRegExp rxLiveJournal( QString( "rel=\"openid.server\" href=\"http://www.livejournal.com/openid/server.bml\"" ) );
    if ( rxLiveJournal.indexIn( httpData ) != -1 ) {
        kDebug() << " rel=\"openid.server\" href=\"http://www.livejournal.com/openid/server.bml\" matched";
        mFetchAPITimer->deleteLater();
        ui.comboApi->setCurrentIndex( 0 );
        ui.txtUrl->setText( "http://www.liverjournal.com/interface/blogger/" );
        ui.txtId->setText( ui.txtUser->text() );
        enableAutoConfBtn();
        return;
    }

    QRegExp rxWordpress( QString( "name=\"generator\" content=\"WordPress" ) );
    if ( rxWordpress.indexIn( httpData ) != -1 ) {
        kDebug() << "name=\"generator\" content=\"WordPress matched";
        mFetchAPITimer->deleteLater();
        ui.comboApi->setCurrentIndex( 3 );
        ui.txtUrl->setText( ui.txtUrl->text() + "/xmlrpc.php" );
        fetchBlogId();
        return;
    }

    // add MT for WordpressBuggy -> URL/xmlrpc.php exists
    KIO::StoredTransferJob *testXmlRpcJob = KIO::storedGet( ui.txtUrl->text() + "/xmlrpc.php", KIO::NoReload, KIO::HideProgressInfo );

    connect( testXmlRpcJob, SIGNAL( result( KJob* ) ), this, SLOT( gotXmlRpcTest( KJob* ) ) );
}

void AddEditBlog::gotXmlRpcTest( KJob *job )
{
    kDebug();
    mFetchAPITimer->deleteLater();
    if ( !job ) return;
    if ( job->error() ) {
        kDebug() << "Auto configuration failed! Error: " << job->errorString();
        ui.btnFetch->setEnabled( true );
        ui.btnAutoConf->setEnabled( true );
        return;
    }

    ui.comboApi->setCurrentIndex( 3 );
    ui.txtUrl->setText( ui.txtUrl->text() + "/xmlrpc.php" );
    fetchBlogId();
}

void AddEditBlog::fetchBlogId()
{
    kDebug() << ui.comboApi->currentIndex();

    switch ( ui.comboApi->currentIndex() ) {
        case 0:
        case 1:
        case 2:
        case 3:
            mBlog = new KBlog::Blogger1( KUrl( ui.txtUrl->text() ), this );
            dynamic_cast<KBlog::Blogger1*>( mBlog )->setUsername( ui.txtUser->text() );
            dynamic_cast<KBlog::Blogger1*>( mBlog )->setPassword( ui.txtPass->text() );
            connect( dynamic_cast<KBlog::Blogger1*>( mBlog ) , SIGNAL( listedBlogs( const QList<QMap<QString, QString> >& ) ),
                     this, SLOT( fetchedBlogId( const QList<QMap<QString, QString> >& ) ) );
            connect( dynamic_cast<KBlog::Blogger1*>( mBlog ), SIGNAL( error( KBlog::Blog::ErrorType, const QString& ) ),
                     this, SLOT( handleFetchError( KBlog::Blog::ErrorType, const QString& ) ) );
            mFetchBlogIdTimer = new QTimer( this );
            mFetchBlogIdTimer->setSingleShot( true );
            connect( mFetchBlogIdTimer, SIGNAL( timeout() ), this, SLOT( handleFetchIDTimeout() ) );
            mFetchBlogIdTimer->start( TIMEOUT );
            dynamic_cast<KBlog::Blogger1*>( mBlog )->listBlogs();
            break;

        case 4:
            mBlog = new KBlog::GData( ui.txtUrl->text() , this );
            dynamic_cast<KBlog::GData*>( mBlog )->setUsername( ui.txtUser->text() );
            dynamic_cast<KBlog::GData*>( mBlog )->setPassword( ui.txtPass->text() );
            connect( dynamic_cast<KBlog::GData*>( mBlog ), SIGNAL( fetchedProfileId( const QString& ) ),
                     this, SLOT( fetchedProfileId( const QString& ) ) );
            dynamic_cast<KBlog::GData*>( mBlog )->fetchProfileId();
            mFetchProfileIdTimer = new QTimer( this );
            mFetchProfileIdTimer->setSingleShot( true );
            connect( mFetchProfileIdTimer, SIGNAL( timeout() ), this, SLOT( handleFetchIDTimeout() ) );
            mFetchProfileIdTimer->start( TIMEOUT );
            break;
    };
    ui.txtId->setText( i18n( "Please wait..." ) );
    ui.txtId->setEnabled( false );
    ui.btnFetch->setEnabled( false );
    ui.btnAutoConf->setEnabled( false );
}

void AddEditBlog::handleFetchIDTimeout()
{
    kDebug();
    KMessageBox::error( this, i18n( "Fetching the blog's id timed out. Check your internet connection,\
                                    and your homepage Url, username or password!\nnote that the url has to contain \"http://\" or ...\
                                    \nfor example: http://bilbo.sf.net/xmlrpc.php is a good url" ) );
    ui.txtId->setText( QString() );
    ui.txtId->setEnabled( true );
    ui.btnFetch->setEnabled( true );
    ui.btnAutoConf->setEnabled( true );
}

void AddEditBlog::handleFetchAPITimeout()
{
    kDebug();
    KMessageBox::sorry( this, i18n( "Sorry, Bilbo cannot get API type automatically,\
                                    please check your internet connection, otherwise you have to set API type on advanced tab handy." ),
                        i18n( "AutoConfiguration Failed" ) );
    ui.txtId->setEnabled( true );
    ui.btnFetch->setEnabled( true );
    ui.btnAutoConf->setEnabled( true );
//  delete mBlog;
//  delete mFetchAPITimer;
}

void AddEditBlog::handleFetchError( KBlog::Blog::ErrorType type, const QString & errorMsg )
{
    kDebug() << " ErrorType: " << type;
    KMessageBox::detailedError( this, i18n( "Fetching BlogID Faild!\nplease check you internet connection." ), errorMsg );
    ui.txtId->setEnabled( true );
    ui.btnFetch->setEnabled( true );
    ui.btnAutoConf->setEnabled( true );
//  delete mBlog;
}

void AddEditBlog::fetchedBlogId( const QList< QMap < QString , QString > > & list )
{
    kDebug();
    mFetchBlogIdTimer->deleteLater();
    if ( list.count() > 1 ) {
        ///TODO handle more than one blog!
        kDebug() << "User has more than ONE blog!";
    }
    ui.txtId->setText( list.first().values().first() );
    ui.lblTitle->setText( list.first().values().last() );
    ui.txtId->setEnabled( true );
    ui.btnFetch->setEnabled( true );
    ui.btnAutoConf->setEnabled( true );

    bBlog->setUrl( QUrl( ui.txtUrl->text() ) );
    bBlog->setUsername( ui.txtUser->text() );
    bBlog->setPassword( ui.txtPass->text() );
    bBlog->setBlogId( ui.txtId->text() );
    bBlog->setTitle( list.first().values().last() );
    this->enableButtonOk( true );
}

void AddEditBlog::fetchedProfileId( const QString &id )
{
    kDebug();
    mFetchProfileIdTimer->deleteLater();
    connect( dynamic_cast<KBlog::GData*>( mBlog ), SIGNAL( listedBlogs( const QList<QMap<QString, QString> >& ) ),
             this, SLOT( fetchedBlogId( const QList<QMap<QString, QString> >& ) ) );
    connect( dynamic_cast<KBlog::GData*>( mBlog ), SIGNAL( error( KBlog::Blog::ErrorType, const QString& ) ),
             this, SLOT( handleFetchError( KBlog::Blog::ErrorType, const QString& ) ) );
    dynamic_cast<KBlog::GData*>( mBlog )->listBlogs();
}

void AddEditBlog::sltAccepted()
{
    ///FIXME Remove this Function, its replaced by slotButtonClicked , and we don't need him any more! !maybe! ;)
    kDebug();
    if ( bBlog->blogid().isEmpty() && ui.txtId->text().isEmpty() ) {
        KMessageBox::sorry( this, i18n( "Sorry, BlogId not retrived yet,\
                                        \nYou have to Fetch blog id by hitting\
                                        \"Auto Configure\" Or \"Fetch ID\" button or Insert your Blog Id manually." ) );
        return;
    }
    bBlog->setApi(( BilboBlog::ApiType )ui.comboApi->currentIndex() );
    bBlog->setDirection(( Qt::LayoutDirection )ui.comboDir->currentIndex() );

//  if(bBlog->password().isEmpty())
    bBlog->setPassword( ui.txtPass->text() );
//  if(bBlog->username().isEmpty())
    bBlog->setUsername( ui.txtUser->text() );
//  if(bBlog->blogid().isEmpty())
    bBlog->setBlogId( ui.txtId->text() );
//  if(bBlog->url().isEmpty())
    bBlog->setUrl( KUrl( ui.txtUrl->text() ) );

    KUrl url( bBlog->url() );
    QString blogDir = DATA_DIR + url.host().replace( '/', '_' );

    if ( isNewBlog ) {
        if ( KStandardDirs::makeDir( blogDir ) ) {
            bBlog->setLocalDirectory( blogDir );
        } else {
            kDebug() << blogDir << " can't be created, as blogDir";
            return;
        }
        int blog_id = DBMan::self()->addBlog( *bBlog );
        bBlog->setId( blog_id );
        if ( blog_id != -1 ) {
            kDebug() << "Emitting sigBlogAdded() ...";
            Q_EMIT sigBlogAdded( *bBlog );
        }
    } else {
        QDir dir = QDir( bBlog->localDirectory() );
        if ( dir.rename( dir.dirName(), url.host().replace( '/', '_' ) ) ) {
            bBlog->setLocalDirectory( blogDir );
        } else {
            kDebug() << "current blog directory can't be renamed to " << blogDir;
        }
        if ( DBMan::self()->editBlog( *bBlog ) ) {
            kDebug() << "Emitting sigBlogEdited() ...";
            Q_EMIT sigBlogEdited( *bBlog );
        } else {
            kDebug() << "Cannot edit blog with id " << bBlog->id();
        }
    }
}

void AddEditBlog::enableOkButton( const QString & txt )
{
    if ( txt.isEmpty() )
        this->enableButtonOk( false );
    else
        this->enableButtonOk( true );
}

void AddEditBlog::sltReturnPressed()
{
    ///FIXME This function commented temporarilly! check its functionality! and uncomment it!
//     if(this->isButtonEnabled(KDialog::Ok)){
//         this->setButtonFocus(KDialog::Ok);
//     } else {
//         if(mainW->currentIndex()==0){
//             if(ui.btnAutoConf->isEnabled()){
//                 autoConfigure();
//             }
//         } else {
//             fetchBlogId();
//         }
//     }
}

void AddEditBlog::sltRejected()
{
    kDebug();
}

AddEditBlog::~AddEditBlog()
{
    kDebug();
}

void AddEditBlog::slotButtonClicked( int button )
{
    kDebug();
    if ( button == KDialog::Ok ) {
        if ( bBlog->blogid().isEmpty() && ui.txtId->text().isEmpty() ) {
            KMessageBox::sorry( this, i18n( "Sorry, BlogId not retrived yet,\
                                            \nYou have to Fetch blog id by hitting\
                                            \"Auto Configure\" Or \"Fetch ID\" button or Insert your Blog Id manually." ) );
            return;
        }
        bBlog->setApi(( BilboBlog::ApiType )ui.comboApi->currentIndex() );
        bBlog->setDirection(( Qt::LayoutDirection )ui.comboDir->currentIndex() );

        //  if(bBlog->password().isEmpty())
        bBlog->setPassword( ui.txtPass->text() );
        //  if(bBlog->username().isEmpty())
        bBlog->setUsername( ui.txtUser->text() );
        //  if(bBlog->blogid().isEmpty())
        bBlog->setBlogId( ui.txtId->text() );
        //  if(bBlog->url().isEmpty())
        bBlog->setUrl( KUrl( ui.txtUrl->text() ) );

        KUrl url( bBlog->url() );
        QString blogDir = DATA_DIR + "/media/";

        if ( isNewBlog ) {
            if( !KStandardDirs::exists( blogDir ) ) {
                if ( KStandardDirs::makeDir( blogDir ) ) {
                } else {
                    kDebug() << blogDir << " can't be created, as blogDir";
                    KMessageBox::error(this, i18n( "Cannot create directory %1,\
for storing media files for locally stored posts\nPlease check permissions or create it manually.",
                                                  blogDir));
                    return;
                }
                bBlog->setLocalDirectory( blogDir );
            }
            int blog_id = DBMan::self()->addBlog( *bBlog );
            bBlog->setId( blog_id );
            if ( blog_id != -1 ) {
                kDebug() << "Emitting sigBlogAdded() ...";
                Q_EMIT sigBlogAdded( *bBlog );
            }
        } else {
//             QDir dir = QDir( bBlog->localDirectory() );
//             if ( dir.rename( dir.dirName(), url.host().replace( '/', '_' ) ) ) {
                bBlog->setLocalDirectory( blogDir );
//             } else {
//                 kDebug() << "current blog directory can't be renamed to " << blogDir;
//             }
            if ( DBMan::self()->editBlog( *bBlog ) ) {
                kDebug() << "Emitting sigBlogEdited() ...";
                Q_EMIT sigBlogEdited( *bBlog );
            } else {
                kDebug() << "Cannot edit blog with id " << bBlog->id();
            }
        }
        accept();
    } else
        KDialog::slotButtonClicked( button );
}

#include "addeditblog.moc"
