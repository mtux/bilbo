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
#include "bilbopost.h"
#include "bilbomedia.h"
#include "backend.h"

PostEntry::PostEntry(QWidget *parent)
    :QFrame(parent)
{
	createUi();
// 	mMediaList = new QMap<QString, BilboMedia*>();
	editPostWidget = new BilboEditor(this);
	editPostWidget->setMediaList(&mMediaList);
// 	editPostWidget->setMediaList(mMediaList);
	this->layout()->addWidget(editPostWidget);
    mCurrentPost = 0;
	
	mCurrentPostBlogId = -1;
}

void PostEntry::createUi()
{
	this->resize(626, 307);
	gridLayout = new QGridLayout(this);
	
	horizontalLayout = new QHBoxLayout(this);
	horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
	
	labelTitle = new QLabel(this);
	labelTitle->setText(i18nc("noun, the post title", "Title:"));
	horizontalLayout->addWidget(labelTitle);

	txtTitle = new KLineEdit(this);
	horizontalLayout->addWidget(txtTitle);
	labelTitle->setBuddy(txtTitle);
    connect(txtTitle, SIGNAL(textChanged(const QString&)), this, 
			SLOT( sltTitleChanged(const QString&) ));
	
	gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);
	
}

void PostEntry::sltTitleChanged(const QString& title)
{
	mCurrentPost->setTitle(title);
	Q_EMIT sigTitleChanged(title);
}

QString PostEntry::postTitle() const
{
	return mCurrentPost->title();
}

QString * PostEntry::postBody()
{
	QString *str = this->editPostWidget->htmlContent();
	if(!mCurrentPost){
		mCurrentPost = new BilboPost;
	}
	mCurrentPost->setContent(*str);
	return str;
}

void PostEntry::setPostTitle(const QString & title)
{
	this->txtTitle->setText(title);
	mCurrentPost->setTitle(title);
}

void PostEntry::setPostBody(const QString & body)
{
	kDebug()<<body;
	mCurrentPost->setContent(body);
    this->editPostWidget->setHtmlContent(body);
}

int PostEntry::currentPostBlogId()
{
    return mCurrentPostBlogId;
}

void PostEntry::setCurrentPostBlogId(int blog_id)
{
    mCurrentPostBlogId = blog_id;
}

BilboPost* PostEntry::currentPost()
{
    mCurrentPost->setContent(*postBody());
    //mCurrentPost->setTitle(postTitle());
    //return (*mCurrentPost);
	return (mCurrentPost);
}

void PostEntry::setCurrentPost(BilboPost post)
{
    mCurrentPost = new BilboPost(post);
    this->setPostBody(mCurrentPost->content());
    this->setPostTitle(mCurrentPost->title());
}

Qt::LayoutDirection PostEntry::defaultLayoutDirection()
{
	return this->txtTitle->layoutDirection();
}

void PostEntry::setDefaultLayoutDirection(Qt::LayoutDirection direction)
{
	this->editPostWidget->setLayoutDirection(direction);
	this->txtTitle->setLayoutDirection(direction);
}

void PostEntry::addMedia(const QString &url)
{
}

PostEntry::~PostEntry()
{
	kDebug();
    delete mCurrentPost;
}

void PostEntry::setCurrentPostProperties(BilboPost post)
{
	post.setTitle(txtTitle->text());
	post.setContent(*this->editPostWidget->htmlContent());
	setCurrentPost(post);
}

QMap< QString, BilboMedia * > & PostEntry::mediaList()
{
	return mMediaList;
}

bool PostEntry::uploadMediaFiles()
{
	bool result = false;
	int numOfFilesToBeUploaded = 0;
	Backend *b = new Backend(mCurrentPostBlogId, this);
	QMap <QString, BilboMedia*>::iterator it = mMediaList.begin();
	QMap <QString, BilboMedia*>::iterator endIt = mMediaList.end();
	for( ; it!=endIt; ++it){
		if(!it.value()->isUploaded()){
			result = true;
			connect(b, SIGNAL(sigMediaUploaded(BilboMedia*)), this, SLOT(sltMediaFileUploaded(BilboMedia*)));
			connect(b, SIGNAL(sigError(const QString&)), this, SLOT(sltError(const QString)));
			connect(b, SIGNAL(sigMediaError(const QString&, BilboMedia*)), this, SLOT(sltMediaError(const QString&, BilboMedia*)));
			b->uploadMedia(it.value());
			++numOfFilesToBeUploaded;
		}
	}
	if(result){
		progress = new QProgressBar(this);
		this->layout()->addWidget(progress);
		progress->setMaximum(numOfFilesToBeUploaded);
		progress->setValue( 0 );
	}
	isUploadingMediaFilesFailed = false;
	return result;
}

void PostEntry::sltMediaFileUploaded(BilboMedia * media)
{
	kDebug();
	progress->setValue(progress->value() + 1);
	if(progress->value()>= progress->maximum()){
		QTimer::singleShot(800, this, SLOT(sltDeleteProgressBar()));
		if(!isUploadingMediaFilesFailed){
			publishPostAfterUploadMediaFiles();
		}
		sender()->deleteLater();
	}
}

void PostEntry::sltError(const QString & errMsg)
{
	kDebug();
	KMessageBox::detailedSorry(this, i18n("An Error occurred on latest transaction."),errMsg);
	if(progress){
		QTimer::singleShot(500, this, SLOT(sltDeleteProgressBar()));
	}
	emit postPublishingDone(errMsg);
	sender()->deleteLater();
}

void PostEntry::sltMediaError(const QString & errorMessage, BilboMedia * media)
{
	kDebug();
	isUploadingMediaFilesFailed = true;
	QString name = media->name();
	kDebug()<<" AN ERROR OCCURRED ON UPLOADING,\tError message is: "<<errorMessage;

	KMessageBox::detailedSorry(this, i18n("Uploading media file %1 (Local Path: %2) failed", name, media->localUrl()),
							    errorMessage, i18n("Uploading media file Failed!"));
	if(progress){
		QTimer::singleShot(500, this, SLOT(sltDeleteProgressBar()));
	}
	emit postPublishingDone(errorMessage);
	sender()->deleteLater();
}

void PostEntry::publishPost(int blogId, BilboPost * postData)
{
	kDebug();
	this->setCurrentPostProperties(*postData);
	mCurrentPostBlogId = blogId;
	if(!this->uploadMediaFiles())
		publishPostAfterUploadMediaFiles();
}

void PostEntry::publishPostAfterUploadMediaFiles()
{
	kDebug();
	
	progress = new QProgressBar(this);
	this->layout()->addWidget(progress);
	progress->setMaximum( 0 );
	progress->setMinimum( 0 );
	
	Backend *b = new Backend(mCurrentPostBlogId);
	connect(b, SIGNAL(sigPostPublished(int, BilboPost*)), this, SLOT(sltPostPublished(int, BilboPost*)));
	connect(b, SIGNAL(sigError(const QString&)), this, SLOT(sltError(const QString&)));
	b->publishPost(mCurrentPost);
}

void PostEntry::sltPostPublished(int blog_id, BilboPost *post )
{
	kDebug()<<"Post Id on server: "<< post->postId();
	///FIXME This DB communication is un necessary! fix it
// 	BilboBlog *b = DBMan::self()->getBlogInfo(blog_id);
	QString blog_name="NOT SET";// = b->title();
// 	delete b;
	QString msg;
	if(post->isPrivate()){
		msg = i18n("New Draft with title \"%1\" saved successfully.", post->title());
	}
	else {
		msg = i18n("New Post with title \"%1\" published successfully.", post->title());
	}
	KMessageBox::information(this, msg, "Successful");
// 	if(KMessageBox::questionYesNo(this, msg, "Successful") != KMessageBox::Yes){
// 		sltRemoveCurrentPostEntry();//FIXME this functionality doesn't work! fix it.
// 	}
	if(progress){
		this->layout()->removeWidget(progress);
		progress->deleteLater();
	}
// 	if(isPrivate){
// 		msg = i18n("Draft saved successfully!");
// 	} else {
// 		msg = i18n("New post published successfully!");
// 	}
	emit postPublishingDone(QString());
	sender()->deleteLater();
}

void PostEntry::sltDeleteProgressBar()
{
	this->layout()->removeWidget(progress);
	progress->deleteLater();
}

#include "postentry.moc"
