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
#include <QtGui> 

#include "postentry.h"
#include "bilboeditor.h"
#include "bilbopost.h"

PostEntry::PostEntry(QWidget *parent)
    :QFrame(parent)
{
	createUi();
	editPostWidget = new BilboEditor();
	this->layout()->addWidget(editPostWidget);
    mCurrentPost = 0;
	
// 	mCurrentPostBlogId = 0;
}

void PostEntry::createUi()
{
	this->resize(626, 307);
	gridLayout = new QGridLayout(this);
	
	horizontalLayout = new QHBoxLayout();
	horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
	
	labelTitle = new QLabel(this);
	labelTitle->setText("&Title :");
	horizontalLayout->addWidget(labelTitle);

	txtTitle = new QLineEdit(this);
	horizontalLayout->addWidget(txtTitle);
	labelTitle->setBuddy(txtTitle);
    connect(txtTitle, SIGNAL(textChanged(const QString&)), this, SIGNAL( sigTitleChanged(const QString&) ));
	
	gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);
	
	wPost = new QWidget(this);
	gridLayout->addWidget(wPost, 1, 0, 1, 1);
	
}

QString PostEntry::postTitle() const
{
	return QString(this->txtTitle->text());
}

QString * PostEntry::postBody()
{
	return this->editPostWidget->htmlContent();
}

void PostEntry::setPostTitle(const QString & title)
{
    this->txtTitle->setText(title);
}

void PostEntry::setPostBody(const QString & body)
{
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

BilboPost PostEntry::currentPost()
{
    return (*mCurrentPost);
}

void PostEntry::setCurrentPost(BilboPost post)
{
//     if(mCurrentPost!=0)
//         delete(mCurrentPost);
//     if(post==0)
//         mCurrentPost = new BilboPost();
//     else
        mCurrentPost = new BilboPost(post);
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

PostEntry::~PostEntry()
{
    delete editPostWidget;
    delete gridLayout;
    delete horizontalLayout;
    delete labelTitle;
    delete txtTitle;
    delete wPost;
    delete mCurrentPost;
}
