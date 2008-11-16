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
#include "addeditblog.h"
#include "global.h"
#include "backend.h"
#include "bilbopost.h"

Toolbox::Toolbox(QStatusBar *mainStatusbar, QWidget *parent)
    :QDockWidget(parent)
{
	qDebug("Toolbox::Toolbox");
	statusbar = mainStatusbar;
	setupUi(this);
	frameBlog->layout()->setAlignment(Qt::AlignTop);
	frameCat->layout()->setAlignment(Qt::AlignTop);
	reloadBlogList();
	currentBlog=0;
	currentPost = new BilboPost();
	datetimeOptionstimestamp->setDateTime(QDateTime::currentDateTime());
	
	connect(btnBlogAdd, SIGNAL(clicked()), this, SLOT(sltAddBlog()));
	connect(btnBlogEdit, SIGNAL(clicked()), this, SLOT(sltEditBlog()));
	connect(btnBlogRemove, SIGNAL(clicked()), this, SLOT(sltRemoveBlog()));
	
	connect(btnCatReload, SIGNAL(clicked()), this, SLOT(sltReloadCategoryList()));
	connect(btnEntriesReload, SIGNAL(clicked()), this, SLOT(sltReloadEntries()));
	connect(box, SIGNAL(currentChanged ( int )), this, SLOT(sltCurrentPageChanged(int)));
	connect(this, SIGNAL(sigCurrentBlogChanged(int)), this, SLOT(sltCurrentBlogChanged(int)));
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
	listBlogs.insertMulti(addedBlog.title(), addedBlog.id());
	QRadioButton *a = new QRadioButton(addedBlog.title());
	a->setToolTip(addedBlog.blogUrl().toString());
	listBlogRadioButtons.append(a);
	frameBlog->layout()->addWidget(a);
	connect(a, SIGNAL(toggled(bool)), this, SLOT(sltSetCurrentBlog(bool)));
}

void Toolbox::sltBlogEdited(BilboBlog &editedBlog)
{
	qDebug("Toolbox::sltBlogEdited");
	blogToEdit->setText(editedBlog.title());
	blogToEdit->setToolTip(editedBlog.blogUrl().toString());
// 	delete(blogToEdit);
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
		rb->setToolTip(db->getBlogInfo(i.value())->blogUrl().toString());
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
	statusbar->showMessage("Request for Selected blog's category list sent to server...");
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
		emit sigCurrentBlogChanged(listBlogs.value(currentBlog->text(), -1));
	}
}

void Toolbox::sltCurrentPageChanged(int index)
{
// 	qDebug("Toolbox::sltCurrentPageChanged");
	if(!currentBlog)
		return;
	switch( index ){
	case 1:
// 		sltLoadEntriesFromDB(listBlogs.value(currentBlog->text(), -1));
		break;
	case 2:
// 		sltLoadCategoryListFromDB(listBlogs.value(currentBlog->text(), -1));
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
	clearEntriesList();
	statusbar->showMessage("Entries list received.", STATUSTIMEOUT);
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
	statusbar->showMessage("Category list received.", STATUSTIMEOUT);
	
	clearCatList();
	listCategories = db->listCategories(blog_id);
	
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
	statusbar->showMessage("Request for Selected blog's Entries list sent to server...");
}

void Toolbox::resetFields()
{
	qDebug("Toolbox::resetFields");
	for(int i=0; i<listCategoryCheckBoxes.count(); ++i){
		listCategoryCheckBoxes[i]->setChecked(false);
	}
	txtCatTags->clear();
	chkOptionsTime->setChecked(false);
	datetimeOptionstimestamp->setDateTime(QDateTime::currentDateTime());
	txtOptionsTrackback->clear();
}

void Toolbox::clearCatList()
{
	listCategories.clear();
	for(int j=0; j<listCategoryCheckBoxes.count(); ++j){
		delete(listCategoryCheckBoxes[j]);
	}
}

void Toolbox::clearEntriesList()
{
	lstEntriesList->clear();
	listEntries.clear();
}

void Toolbox::sltCurrentBlogChanged(int blog_id)
{
	///TODO: Save current state to a temporary variable!
	sltLoadCategoryListFromDB(blog_id);
	sltLoadEntriesFromDB(blog_id);
	Qt::LayoutDirection ll = db->getBlogInfo(blog_id)->direction();
	frameCat->setLayoutDirection(ll);
	lstEntriesList->setLayoutDirection(ll);
	
}

BilboPost * Toolbox::getFieldsValue()
{
	qDebug("Toolbox::getFieldsValue");
	currentPost->setCategories(this->selectedCategoriesTitle());
	currentPost->setTags(this->currentTags());
	if(currentPost->status()==KBlog::BlogPost::Fetched || currentPost->status()==KBlog::BlogPost::Modified){
		if(chkOptionsTime->isChecked())
			currentPost->setMTime(datetimeOptionstimestamp->dateTime());
		else
			currentPost->setMTime(QDateTime::currentDateTime());
	} else {
		if(chkOptionsTime->isChecked()){
			currentPost->setMTime(datetimeOptionstimestamp->dateTime());
			currentPost->setCTime(datetimeOptionstimestamp->dateTime());
		}
		else {
			currentPost->setMTime(QDateTime::currentDateTime());
			currentPost->setCTime(QDateTime::currentDateTime());
		}
	}
	
	currentPost->setPrivate((comboOptionsStatus->currentIndex()==1)?true:false);
	currentPost->setCommentAllowed(chkOptionsComments->isChecked());
	currentPost->setTrackBackAllowed(chkOptionsTrackback->isChecked());
	currentPost->setPosition((BilboPost::Position)comboOptionsStatus->currentIndex());
	return currentPost;
}

void Toolbox::setFieldsValue(const BilboPost & post)
{
}

QStringList Toolbox::selectedCategoriesTitle()
{
	qDebug("Toolbox::selectedCategoriesTitle");
	QStringList list;
	for( int i=0; i<listCategoryCheckBoxes.count(); ++i){
		if(listCategoryCheckBoxes[i]->isChecked())
			list.append(listCategoryCheckBoxes[i]->text());
	}
	return list;
}

QList< int > Toolbox::selectedCategoriesId()
{
	///TODO: Implement it
	qDebug("QList< int > Toolbox::selectedCategoriesId() : NOT IMPLEMENTED YET!");
	return QList<int>();
}

void Toolbox::setSelectedCategories(const QStringList &list)
{
	for( int i=0; i<listCategoryCheckBoxes.count(); ++i){
		if(list.contains(listCategoryCheckBoxes[i]->text(), Qt::CaseInsensitive))
			listCategoryCheckBoxes[i]->setChecked(true);
	}
}

void Toolbox::setSelectedCategories(const QList< int > &)
{
	///TODO: Implement it
	qDebug("Toolbox::setSelectedCategories(const QList< int > &) : NOT IMPLEMENTED YET!");
}

QStringList Toolbox::currentTags()
{
	qDebug("Toolbox::currentTags");
	QStringList t;
	t = txtCatTags->text().split(',', QString::SkipEmptyParts);
	for(int i=0; i < t.count() ; ++i){
		t[i] = t[i].trimmed();
	}
	return t;
}

int Toolbox::currentBlogId()
{
	if(currentBlog)
		return listBlogs.value(currentBlog->text(), -1);
	else
		return -1;
}
