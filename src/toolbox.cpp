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

#include <kstatusbar.h>
#include <kdebug.h>
#include <kxmlguiwindow.h>
#include <kmessagebox.h>
#include <QButtonGroup>

#include "toolbox.h"
#include "entriescountdialog.h"
#include "addeditblog.h"
#include "global.h"
#include "backend.h"
#include "bilbopost.h"
#include "bilboblog.h"

Toolbox::Toolbox(QWidget *parent)
    :QWidget(parent)
{
	kDebug();
    if(parent)
        this->statusbar = qobject_cast<KXmlGuiWindow*>(parent)->statusBar();
    else
        this->statusbar = new KStatusBar(this);
	setupUi(this);
    setButtonsIcon();
	frameBlog->layout()->setAlignment(Qt::AlignTop);
	frameCat->layout()->setAlignment(Qt::AlignTop);
	reloadBlogList();
// 	currentBlog=0;
	currentPost = new BilboPost();
	datetimeOptionstimestamp->setDateTime(QDateTime::currentDateTime());
	
	connect(btnBlogAdd, SIGNAL(clicked()), this, SLOT(sltAddBlog()));
	connect(btnBlogEdit, SIGNAL(clicked()), this, SLOT(sltEditBlog()));
	connect(btnBlogRemove, SIGNAL(clicked()), this, SLOT(sltRemoveBlog()));
	
	connect(btnCatReload, SIGNAL(clicked()), this, SLOT(sltReloadCategoryList()));
	connect(btnEntriesReload, SIGNAL(clicked()), this, SLOT(sltReloadEntries()));
	connect(box, SIGNAL(currentChanged ( int )), this, SLOT(sltCurrentPageChanged(int)));
	connect(this, SIGNAL(sigCurrentBlogChanged(int)), this, SLOT(sltCurrentBlogChanged(int)));//Replaced with next statment!
    connect(&listBlogRadioButtons, SIGNAL(buttonClicked ( int )), this, SLOT(sltSetCurrentBlog()));
    
    connect(lstEntriesList, SIGNAL(itemDoubleClicked( QListWidgetItem* )),
            this, SLOT( sltEntrySelected( QListWidgetItem* )));
    connect(btnEntriesCopyUrl, SIGNAL(clicked( bool )), this, SLOT(sltEntriesCopyUrl()));
    
    lblOptionsTrackBack->setVisible(false);
    txtOptionsTrackback->setVisible(false);
}

void Toolbox::sltAddBlog()
{
	kDebug();
	addEditBlogWindow = new AddEditBlog(-1, this);
    addEditBlogWindow->setAttribute(Qt::WA_DeleteOnClose);
	addEditBlogWindow->show();
	connect(addEditBlogWindow, SIGNAL(sigBlogAdded(BilboBlog&)), this, SLOT(sltBlogAdded(BilboBlog&)));
}

void Toolbox::sltEditBlog()
{
	kDebug();
	if(!listBlogRadioButtons.checkedButton()){
		KMessageBox::sorry(this, i18n("There isn't any selected blog, you have to select a blog first."));
		return;
	}
    blogToEdit = qobject_cast<QRadioButton*>(listBlogRadioButtons.checkedButton());
	
	addEditBlogWindow = new AddEditBlog(listBlogs.value(blogToEdit->text(), -1), this);
    addEditBlogWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(addEditBlogWindow, SIGNAL(sigBlogEdited(BilboBlog&)), this, SLOT(sltBlogEdited(BilboBlog&)));
	addEditBlogWindow->show();
}

void Toolbox::sltRemoveBlog()
{
	kDebug();
	__db->removeBlog(listBlogs.value(listBlogRadioButtons.checkedButton()->text()));
    listBlogs.remove(listBlogRadioButtons.checkedButton()->text());
    listBlogRadioButtons.removeButton(listBlogRadioButtons.checkedButton());
// 	delete currentBlog;
// 	currentBlog=0;
}

void Toolbox::sltBlogAdded(BilboBlog &addedBlog)
{
	kDebug();
	listBlogs.insert(addedBlog.title(), addedBlog.id());
	QRadioButton *a = new QRadioButton(addedBlog.title());
	a->setToolTip(addedBlog.blogUrl());
	listBlogRadioButtons.addButton(a);
	frameBlog->layout()->addWidget(a);
// 	connect(a, SIGNAL(toggled(bool)), this, SLOT(sltSetCurrentBlog(bool)));
    a->setChecked(true);
    sltReloadCategoryList();
    delete addEditBlogWindow;
}

void Toolbox::sltBlogEdited(BilboBlog &editedBlog)
{
	kDebug();
    listBlogs.remove(blogToEdit->text());
	blogToEdit->setText(editedBlog.title());
	blogToEdit->setToolTip(editedBlog.blogUrl());
    listBlogs.insert(editedBlog.title(), editedBlog.id());
//     Q_EMIT sigCurrentBlogChanged(listBlogs.value(listBlogRadioButtons.checkedButton()->text(), -1));
    sltCurrentBlogChanged(listBlogs.value(listBlogRadioButtons.checkedButton()->text(), -1));
    sltReloadCategoryList();
    delete addEditBlogWindow;
}

void Toolbox::reloadBlogList()
{
	kDebug();
	listBlogs.clear();
    listBlogs = __db->listBlogsTitle();
    foreach( QAbstractButton *ab,listBlogRadioButtons.buttons()){
		delete ab;
	}
	listBlogRadioButtons.buttons().clear();
	QMap<QString, int>::iterator i;
	for( i = listBlogs.begin(); i != listBlogs.end(); ++i ){
		QRadioButton *rb = new QRadioButton(i.key());
        BilboBlog *bb = __db->getBlogInfo(i.value());
		rb->setToolTip(bb->blogUrl());
        delete bb;
		listBlogRadioButtons.addButton(rb);
		frameBlog->layout()->addWidget(rb);
// 		connect(rb, SIGNAL(toggled(bool)), this, SLOT(sltSetCurrentBlog(bool)));
	}
}

void Toolbox::sltReloadCategoryList()
{
	kDebug();
    if(!listBlogRadioButtons.checkedButton()){
		KMessageBox::sorry(this, i18n("There isn't any selected blog, you have to select a blog from Blogs page befor ask for Category list"));
		return;
	}
	
	int blog_id;
	
    blog_id = listBlogs.value(listBlogRadioButtons.checkedButton()->text());

	Backend *b = new Backend(blog_id);
	b->getCategoryListFromServer();
	connect(b, SIGNAL(sigCategoryListFetched(int)), this, SLOT(sltLoadCategoryListFromDB(int)));
    connect(b, SIGNAL(sigError(QString&)), this, SLOT(sltError(QString&)));
	statusbar->showMessage(i18n("Requesting category list..."));
    this->setCursor(Qt::BusyCursor);
}

void Toolbox::sltReloadEntries()
{
	kDebug();
    if(!listBlogRadioButtons.checkedButton()){
		KMessageBox::sorry(this, i18n("There isn't any selected blog, you have to select a blog from Blogs page befor ask for Entries list"));
		kDebug()<<"There isn't any selected blog.";
		return;
	}
	EntriesCountDialog *dia = new EntriesCountDialog(this);
    dia->setAttribute( Qt::WA_DeleteOnClose );
	dia->show();
	connect(dia, SIGNAL(sigAccepted(int)), this, SLOT(sltGetEntriesCount(int)));
}

void Toolbox::sltSetCurrentBlog()
{
    kDebug()<<"Current blog is: "<<listBlogRadioButtons.checkedButton()->text();
    int id = listBlogs.value(listBlogRadioButtons.checkedButton()->text(), -1);
    kDebug()<<"Current Blog Id: "<<id;
// 	if(checked){
// 		currentBlog = dynamic_cast<QRadioButton*>(sender());
        emit sigCurrentBlogChanged(id);
// 	}
}

void Toolbox::sltCurrentPageChanged(int index)
{
// 	kDebug();
    if(!listBlogRadioButtons.checkedButton())
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
	kDebug();
	if(blog_id==-1){
        kDebug()<<"Blog Id do not sets correctly";
		return;
	}
	clearEntriesList();
	statusbar->showMessage(i18n("Entries list received."), STATUSTIMEOUT);
    this->unsetCursor();
	listEntries = __db->listPostsTitle(blog_id);
	lstEntriesList->addItems(listEntries.keys());
}

void Toolbox::sltLoadCategoryListFromDB(int blog_id)
{
	kDebug();
	if(blog_id==-1){
		kDebug()<<"Blog Id do not sets correctly";
		return;
	}
	statusbar->showMessage(i18n("Category list received."), STATUSTIMEOUT);
	this->unsetCursor();
	clearCatList();
	listCategories = __db->listCategories(blog_id);
	
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
	kDebug();
    Backend *entryB = new Backend(listBlogs.value(listBlogRadioButtons.checkedButton()->text()));
	entryB->getEntriesListFromServer(count);
	connect(entryB, SIGNAL(sigEntriesListFetched(int)), this, SLOT(sltLoadEntriesFromDB(int)));
    connect(entryB, SIGNAL(sigError(QString&)), this, SLOT(sltError(QString&)));
	statusbar->showMessage(i18n("Requesting Entry list..."));
    this->setCursor(Qt::BusyCursor);
}

void Toolbox::resetFields()
{
	kDebug();
	for(int i=0; i<listCategoryCheckBoxes.count(); ++i){
		listCategoryCheckBoxes[i]->setChecked(false);
	}
	txtCatTags->clear();
	chkOptionsTime->setChecked(false);
	datetimeOptionstimestamp->setDateTime(QDateTime::currentDateTime());
	txtOptionsTrackback->clear();
    txtSummary->setPlainText(QString());
    chkOptionsComments->setChecked(true);
    chkOptionsTrackback->setChecked(true);
    comboOptionsStatus->setCurrentIndex(0);
}

void Toolbox::clearCatList()
{
    kDebug();
	listCategories.clear();
	for(int j=0; j<listCategoryCheckBoxes.count(); ++j){
		delete(listCategoryCheckBoxes[j]);
	}
}

void Toolbox::clearEntriesList()
{
    kDebug();
	lstEntriesList->clear();
	listEntries.clear();
}

void Toolbox::sltCurrentBlogChanged(int blog_id)
{
	///TODO Save current state to a temporary variable!
    kDebug();
    if(blog_id==-1){
        kDebug()<<"Blog id do not sets correctly";
        return;
    }
    __currentBlogId = blog_id;
	sltLoadCategoryListFromDB(blog_id);
	sltLoadEntriesFromDB(blog_id);
    Qt::LayoutDirection ll = __db->getBlogInfo(blog_id)->direction();
	frameCat->setLayoutDirection(ll);
	lstEntriesList->setLayoutDirection(ll);
}

BilboPost * Toolbox::getFieldsValue()
{
	kDebug();
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
    currentPost->setSummary(txtSummary->toPlainText());
	return currentPost;
}

void Toolbox::setFieldsValue(const BilboPost & post)
{
    kDebug();
//     kDebug()<<"New Post is: "<<post.toString();
    delete currentPost;
    currentPost = new BilboPost(post);
    setSelectedCategories(post.categories());
    txtCatTags->setText(post.tags().join(", "));
    comboOptionsStatus->setCurrentIndex(post.position());
    if(post.position()!=BilboPost::Local && post.isPrivate())
        comboOptionsStatus->setCurrentIndex(1);
    chkOptionsComments->setChecked(post.isCommentAllowed());
    chkOptionsTrackback->setChecked(post.isTrackBackAllowed());
    datetimeOptionstimestamp->setDateTime(post.mTime());
    txtSummary->setPlainText(post.summary());
//     txtOptionsTrackback->setText(post.);
}

QStringList Toolbox::selectedCategoriesTitle()
{
	kDebug();
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
	kDebug()<<"NOT IMPLEMENTED YET!";
	return QList<int>();
}

void Toolbox::setSelectedCategories(const QStringList &list)
{
    unCheckCatList();
	for( int i=0; i<listCategoryCheckBoxes.count(); ++i){
		if(list.contains(listCategoryCheckBoxes[i]->text(), Qt::CaseInsensitive))
			listCategoryCheckBoxes[i]->setChecked(true);
	}
}

void Toolbox::setSelectedCategories(const QList< int > &)
{
	///TODO: Implement it
	kDebug()<<"NOT IMPLEMENTED YET!";
}

QStringList Toolbox::currentTags()
{
	kDebug();
	QStringList t;
	t = txtCatTags->text().split(',', QString::SkipEmptyParts);
	for(int i=0; i < t.count() ; ++i){
		t[i] = t[i].trimmed();
	}
	return t;
}

int Toolbox::currentBlogId()
{
    kDebug();
    if(listBlogRadioButtons.checkedButton()){
        int id = listBlogs.value(listBlogRadioButtons.checkedButton()->text(), -1);
        kDebug()<<id;
        return id;
    }
	else
		return -1;
}

void Toolbox::sltEntrySelected(QListWidgetItem * item)
{
    kDebug();
    BilboPost *post = __db->getPostInfo(listEntries.value(item->text()));
//     setFieldsValue(*post);
    kDebug()<<"Emiting sigEntrySelected...";
	Q_EMIT sigEntrySelected(post);
}

void Toolbox::setCurrentBlog(int blog_id)
{
    kDebug();
    QString blogTitle = listBlogs.key(blog_id);
    foreach( QAbstractButton *b, listBlogRadioButtons.buttons()){
        if(b->text()==blogTitle){
            b->setChecked(true);
            break;
        }
    }
}

void Toolbox::setCurrentPage(int index)
{
    box->setCurrentIndex(index);
}

void Toolbox::sltEntriesCopyUrl()
{
    QClipboard *clip = QApplication::clipboard();
    BilboPost *p = __db->getPostInfo(listEntries.value(lstEntriesList->currentItem()->text()));
    clip->setText(p->postLink().toString());
}

Toolbox::~Toolbox()
{
//     delete addEditBlogWindow;
//     delete blogToEdit;
//     delete currentBlog;
//     delete currentPost;
//     delete statusbar;
}

void Toolbox::unCheckCatList()
{
    for(int j=0; j<listCategoryCheckBoxes.count(); ++j){
        listCategoryCheckBoxes[j]->setChecked(false);
    }
}

void Toolbox::setButtonsIcon()
{
    btnBlogAdd->setIcon(KIcon("list-add"));
    btnBlogEdit->setIcon(KIcon("edit-rename"));
    btnBlogRemove->setIcon(KIcon("list-remove"));
    btnEntriesReload->setIcon(KIcon("view-refresh"));
    btnEntriesUpdate->setIcon(KIcon("arrow-down"));
    btnEntriesCopyUrl->setIcon(KIcon("edit-copy"));
    btnCatReload->setIcon(KIcon("view-refresh"));
    btnCatAdd->setIcon(KIcon("list-add"));
}

#include "toolbox.moc"
