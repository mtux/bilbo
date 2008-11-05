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
#include "toolbox.h"

Toolbox::Toolbox(QWidget *parent)
    :QDockWidget(parent)
{
	setupUi(this);
	frameBlog->layout()->setAlignment(Qt::AlignTop);
	frameCat->layout()->setAlignment(Qt::AlignTop);
	reloadBlogList();
	
	connect(btnBlogAdd, SIGNAL(clicked()), this, SLOT(sltAddBlog()));
	connect(btnBlogEdit, SIGNAL(clicked()), this, SLOT(sltEditBlog()));
	connect(btnBlogRemove, SIGNAL(clicked()), this, SLOT(sltRemoveBlog()));
}

void Toolbox::sltAddBlog()
{
	addEditBlogWindow = new AddEditBlog(-1, this);
	addEditBlogWindow->show();
	connect(addEditBlogWindow, SIGNAL(sigBlogAdded(BilboBlog&)), this, SLOT(sltBlogAdded(BilboBlog&)));
}

void Toolbox::sltEditBlog()
{
	if(!currentBlog){
		QMessageBox::warning(this, "First select a blog", "There isn't any selected blog, you have to select a blog first.");
		return;
	}
	blogToEdit = currentBlog;
	
	addEditBlogWindow = new AddEditBlog(listBlogs.value(blogToEdit->text(), -1), this);
	addEditBlogWindow->show();
}

void Toolbox::sltRemoveBlog()
{
	db->removeBlog(listBlogs.value(currentBlog->text()));
	listBlogs.remove(currentBlog->text());
	delete currentBlog;
}

void Toolbox::sltBlogAdded(BilboBlog &addedBlog)
{
	QRadioButton *a = new QRadioButton(addedBlog.title());
	listBlogRadioButtons.append(a);
	frameBlog->layout()->addWidget(a);
	connect(a, SIGNAL(toggled(bool)), this, SLOT(sltSetCurrentBlog(bool)));
}

void Toolbox::sltBlogEdited(BilboBlog &editedBlog)
{
	blogToEdit->setText(editedBlog.title());
	delete(blogToEdit);
}

void Toolbox::reloadBlogList()
{
	listBlogs.clear();
	listBlogs = db->listBlogsTitle();
	for(int j=0; j<listBlogRadioButtons.count(); ++j){
		delete(listBlogRadioButtons[j]);
	}
	listBlogRadioButtons.clear();
	QMap<QString, int>::iterator i;
	for( i = listBlogs.begin(); i != listBlogs.end(); ++i ){
		QRadioButton *rb = new QRadioButton(i.key());
		listBlogRadioButtons.append(rb);
		frameBlog->layout()->addWidget(rb);
		connect(rb, SIGNAL(toggled(bool)), this, SLOT(sltSetCurrentBlog(bool)));
	}
}

void Toolbox::sltLoadCategoryList()
{
	if(!currentBlog){
		QMessageBox::warning(this, "First select a blog", "There isn't any selected blog, you have to select a blog from Blogs page befor ask for Category list");
		return;
	}
	
	int blog_id;
	

	blog_id = listBlogs.value(currentBlog->text());
	
	listCategories.clear();
	listCategories = db->listCategories(blog_id);
	
	for(int j=0; j<listCategoryCheckBoxes.count(); ++j){
		delete(listCategoryCheckBoxes[j]);
	}
	
	listCategoryCheckBoxes.clear();
	QMap<QString, int>::iterator i;
	for( i = listCategories.begin(); i != listCategories.end(); ++i ){
		QCheckBox *cb = new QCheckBox(i.key());
		listCategoryCheckBoxes.append(cb);
		frameCat->layout()->addWidget(cb);
	}
}

void Toolbox::sltReloadEntries()
{
	
}

void Toolbox::sltSetCurrentBlog(bool checked)
{
	if(checked)
		currentBlog = dynamic_cast<QRadioButton*>(sender());
}
