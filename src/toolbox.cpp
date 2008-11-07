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
#include "entriescountdialog.h"

Toolbox::Toolbox(QWidget *parent)
    :QDockWidget(parent)
{
	qDebug("Toolbox::Toolbox");
	setupUi(this);
	frameBlog->layout()->setAlignment(Qt::AlignTop);
	frameCat->layout()->setAlignment(Qt::AlignTop);
	reloadBlogList();
	currentBlog=0;
	
	connect(btnBlogAdd, SIGNAL(clicked()), this, SLOT(sltAddBlog()));
	connect(btnBlogEdit, SIGNAL(clicked()), this, SLOT(sltEditBlog()));
	connect(btnBlogRemove, SIGNAL(clicked()), this, SLOT(sltRemoveBlog()));
	
	connect(btnCatReload, SIGNAL(clicked()), this, SLOT(sltReloadCategoryList()));
	connect(btnEntriesReload, SIGNAL(clicked()), this, SLOT(sltReloadEntries()));
	connect(box, SIGNAL(currentChanged ( int )), this, SLOT(sltCurrentPageChanged(int)));
}

void Toolbox::sltAddBlog()
{
	qDebug("Toolbox::sltAddBlog");
	addEditBlogWindow = new AddEditBlog(-1, this);
	addEditBlogWindow->show();
	connect(addEditBlogWindow, SIGNAL(sigBlogAdded(BilboBlog&)), this, SLOT(sltBlogAdded(BilboBlog&)));
}

void Toolbox::sltEditBlog()
{
	qDebug("Toolbox::sltEditBlog");
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
	qDebug("Toolbox::sltRemoveBlog");
	db->removeBlog(listBlogs.value(currentBlog->text()));
	listBlogs.remove(currentBlog->text());
	delete currentBlog;
	currentBlog=0;
}

void Toolbox::sltBlogAdded(BilboBlog &addedBlog)
{
	qDebug("Toolbox::sltBlogAdded");
	QRadioButton *a = new QRadioButton(addedBlog.title());
	listBlogRadioButtons.append(a);
	frameBlog->layout()->addWidget(a);
	connect(a, SIGNAL(toggled(bool)), this, SLOT(sltSetCurrentBlog(bool)));
}

void Toolbox::sltBlogEdited(BilboBlog &editedBlog)
{
	qDebug("Toolbox::sltBlogEdited");
	blogToEdit->setText(editedBlog.title());
	delete(blogToEdit);
}

void Toolbox::reloadBlogList()
{
	qDebug("Toolbox::reloadBlogList");
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

void Toolbox::sltReloadCategoryList()
{
	qDebug("Toolbox::sltReloadCategoryList");
	if(!currentBlog){
		QMessageBox::warning(this, "First select a blog", "There isn't any selected blog, you have to select a blog from Blogs page befor ask for Category list");
		return;
	}
	
	int blog_id;
	
	blog_id = listBlogs.value(currentBlog->text());

	Backend *b = new Backend(blog_id);
	b->getCategoryListFromServer();
	connect(b, SIGNAL(sigCategoryListFetched(int)), this, SLOT(sltLoadCategoryListFromDB(int)));
}

void Toolbox::sltReloadEntries()
{
	qDebug("Toolbox::sltReloadEntries");
	if(!currentBlog){
		QMessageBox::warning(this, "First select a blog", "There isn't any selected blog, you have to select a blog from Blogs page befor ask for Entries list");
		qDebug("there isn't any selected blog.");
		return;
	}
	EntriesCountDialog *dia = new EntriesCountDialog(this);
	dia->show();
	connect(dia, SIGNAL(sigAccepted(int)), this, SLOT(sltGetEntriesCount(int)));
}

void Toolbox::sltSetCurrentBlog(bool checked)
{
	if(checked){
		currentBlog = dynamic_cast<QRadioButton*>(sender());
	}
}

void Toolbox::sltCurrentPageChanged(int index)
{
	qDebug("Toolbox::sltCurrentPageChanged");
	if(!currentBlog)
		return;
	switch( index ){
	case 1:
		sltLoadEntriesFromDB(listBlogs.value(currentBlog->text(), -1));
		break;
	case 2:
		sltLoadCategoryListFromDB(listBlogs.value(currentBlog->text(), -1));
		break;
	}
}

void Toolbox::sltLoadEntriesFromDB(int blog_id)
{
	qDebug("Toolbox::sltLoadEntriesFromDB");
	if(blog_id==-1){
		qDebug("Toolbox::loadEntriesFromDB: Blog Id not sets correctly");
		return;
	}
	lstEntriesList->clear();
	listEntries.clear();
	
	listEntries = db->listPostsTitle(blog_id);
	lstEntriesList->addItems(listEntries.keys());
}

void Toolbox::sltLoadCategoryListFromDB(int blog_id)
{
	qDebug("Toolbox::sltLoadCategoryListFromDB");
	if(blog_id==-1){
		qDebug("Toolbox::sltLoadCategoryListFromDB: Blog Id not sets correctly");
		return;
	}
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

void Toolbox::sltGetEntriesCount(int count)
{
	qDebug("Toolbox::sltGetEntriesCount");
	Backend *entryB = new Backend(listBlogs.value(currentBlog->text()));
	entryB->getEntriesListFromServer(count);
	connect(entryB, SIGNAL(sigEntriesListFetched(int)), this, SLOT(sltLoadEntriesFromDB(int)));
}
