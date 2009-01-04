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


#include <kblog/gdata.h>
#include <kblog/blogger1.h>
#include <kblog/metaweblog.h>
#include <kblog/movabletype.h>
#include <kblog/wordpressbuggy.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "bilboblog.h"
#include "addeditblog.h"
#include "global.h"

#define TIMEOUT 40000

AddEditBlog::AddEditBlog(int blog_id, QWidget *parent)
    :QDialog(parent)
{
	kDebug();
	setupUi(this);
	this->setWindowTitle(i18n("Add a new blog"));
	isNewBlog=true;
	if(blog_id>-1){
		this->setWindowTitle(i18n("Edit Blog Settings"));
		btnOk->setEnabled(true);
		btnFetch->setEnabled(true);
		btnAutoConf->setEnabled(true);
		isNewBlog=false;
        bBlog = __db->getBlogInfo(blog_id);
		//txtUrl->setText(bBlog->url().toString());
		txtUrl->setText(bBlog->url().url());
		txtUser->setText(bBlog->username());
		txtPass->setText(bBlog->password());
		txtId->setText(bBlog->blogid());
		lblTitle->setText(bBlog->title());
		comboApi->setCurrentIndex(bBlog->api());
		comboDir->setCurrentIndex(bBlog->direction());
	} else {
        bBlog = new BilboBlog(this);
        bBlog->setBlogId(0);
    }
	connect(txtId, SIGNAL(textChanged( const QString& )), this, SLOT(enableOkButton(const QString&)));
	connect(txtUrl, SIGNAL(textChanged(const QString &)), this, SLOT(enableAutoConfBtn()));
	connect(txtUser, SIGNAL(textChanged(const QString &)), this, SLOT(enableAutoConfBtn()));
	connect(txtPass, SIGNAL(textChanged(const QString &)), this, SLOT(enableAutoConfBtn()));
	connect(btnAutoConf, SIGNAL(clicked()), this, SLOT(autoConfigure()));
	connect(btnFetch, SIGNAL(clicked()), this, SLOT(fetchBlogId()));
	connect(this, SIGNAL(accepted()), this, SLOT(sltAccepted()));
    connect(this, SIGNAL(rejected()), this, SLOT(sltRejected()));
    connect(txtUrl, SIGNAL(returnPressed()), this, SLOT(sltReturnPressed()));
    connect(txtUser, SIGNAL(returnPressed()), this, SLOT(sltReturnPressed()));
    connect(txtPass, SIGNAL(returnPressed()), this, SLOT(sltReturnPressed()));
    connect(txtId, SIGNAL(returnPressed()), this, SLOT(sltReturnPressed()));
	
    btnOk->setIcon(KIcon("dialog-ok-apply"));
    btnCancel->setIcon(KIcon("dialog-cancel"));
    
	txtUrl->setFocus();
}

void AddEditBlog::enableAutoConfBtn()
{
	if(txtUrl->text().isEmpty() || txtUser->text().isEmpty() || txtPass->text().isEmpty()){
		btnAutoConf->setEnabled(false);
		btnFetch->setEnabled(false);
	} else{
		btnAutoConf->setEnabled(true);
		btnFetch->setEnabled(true);
	}
}

void AddEditBlog::autoConfigure()
{
	kDebug();
	if(txtUrl->text().isEmpty() || txtUser->text().isEmpty() || txtPass->text().isEmpty()){
		kDebug()<<"Username, Password or Url doesn't set!";
		KMessageBox::sorry(this, i18n("You have to set the username, password and url of your blog or website."), i18n("Incomplete fields"));
		return;
	}
	btnAutoConf->setEnabled(false);
	btnFetch->setEnabled(false);
	
	///Guess API with Url:
	if(txtUrl->text().contains("xmlrpc.php", Qt::CaseInsensitive)){
		comboApi->setCurrentIndex(3);
		fetchBlogId();
		return;
	}
	if(txtUrl->text().contains("blogspot", Qt::CaseInsensitive)){
		comboApi->setCurrentIndex(4);
		fetchBlogId();
		return;
	}
	if(txtUrl->text().contains("wordpress", Qt::CaseInsensitive)){
		comboApi->setCurrentIndex(3);
		txtUrl->setText(txtUrl->text() + "/xmlrpc.php");
		fetchBlogId();
		return;
	}
	if(txtUrl->text().contains("livejournal", Qt::CaseInsensitive)){
		comboApi->setCurrentIndex(0);
		txtUrl->setText("http://www.livejournal.com/interface/blogger/");
		txtId->setText(txtUser->text());
		btnAutoConf->setEnabled(true);
		btnFetch->setEnabled(true);
		return;
	}
	//TODO Otherwise try to get the api from the html
}

void AddEditBlog::fetchBlogId()
{
	kDebug();

	switch( comboApi->currentIndex() ){
		case 0:
		case 1:
		case 2:
		case 3:
			mBlog = new KBlog::Blogger1(KUrl(txtUrl->text()), this);
			dynamic_cast<KBlog::Blogger1*>(mBlog)->setUsername(txtUser->text());
			dynamic_cast<KBlog::Blogger1*>(mBlog)->setPassword(txtPass->text());
			connect( dynamic_cast<KBlog::Blogger1*>(mBlog) , SIGNAL(listedBlogs( const QList<QMap<QString, QString> >&)), this, SLOT(fetchedBlogId( const QList<QMap<QString, QString> >&)));
// 			connect( dynamic_cast<KBlog::Blogger1*>(mBlog), SIGNAL(fetchedProfileId( const QString& ) ), this, SLOT( fetchedProfileId( const QString& ) ) );
			connect( dynamic_cast<KBlog::Blogger1*>(mBlog), SIGNAL(error( KBlog::Blog::ErrorType, const QString& ) ), this, SLOT( handleFetchError( KBlog::Blog::ErrorType, const QString& ) ) );
			mFetchBlogIdTimer = new QTimer(this);
			mFetchBlogIdTimer->setSingleShot(true);
			connect( mFetchBlogIdTimer, SIGNAL( timeout() ), this, SLOT( handleFetchIDTimeout() ) );
			mFetchBlogIdTimer->start(TIMEOUT);
			dynamic_cast<KBlog::Blogger1*>(mBlog)->listBlogs();
			break;
			
		case 4:
			mBlog = new KBlog::GData( txtUrl->text() , this );
			dynamic_cast<KBlog::GData*>(mBlog)->setUsername( txtUser->text() );
			dynamic_cast<KBlog::GData*>(mBlog)->setPassword( txtPass->text() );
			connect( dynamic_cast<KBlog::GData*>(mBlog), SIGNAL(fetchedProfileId( const QString& ) ),
					 this, SLOT( fetchedProfileId( const QString& ) ) );
			dynamic_cast<KBlog::GData*>(mBlog)->fetchProfileId();
			mFetchProfileIdTimer = new QTimer(this);
			mFetchProfileIdTimer->setSingleShot(true);
			connect( mFetchProfileIdTimer, SIGNAL( timeout() ), this, SLOT( handleFetchIDTimeout() ) );
			mFetchProfileIdTimer->start(TIMEOUT);
			break;
	};
	txtId->setText(i18n("Please wait..."));
	txtId->setEnabled(false);
	btnFetch->setEnabled(false);
	btnAutoConf->setEnabled(false);
}

void AddEditBlog::handleFetchIDTimeout()
{
	kDebug();
	KMessageBox::error(this, i18n("Fetching the blog's id timed out. Check your internet connection, Or your homepage Url!\nnote that the url has to contain \"http://\" or ...\nfor example: http://bilbo.sf.net/xmlrpc.php is a good url"));
	txtId->setText(QString());
	txtId->setEnabled(true);
	btnFetch->setEnabled(true);
	btnAutoConf->setEnabled(true);
// 	delete mBlog;
// 	delete mFetchProfileIdTimer;
// 	delete mFetchBlogIdTimer;
}

void AddEditBlog::handleFetchAPITimeout()
{
	kDebug();
    KMessageBox::sorry(this, i18n("Sorry, Bilbo cannot get API type automatically, please check your internet connection, otherwise you have to set API type on advanced tab handy."), i18n("AutoConfiguration Failed"));
	txtId->setEnabled(true);
	btnFetch->setEnabled(true);
	btnAutoConf->setEnabled(true);
// 	delete mBlog;
// 	delete mFetchAPITimer;
}

void AddEditBlog::handleFetchError(KBlog::Blog::ErrorType type, const QString & errorMsg)
{
	kDebug()<<" ErrorType: "<< type;
	KMessageBox::detailedError(this, i18n("Fetching BlogID Faild!\nplease check you internet connection."), errorMsg);
	txtId->setEnabled(true);
	btnFetch->setEnabled(true);
	btnAutoConf->setEnabled(true);
// 	delete mBlog;
}

void AddEditBlog::fetchedBlogId(const QList< QMap < QString , QString > > & list)
{
	kDebug();
// 	delete mFetchBlogIdTimer;
	if(list.count()>1){
		///TODO: handle more than one blog!
		kDebug()<<"User has more than ONE blog!";
	}
	txtId->setText(list.first().values().first());
	lblTitle->setText(list.first().values().last());
	txtId->setEnabled(true);
	btnFetch->setEnabled(true);
	btnAutoConf->setEnabled(true);
	
	bBlog->setUrl(QUrl(txtUrl->text()));
	bBlog->setUsername(txtUser->text());
	bBlog->setPassword(txtPass->text());
	bBlog->setBlogId(txtId->text());
	bBlog->setTitle(list.first().values().last());
	btnOk->setEnabled(true);
}

void AddEditBlog::fetchedProfileId(const QString &id)
{
	kDebug();
// 	delete mFetchProfileIdTimer;
	connect( dynamic_cast<KBlog::GData*>(mBlog), SIGNAL(listedBlogs( const QList<QMap<QString, QString> >&)),
			 this, SLOT(fetchedBlogId( const QList<QMap<QString, QString> >&)));
	connect( dynamic_cast<KBlog::GData*>(mBlog), SIGNAL(error( KBlog::Blog::ErrorType, const QString& ) ),
			 this, SLOT( handleFetchError( KBlog::Blog::ErrorType, const QString& ) ) );
	dynamic_cast<KBlog::GData*>(mBlog)->listBlogs();
}

void AddEditBlog::sltAccepted()
{
	kDebug();
	if(bBlog->blogid().isEmpty() && txtId->text().isEmpty()){
		KMessageBox::sorry(this, i18n("Sorry, BlogId not retrived yet,\nYou have to Fetch blog id by hitting \"Auto Configure\" Or \"Fetch ID\" button or Insert your Blog Id manually."));
		return;
	}
	bBlog->setApi((BilboBlog::ApiType)comboApi->currentIndex());
	bBlog->setDirection((Qt::LayoutDirection)comboDir->currentIndex());
	
	if(bBlog->password().isEmpty())
		bBlog->setPassword(txtPass->text());
	if(bBlog->username().isEmpty())
		bBlog->setUsername(txtUser->text());
	if(bBlog->blogid().isEmpty())
		bBlog->setBlogId(txtId->text());
	if(bBlog->url().isEmpty())
		bBlog->setUrl(QUrl(txtUrl->text()));
	
	KUrl url = KUrl(bBlog->url());
	QString blogDir = DATA_DIR + url.host().replace('/', '_');
	
	if(isNewBlog){
		
		if (KStandardDirs::makeDir(blogDir)) {
			bBlog->setLocalDirectory(blogDir);
		} else {
			kDebug() << blogDir << " can't be created, as blogDir";
			return;
		}
		
        int blog_id = __db->addBlog(*bBlog);
		bBlog->setId(blog_id);
		if(blog_id!=-1)
			Q_EMIT sigBlogAdded(*bBlog);
	} else{
		
		QDir dir = QDir(bBlog->localDirectory());
		if (dir.rename(dir.dirName(), url.host().replace('/', '_'))) {
			bBlog->setLocalDirectory(blogDir);
		} else {
			kDebug() << "current blog directory can't be renamed to " << blogDir;
			return;
		}
        if(__db->editBlog(*bBlog))
			Q_EMIT sigBlogEdited(*bBlog);
	}
}

void AddEditBlog::enableOkButton( const QString & txt)
{
	if(txt.isEmpty())
		btnOk->setEnabled(false);
	else
		btnOk->setEnabled(true);
}

void AddEditBlog::sltReturnPressed()
{
    if(btnOk->isEnabled()){
		btnOk->setFocus(Qt::OtherFocusReason);
    } else {
        if(tabs->currentIndex()==0){
            if(btnAutoConf->isEnabled()){
                autoConfigure();
            }
        } else {
            fetchBlogId();
        }
    }
}

void AddEditBlog::sltRejected()
{
    ///TODO check timers behavior on accept and reject.
//     mFetchProfileIdTimer->stop();
//     mFetchBlogIdTimer->stop();
//     mFetchAPITimer->stop();
//     
//     delete mFetchProfileIdTimer;
//     delete mFetchBlogIdTimer;
//     delete mFetchAPITimer;
}

AddEditBlog::~AddEditBlog()
{
	kDebug();
//     delete bBlog;
//     delete mFetchProfileIdTimer;
//     delete mFetchBlogIdTimer;
//     delete mFetchAPITimer;
}

#include "addeditblog.moc"
