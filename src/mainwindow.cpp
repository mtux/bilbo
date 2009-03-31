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
#include "multilinetextedit.h"
#include "blogsettings.h"

#include "ui_advancedsettingsbase.h"
#include "ui_settingsbase.h"
#include "ui_editorsettingsbase.h"

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
#include <KFileDialog>
#include <KSelectAction>


#define TIMEOUT 5000

MainWindow::MainWindow(): KXmlGuiWindow(),
        tabPosts( new KTabWidget( this ) ), mCurrentBlogId(__currentBlogId)
{
    kDebug();
    previousActivePostIndex = -1;
    mCurrentBlogId = -1;
    activePost = 0;
    busyNumber = 0;
    progress = 0;
    this->setWindowTitle( i18n("Bilbo Blogger") );
    tabPosts->setElideMode( Qt::ElideRight );///TODO make this Optional!
    tabPosts->setTabCloseActivatePrevious( true );
    setCentralWidget( tabPosts );
//     this->setDockOptions( QMainWindow::ForceTabbedDocks);

    toolbox = new Toolbox( this );
    toolboxDock = new QDockWidget( i18n( "Toolbox" ), this );
    toolboxDock->setAllowedAreas( Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea );
    toolboxDock->setFeatures( QDockWidget::AllDockWidgetFeatures );
    toolboxDock->setObjectName( "dock_toolbox" );
    toolbox->setObjectName( "toolbox" );
    toolboxDock->setWidget( toolbox );
    this->addDockWidget( Qt::RightDockWidgetArea, toolboxDock );

    btnRemovePost = new QToolButton( tabPosts );
    btnRemovePost->setIcon( KIcon( "tab-close" ) );
    btnRemovePost->setToolTip( i18n( "Remove current post" ) );
    tabPosts->setCornerWidget( btnRemovePost, Qt::TopRightCorner );
    connect( btnRemovePost, SIGNAL( clicked( bool ) ), this, SLOT( sltRemovePostEntry() ) );

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

    toolbox->setVisible( Settings::showToolboxOnStart() );
    actionCollection()->action("toggle_toolbox")->setChecked( Settings::showToolboxOnStart() );

    setupSystemTray();

    connect( tabPosts, SIGNAL( currentChanged( int ) ), this, SLOT( sltActivePostChanged( int ) ) );
    connect( toolbox, SIGNAL( sigEntrySelected( BilboPost &, int ) ), this, SLOT( sltNewPostOpened( BilboPost&, int ) ) );
//     connect( toolbox, SIGNAL( sigCurrentBlogChanged( int ) ), this, SLOT( sltCurrentBlogChanged( int ) ) );
    connect( toolbox, SIGNAL( sigError( const QString& ) ), this, SLOT( sltError( const QString& ) ) );
    connect( toolbox, SIGNAL( sigBusy(bool) ), this, SLOT( slotBusy(bool) ));

    QList<BilboBlog*> blogList = DBMan::self()->blogList().values();
    int count = blogList.count();
    for(int i=0; i < count; ++i) {
        QAction *act = new QAction( blogList[i]->title(), blogs );
        act->setCheckable( true );
        act->setData( blogList[i]->id() );
        blogs->addAction( act );
    }
    connect( blogs, SIGNAL(triggered( QAction* )), this, SLOT(currentBlogChanged(QAction*)) );
    QTimer::singleShot( 0, this, SLOT( loadTempPosts() ) );
}

MainWindow::~MainWindow()
{
    kDebug();
    writeConfigs();
}

void MainWindow::setupActions()
{
    KStandardAction::quit( qApp, SLOT( quit() ), actionCollection() );

    KStandardAction::preferences( this, SLOT( optionsPreferences() ), actionCollection() );

    // custom menu and menu item
    KAction *actNewPost = new KAction( KIcon( "document-new" ), i18n( "New Post" ), this );
    actionCollection()->addAction( QLatin1String( "new_post" ), actNewPost );
    actNewPost->setShortcut( Qt::CTRL + Qt::Key_N );
    connect( actNewPost, SIGNAL( triggered( bool ) ), this, SLOT( sltCreateNewPost() ) );

    KAction *actAddBlog = new KAction( KIcon( "list-add" ), i18n( "Add Blog..." ), this );
    actionCollection()->addAction( QLatin1String( "add_blog" ), actAddBlog );
    connect( actAddBlog, SIGNAL( triggered( bool ) ), this, SLOT( addBlog() ) );

    KAction *actPublish = new KAction( KIcon( "arrow-up" ), i18n( "Submit..." ), this );
    actionCollection()->addAction( QLatin1String( "publish_post" ), actPublish );
    connect( actPublish, SIGNAL( triggered( bool ) ), this, SLOT( sltPublishPost() ) );

    KAction *actUpload = new KAction( KIcon( "upload-media" ), i18n( "Upload Media..." ), this );
    actionCollection()->addAction( QLatin1String( "upload_media" ), actUpload );
    connect( actUpload, SIGNAL( triggered( bool ) ), this, SLOT( uploadMediaObject() ) );

    KAction *actSaveLocally = new KAction( KIcon( "document-save" ), i18n( "Save Locally" ), this );
    actionCollection()->addAction( QLatin1String( "save_locally" ), actSaveLocally );
    actSaveLocally->setShortcut( Qt::CTRL + Qt::Key_S );
    connect( actSaveLocally, SIGNAL( triggered( bool ) ), this, SLOT( sltSavePostLocally() ) );

    KToggleAction *actToggleToolboxVisible = new KToggleAction( i18n( "Show Toolbox" ), this );
    actionCollection()->addAction( QLatin1String( "toggle_toolbox" ), actToggleToolboxVisible );
    actToggleToolboxVisible->setShortcut( Qt::CTRL + Qt::Key_T );
    connect( actToggleToolboxVisible, SIGNAL( toggled( bool ) ), this, SLOT( sltToggleToolboxVisible( bool ) ) );

    KAction *actClearImageCache = new KAction( KIcon( "edit-clear" ), i18n( "Clear cached images" ), this );
    actionCollection()->addAction( QLatin1String( "clear_image_cache" ), actClearImageCache );
    connect( actClearImageCache, SIGNAL( triggered( bool ) ), this, SLOT( sltClearCache() ) );

    KAction *actHide = new KAction( KIcon( "dialog-close" ), i18n( "Hide Bilbo Window" ), this );
    actionCollection()->addAction( QLatin1String( "hide_mainwin" ), actHide );
    actHide->setShortcut( Qt::Key_Escape );
    connect( actHide, SIGNAL( triggered( bool ) ), this, SLOT( hide() ) );

    blogs = new KSelectAction( this );
    actionCollection()->addAction( QLatin1String( "blogs_list" ), blogs );
}

void MainWindow::loadTempPosts()
{
    kDebug();
    QMap<BilboPost*, int> tempList = DBMan::self()->listTempPosts();
    int count = tempList.count();
    if( count > 0 ){
        QMap<BilboPost*, int>::ConstIterator it = tempList.constBegin();
        QMap<BilboPost*, int>::ConstIterator endIt = tempList.constEnd();
        for( ; it != endIt; ++it ) {
            createPostEntry(it.value(), (*it.key()));
        }
    } else {
        sltCreateNewPost();
    }
//     activePost = qobject_cast<PostEntry*>( tabPosts->currentWidget() );
    previousActivePostIndex = 0;
    if( activePost )
        setCurrentBlog( activePost->currentPostBlogId() );
}

void MainWindow::setCurrentBlog( int blog_id )
{
    kDebug()<<blog_id;
    if(blog_id == -1) {
        blogs->setCurrentItem( -1 );
        toolbox->setCurrentBlogId( blog_id );
//         actionCollection()->action("publish_post")->setEnabled( false );
        return;
    }
    int count = blogs->items().count();
    for (int i=0; i<count; ++i) {
        if( blogs->action(i)->data().toInt() == blog_id ) {
            blogs->setCurrentItem( i );
            currentBlogChanged( blogs->action( i ) );
            break;
        }
    }
}

void MainWindow::currentBlogChanged( QAction *act )
{
    if( act ) {
        if( mCurrentBlogId == act->data().toInt() )
            return;
        mCurrentBlogId = act->data().toInt();
//         __currentBlogId = mCurrentBlogId;
        if( activePost ) {
//             actionCollection()->action("publish_post")->setEnabled( true );
            activePost->setCurrentPostBlogId( mCurrentBlogId );
        } else {
//             actionCollection()->action("publish_post")->setEnabled( false );
        }
        blogs->setToolTip( DBMan::self()->blogList().value( mCurrentBlogId )->blogUrl() );
    } else {
        mCurrentBlogId = -1;
        if( activePost )
            activePost->setCurrentPostBlogId( mCurrentBlogId );
    }
    toolbox->setCurrentBlogId( mCurrentBlogId );
}

void MainWindow::sltCreateNewPost()
{
    kDebug();

    tabPosts->setCurrentWidget( createPostEntry( mCurrentBlogId, BilboPost()) );
    if( mCurrentBlogId == -1 ) {
        if( blogs->items().count() > 0 ) {
            blogs->setCurrentItem( 0 );
            currentBlogChanged( blogs->action( 0 ) );
        }
    }
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
    //avoid having 2 dialogs shown
    if ( KConfigDialog::showDialog( "settings" ) )  {
        return;
    }
    KConfigDialog *dialog = new KConfigDialog( this, "settings", Settings::self() );
    QWidget *generalSettingsDlg = new QWidget;
    generalSettingsDlg->setAttribute( Qt::WA_DeleteOnClose );
    Ui::SettingsBase ui_prefs_base;
    Ui::EditorSettingsBase ui_editorsettings_base;
    ui_prefs_base.setupUi( generalSettingsDlg );
    BlogSettings *blogSettingsDlg = new BlogSettings;
    blogSettingsDlg->setAttribute( Qt::WA_DeleteOnClose );
    connect( blogSettingsDlg, SIGNAL(blogAdded(const BilboBlog &)),
             this, SLOT(slotBlogAdded(const BilboBlog &)) );
    connect( blogSettingsDlg, SIGNAL(blogEdited(const BilboBlog &)),
             this, SLOT(slotBlogEdited(const BilboBlog &)) );
    connect( blogSettingsDlg, SIGNAL(blogRemoved(int)), this, SLOT(slotBlogRemoved(int)) );
    QWidget *editorSettingsDlg = new QWidget;
    editorSettingsDlg->setAttribute( Qt::WA_DeleteOnClose );
    ui_editorsettings_base.setupUi( editorSettingsDlg );
    QWidget *advancedSettingsDlg = new QWidget;
    advancedSettingsDlg->setAttribute( Qt::WA_DeleteOnClose );
    Ui::AdvancedSettingsBase ui_advancedsettings_base;
    ui_advancedsettings_base.setupUi( advancedSettingsDlg );
    dialog->addPage( generalSettingsDlg, i18n( "General" ), "configure" );
    dialog->addPage( blogSettingsDlg, i18n( "Blogs" ), "document-properties");
    dialog->addPage( editorSettingsDlg, i18n( "Editor" ), "accessories-text-editor" );
    dialog->addPage( advancedSettingsDlg, i18n( "Advanced" ), "applications-utilities");
    connect( dialog, SIGNAL( settingsChanged( const QString& ) ), this, SIGNAL( settingsChanged() ) );
    connect( dialog, SIGNAL(destroyed(QObject*)), this, SLOT(slotConfigWindowDestroyed(QObject*)));
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->resize( Settings::configWindowSize() );
    dialog->show();
}

void MainWindow::slotConfigWindowDestroyed( QObject *win )
{
    QSize size = qobject_cast<QWidget *>(win)->size();
    Settings::setConfigWindowSize( size );
}

void MainWindow::addBlog()
{
    AddEditBlog *addEditBlogWindow = new AddEditBlog( -1, this );
    addEditBlogWindow->setWindowModality( Qt::ApplicationModal );
    addEditBlogWindow->setAttribute( Qt::WA_DeleteOnClose );
    connect( addEditBlogWindow, SIGNAL( sigBlogAdded( const BilboBlog& ) ),
             this, SLOT( slotBlogAdded( const BilboBlog& ) ) );
    addEditBlogWindow->show();
}

void MainWindow::slotBlogAdded( const BilboBlog &blog )
{
    QAction *act = new QAction( blog.title(), blogs );
    act->setCheckable( true );
    act->setData( blog.id() );
    blogs->addAction( act );
    blogs->setCurrentAction( act );
    currentBlogChanged( act );
    toolbox->sltReloadCategoryList();
    toolbox->sltGetEntriesCount( 20 );
}

void MainWindow::slotBlogEdited( const BilboBlog &blog )
{
    int count = blogs->actions().count();
    for(int i=0; i< count; ++i){
        if( blogs->action( i )->data().toInt() == blog.id() ) {
            blogs->action( i )->setText( blog.title() );
            break;
        }
    }
}

void MainWindow::slotBlogRemoved( int blog_id )
{
    int count = blogs->actions().count();
    for(int i=0; i< count; ++i){
        if( blogs->action( i )->data().toInt() == blog_id ) {
            if( blogs->currentItem() == i ) {
                blogs->setCurrentItem( i-1 );
                currentBlogChanged( blogs->action( i-1 ) );
            }
            blogs->removeAction( blogs->action( i ) );
            if(blogs->currentItem() == -1)
                toolbox->clearFields();
            break;
        }
    }
}

void MainWindow::setupSystemTray()
{
    systemTray = new SysTray( this );
//     systemTray->actionCollection()->addAction( "new_post", this->actNewPost );
    systemTray->contextMenu()->addAction( actionCollection()->action("new_post") );
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
        prevActivePost->setCurrentPostBlogId( mCurrentBlogId );
    }

    if ( index >= 0 ) {
        activePostBlogId = activePost->currentPostBlogId();
        if ( activePostBlogId != -1 && activePostBlogId != prevPostBlogId ) {
                setCurrentBlog( activePostBlogId );
        }
        toolbox->setFieldsValue( activePost->currentPost() );
    } else {
        kError() << "ActivePost is NULL! tabPosts Current index is: " << tabPosts->currentIndex() ;
    }
    previousActivePostIndex = index;
}

void MainWindow::sltPublishPost()
{
    kDebug();
    if ( mCurrentBlogId == -1 ) {
        KMessageBox::sorry( this, i18n( "You have to select a blog to publish this post to it." ) );
        kDebug() << "Blog id not sets correctly.";
        return;
    }
    if( !activePost || tabPosts->currentIndex() == -1) {
        KMessageBox::sorry( this, i18n( "There isn't any open post to submit!" ) );
        kDebug() << "There isn't any post";
        return;
    }
    BilboPost post = *activePost->currentPost();
    toolbox->getFieldsValue( post );
//     post.setPrivate( false );
    activePost->publishPost( mCurrentBlogId, post );
    this->setCursor( Qt::BusyCursor );
    toolbox->setCursor( Qt::BusyCursor );
}

void MainWindow::sltRemovePostEntry( PostEntry *widget )
{
    kDebug();
    if( !widget ) {
        widget = activePost;
    }
    DBMan::self()->removeTempEntry( *widget->currentPost() );
    widget->close();

//     if( tabPosts->count() == 0 ){
//         sltCreateNewPost();
//         previousActivePostIndex = 0;
//     }
    if( tabPosts->count() < 1 ) {
        activePost = 0;
//         actionCollection()->action("publish_post")->setEnabled( false );
    }
}

void MainWindow::sltNewPostOpened( BilboPost &newPost, int blog_id )
{
    kDebug();
    QWidget * w = createPostEntry( blog_id, newPost );
    tabPosts->setCurrentWidget( w );
}

void MainWindow::sltSavePostLocally()
{
    kDebug();
    if(activePost && tabPosts->count() > 0) {
        toolbox->getFieldsValue(*activePost->currentPost());
        activePost->saveLocally();
        toolbox->reloadLocalPosts();
    }
}

void MainWindow::sltError( const QString & errorMessage )
{
    kDebug() << "Error message: " << errorMessage;
    KMessageBox::detailedError( this, i18n( "An error ocurred on latest transaction " ), errorMessage );
    statusBar()->clearMessage();
    slotBusy(false);
}

void MainWindow::writeConfigs()
{
    kDebug();
    if ( toolboxDock->isVisible() )
        Settings::setShowToolboxOnStart( true );
    else
        Settings::setShowToolboxOnStart( false );
}

void MainWindow::keyPressEvent( QKeyEvent * event )
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
            case Qt::Key_W:
                sltRemovePostEntry();
                break;
            default:
                KXmlGuiWindow::keyPressEvent( event );
                break;
        }
    }
}

void MainWindow::postManipulationDone( bool isError, const QString &customMessage )
{
    kDebug();
    if(isError){
        KMessageBox::detailedError(this, i18n("Submiting post failed"), customMessage);
    } else {
        if(KMessageBox::questionYesNo(this, i18n("%1\nDo you want to keep post open?",
            customMessage)) == KMessageBox::No) {
            sltRemovePostEntry(qobject_cast<PostEntry*>(sender()));
        }
        toolbox->sltLoadEntriesFromDB( mCurrentBlogId );
    }
    this->unsetCursor();
    toolbox->unsetCursor();
}

void MainWindow::slotBusy(bool isBusy)
{
    kDebug()<<"isBusy="<<isBusy<<"\tbusyNumber="<<busyNumber;
    if(isBusy){
        this->setCursor(Qt::BusyCursor);
        toolbox->setCursor( Qt::BusyCursor );
        ++busyNumber;
        if(!progress){
            progress = new QProgressBar(statusBar());
            progress->setMinimum( 0 );
            progress->setMaximum( 0 );
            progress->setFixedSize(250, 17);
            statusBar()->addPermanentWidget(progress);
        }
    } else {
        --busyNumber;
        if( busyNumber < 1 ){
            this->unsetCursor();
            toolbox->unsetCursor();
            if(progress){
                statusBar()->removeWidget(progress);
                progress->deleteLater();
                progress = 0;
            }
//             busyNumber = 0;
        }
    }
}

QWidget* MainWindow::createPostEntry(int blog_id, const BilboPost& post)
{
    kDebug();
    PostEntry *temp = new PostEntry( this );
    temp->setAttribute( Qt::WA_DeleteOnClose );
    temp->setCurrentPost(post);
    temp->setCurrentPostBlogId( blog_id );

    connect( temp, SIGNAL( sigTitleChanged( const QString& ) ),
             this, SLOT( sltPostTitleChanged( const QString& ) ) );
    connect( temp, SIGNAL( postPublishingDone( bool, const QString& ) ),
            this, SLOT( postManipulationDone( bool, const QString& ) ) );
    connect( this, SIGNAL( settingsChanged() ), temp, SLOT( settingsChanged() ));
    connect( temp, SIGNAL( showStatusMessage(QString,bool)),
             this, SLOT(slotShowStatusMessage(QString,bool)));

    connect( temp, SIGNAL( sigBusy( bool ) ), this, SLOT( slotBusy( bool ) ) );
//     connect( temp, SIGNAL( postModified() ), this, SLOT(slotPostModified()) );
//     connect( temp, SIGNAL( postSavedLocally() ), this, SLOT( slotPostSaved() ) );

    tabPosts->addTab( temp, post.title() );
    return temp;
}

void MainWindow::sltClearCache()
{
    QDir cacheDir( CACHED_MEDIA_DIR );
    QStringListIterator i( cacheDir.entryList() );
    while ( i.hasNext() ) {
        cacheDir.remove( i.next() );
    }
    MultiLineTextEdit::clearCache();
}

void MainWindow::slotShowStatusMessage(const QString &message, bool isPermanent)
{
    statusBar()->showMessage(message, (isPermanent ? 0 : TIMEOUT));
}

void MainWindow::uploadMediaObject()
{
    if( mCurrentBlogId == -1 ) {
        KMessageBox::sorry( this, i18n( "You have to select a blog to upload media to it." ) );
        return;
    }
    if(  DBMan::self()->blogList().value(mCurrentBlogId)->supportMediaObjectUploading() ) {
        QString mediaPath = KFileDialog::getOpenFileName( KUrl("kfiledialog:///image?global"),
                                                      "image/png image/jpeg image/gif", this,
                                                          i18n("Select media to upload"));
        if(mediaPath.isEmpty())
            return;
        KDialog *dialog = new KDialog(this);
        QLabel *graphic= new QLabel(dialog);
        graphic->setPixmap( QPixmap(mediaPath) );
//         dialog->setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel);
        dialog->setWindowTitle( i18n( "Upload media..." ) );
        dialog->setButtonText(KDialog::Ok, i18n("Upload") );
        dialog->setMainWidget(graphic);
        int i = dialog->exec();
        kDebug()<< i;
        ///TODO Add option to reselect media!
        if( i ) {
            KUrl mediaUrl(mediaPath);
            BilboMedia *media = new BilboMedia(this);
            media->setLocalUrl(mediaUrl);
            media->setName( mediaUrl.fileName() );
            media->setBlogId( mCurrentBlogId );
            media->setMimeType( KMimeType::findByUrl( mediaUrl, 0, true )->name() );
            Backend *b = new Backend( mCurrentBlogId, this);
            connect( b, SIGNAL( sigMediaUploaded(BilboMedia*) ),
                    this, SLOT( slotMediaObjectUploaded(BilboMedia*) ) );
            connect( b, SIGNAL( sigError(QString)), this, SLOT( sltError(QString) ) );
            connect( b, SIGNAL( sigMediaError(QString,BilboMedia*) ), this, SLOT(sltError(QString)) );
            b->uploadMedia( media );
            slotBusy(true);
        }
    } else {
        KMessageBox::error(this, i18n( "API of current selected blog doesn't support uploading media objects." ));
    }
}

void MainWindow::slotMediaObjectUploaded( BilboMedia *media )
{
    slotBusy(false);
    QString msg;
    if( Settings::copyMediaUrl() ) {
        KApplication::clipboard()->setText( media->remoteUrl().prettyUrl() );
        msg = i18n( "Media uploaded, and URL copied to clipboard.\nYou can find it here:\n%1",
                                         media->remoteUrl().prettyUrl() );
    } else {
        msg = i18n( "Media uploaded.\nYou can find it here:\n%1",
                                         media->remoteUrl().prettyUrl() );
    }
    KMessageBox::information(this, msg, i18n( "Successfully uploaded" ), QString(), KMessageBox::AllowLink);
    ///TODO Add to Post!
    media->deleteLater();
    sender()->deleteLater();
}

// void MainWindow::slotPostModified()
// {
//     kDebug();
//     int index = tabPosts->indexOf(qobject_cast< QWidget* >(sender()));
//     tabPosts->setTabIcon(index, KIcon("document-save"));
// }
// 
// void MainWindow::slotPostSaved()
// {
//     kDebug();
//     int index = tabPosts->indexOf(qobject_cast< QWidget* >(sender()));
//     tabPosts->setTabIcon(index, KIcon());
// }

#include "mainwindow.moc"
