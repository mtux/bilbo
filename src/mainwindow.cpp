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


#include <ktabwidget.h>
#include <kstatusbar.h>
#include <kaction.h>
#include <KToggleAction>
#include <kactioncollection.h>
#include <kstandardaction.h>
#include <kconfigdialog.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <KDE/KLocale>
#include <QDir>
#include <QFile>
#include <KApplication>
#include <QMap>

#include "mainwindow.h"
#include "global.h"
#include "dbman.h"
#include "toolbox.h"
#include "postentry.h"
#include "addeditblog.h"
#include "backend.h"
#include "bilbomedia.h"
#include "settings.h"
#include "systray.h"
#include "bilboblog.h"
#include "editorsettings.h"

MainWindow::MainWindow(): KXmlGuiWindow(),
        tabPosts( new KTabWidget( this ) )
{
    kDebug();
    previousActivePostIndex = -1;
    busyNumber = 0;
    progress = 0;
    tabPosts->setElideMode( Qt::ElideRight );///TODO make this Optional!
    setCentralWidget( tabPosts );

    toolbox = new Toolbox( this );
    toolboxDock = new QDockWidget( i18n( "Post properties" ), this );
    toolboxDock->setObjectName( "dock_toolbox" );
    toolbox->setObjectName( "toolbox" );
    toolboxDock->setWidget( toolbox );
    this->addDockWidget( Qt::RightDockWidgetArea, toolboxDock );

    btnRemovePost = new QToolButton( tabPosts );
    btnRemovePost->setIcon( KIcon( "tab-close" ) );
    btnRemovePost->setToolTip( i18n( "Remove current post" ) );
    tabPosts->setCornerWidget( btnRemovePost, Qt::TopRightCorner );
    connect( btnRemovePost, SIGNAL( clicked( bool ) ), this, SLOT( sltRemoveCurrentPostEntry() ) );

    // then, setup our actions
    setupActions();

    // add a status bar
    statusBar()->show();
//     toolbox->show();

    // a call to KXmlGuiWindow::setupGUI() populates the GUI
    // with actions, using KXMLGUI.
    // It also applies the saved mainwindow settings, if any, and ask the
    // mainwindow to automatically save settings if changed: window size,
    // toolbar position, icon size, etc.
    setupGUI();


    sltCreateNewPost();
    activePost = qobject_cast<PostEntry*>( tabPosts->currentWidget() );
    previousActivePostIndex = 0;
    toolbox->setFieldsValue( activePost->currentPost() );

//     this->setWindowIcon(KIcon(":/media/bilbo.png"));
//     readConfig();
    toolbox->setVisible( Settings::show_toolbox_on_start() );
    actToggleToolboxVisible->setChecked( Settings::show_toolbox_on_start() );
//     if ( Settings::show_toolbox_on_start() ) {
//         actToggleToolboxVisible->setText( i18n( "Hide Toolbox" ) );
//     } else {
//         actToggleToolboxVisible->setText( i18n( "Show Toolbox" ) );
//     }
    setupSystemTray();

    connect( tabPosts, SIGNAL( currentChanged( int ) ), this, SLOT( sltActivePostChanged( int ) ) );
    connect( toolbox, SIGNAL( sigEntrySelected( BilboPost * ) ), this, SLOT( sltNewPostOpened( BilboPost* ) ) );
    connect( toolbox, SIGNAL( sigCurrentBlogChanged( int ) ), this, SLOT( sltCurrentBlogChanged( int ) ) );
    connect( toolbox, SIGNAL( sigError( const QString& ) ), this, SLOT( sltError( const QString& ) ) );
    connect( toolbox, SIGNAL( sigBusy(bool) ), this, SLOT( slotBusy(bool) ));

//     if(Settings::show_main_on_start())
//         this->show();
//     else
//         this->hide();
}

MainWindow::~MainWindow()
{
    kDebug();
    writeConfigs();
}

void MainWindow::setupActions()
{
    KStandardAction::quit( kapp, SLOT( quit() ), actionCollection() );

    KStandardAction::preferences( this, SLOT( optionsPreferences() ), actionCollection() );

    // custom menu and menu item - the slot is in the class bilboView
    actNewPost = new KAction( KIcon( "document-new" ), i18n( "New Post" ), this );
    actionCollection()->addAction( QLatin1String( "new_post" ), actNewPost );
    actNewPost->setShortcut( Qt::CTRL + Qt::Key_N );
    connect( actNewPost, SIGNAL( triggered( bool ) ), this, SLOT( sltCreateNewPost() ) );

    actAddBlog = new KAction( KIcon( "list-add" ), i18n( "Add Blog" ), this );
    actionCollection()->addAction( QLatin1String( "add_blog" ), actAddBlog );
    connect( actAddBlog, SIGNAL( triggered( bool ) ), toolbox, SLOT( sltAddBlog() ) );

    actUploadAll = new KAction( KIcon( "arrow-up-double" ), i18n( "Upload All Changes" ), this );
    actionCollection()->addAction( QLatin1String( "upload_all" ), actUploadAll );
    connect( actUploadAll, SIGNAL( triggered( bool ) ), this, SLOT( sltUploadAllChanges() ) );

    actPublish = new KAction( KIcon( "arrow-up" ), i18n( "Publish" ), this );
    actionCollection()->addAction( QLatin1String( "publish_post" ), actPublish );
    connect( actPublish, SIGNAL( triggered( bool ) ), this, SLOT( sltPublishPost() ) );

    actSave = new KAction( KIcon( "document-save" ), i18n( "Save" ), this );
    actionCollection()->addAction( QLatin1String( "save" ), actSave );
    connect( actSave, SIGNAL( triggered( bool ) ), this, SLOT( sltSavePostLocally() ) );

    actSaveLocally = new KAction( KIcon( "document-save" ), i18n( "Save Locally" ), this );
    actionCollection()->addAction( QLatin1String( "save_locally" ), actSaveLocally );
    actSaveLocally->setShortcut( Qt::CTRL + Qt::Key_S );
    connect( actSaveLocally, SIGNAL( triggered( bool ) ), this, SLOT( sltSavePostLocally() ) );

    actSaveDraft = new KAction( KIcon( "document-save-as" ), i18n( "Save as Draft" ), this );
    actionCollection()->addAction( QLatin1String( "save_draft" ), actSaveDraft );
    actSaveDraft->setShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_S );
    connect( actSaveDraft, SIGNAL( triggered( bool ) ), this, SLOT( sltSaveAsDraft() ) );

    actDeletePost = new KAction( KIcon( "edit-delete" ), i18n( "Delete from Server" ), this );
    actionCollection()->addAction( QLatin1String( "delete_post" ), actDeletePost );
//  connect(actDeletePost,SIGNAL(triggered( bool )),activePost,SLOT(sltDelPost()));

    actDeleteLocally = new KAction( KIcon( "edit-delete" ), i18n( "Delete Locally" ), this );
    actionCollection()->addAction( QLatin1String( "delete_locally" ), actDeleteLocally );
//  connect(actDeleteLocally,SIGNAL(triggered( bool )),activePost,SLOT(sltDelLocally()));

    actToggleToolboxVisible = new KToggleAction( i18n( "Show Toolbox" ), this );
    actionCollection()->addAction( QLatin1String( "toggle_toolbox" ), actToggleToolboxVisible );
    actToggleToolboxVisible->setShortcut( Qt::CTRL + Qt::Key_T );
    connect( actToggleToolboxVisible, SIGNAL( toggled( bool ) ), this, SLOT( sltToggleToolboxVisible( bool ) ) );
}

void MainWindow::sltCreateNewPost()
{
    kDebug();
    PostEntry *temp = new PostEntry( this );
    tabPosts->addTab( temp, i18n( "Untitled" ) );
//     temp->setCurrentPost();
    temp->setCurrentPostBlogId( toolbox->currentBlogId() );

//  // FIXME these lines added to set direction for new posts, but it generates Segmentation fault at run time!
    // What SegFault!? (I think ^its about previous codes! and need to test and remove!) -Mehrdad
    int tempId = toolbox->currentBlogId();
    if ( tempId != -1 ) {
        BilboBlog *tmp = DBMan::self()->getBlogInfo( tempId );
        temp->setDefaultLayoutDirection( tmp->direction() );
        tmp->deleteLater();
    }

    connect( temp, SIGNAL( sigTitleChanged( const QString& ) ),
             this, SLOT( sltPostTitleChanged( const QString& ) ) );
    connect( temp, SIGNAL( postPublishingDone( bool, const QString& ) ),
             this, SLOT( postManipulationDone( bool, const QString& ) ) );

    tabPosts->setCurrentWidget( temp );

    if ( this->isVisible() == false ) {
        this->show();
    }
}

void MainWindow::optionsPreferences()
{
    // The preference dialog is derived from prefs_base.ui
    //
    // compare the names of the widgets in the .ui file
    // to the names of the variables in the .kcfg file
    //avoid to have 2 dialogs shown
    if ( KConfigDialog::showDialog( "settings" ) )  {
        return;
    }
    KConfigDialog *dialog = new KConfigDialog( this, "settings", Settings::self() );
//     QWidget *generalSettingsDlg = new QWidget;
    QWidget *generalSettingsDlg = new QWidget( dialog );
    ui_prefs_base.setupUi( generalSettingsDlg );
//     QWidget *editorSettingsDlg = new QWidget;
//     ui_editorsettings_base.setupUi( editorSettingsDlg );
    EditorSettings *editorSettingsDlg = new EditorSettings( dialog );
    dialog->addPage( generalSettingsDlg, i18n( "General" ), "configure" );
    dialog->addPage( editorSettingsDlg, i18n( "Editor" ), "accessories-text-editor" );
    connect( dialog, SIGNAL( settingsChanged( const QString& ) ), this, SLOT( settingsChanged() ) );
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->show();
}

void MainWindow::settingsChanged()
{
}

void MainWindow::setupSystemTray()
{
    systemTray = new SysTray( this );
//     systemTray->actionCollection()->addAction( "new_post", this->actNewPost );
    systemTray->contextMenu()->addAction( this->actNewPost );
    systemTray->show();
}

void MainWindow::sltUploadAllChanges()
{
    kDebug();
}

void MainWindow::sltPostTitleChanged( const QString& title )
{
//     kDebug();
    tabPosts->setTabText( tabPosts->currentIndex(), title );
}

void MainWindow::sltToggleToolboxVisible( bool isVisible )
{
    toolboxDock->setVisible( isVisible );
//     if ( toolbox->isVisible() ) {
//         toolboxDock->hide();
//         actToggleToolboxVisible->setText( i18n( "Show Toolbox" ) );
//     } else {
//         toolboxDock->show();
//         actToggleToolboxVisible->setText( i18n( "Hide Toolbox" ) );
//     }
}

void MainWindow::sltActivePostChanged( int index )
{
    kDebug() << "new post index: " << index << "\tPrev Index: " << previousActivePostIndex;

    activePost = qobject_cast<PostEntry*>( tabPosts->widget( index ) );
    PostEntry *prevActivePost = qobject_cast<PostEntry*>( tabPosts->widget( previousActivePostIndex ) );
    int activePostBlogId = -1;
    int prevPostBlogId = -1;

    if (( prevActivePost != 0 ) && ( index != previousActivePostIndex ) ) {
        prevPostBlogId = prevActivePost->currentPostBlogId();
        toolbox->getFieldsValue( *prevActivePost->currentPost() );
        prevActivePost->setCurrentPostBlogId( toolbox->currentBlogId() );
    }

    if ( activePost != 0 ) {
        activePostBlogId = activePost->currentPostBlogId();
        if ( activePostBlogId != -1 ) {
            if ( activePostBlogId != prevPostBlogId ) {
                toolbox->setCurrentBlog( activePostBlogId );
                toolbox->sltLoadCategoryListFromDB( activePostBlogId );
                toolbox->sltLoadEntriesFromDB( activePostBlogId );
            }
        }
        toolbox->setFieldsValue( activePost->currentPost() );
        previousActivePostIndex = index;
    } else {
        kError() << "ActivePost is NULL! tabPosts Current index is: " << tabPosts->currentIndex() ;
    }
}

void MainWindow::sltPublishPost()
{
    kDebug();
    int blog_id = toolbox->currentBlogId();
    if ( blog_id == -1 ) {
        KMessageBox::sorry( this, i18n( "You have to select a blog to publish this post to it." ) );
        kDebug() << "Blog id not sets correctly.";
        return;
    }
    BilboPost post;
    toolbox->getFieldsValue( post );
    if ( activePost->postBody().isEmpty() || activePost->postTitle().isEmpty() ) {
        if ( KMessageBox::warningContinueCancel( this,
                i18n( "Your post title or body is empty!\nAre you sure of pubishing this post?" )
                                               ) == KMessageBox::Cancel )
            return;
    }
    post.setPrivate( false );
    activePost->publishPost( blog_id, post );
    statusBar()->showMessage( i18n( "publishing new post..." ) );
    this->setCursor( Qt::BusyCursor );
    toolbox->setCursor( Qt::BusyCursor );
}

void MainWindow::sltRemoveCurrentPostEntry()
{
    kDebug();
//     if(tabPosts->count()==1){
//         sltCreateNewPost();
//         previousActivePostIndex = 0;
//     }
    tabPosts->removeTab( tabPosts->currentIndex() );
//     tabPosts->setCurrentIndex(previousActivePostIndex);
}

void MainWindow::sltNewPostOpened( BilboPost * newPost )
{
    kDebug();
    PostEntry *temp = new PostEntry( this );
    tabPosts->addTab( temp, newPost->title() );

    temp->setCurrentPost( *newPost );
    temp->setCurrentPostBlogId( toolbox->currentBlogId() );

    BilboBlog *tmp = DBMan::self()->getBlogInfo( toolbox->currentBlogId() );
    temp->setDefaultLayoutDirection( tmp->direction() );
    delete tmp;
    ///^^^ FIXME I think we don't need this^, it's better to remain on the current state! :-/ Or maybe not :D -Mehrdad
    ///Yes! Of course we need it! :P -Mehrdad

    tabPosts->setCurrentWidget( temp );
    connect( temp, SIGNAL( sigTitleChanged( const QString& ) ), this, SLOT( sltPostTitleChanged( const QString& ) ) );
}

void MainWindow::sltCurrentBlogChanged( int blog_id )
{
    kDebug();
    if ( blog_id < 0 ) {
        kDebug() << "Blog id do not sets correctly";
        return;
    }

    BilboBlog *tmp = DBMan::self()->getBlogInfo( blog_id );
    this->activePost->setDefaultLayoutDirection( tmp->direction() );
    this->activePost->setCurrentPostBlogId( blog_id );
    this->actPublish->setText( i18n( "Publish to \"%1\"", tmp->title() ) );
    tmp->deleteLater();
}

void MainWindow::sltSavePostLocally()
{
    kDebug();
    int blog_id = toolbox->currentBlogId();
    QDir blogDir;

    if ( blog_id == -1 ) {
        kDebug() << "no blogs selected";
        blogDir = QDir( UNKNOWN_BLOG_DIR );
    } else {
        blogDir = QDir( DBMan::self()->getBlogInfo( blog_id )->localDirectory() );
//   kDebug() << __db->getBlogInfo(blog_id)->localDirectory() << " " << blog_id;
        if ( ! blogDir.exists() ) {
            kDebug() << "error: no directory created for the selected blog";
            statusBar()->showMessage( i18n( "error: no directory created for the selected blog." )
                                      , STATUSTIMEOUT );
            return;
        }
    }
    QMap <QString, BilboMedia*>::const_iterator i =
        activePost->mediaList().constBegin();
    while ( i != activePost->mediaList().constEnd() ) {

        if ( i.key().startsWith( "file://" + CACHED_MEDIA_DIR ) ) {
            kDebug() << blogDir.path();
            QFile::copy( i.key(), blogDir.path() + '/' + i.value()->name() );
        }
        ++i;
    }
    statusBar()->showMessage( i18n( "Current post saved in \"%1\".", blogDir.path() ),
                              STATUSTIMEOUT );
}

void MainWindow::sltSaveAsDraft()
{
    kDebug();
    int blog_id = toolbox->currentBlogId();
    if ( blog_id == -1 ) {
        KMessageBox::sorry( this, i18n( "You have to select a blog to save this post as draft on it." ) );
        kDebug() << "Blog id not sets correctly.";
        return;
    }
    BilboPost post;
    if ( activePost->postBody().isEmpty() || activePost->postTitle().isEmpty() ) {
        if ( KMessageBox::warningContinueCancel( this, i18n( "Your post title or body is empty!\n\
Are you sure of pubishing this post?" ) ) == KMessageBox::Cancel )
            return;
    }
    toolbox->getFieldsValue( post );
    post.setPrivate( true );
    activePost->publishPost( blog_id, post );
    statusBar()->showMessage( i18n( "Saving draft..." ) );
    this->setCursor( Qt::BusyCursor );
    toolbox->setCursor( Qt::BusyCursor );
}

void MainWindow::sltError( const QString & errorMessage )
{
    kDebug() << "Error message: " << errorMessage;
    KMessageBox::detailedError( this, i18n( "An error ocurred on latest transaction " ), errorMessage );
    statusBar()->clearMessage();
    this->unsetCursor();
    toolbox->unsetCursor();
}

void MainWindow::writeConfigs()
{
    kDebug();
    if ( toolboxDock->isVisible() )
        Settings::setShow_toolbox_on_start( true );
    else
        Settings::setShow_toolbox_on_start( false );
}

void MainWindow::keyReleaseEvent( QKeyEvent * event )
{
    if ( event->modifiers() == Qt::CTRL ) {
        switch ( event->key() ) {
            case  Qt::Key_1:
                toolbox->setCurrentPage( 0 );
                break;
            case Qt::Key_2:
                toolbox->setCurrentPage( 1 );
                break;
            case Qt::Key_3:
                toolbox->setCurrentPage( 2 );
                break;
            case Qt::Key_4:
                toolbox->setCurrentPage( 3 );
                break;
            case Qt::Key_5:
                toolbox->setCurrentPage( 4 );
                break;
            default:
                QMainWindow::keyPressEvent( event );
                break;
        }
    }
}

void MainWindow::postManipulationDone( bool isError, const QString &customMessage )
{
    kDebug();
    if(isError){
        KMessageBox::detailedError(this, i18n("Uploading post failed"), customMessage);
    } else {
        if(KMessageBox::questionYesNo(this, i18n("%1\nDo you want to keep post open?", customMessage)) != KMessageBox::No){
            tabPosts->removePage(qobject_cast<QWidget*>(sender()));
        }
    }
    this->unsetCursor();
    toolbox->unsetCursor();
}

bool MainWindow::queryClose()
{
    hide();
    return false;
}

bool MainWindow::queryExit()
{
    //TODO Save Current Open posts!
    delete DBMan::self();
    this->deleteLater();
    return true;
}

void MainWindow::slotBusy(bool isBusy)
{
    kDebug()<<"isBusy="<<isBusy<<"\tbusyNumber="<<busyNumber;
    if(isBusy){
        this->setCursor(Qt::BusyCursor);
        ++busyNumber;
        if(!progress){
            progress = new QProgressBar(statusBar());
            progress->setMinimum( 0 );
            progress->setMaximum( 0 );
            statusBar()->addPermanentWidget(progress);
        }
    } else {
        --busyNumber;
        if( busyNumber < 1 ){
            this->unsetCursor();
            if(progress){
                statusBar()->removeWidget(progress);
                progress->deleteLater();
                progress = 0;
            }
            busyNumber = 0;
        }
    }
}

#include "mainwindow.moc"
