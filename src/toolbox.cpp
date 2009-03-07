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

#include <kstatusbar.h>
#include <kdebug.h>
#include <kxmlguiwindow.h>
#include <kmessagebox.h>
#include <QButtonGroup>
#include <kdatetime.h>
#include <kurl.h>
#include <QDir>

#include "toolbox.h"
#include "dbman.h"
#include "entriescountdialog.h"
#include "addeditblog.h"
#include "global.h"
#include "backend.h"
#include "bilbopost.h"
#include "bilboblog.h"
#include "blogradiobutton.h"
#include "catcheckbox.h"

Toolbox::Toolbox( QWidget *parent )
        : QWidget( parent )
{
    kDebug();
    if ( parent )
        this->statusbar = qobject_cast<KXmlGuiWindow*>( parent )->statusBar();
    else
        this->statusbar = new KStatusBar( this );
    setupUi( this );
    setButtonsIcon();
    frameBlog->layout()->setAlignment( Qt::AlignTop );
    frameCat->layout()->setAlignment( Qt::AlignTop );
    reloadBlogList();
    optionsDate->setDate( QDateTime::currentDateTime().date() );
    optionsTime->setTime( QDateTime::currentDateTime().time() );
    connect( btnBlogAdd, SIGNAL( clicked() ), this, SLOT( sltAddBlog() ) );
    connect( btnBlogEdit, SIGNAL( clicked() ), this, SLOT( sltEditBlog() ) );
    connect( btnBlogRemove, SIGNAL( clicked() ), this, SLOT( sltRemoveBlog() ) );

    connect( btnCatReload, SIGNAL( clicked() ), this, SLOT( sltReloadCategoryList() ) );
    connect( btnEntriesUpdate, SIGNAL( clicked() ), this, SLOT( sltUpdateEntries() ) );

    connect( this, SIGNAL( sigCurrentBlogChanged( int ) ), this, SLOT( sltCurrentBlogChanged( int ) ) );
    connect( &listBlogRadioButtons, SIGNAL( buttonClicked( int ) ), this, SLOT( sltSetCurrentBlog() ) );

    connect( lstEntriesList, SIGNAL( itemDoubleClicked( QListWidgetItem* ) ),
             this, SLOT( sltEntrySelected( QListWidgetItem* ) ) );
    connect( btnEntriesCopyUrl, SIGNAL( clicked( bool ) ), this, SLOT( sltEntriesCopyUrl() ) );
    connect( btnEntriesRemove, SIGNAL( clicked(bool) ), this, SLOT( sltRemoveSelectedEntryFromServer() ) );

    connect( localEntriesTable, SIGNAL( cellDoubleClicked(int,int) ),
             this, SLOT(sltLocalEntrySelected(int,int)) );
    connect( btnLocalRemove, SIGNAL( clicked( bool ) ) , this, SLOT( sltRemoveLocalEntry() ) );

    lblOptionsTrackBack->setVisible( false );
    txtOptionsTrackback->setVisible( false );
    btnCatAdd->setVisible( false );
    btnEntriesReload->setVisible(false);

    QTimer::singleShot(1000, this, SLOT(reloadLocalPosts()));
}

void Toolbox::sltAddBlog()
{
    kDebug();
    AddEditBlog *addEditBlogWindow = new AddEditBlog( -1, this );
    addEditBlogWindow->setAttribute( Qt::WA_DeleteOnClose );
    addEditBlogWindow->show();
    connect( addEditBlogWindow, SIGNAL( sigBlogAdded( BilboBlog& ) ), this, SLOT( sltBlogAdded( BilboBlog& ) ) );
}

void Toolbox::sltEditBlog()
{
    kDebug();
    blogToEdit = qobject_cast<BlogRadioButton*>( listBlogRadioButtons.checkedButton() );
    if ( !blogToEdit ) {
        KMessageBox::sorry( this, i18n( "There isn't any selected blog, you have to select a blog first." ) );
        return;
    }
//  blogToEditDir = QDir(DATA_DIR + DBMan::self()->getBlogInfo(blogToEdit->blogId())->blogUrl());

    AddEditBlog *addEditBlogWindow = new AddEditBlog( blogToEdit->blogId(), this );
    addEditBlogWindow->setAttribute( Qt::WA_DeleteOnClose );
    connect( addEditBlogWindow, SIGNAL( sigBlogEdited( BilboBlog& ) ), this, SLOT( sltBlogEdited( BilboBlog& ) ) );
    addEditBlogWindow->show();
}

void Toolbox::sltRemoveBlog()
{
    kDebug();
    QAbstractButton *btn = listBlogRadioButtons.checkedButton();
    if(!btn){
        KMessageBox::sorry( this, i18n( "There isn't any selected blog, you have to select a blog first." ) );
        return;
    }
    if(KMessageBox::warningYesNo(this, i18n("Are you sure of removing selected blog?")) 
        == KMessageBox::NoExec)
        return;

    DBMan::self()->removeBlog( qobject_cast<BlogRadioButton*>( btn )->blogId() );
    emit sigCurrentBlogChanged( -1 );
    listBlogRadioButtons.removeButton( btn );
    btn->deleteLater();
    lstEntriesList->clear();
    clearCatList();
    txtCatTags->clear();
    sltSetCurrentBlog();
}

void Toolbox::sltBlogAdded( BilboBlog &addedBlog )
{
    kDebug();
    mBlogList[ addedBlog.id() ] = new BilboBlog(addedBlog, this);
    BlogRadioButton *a = new BlogRadioButton( addedBlog.title() );
    a->setToolTip( addedBlog.blogUrl() );
    a->setBlogId( addedBlog.id() );
    listBlogRadioButtons.addButton( a );
    frameBlog->layout()->addWidget( a );
    a->setChecked( true );
    sltReloadCategoryList();
    sltSetCurrentBlog();

}

void Toolbox::sltBlogEdited( BilboBlog &editedBlog )
{
    kDebug();
    mBlogList[ editedBlog.id() ] = new BilboBlog(editedBlog, this);
    blogToEdit->setText( editedBlog.title() );
    blogToEdit->setToolTip( editedBlog.blogUrl() );
    blogToEdit->setChecked( true );
    sltCurrentBlogChanged( blogToEdit->blogId() );
    sltReloadCategoryList();
}

void Toolbox::reloadBlogList()
{
    kDebug();
    foreach( QAbstractButton *ab, listBlogRadioButtons.buttons() ) {
        delete ab;
    }
    listBlogRadioButtons.buttons().clear();
    mBlogList.clear();
    QList<BilboBlog*> listBlogs = DBMan::self()->listBlogs();
    int count = listBlogs.count();
    for ( int i = 0; i < count; ++i ) {
        mBlogList [ listBlogs[i]->id() ] = listBlogs[i];
        BlogRadioButton *rb = new BlogRadioButton( listBlogs[i]->title(), this );
        rb->setBlogId( listBlogs[i]->id() );
        rb->setToolTip( listBlogs[i]->blogUrl() );
        listBlogRadioButtons.addButton( rb );
        frameBlog->layout()->addWidget( rb );
    }
}

void Toolbox::sltReloadCategoryList()
{
    kDebug();
    QAbstractButton *btn = listBlogRadioButtons.checkedButton();
    if ( !btn ) {
        KMessageBox::sorry( this, i18n( "There isn't any selected blog, \
you have to select a blog from Blogs page before asking for Category list" ) );
        return;
    }

    int blog_id;

    blog_id = qobject_cast<BlogRadioButton*>( btn )->blogId();

    Backend *b = new Backend( blog_id );
    b->getCategoryListFromServer();
    connect( b, SIGNAL( sigCategoryListFetched( int ) ), this, SLOT( sltLoadCategoryListFromDB( int ) ) );
    connect( b, SIGNAL( sigError( const QString& ) ), this, SIGNAL( sigError( const QString& ) ) );
    statusbar->showMessage( i18n( "Requesting category list..." ) );
    this->setCursor( Qt::BusyCursor );
    emit sigBusy( true );
}

void Toolbox::sltUpdateEntries()
{
    kDebug();
    if ( !listBlogRadioButtons.checkedButton() ) {
        KMessageBox::sorry( this, i18n( "There isn't any selected blog, \
you have to select a blog from Blogs page before asking for Entries list" ) );
        kDebug() << "There isn't any selected blog.";
        return;
    }
    EntriesCountDialog *dia = new EntriesCountDialog( this );
    dia->setAttribute( Qt::WA_DeleteOnClose );
    connect( dia, SIGNAL( sigAccepted( int ) ), this, SLOT( sltGetEntriesCount( int ) ) );
    dia->show();
}

void Toolbox::sltGetEntriesCount( int count )
{
    kDebug();
    Backend *entryB = new Backend( qobject_cast<BlogRadioButton*>( listBlogRadioButtons.checkedButton() )->blogId(),
                                   this);
    entryB->getEntriesListFromServer( count );
    connect( entryB, SIGNAL( sigEntriesListFetched( int ) ), this, SLOT( sltLoadEntriesFromDB( int ) ) );
    connect( entryB, SIGNAL( sigError( const QString& ) ), this, SIGNAL( sigError( const QString& ) ) );
    statusbar->showMessage( i18n( "Requesting Entries list..." ) );
    this->setCursor( Qt::BusyCursor );
    emit sigBusy( true );
}

void Toolbox::sltSetCurrentBlog()
{
    kDebug();
    int id = -1;
    QAbstractButton *btn = listBlogRadioButtons.checkedButton();
    if(btn){
        kDebug() << "Current blog is: " << btn->text();
        id = qobject_cast<BlogRadioButton*>( btn )->blogId();
    }
    kDebug() << "Current Blog Id: " << id;
    emit sigCurrentBlogChanged( id );
}

void Toolbox::sltLoadEntriesFromDB( int blog_id )
{
    kDebug();
    if ( blog_id == -1 ) {
        kDebug() << "Blog Id doesn't set correctly";
        return;
    }
    lstEntriesList->clear();
    QList<QVariantMap> listEntries;
    listEntries = DBMan::self()->listPostsInfo( blog_id );
    int count = listEntries.count();
    for ( int i=0; i < count; ++i ) {
        QListWidgetItem *lstItem = new QListWidgetItem( listEntries[i].value("title").toString() );
        lstItem->setToolTip(listEntries[i].value("m_time").toDateTime().toString());
        if(listEntries[i].value("is_private").toBool())
            lstItem->setForeground(QBrush(Qt::blue));
        lstItem->setData( 32, listEntries[i].value("id").toInt() );
        lstEntriesList->addItem( lstItem );
    }
    statusbar->showMessage( i18n( "Entries list received." ), STATUSTIMEOUT );
    this->unsetCursor();
    emit sigBusy( false );
}

void Toolbox::sltLoadCategoryListFromDB( int blog_id )
{
    kDebug();
    if ( blog_id == -1 ) {
        kDebug() << "Blog Id do not sets correctly";
        return;
    }
    clearCatList();
    QList<Category> listCategories;
    listCategories = DBMan::self()->listCategories( blog_id );

    QList<Category>::const_iterator i;
    QList<Category>::const_iterator endIt = listCategories.constEnd();
    for ( i = listCategories.constBegin(); i != endIt; ++i ) {
        CatCheckBox *cb = new CatCheckBox( i->name, this );
        cb->setCategory( *i );
        listCategoryCheckBoxes.append( cb );
        frameCat->layout()->addWidget( cb );
    }
    statusbar->showMessage( i18n( "Category list received." ), STATUSTIMEOUT );
    this->unsetCursor();
    emit sigBusy( false );
}

void Toolbox::sltRemoveSelectedEntryFromServer()
{
    if(lstEntriesList->selectedItems().count() < 1)
        return;
    if( KMessageBox::warningYesNoCancel(this, i18n( "Removing a post from your blog is not undoable!\
\nAre you sure of removing post with title \"%1\" from your blog?", lstEntriesList->currentItem()->text() ))
    == KMessageBox::Yes) {
        BilboPost post = DBMan::self()->getPostInfo( lstEntriesList->currentItem()->data(32).toInt() );
        Backend *b = new Backend(currentBlogId(), this);
        connect(b, SIGNAL(sigPostRemoved(int,const BilboPost&)), this, SLOT(slotPostRemoved(int,const BilboPost&)) );
        b->removePost(post);
        statusbar->showMessage( i18n( "Removing post..." ) );
    }
}

void Toolbox::slotPostRemoved( int blog_id, const BilboPost &post )
{
    KMessageBox::information( this, i18n( "Post with title \"%1\" removed from \"%2\".",
                                          post.title(), mBlogList.value(blog_id)->title() ) );
    sltLoadEntriesFromDB( blog_id );
    statusbar->showMessage( i18n( "Post removed" ), STATUSTIMEOUT );
    sender()->deleteLater();
}

void Toolbox::resetFields()
{
    kDebug();
    unCheckCatList();
    txtCatTags->clear();
    chkOptionsTime->setChecked( false );
    optionsDate->setDate( QDateTime::currentDateTime().date() );
    optionsTime->setTime( QDateTime::currentDateTime().time() );
    txtOptionsTrackback->clear();
    txtSummary->clear();
    chkOptionsComments->setChecked( true );
    chkOptionsTrackback->setChecked( true );
    comboOptionsStatus->setCurrentIndex( 0 );
}

void Toolbox::clearCatList()
{
    kDebug();
    foreach( CatCheckBox* cat, listCategoryCheckBoxes ){
        cat->deleteLater();
    }
    listCategoryCheckBoxes.clear();
}

void Toolbox::sltCurrentBlogChanged( int blog_id )
{
    kDebug();
    if ( blog_id == -1 ) {
        kDebug() << "Blog id do not sets correctly";
        btnBlogEdit->setEnabled( false );
        btnBlogRemove->setEnabled( false );
        return;
    }
    if( __currentBlogId == blog_id )
        return;
    btnBlogEdit->setEnabled( true );
    btnBlogRemove->setEnabled( true );
    __currentBlogId = blog_id;
    sltLoadCategoryListFromDB( blog_id );
    sltLoadEntriesFromDB( blog_id );
    Qt::LayoutDirection ll = mBlogList.value(blog_id)->direction();
    frameCat->setLayoutDirection( ll );
    lstEntriesList->setLayoutDirection( ll );
}

void Toolbox::getFieldsValue( BilboPost &currentPost )
{
    kDebug();
    currentPost.setCategoryList( this->selectedCategories() );
    currentPost.setTags( this->currentTags() );
    currentPost.setModifyTimeStamp( this->chkOptionsTime->isChecked() );
    if ( currentPost.status() == KBlog::BlogPost::New ) {
        if ( chkOptionsTime->isChecked() ) {
            currentPost.setModificationDateTime( KDateTime( optionsDate->date(), optionsTime->time() ) );
            currentPost.setCreationDateTime( KDateTime( optionsDate->date(), optionsTime->time() ) );
        } else {
            currentPost.setModificationDateTime( KDateTime::currentLocalDateTime() );
            currentPost.setCreationDateTime( KDateTime::currentLocalDateTime() );
        }
    } else {
        if ( chkOptionsTime->isChecked() ) {
            currentPost.setModificationDateTime( KDateTime( optionsDate->date(), optionsTime->time() ) );
            currentPost.setCreationDateTime( KDateTime( optionsDate->date(), optionsTime->time() ) );
        }
    }

    currentPost.setPrivate(( comboOptionsStatus->currentIndex() == 1 ) ? true : false );
    currentPost.setCommentAllowed( chkOptionsComments->isChecked() );
    currentPost.setTrackBackAllowed( chkOptionsTrackback->isChecked() );
    currentPost.setSummary( txtSummary->toPlainText() );
}

void Toolbox::setFieldsValue( BilboPost* post )
{
    kDebug();
//     kDebug()<<"New Post is: "<<post.toString();
    //delete currentPost;
    if ( post == 0 ) {
        resetFields();
        kDebug()<<"post is NULL";
        return;
    }

    setSelectedCategories( post->categories() );
    txtCatTags->setText( post->tags().join( ", " ) );
//     kDebug() << "Post status is: " << post->status();
    if ( post->status() == KBlog::BlogPost::New )
        comboOptionsStatus->setCurrentIndex( 2 );
    else if ( post->isPrivate() )
        comboOptionsStatus->setCurrentIndex( 1 );
    else
        comboOptionsStatus->setCurrentIndex( 0 );
    chkOptionsComments->setChecked( post->isCommentAllowed() );
    chkOptionsTrackback->setChecked( post->isTrackBackAllowed() );
    chkOptionsTime->setChecked( post->isModifyTimeStamp() );
    optionsTime->setTime( post->creationDateTime().time() );
    optionsDate->setDate( post->creationDateTime().date() );
    txtSummary->setPlainText( post->summary() );
    if( post->status() != BilboPost::New ) {
        chkOptionsTime->setChecked(true);
    }
}

QList< Category > Toolbox::selectedCategories()
{
    kDebug();
    QList<Category> list;
    int count = listCategoryCheckBoxes.count();
    for ( int i = 0; i < count; ++i ) {
        if ( listCategoryCheckBoxes[i]->isChecked() )
            list.append( listCategoryCheckBoxes[i]->category() );
    }
    return list;
}

QStringList Toolbox::selectedCategoriesTitle()
{
    kDebug();
    QStringList list;
    int count = listCategoryCheckBoxes.count();
    for ( int i = 0; i < count; ++i ) {
        if ( listCategoryCheckBoxes[i]->isChecked() )
            list.append( listCategoryCheckBoxes[i]->category().name );
    }
    return list;
}

QList< int > Toolbox::selectedCategoriesId()
{
    ///TODO Implement it
    kDebug() << "NOT IMPLEMENTED YET!";
    return QList<int>();
}

void Toolbox::setSelectedCategories( const QStringList &list )
{
    unCheckCatList();
    int count = listCategoryCheckBoxes.count();
    for ( int i = 0; i < count; ++i ) {
        if ( list.contains( listCategoryCheckBoxes[i]->category().name, Qt::CaseInsensitive ) )
            listCategoryCheckBoxes[i]->setChecked( true );
    }
}

void Toolbox::setSelectedCategories( const QList< int > & )
{
    ///TODO Implement it
    kDebug() << "NOT IMPLEMENTED YET!";
}

QStringList Toolbox::currentTags()
{
    kDebug();
    QStringList t;
    t = txtCatTags->text().split( ',', QString::SkipEmptyParts );
    for ( int i = 0; i < t.count() ; ++i ) {
        t[i] = t[i].trimmed();
    }
    return t;
}

int Toolbox::currentBlogId()
{
    kDebug();
    QAbstractButton *btn = listBlogRadioButtons.checkedButton();
    if ( btn ) {
        int id = qobject_cast<BlogRadioButton*>( btn )->blogId();
        kDebug() << id;
        return id;
    } else
        return -1;
}

const BilboBlog * Toolbox::currentBlog()
{
    return mBlogList.value( currentBlogId() );
}

void Toolbox::sltEntrySelected( QListWidgetItem * item )
{
    kDebug();
//     setFieldsValue(*post);
    BilboPost post = DBMan::self()->getPostInfo( item->data( 32 ).toInt() );
    kDebug() << "Emiting sigEntrySelected...";
    Q_EMIT sigEntrySelected( post, currentBlogId() );
}

void Toolbox::setCurrentBlog( int blog_id )
{
    kDebug();
    if(currentBlogId() == blog_id)
        return;
//     QString blogTitle = listBlogs.key(blog_id);
    foreach( QAbstractButton *b, listBlogRadioButtons.buttons() ) {
        if ( qobject_cast<BlogRadioButton*>( b )->blogId() == blog_id ) {
            b->setChecked( true );
            emit sigCurrentBlogChanged(blog_id);
            break;
        }
    }
}

void Toolbox::setCurrentPage( int index )
{
    box->setCurrentIndex( index );
}

void Toolbox::sltEntriesCopyUrl()
{
    if ( lstEntriesList->currentItem() == 0 ) {
        KMessageBox::sorry( this, i18n( "There isn't any selected entry!\nIn order to use this function you have to select an entry first." ) );
        return;
    }
    BilboPost post = DBMan::self()->getPostInfo( lstEntriesList->currentItem()->data( 32 ).toInt() );
    if( !post.permaLink().isEmpty() )
        QApplication::clipboard()->setText( post.permaLink().prettyUrl() );
    else if ( !post.link().isEmpty() )
        QApplication::clipboard()->setText( post.link().prettyUrl() );
    else
        KMessageBox::sorry(this, i18n( "Sorry, There isn't any link field available for this entry on Database." ) );
}

Toolbox::~Toolbox()
{
    kDebug();
}

void Toolbox::unCheckCatList()
{
    int count = listCategoryCheckBoxes.count();
    for ( int j = 0; j < count; ++j ) {
        listCategoryCheckBoxes[j]->setChecked( false );
    }
}

void Toolbox::setButtonsIcon()
{
    btnBlogAdd->setIcon( KIcon( "list-add" ) );
    btnBlogEdit->setIcon( KIcon( "document-edit" ) );
    btnBlogRemove->setIcon( KIcon( "list-remove" ) );
    btnEntriesReload->setIcon( KIcon( "view-refresh" ) );
    btnEntriesUpdate->setIcon( KIcon( "arrow-down" ) );
    btnEntriesCopyUrl->setIcon( KIcon( "edit-copy" ) );
    btnEntriesRemove->setIcon( KIcon( "list-remove" ) );
    btnCatReload->setIcon( KIcon( "view-refresh" ) );
    btnCatAdd->setIcon( KIcon( "list-add" ) );
    btnLocalRemove->setIcon( KIcon( "list-remove" ) );
    ///TODO Add option for selecting only text or only Icon for Toolbox buttons!
    btnBlogAdd->setText( QString() );
    btnBlogEdit->setText( QString() );
    btnBlogRemove->setText( QString() );
    btnEntriesReload->setText( QString() );
    btnEntriesUpdate->setText( QString() );
    btnEntriesCopyUrl->setText( QString() );
    btnEntriesRemove->setText( QString() );
    btnCatReload->setText( QString() );
    btnCatAdd->setText( QString() );
    btnLocalRemove->setText( QString() );
}

QMap<int, BilboBlog*> & Toolbox::blogList()
{
    return mBlogList;
}

void Toolbox::reloadLocalPosts()
{
    kDebug();
    localEntriesTable->clearContents();
    localEntriesTable->setRowCount(0);
    QList<QVariantMap> localList = DBMan::self()->listLocalPosts();
//     QList<QVariantMap>::ConstIterator it = localList.constBegin();
//     QList<QVariantMap>::ConstIterator endIt = localList.constEnd();
    int count = localList.count();
    kDebug()<<count;
    for (int i=0; i < count; ++i){
        int newRow = localEntriesTable->rowCount();
        localEntriesTable->insertRow(newRow);
        QTableWidgetItem *item1 = new QTableWidgetItem( localList[i].value( "post_title" ).toString() );
        item1->setData(32, localList[i].value( "local_id" ).toInt());//Post_id
        localEntriesTable->setItem( newRow, 0, item1 );
        QTableWidgetItem *item2 = new QTableWidgetItem( localList[i].value( "blog_title" ).toString() );
        item2->setData(32, localList[i].value( "blog_id" ).toInt());//blog_id
        localEntriesTable->setItem( newRow, 1, item2 );
    }
}

void Toolbox::sltLocalEntrySelected( int row, int column )
{
    kDebug()<<"Emitting sigEntrySelected...";
    Q_UNUSED(column);
    BilboPost post = DBMan::self()->localPost(localEntriesTable->item(row, 0)->data(32).toInt());
    emit sigEntrySelected( post, localEntriesTable->item(row, 1)->data(32).toInt() );
}

void Toolbox::sltRemoveLocalEntry()
{
    kDebug();
    if(localEntriesTable->selectedItems().count() > 0) {
        int local_id = localEntriesTable->item(0, localEntriesTable->currentRow())->data(32).toInt();
        if( KMessageBox::warningYesNo(this, i18n("Are you sure of removing selected local entry?")) 
            == KMessageBox::NoExec )
            return;

        if( DBMan::self()->removeLocalEntry(local_id) ) {
            localEntriesTable->removeRow(localEntriesTable->currentRow());
        } else {
            KMessageBox::detailedError(this, i18n("Cannot remove selected local entry!"), DBMan::self()->lastErrorText());
        }
    } else {
        KMessageBox::sorry(this, i18n("You have to select at least one entry from list."));
    }
}

#include "toolbox.moc"
