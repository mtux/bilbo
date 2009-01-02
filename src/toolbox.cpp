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
#include <kdatetime.h>
#include <kurl.h>

#include "toolbox.h"
#include "entriescountdialog.h"
#include "addeditblog.h"
#include "global.h"
#include "backend.h"
#include "bilbopost.h"
#include "bilboblog.h"
#include "blogradiobutton.h"
#include "catcheckbox.h"

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
// 	currentPost = new BilboPost();
	optionsDate->setDate(QDateTime::currentDateTime().date());
	optionsTime->setTime(QDateTime::currentDateTime().time());
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
    blogToEdit = qobject_cast<BlogRadioButton*>(listBlogRadioButtons.checkedButton());
	
	addEditBlogWindow = new AddEditBlog(blogToEdit->blogId(), this);
    addEditBlogWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(addEditBlogWindow, SIGNAL(sigBlogEdited(BilboBlog&)), this, SLOT(sltBlogEdited(BilboBlog&)));
	addEditBlogWindow->show();
}

void Toolbox::sltRemoveBlog()
{
	kDebug();
    __db->removeBlog(qobject_cast<BlogRadioButton*>(listBlogRadioButtons.checkedButton())->blogId());
//     listBlogs.remove(listBlogRadioButtons.checkedButton()->text());
	QAbstractButton *tmp = listBlogRadioButtons.checkedButton();
    listBlogRadioButtons.removeButton(listBlogRadioButtons.checkedButton());
	delete tmp;
	clearEntriesList();
	clearCatList();
	txtCatTags->clear();
}

void Toolbox::sltBlogAdded(BilboBlog &addedBlog)
{
	kDebug();
// 	listBlogs.insert(addedBlog.title(), addedBlog.id());
    BlogRadioButton *a = new BlogRadioButton(addedBlog.title());
	a->setToolTip(addedBlog.blogUrl());
    a->setBlogId(addedBlog.id());
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
//     listBlogs.remove(blogToEdit->text());
	blogToEdit->setText(editedBlog.title());
	blogToEdit->setToolTip(editedBlog.blogUrl());
//     listBlogs.insert(editedBlog.title(), editedBlog.id());
//     Q_EMIT sigCurrentBlogChanged(listBlogs.value(listBlogRadioButtons.checkedButton()->text(), -1));
    sltCurrentBlogChanged(qobject_cast<BlogRadioButton*>(listBlogRadioButtons.checkedButton())->blogId());
    sltReloadCategoryList();
    delete addEditBlogWindow;
}

void Toolbox::reloadBlogList()
{
	kDebug();
// 	listBlogs.clear();
    QMap<QString, int> listBlogs;
    listBlogs = __db->listBlogsTitle();
    foreach( QAbstractButton *ab,listBlogRadioButtons.buttons()){
		delete ab;
	}
	listBlogRadioButtons.buttons().clear();
    QMap<QString, int>::const_iterator i = listBlogs.constBegin();
    QMap<QString, int>::const_iterator endI = listBlogs.constEnd();
	for( ; i != endI; ++i ){
        BlogRadioButton *rb = new BlogRadioButton(i.key());
        rb->setBlogId(i.value());
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
		KMessageBox::sorry(this, i18n("There isn't any selected blog, you have to select a blog from Blogs page before asking for Category list"));
		return;
	}
	
	int blog_id;
	
    blog_id = qobject_cast<BlogRadioButton*>(listBlogRadioButtons.checkedButton())->blogId();

	Backend *b = new Backend(blog_id);
	b->getCategoryListFromServer();
	connect(b, SIGNAL(sigCategoryListFetched(int)), this, SLOT(sltLoadCategoryListFromDB(int)));
    connect(b, SIGNAL(sigError(QString&)), this, SIGNAL(sigError(QString&)));
	statusbar->showMessage(i18n("Requesting category list..."));
    this->setCursor(Qt::BusyCursor);
	parentWidget()->setCursor(Qt::BusyCursor);
}

void Toolbox::sltReloadEntries()
{
	kDebug();
    if(!listBlogRadioButtons.checkedButton()){
		KMessageBox::sorry(this, i18n("There isn't any selected blog, you have to select a blog from Blogs page before asking for Entries list"));
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
//     int id = listBlogs.value(listBlogRadioButtons.checkedButton()->text(), -1);
    int id = qobject_cast<BlogRadioButton*>(listBlogRadioButtons.checkedButton())->blogId();
    kDebug()<<"Current Blog Id: "<<id;
// 	if(checked){
// 		currentBlog = dynamic_cast<BlogRadioButton*>(sender());
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
	parentWidget()->unsetCursor();
	QMap<int, QString> listEntries;
	listEntries = __db->listPostsTitle(blog_id);
	QMap<int, QString>::const_iterator endIt = listEntries.constEnd();
	QMap<int, QString>::const_iterator it = listEntries.constBegin();
	for(; it!=endIt; ++it){
		QListWidgetItem *lstItem = new QListWidgetItem(it.value());
		lstItem->setData(32, it.key());
		lstEntriesList->addItem(lstItem);
	}
// 	lstEntriesList->addItems(listEntries.keys());
}

void Toolbox::sltLoadCategoryListFromDB(int blog_id)
{
	kDebug();
	if(blog_id==-1){
		kDebug()<<"Blog Id do not sets correctly";
		return;
	}
	statusbar->showMessage(i18n("Category list received."), STATUSTIMEOUT);
	parentWidget()->unsetCursor();
	this->unsetCursor();
	clearCatList();
	QList<Category> listCategories;
	listCategories = __db->listCategories(blog_id);
	
	listCategoryCheckBoxes.clear();
	QList<Category>::const_iterator i;
	QList<Category>::const_iterator endIt= listCategories.constEnd();
	for( i = listCategories.constBegin(); i != endIt; ++i ){
		CatCheckBox *cb = new CatCheckBox(i->name, this);
		cb->setCategory(*i);
		listCategoryCheckBoxes.append(cb);
		frameCat->layout()->addWidget(cb);
	}
}

void Toolbox::sltGetEntriesCount(int count)
{
	kDebug();
    Backend *entryB = new Backend(qobject_cast<BlogRadioButton*>(listBlogRadioButtons.checkedButton())->blogId());
	entryB->getEntriesListFromServer(count);
	connect(entryB, SIGNAL(sigEntriesListFetched(int)), this, SLOT(sltLoadEntriesFromDB(int)));
    connect(entryB, SIGNAL(sigError(QString&)), this, SIGNAL(sigError(QString&)));
	statusbar->showMessage(i18n("Requesting Entry list..."));
    this->setCursor(Qt::BusyCursor);
	parentWidget()->setCursor(Qt::BusyCursor);
}

void Toolbox::resetFields()
{
	kDebug();
	unCheckCatList();
	txtCatTags->clear();
	chkOptionsTime->setChecked(false);
	optionsDate->setDate(QDateTime::currentDateTime().date());
	optionsTime->setTime(QDateTime::currentDateTime().time());
	txtOptionsTrackback->clear();
    txtSummary->setPlainText(QString());
    chkOptionsComments->setChecked(true);
    chkOptionsTrackback->setChecked(true);
    comboOptionsStatus->setCurrentIndex(0);
}

void Toolbox::clearCatList()
{
    kDebug();
// 	listCategories.clear();
	qDeleteAll(listCategoryCheckBoxes.constBegin(), listCategoryCheckBoxes.constEnd());
// 	for(int j=0; j<listCategoryCheckBoxes.count(); ++j){
// 		delete(listCategoryCheckBoxes[j]);
// 	}
}

void Toolbox::clearEntriesList()
{
    kDebug();
	lstEntriesList->clear();
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

void Toolbox::getFieldsValue(BilboPost *currentPost)
{
	kDebug();
	currentPost->setCategoryList(this->selectedCategories());
	currentPost->setTags(this->currentTags());
	currentPost->setModifyTimeStamp(this->chkOptionsTime->isChecked());
	if(currentPost->status()==KBlog::BlogPost::Fetched || currentPost->status()==KBlog::BlogPost::Modified){///FIXME there is a BUG here!
		if(chkOptionsTime->isChecked())
			currentPost->setModificationDateTime(KDateTime(optionsDate->date(), optionsTime->time()));
		else
            currentPost->setModificationDateTime(KDateTime::currentLocalDateTime());
	} else {
		if(chkOptionsTime->isChecked()){
			currentPost->setModificationDateTime(KDateTime(optionsDate->date(), optionsTime->time()));
//             currentPost->setCreationDateTime(KDateTime(datetimeOptionstimestamp->dateTime()));
		}
		else {
            currentPost->setModificationDateTime(KDateTime::currentLocalDateTime());
// 			currentPost->setCreationDateTime(KDateTime::currentLocalDateTime());
		}
	}

	currentPost->setPrivate((comboOptionsStatus->currentIndex()==1)?true:false);
	currentPost->setCommentAllowed(chkOptionsComments->isChecked());
	currentPost->setTrackBackAllowed(chkOptionsTrackback->isChecked());
	currentPost->setPosition((BilboPost::Position)comboOptionsStatus->currentIndex());
    currentPost->setSummary(txtSummary->toPlainText());
}

void Toolbox::setFieldsValue(BilboPost* post)
{
	kDebug();
//     kDebug()<<"New Post is: "<<post.toString();
	//delete currentPost;
	if (post == 0) {
		resetFields();
// 		currentPost = post;
	}

	setSelectedCategories(post->categories());
	txtCatTags->setText(post->tags().join(", "));
	comboOptionsStatus->setCurrentIndex(post->position());
	if(post->position()!=BilboPost::Local && post->isPrivate())
        comboOptionsStatus->setCurrentIndex(1);
	chkOptionsComments->setChecked(post->isCommentAllowed());
	chkOptionsTrackback->setChecked(post->isTrackBackAllowed());
	chkOptionsTime->setChecked(post->isModifyTimeStamp());
	optionsTime->setTime(post->modificationDateTime().time());
	optionsDate->setDate(post->modificationDateTime().date());
	txtSummary->setPlainText(post->summary());
//     txtOptionsTrackback->setText(post.);
}

QList< Category > Toolbox::selectedCategories()
{
	kDebug();
	QList<Category> list;
	int count = listCategoryCheckBoxes.count();
	for( int i=0; i<count; ++i){
		if(listCategoryCheckBoxes[i]->isChecked())
			list.append(listCategoryCheckBoxes[i]->category());
	}
	return list;
}

QStringList Toolbox::selectedCategoriesTitle()
{
	kDebug();
	QStringList list;
	int count = listCategoryCheckBoxes.count();
	for( int i=0; i<count; ++i){
		if(listCategoryCheckBoxes[i]->isChecked())
			list.append(listCategoryCheckBoxes[i]->category().name);
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
	int count = listCategoryCheckBoxes.count();
	for( int i=0; i<count; ++i){
		if(list.contains(listCategoryCheckBoxes[i]->category().name, Qt::CaseInsensitive))
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
        int id = qobject_cast<BlogRadioButton*>(listBlogRadioButtons.checkedButton())->blogId();
        kDebug()<<id;
        return id;
    }
	else
		return -1;
}

void Toolbox::sltEntrySelected(QListWidgetItem * item)
{
    kDebug();
	BilboPost *post = __db->getPostInfo(lstEntriesList->currentItem()->data(32).toInt());
//     setFieldsValue(*post);
    kDebug()<<"Emiting sigEntrySelected...";
	Q_EMIT sigEntrySelected(post);
}

void Toolbox::setCurrentBlog(int blog_id)
{
    kDebug();
//     QString blogTitle = listBlogs.key(blog_id);
    foreach( QAbstractButton *b, listBlogRadioButtons.buttons()){
        if(qobject_cast<BlogRadioButton*>(b)->blogId()==blog_id){
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
	if(lstEntriesList->currentItem()==0){
		KMessageBox::sorry(this, i18n("There isn't any selected entry!\nIn order to use this function you have to select an entry first."));
		return;
	}
	QClipboard *clip = QApplication::clipboard();
	BilboPost *p = __db->getPostInfo(lstEntriesList->currentItem()->data(32).toInt());
    clip->setText(p->link().url());
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
	int count = listCategoryCheckBoxes.count();
    for(int j=0; j<count; ++j){
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
    ///TODO Add option for selecting only text or only Icon for Toolbox buttons!
    btnBlogAdd->setText(QString());
    btnBlogEdit->setText(QString());
    btnBlogRemove->setText(QString());
    btnEntriesReload->setText(QString());
    btnEntriesUpdate->setText(QString());
    btnEntriesCopyUrl->setText(QString());
    btnCatReload->setText(QString());
    btnCatAdd->setText(QString());
}


#include "toolbox.moc"
