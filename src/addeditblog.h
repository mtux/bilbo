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

#ifndef ADDEDITBLOG_H
#define ADDEDITBLOG_H

#include <QDialog>

#include "ui_addeditblogbase.h"
#include <kblog/blog.h>
class BilboBlog;


/**
	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
 */
class AddEditBlog: public QDialog, public Ui::AddEditBlogBase {
Q_OBJECT
public:
    /**
     * 
     * @param blog_id id of blog we will edit it, for adding a blog this should be "-1"
     * @param parent Parent
     */
    AddEditBlog(int blog_id, QWidget *parent = 0);
    ~AddEditBlog();
	
protected Q_SLOTS:
	void enableAutoConfBtn();
	void enableOkButton( const QString & );
	void autoConfigure();
	void fetchBlogId();
	
	void fetchedBlogId(const QList<QMap<QString, QString> >& list);
	void fetchedProfileId( const QString& );
	
	void handleFetchIDTimeout();
	void handleFetchAPITimeout();
	void handleFetchError( KBlog::Blog::ErrorType type, const QString& errorMsg);
	
	void sltAccepted();
    void sltRejected();
	
    void sltReturnPressed();
Q_SIGNALS:
	void sigBlogAdded(BilboBlog&);
	void sigBlogEdited(BilboBlog&);
	
private:
	bool isNewBlog;
	BilboBlog *bBlog;
	KBlog::Blog *mBlog;
	QTimer* mFetchProfileIdTimer;
	QTimer* mFetchBlogIdTimer;
	QTimer* mFetchAPITimer;
    bool isIdFetched;
};

#endif
