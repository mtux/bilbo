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
#include "addeditblog.h"
#include "global.h"
#include "bilboblog.h"

#include <QMessageBox>

#define TIMEOUT 40000

AddEditBlog::AddEditBlog(int blog_id, QWidget *parent)
    :QDialog(parent)
{
	qDebug("AddEditBlog::AddEditBlog");
	setupUi(this);
	this->setWindowTitle("Add a new blog");
	isNewBlog=true;
	if(blog_id>-1){
		this->setWindowTitle("Edit Blog Settings");
		isNewBlog=false;
		bBlog = db->getBlogInfo(blog_id);
		txtUrl->setText(bBlog->blogUrl().toString());
		txtUser->setText(bBlog->username());
		txtPass->setText(bBlog->password());
		txtID->setText(bBlog->blogid());
		lblTitle->setText(bBlog->title());
		comboAPI->setCurrentIndex(bBlog->api());
		comboDir->setCurrentIndex(bBlog->direction());
	}
	connect(txtID, SIGNAL(textChanged( const QString& )), this, SLOT(enableOkButton(const QString&)));
	connect(txtUrl, SIGNAL(textChanged(const QString &)), this, SLOT(enableAutoConfBtn()));
	connect(txtUser, SIGNAL(textChanged(const QString &)), this, SLOT(enableAutoConfBtn()));
	connect(txtPass, SIGNAL(textChanged(const QString &)), this, SLOT(enableAutoConfBtn()));
	connect(btnAutoConf, SIGNAL(clicked()), this, SLOT(autoConfigure()));
	connect(btnFetch, SIGNAL(clicked()), this, SLOT(fetchBlogId()));
	connect(this, SIGNAL(accepted()), this, SLOT(sltAccepted()));
	
	txtUrl->setFocus();
	
	bBlog = new BilboBlog();
	bBlog->setBlogId(0);
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
	qDebug("AddEditBlog::autoConfigure");
	if(txtUrl->text().isEmpty() || txtUser->text().isEmpty() || txtPass->text().isEmpty()){
		qDebug("AddEditBlog::autoConfigure: Username, Password or Url isn't set!");
		QMessageBox::warning(this, "Incomplete fields", "You need to set the username, password and url of your blog or website.");
		return;
	}
	btnAutoConf->setEnabled(false);
	btnFetch->setEnabled(false);
	
	///Guess API with Url:
	if(txtUrl->text().contains("xmlrpc.php", Qt::CaseInsensitive)){
		comboAPI->setCurrentIndex(3);
		fetchBlogId();
		return;
	}
	if(txtUrl->text().contains("blogspot", Qt::CaseInsensitive)){
		comboAPI->setCurrentIndex(4);
		fetchBlogId();
		return;
	}
	if(txtUrl->text().contains("wordpress", Qt::CaseInsensitive)){
		comboAPI->setCurrentIndex(3);
		txtUrl->setText(txtUrl->text() + "/xmlrpc.php");
		fetchBlogId();
		return;
	}
	if(txtUrl->text().contains("livejournal", Qt::CaseInsensitive)){
		comboAPI->setCurrentIndex(0);
		txtUrl->setText("http://www.livejournal.com/interface/blogger/");
		txtID->setText(txtUser->text());
		btnAutoConf->setEnabled(true);
		btnFetch->setEnabled(true);
		return;
	}
	//TODO Otherwise try to get the api from the html
}

void AddEditBlog::fetchBlogId()
{
	qDebug("AddEditBlog::fetchBlogId");
	
	

	switch( comboAPI->currentIndex() ){
		case 0:
		case 1:
		case 2:
		case 3:
			mBlog = new KBlog::Blogger1(KUrl(txtUrl->text()));
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
			mBlog = new KBlog::GData( txtUrl->text() );
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
	txtID->setText("Please wait...");
	txtID->setEnabled(false);
	btnFetch->setEnabled(false);
	btnAutoConf->setEnabled(false);
}

void AddEditBlog::handleFetchIDTimeout()
{
	qDebug("AddEditBlog::handleFetchIDTimeout");
	QMessageBox::critical(this, "Error!", "Fetching the blog's id timed out. Check your internet connection, Or your homepage Url!\nnote that url have to included \"http://\" or ...\nfor example: http://bilbo.sf.net/xmlrpc.php is a good url");
	txtID->setText(QString());
	txtID->setEnabled(true);
	btnFetch->setEnabled(true);
	btnAutoConf->setEnabled(true);
// 	delete mFetchProfileIdTimer;
// 	delete mFetchBlogIdTimer;
}

void AddEditBlog::handleFetchAPITimeout()
{
	qDebug("AddEditBlog::handleFetchAPITimeout");
	QMessageBox::warning(this, "AutoConfiguration Failed", "App cannot get API type automatically, please check your internet connection, otherwise you have to set API type handy.");
	txtID->setEnabled(true);
	btnFetch->setEnabled(true);
	btnAutoConf->setEnabled(true);
// 	delete mFetchAPITimer;
}

void AddEditBlog::handleFetchError(KBlog::Blog::ErrorType type, const QString & errorMsg)
{
	qDebug("AddEditBlog::handleFetchError: ErrorType: %d", type);
	QMessageBox::critical(this, "Fetching BlogID Faild!", errorMsg);
	txtID->setEnabled(true);
	btnFetch->setEnabled(true);
	btnAutoConf->setEnabled(true);
}

void AddEditBlog::fetchedBlogId(const QList< QMap < QString , QString > > & list)
{
	qDebug("AddEditBlog::fetchedBlogId");
	delete mFetchBlogIdTimer;
	if(list.count()>1){
		///TODO: handle more than one blog!
		qDebug("AddEditBlog::fetchedBlogId: User has more than ONE blog!");
	}
	txtID->setText(list.first().values().first());
	lblTitle->setText(list.first().values().last());
	txtID->setEnabled(true);
	btnFetch->setEnabled(true);
	btnAutoConf->setEnabled(true);
	
	bBlog->setBlogUrl(QUrl(txtUrl->text()));
	bBlog->setUsername(txtUser->text());
	bBlog->setPassword(txtPass->text());
	bBlog->setBlogId(txtID->text());
	bBlog->setTitle(list.first().values().last());
	btnOk->setEnabled(true);
}

void AddEditBlog::fetchedProfileId(const QString &id)
{
	qDebug("AddEditBlog::fetchedProfileId");
	delete mFetchProfileIdTimer;
	connect( dynamic_cast<KBlog::GData*>(mBlog), SIGNAL(listedBlogs( const QList<QMap<QString, QString> >&)),
			 this, SLOT(fetchedBlogId( const QList<QMap<QString, QString> >&)));
	connect( dynamic_cast<KBlog::GData*>(mBlog), SIGNAL(error( KBlog::Blog::ErrorType, const QString& ) ),
			 this, SLOT( handleFetchError( KBlog::Blog::ErrorType, const QString& ) ) );
	dynamic_cast<KBlog::GData*>(mBlog)->listBlogs();
}

void AddEditBlog::sltAccepted()
{
	qDebug("AddEditBlog::sltAccepted");
	if(bBlog->blogid().isEmpty() && txtID->text().isEmpty()){
		QMessageBox::critical(this, "Failed to get blog id", "You have to Fetch blog id by hitting \"Auto Configure\" Or \"Fetch ID\" button or Insert your Blog Id manually.");
		return;
	}
	bBlog->setApi((BilboBlog::ApiType)comboAPI->currentIndex());
	bBlog->setDirection((BilboBlog::TextDirection)comboDir->currentIndex());
	
	if(bBlog->password().isEmpty())
		bBlog->setPassword(txtPass->text());
	if(bBlog->username().isEmpty())
		bBlog->setUsername(txtUser->text());
	if(bBlog->blogid().isEmpty())
		bBlog->setBlogId(txtID->text());
	if(bBlog->blogUrl().isEmpty())
		bBlog->setBlogUrl(QUrl(txtUrl->text()));
	
	if(isNewBlog){
		int blog_id = db->addBlog(*bBlog);
		bBlog->setId(blog_id);
		if(blog_id!=-1)
			emit sigBlogAdded(*bBlog);
	}
	else{
		if(db->editBlog(*bBlog))
			emit sigBlogEdited(*bBlog);
	}
}

void AddEditBlog::enableOkButton( const QString & txt)
{
	if(txt.isEmpty())
		btnOk->setEnabled(false);
	else
		btnOk->setEnabled(true);
}
