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
#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QDockWidget>
#include <QtGui>

#include "ui_toolbox.h"
#include "addeditblog.h"
#include "global.h"
#include "backend.h"

/**
	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
 */
class Toolbox: public QDockWidget, public Ui::ToolboxWidget {
Q_OBJECT
public:
    Toolbox(QWidget *parent = 0);
	
	void reloadBlogList();
	BilboPost* getFieldsValue();
	void setFieldsValue(const BilboPost& post);
	int currentBlogId();
	
	QMap<QString, int> listBlogs;///Contain Blog title(QString) and Blog_id(int)
	QList<QRadioButton*> listBlogRadioButtons;
	
	QMap<QString, int> listCategories;///Contain Category name(QString) and Category id(int)
	QList<QCheckBox*> listCategoryCheckBoxes;
	
	QMap<QString, int> listEntries;///Contain Post title(QString) and Post id(int)
	
public slots:
	void sltAddBlog();
	void sltBlogAdded(BilboBlog&);
	void sltEditBlog();
	void sltBlogEdited(BilboBlog&);
	void sltRemoveBlog();
	void sltSetCurrentBlog( bool );
	void sltReloadCategoryList();
	void sltLoadCategoryListFromDB( int blog_id );
	void sltReloadEntries();
	void sltGetEntriesCount(int);
	void sltLoadEntriesFromDB( int blog_id );
	void sltCurrentPageChanged( int );
	void sltCurrentBlogChanged( int blog_id );
	void resetFields();
	
signals:
	void sigCurrentBlogChanged(int blog_id);
	
private:
	QStringList selectedCategoriesTitle();
	QList<int> selectedCategoriesId();
	void setSelectedCategories(const QStringList&);
	void setSelectedCategories(const QList<int>&);
	QStringList currentTags();
	void clearCatList();
	void clearEntriesList();
	
	
	AddEditBlog *addEditBlogWindow;
	QRadioButton *blogToEdit;
	QRadioButton *currentBlog;
	BilboPost *currentPost;
};

#endif
