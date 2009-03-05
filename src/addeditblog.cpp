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

#include "waitwidget.h"
#include "bilboblog.h"
#include "addeditblog.h"
#include "dbman.h"
#include "global.h"

static const int TIMEOUT = 45000;

AddEditBlog::AddEditBlog( int blog_id, QWidget *parent, Qt::WFlags flags )
        : KDialog( parent, flags )
{
    kDebug();
    wait = 0;
    mainW = new KTabWidget( this );
    ui.setupUi( mainW );
    this->setMainWidget( mainW );
    this->setWindowTitle( i18n( "Add a new blog" ) );
    isNewBlog = true;

    connect( ui.txtId, SIGNAL( textChanged( const QString& ) ), this, SLOT( enableOkButton( const QString& ) ) );
    connect( ui.txtUrl, SIGNAL( textChanged( const QString & ) ), this, SLOT( enableAutoConfBtn() ) );
    connect( ui.txtUser, SIGNAL( textChanged( const QString & ) ), this, SLOT( enableAutoConfBtn() ) );
    connect( ui.txtPass, SIGNAL( textChanged( const QString & ) ), this, SLOT( enableAutoConfBtn() ) );
    connect( ui.btnAutoConf, SIGNAL( clicked() ), this, SLOT( autoConfigure() ) );
    connect( ui.btnFetch, SIGNAL( clicked() ), this, SLOT( fetchBlogId() ) );
    connect( ui.comboApi, SIGNAL( currentIndexChanged(int) ), this, SLOT( slotComboApiChanged(int) ) );
    connect( ui.txtUrl, SIGNAL( returnPressed() ), this, SLOT( sltReturnPressed() ) );
    connect( ui.txtUser, SIGNAL( returnPressed() ), this, SLOT( sltReturnPressed() ) );
    connect( ui.txtPass, SIGNAL( returnPressed() ), this, SLOT( sltReturnPressed() ) );
    connect( ui.txtId, SIGNAL( returnPressed() ), this, SLOT( sltReturnPressed() ) );

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
        this->enableButtonOk( false );
    }

    slotComboApiChanged( ui.comboApi->currentIndex() );
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
    showWaitWidget( i18n("Trying to guess blog and API type...") );
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
        hideWaitWidget();
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
        hideWaitWidget();
        return;
    }
    QString httpData( dynamic_cast<KIO::StoredTransferJob*>( job )->data() );
    job->deleteLater();

    QRegExp rxGData( QString( "content='blogger' name='generator'" ) );
    if ( rxGData.indexIn( httpData ) != -1 ) {
        kDebug() << "content='blogger' name='generator' matched";
        mFetchAPITimer->deleteLater();
        ui.comboApi->setCurrentIndex( 4 );
        QRegExp rxBlogId( QString( "BlogID=(\\d+)" ) );
        ui.txtId->setText( rxBlogId.cap( 1 ) );
        hideWaitWidget();
        return;
    }

    QRegExp rxLiveJournal( QString( "rel=\"openid.server\" href=\"http://www.livejournal.com/openid/server.bml\"" ) );
    if ( rxLiveJournal.indexIn( httpData ) != -1 ) {
        kDebug() << " rel=\"openid.server\" href=\"http://www.livejournal.com/openid/server.bml\" matched";
        mFetchAPITimer->deleteLater();
        ui.comboApi->setCurrentIndex( 0 );
        ui.txtUrl->setText( "http://www.liverjournal.com/interface/blogger/" );
        ui.txtId->setText( ui.txtUser->text() );
        hideWaitWidget();
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
    KIO::StoredTransferJob *testXmlRpcJob = KIO::storedGet( ui.txtUrl->text() + "/xmlrpc.php",
                                                            KIO::NoReload, KIO::HideProgressInfo );

    connect( testXmlRpcJob, SIGNAL( result( KJob* ) ), this, SLOT( gotXmlRpcTest( KJob* ) ) );
}

void AddEditBlog::gotXmlRpcTest( KJob *job )
{
    kDebug();
    mFetchAPITimer->deleteLater();
    if ( !job ) return;
    if ( job->error() ) {
        kDebug() << "Auto configuration failed! Error: " << job->errorString();
        hideWaitWidget();
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
        default:
            kDebug()<<"Unknown API";
            return;
            break;
    };
    connect( mBlog, SIGNAL( error( KBlog::Blog::ErrorType, const QString& ) ),
             this, SLOT( handleFetchError( KBlog::Blog::ErrorType, const QString& ) ) );
    ui.txtId->setText( i18n( "Please wait..." ) );
    ui.txtId->setEnabled( false );
    showWaitWidget( i18n( "Fetching Blog Id..." ) );
}

void AddEditBlog::handleFetchIDTimeout()
{
    kDebug();
    ui.txtId->setText( QString() );
    ui.txtId->setEnabled( true );
    hideWaitWidget();
    KMessageBox::error( this, i18n( "Fetching the blog's id timed out. Check your internet connection,\
and your homepage Url, username or password!\nnote that the url has to contain \"http://\"\
\nIf you are using a self hosted Wordpress blog, you have to enable Remote Publishing on its configurations" ) );
}

void AddEditBlog::handleFetchAPITimeout()
{
    kDebug();
    hideWaitWidget();
    ui.txtId->setEnabled( true );
    KMessageBox::sorry( this, i18n( "Sorry, Bilbo cannot get API type automatically,\
please check your internet connection, otherwise you have to set API type on advanced tab manually." ),
                        i18n( "AutoConfiguration Failed" ) );
}

void AddEditBlog::handleFetchError( KBlog::Blog::ErrorType type, const QString & errorMsg )
{
    kDebug() << " ErrorType: " << type;
    ui.txtId->setEnabled( true );
    hideWaitWidget();
    KMessageBox::detailedError( this, i18n( "Fetching BlogID Faild!\nplease check you internet connection." ), errorMsg );
}

void AddEditBlog::fetchedBlogId( const QList< QMap < QString , QString > > & list )
{
    kDebug();
    mFetchBlogIdTimer->deleteLater();
    hideWaitWidget();
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
    Q_UNUSED(id);
    mFetchProfileIdTimer->deleteLater();
    connect( dynamic_cast<KBlog::GData*>( mBlog ), SIGNAL( listedBlogs( const QList<QMap<QString, QString> >& ) ),
             this, SLOT( fetchedBlogId( const QList<QMap<QString, QString> >& ) ) );
    connect( dynamic_cast<KBlog::GData*>( mBlog ), SIGNAL( error( KBlog::Blog::ErrorType, const QString& ) ),
             this, SLOT( handleFetchError( KBlog::Blog::ErrorType, const QString& ) ) );
    dynamic_cast<KBlog::GData*>( mBlog )->listBlogs();
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
    if(this->isButtonEnabled(KDialog::Ok)){
        this->setButtonFocus(KDialog::Ok);
    } else {
        if(mainW->currentIndex()==0){
            if(ui.btnAutoConf->isEnabled()){
                autoConfigure();
            }
        } else {
            fetchBlogId();
        }
    }
}

AddEditBlog::~AddEditBlog()
{
    kDebug();
}

void AddEditBlog::setSupportedFeatures( BilboBlog::ApiType api )
{
    QString yesStyle = "QLabel{color: green;}";
    QString yesText = i18n( "Yes" );
    QString noStyle = "QLabel{color: red;}";
    QString noText = i18n( "No, API doesn't support" );
    QString notYetText = i18n( "No, Bilbo doesn't support yet" );

    ui.featureCreatePost->setText( yesText );
    ui.featureCreatePost->setStyleSheet( yesStyle );
    ui.featureRemovePost->setText( yesText );
    ui.featureRemovePost->setStyleSheet( yesStyle );
    ui.featurRecentPosts->setText( yesText );
    ui.featurRecentPosts->setStyleSheet( yesStyle );

    ui.featureCreateCategory->setStyleSheet( noStyle );

    switch( api ) {
        case BilboBlog::BLOGGER1_API:
            ui.featureUploadMedia->setText( noText );
            ui.featureUploadMedia->setStyleSheet( noStyle );
            ui.featureCategories->setText( noText );
            ui.featureCategories->setStyleSheet( noStyle );
            ui.featureMultipagedPosts->setText( noText );
            ui.featureMultipagedPosts->setStyleSheet( noStyle );
            ui.featureCreateCategory->setText( noText );
            ui.featureTags->setText( noText );
            ui.featureTags->setStyleSheet( noStyle );
            break;
        case BilboBlog::METAWEBLOG_API:
            ui.featureUploadMedia->setText( yesText );
            ui.featureUploadMedia->setStyleSheet( yesStyle );
            ui.featureCategories->setText( noText );
            ui.featureCategories->setStyleSheet( noStyle );
            ui.featureMultipagedPosts->setText( noText );
            ui.featureMultipagedPosts->setStyleSheet( noStyle );
            ui.featureCreateCategory->setText( noText );
            ui.featureTags->setText( noText );
            ui.featureTags->setStyleSheet( noStyle );
            break;
        case BilboBlog::MOVABLETYPE_API:
            ui.featureUploadMedia->setText( yesText );
            ui.featureUploadMedia->setStyleSheet( yesStyle );
            ui.featureCategories->setText( yesText );
            ui.featureCategories->setStyleSheet( yesStyle );
            ui.featureMultipagedPosts->setText( yesText );
            ui.featureMultipagedPosts->setStyleSheet( yesStyle );
            ui.featureCreateCategory->setText( noText );
            ui.featureTags->setText( yesText );
            ui.featureTags->setStyleSheet( yesStyle );
            break;
        case BilboBlog::WORDPRESSBUGGY_API:
            ui.featureUploadMedia->setText( yesText );
            ui.featureUploadMedia->setStyleSheet( yesStyle );
            ui.featureCategories->setText( yesText );
            ui.featureCategories->setStyleSheet( yesStyle );
            ui.featureMultipagedPosts->setText( yesText );
            ui.featureMultipagedPosts->setStyleSheet( yesStyle );
            ui.featureCreateCategory->setText( notYetText );
            ui.featureTags->setText( yesText );
            ui.featureTags->setStyleSheet( yesStyle );
            break;
        case BilboBlog::GDATA_API:
            ui.featureUploadMedia->setText( noText );
            ui.featureUploadMedia->setStyleSheet( noStyle );
            ui.featureCategories->setText( noText );
            ui.featureCategories->setStyleSheet( noStyle );
            ui.featureMultipagedPosts->setText( noText );
            ui.featureMultipagedPosts->setStyleSheet( noStyle );
            ui.featureCreateCategory->setText( noText );
            ui.featureTags->setText( yesText );
            ui.featureTags->setStyleSheet( yesStyle );
            break;
    };
}

void AddEditBlog::slotComboApiChanged( int index )
{
    ///This wrapper is to change api if needed!
    setSupportedFeatures( (BilboBlog::ApiType) index );
}

void AddEditBlog::slotButtonClicked( int button )
{
    kDebug();
    if ( button == KDialog::Ok ) {
        if ( bBlog->blogid().isEmpty() && ui.txtId->text().isEmpty() ) {
            KMessageBox::sorry( this, i18n( "Sorry, BlogId not retrived yet,\
                                            \nYou have to Fetch blog id by hitting\
                                            \"Auto Configure\" Or \"Fetch ID\" button or Insert your Blog Id manually." )
                                            );
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

void AddEditBlog::showWaitWidget( QString text )
{
    ui.btnAutoConf->setEnabled( false );
    ui.btnFetch->setEnabled( false );
    if( !wait ) {
        wait = new WaitWidget(this);
        wait->setWindowModality( Qt::WindowModal );
        wait->setBusyState();
    }
    wait->setText( text );
    wait->show();
}

void AddEditBlog::hideWaitWidget()
{
    ui.btnAutoConf->setEnabled( true );
    ui.btnFetch->setEnabled( true );
    if( wait )
        wait->deleteLater();
    wait = 0;
}
#include "addeditblog.moc"
