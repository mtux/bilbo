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
#include "postentry.h"

PostEntry::PostEntry(QWidget *parent)
    :QFrame(parent)
{
	prop = new PostProperties;
	setupUi(parent);
	editPostWidget=new BilboEditor();
	this->layout()->addWidget(editPostWidget);
}
void PostEntry::setupUi(QWidget *parentWidget)
{
	this->resize(626, 307);
	gridLayout = new QGridLayout(this);
	
	horizontalLayout = new QHBoxLayout();
	horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
	
	titleLabel = new QLabel(this);
	titleLabel->setText("&Title :");
	horizontalLayout->addWidget(titleLabel);

	txtTitle = new QLineEdit(this);
	horizontalLayout->addWidget(txtTitle);
	titleLabel->setBuddy(txtTitle);
	connect(txtTitle,SIGNAL(textChanged(const QString&)),parentWidget,SLOT(sltPostTitleChanged(const QString&)));
	
	gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);
	
	wPost = new QWidget(this);
	gridLayout->addWidget(wPost, 1, 0, 1, 1);
	
	//PostWidget->setWindowTitle(QApplication::translate("PostWidget", "Form", 0, QApplication::UnicodeUTF8));
}

void PostEntry::sltPublishPost()
{
	qDebug("publish slot is working now!");
}

void PostEntry::sltSavePostLocally()
{
}

void PostEntry::sltSaveAsDraft()
{
}

void PostEntry::sltDelPost()
{
}

void PostEntry::sltDelLocally()
{
}
