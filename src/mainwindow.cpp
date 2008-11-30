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

#include <ktabwidget.h>
#include <kstatusbar.h>
#include <kaction.h>
// #include <ktoggleaction.h>
#include <kactioncollection.h>
#include <kstandardaction.h>
#include <kconfigdialog.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <KDE/KLocale>

#include "mainwindow.h"
#include "global.h"
#include "toolbox.h"
#include "postentry.h"
#include "addeditblog.h"
#include "backend.h"
#include "bilbomedia.h"
#include "settings.h"
#include "systray.h"
#include "bilboblog.h"

MainWindow::MainWindow(): KXmlGuiWindow(),
        tabPosts(new KTabWidget(this))
{
    kDebug();
    previousActivePostIndex = -1;
        
    // tell the KXmlGuiWindow that this is indeed the main widget
    tabPosts->setElideMode(Qt::ElideRight);
    setCentralWidget(tabPosts);

    toolbox=new Toolbox(this);
    toolboxDock = new QDockWidget(i18n("Post properties"), this);
    toolboxDock->setObjectName("dock_toolbox");
    toolbox->setObjectName("toolbox");
    toolboxDock->setWidget(toolbox);
    this->addDockWidget(Qt::RightDockWidgetArea, toolboxDock);
    
    btnRemovePost = new QToolButton(tabPosts);
    btnRemovePost->setIcon(KIcon("tab-close"));
    btnRemovePost->setToolTip(i18n("Remove current post"));
    tabPosts->setCornerWidget(btnRemovePost, Qt::TopRightCorner);
    connect(btnRemovePost, SIGNAL(clicked( bool )), this, SLOT(sltRemoveCurrentPostEntry()));
    
    // then, setup our actions
    setupActions();

    // add a status bar
    statusBar()->show();
    toolbox->show();

    // a call to KXmlGuiWindow::setupGUI() populates the GUI
    // with actions, using KXMLGUI.
    // It also applies the saved mainwindow settings, if any, and ask the
    // mainwindow to automatically save settings if changed: window size,
    // toolbar position, icon size, etc.
    setupGUI();
    
    
    sltCreateNewPost();
    activePost = qobject_cast<PostEntry*>(tabPosts->currentWidget());
    previousActivePostIndex = 0;
    
//     this->setWindowIcon(KIcon(":/media/bilbo.png"));
//     readConfig();
    toolbox->setVisible(Settings::show_toolbox_on_start());
    if(Settings::show_toolbox_on_start()){
        actToggleToolboxVisible->setText(i18n("Hide Toolbox"));
    } else {
        actToggleToolboxVisible->setText(i18n("Show Toolbox"));
    }
    setupSystemTray();
	
    connect(tabPosts, SIGNAL(currentChanged( int )), this, SLOT(sltActivePostChanged(int)));
    connect(toolbox, SIGNAL(sigEntrySelected(BilboPost *)), this, SLOT(sltNewPostSelected(BilboPost*)));
    connect(toolbox, SIGNAL(sigCurrentBlogChanged(int)), this, SLOT(sltCurrentBlogChanged(int)));
}

MainWindow::~MainWindow()
{
//     delete toolbox;
//     delete activePost;
//     delete addBlogPage;
//     delete tabPosts;
//     delete toolbarPost;
//     delete toolbarBlogger;
//     delete toolbarAction;
//     delete menubar;
//     delete menuBilbo;
//     delete menuPost;
//     delete menuAbout;
//     delete statusbar;
//     delete btnRemovePost;
//     delete actAddBlog;
//     delete actUploadAll;
//     delete actNewPost;
//     delete actPublish;
//     delete actSaveLocally;
//     delete actSaveDraft;
//     delete actDeletePost;
//     delete actDeleteLocally;
//     delete actAbout;
//     delete actQuit;
//     delete actToggleToolboxVisible;
}

void MainWindow::setupActions()
{
    KStandardAction::quit(qApp, SLOT(closeAllWindows()), actionCollection());

    KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());

    // custom menu and menu item - the slot is in the class bilboView
    actNewPost = new KAction(KIcon("document-new"), i18n("&New Post"), this);
    actionCollection()->addAction( QLatin1String("new_post"), actNewPost );
    actNewPost->setShortcut(Qt::CTRL + Qt::Key_N);
    connect(actNewPost, SIGNAL(triggered(bool)), this, SLOT(sltCreateNewPost()));
    
    actAddBlog=new KAction(KIcon("document-new"), i18n("Add &Blog"),this);
    actionCollection()->addAction(QLatin1String("add_blog"), actAddBlog);
    connect(actAddBlog, SIGNAL(triggered( bool )), toolbox, SLOT(sltAddBlog()));
	
    actUploadAll=new KAction(KIcon("arrow-up-double"), i18n("&Upload All Changes"),this);
    actionCollection()->addAction(QLatin1String("upload_all"), actUploadAll);
    connect(actUploadAll,SIGNAL(triggered( bool )),this,SLOT(sltUploadAllChanges()));
	
    actPublish=new KAction(KIcon("arrow-up"),i18n("&Publish"),this);
    actionCollection()->addAction(QLatin1String("publish_post"), actPublish);
    connect(actPublish,SIGNAL(triggered( bool )),this,SLOT(sltPublishPost()));
    
    actSave = new KAction(KIcon("document-save"), i18n("Save"), this);
    actionCollection()->addAction(QLatin1String("save"), actSave);
    connect(actSave, SIGNAL(triggered( bool )), this, SLOT(sltSavePostLocally()));
	
    actSaveLocally=new KAction(KIcon("document-save"), i18n("Save Locally"),this);
    actionCollection()->addAction(QLatin1String("save_locally"), actSaveLocally);
    actSaveLocally->setShortcut(Qt::CTRL + Qt::Key_S);
    connect(actSaveLocally, SIGNAL(triggered( bool )), this, SLOT(sltSavePostLocally()));
	
    actSaveDraft=new KAction(KIcon("document-save-as"), i18n("Save as Draft"),this);
    actionCollection()->addAction(QLatin1String("save_draft"), actSaveDraft);
    actSaveDraft->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
    connect(actSaveDraft, SIGNAL(triggered( bool )), this, SLOT(sltSaveAsDraft()));
	
    actDeletePost=new KAction(KIcon("edit-delete"), i18n("Delete from Server"),this);
    actionCollection()->addAction(QLatin1String("delete_post"), actDeletePost);
// 	connect(actDeletePost,SIGNAL(triggered( bool )),activePost,SLOT(sltDelPost()));
	
    actDeleteLocally=new KAction(KIcon("edit-delete"), i18n("Delete Locally"),this);
    actionCollection()->addAction(QLatin1String("delete_locally"), actDeleteLocally);
// 	connect(actDeleteLocally,SIGNAL(triggered( bool )),activePost,SLOT(sltDelLocally()));
	
    actToggleToolboxVisible = new KAction( i18n("Hide Toolbox"), this);
    actionCollection()->addAction(QLatin1String("toggle_toolbox"), actToggleToolboxVisible );
    actToggleToolboxVisible->setShortcut(Qt::CTRL + Qt::Key_T);
    connect(actToggleToolboxVisible, SIGNAL(triggered( bool )), this, SLOT(sltToggleToolboxVisible()));
}

void MainWindow::sltCreateNewPost()
{
    kDebug();
    PostEntry *temp=new PostEntry(this);
    tabPosts->addTab(temp,i18n("Untitled"));
    temp->setCurrentPost();
    temp->setCurrentPostBlogId(toolbox->currentBlogId());
	
// 	// FIXME these lines added to set direction for new posts, but it generates Segmentation fault at run time!
// // 	BilboBlog *tmp = __db->getBlogInfo(toolbox->currentBlogId());
// // 	temp->setDefaultLayoutDirection(tmp->direction());
// // 	delete tmp;
// 	
//     connect(temp, SIGNAL(sigTitleChanged(const QString& )), this, SLOT(sltPostTitleChanged(const QString&)));
// // 	activePost=temp;
    tabPosts->setCurrentWidget(temp);
    if(this->isVisible()==false)
        this->show();
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
    KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self());
    QWidget *generalSettingsDlg = new QWidget;
    ui_prefs_base.setupUi(generalSettingsDlg);
    dialog->addPage(generalSettingsDlg, i18n("General"), "package_setting");
    connect(dialog, SIGNAL(settingsChanged(QString)), this, SLOT(settingsChanged()));
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->show();
}

void MainWindow::settingsChanged()
{
}

void MainWindow::setupSystemTray()
{
    systemTray = new SysTray(this);
    systemTray->actionCollection()->addAction("new_post", this->actNewPost);
    systemTray->show();
}

void MainWindow::sltUploadAllChanges()
{
    kDebug();
}

void MainWindow::sltPostTitleChanged(const QString& title)
{
    kDebug();
    tabPosts->setTabText(tabPosts->currentIndex(),title);
}

void MainWindow::sltToggleToolboxVisible()
{
    if(toolbox->isVisible()){
        toolboxDock->hide();
        actToggleToolboxVisible->setText(i18n("Show Toolbox"));
    } else {
        toolboxDock->show();
        actToggleToolboxVisible->setText(i18n("Hide Toolbox"));
    }
}

void MainWindow::sltActivePostChanged(int index)
{
    kDebug() << "new post index: " << index << "\tPrev Index: " << previousActivePostIndex;
    
    activePost = qobject_cast<PostEntry*>( tabPosts->widget(index) );
    PostEntry *prevActivePost = qobject_cast<PostEntry*>( tabPosts->widget( previousActivePostIndex ) );
    if ((prevActivePost != 0) && (index != previousActivePostIndex)) {
		prevActivePost->setCurrentPost((*toolbox->getFieldsValue()));
		prevActivePost->setCurrentPostBlogId(toolbox->currentBlogId());
		tabPosts->setTabText(previousActivePostIndex, prevActivePost->postTitle());
    }

    if (activePost != 0) {
        toolbox->setFieldsValue(activePost->currentPost());
        toolbox->setCurrentBlog(activePost->currentPostBlogId());
        previousActivePostIndex = index;
        sltPostTitleChanged( activePost->postTitle() );
    } else {
        kError() << "ActivePost is NULL! tabPosts Current index is: " << tabPosts->currentIndex() ;
    }
}

void MainWindow::sltPublishPost()
{
    kDebug();
    int blog_id = toolbox->currentBlogId();
    if(blog_id==-1){
        KMessageBox::sorry(this, i18n("You have to select a blog to publish this post to it."));
        kDebug()<<"Blog id not sets correctly.";
        return;
    }
    Backend *b = new Backend(blog_id);
    connect(b, SIGNAL(sigPostPublished(int, int, bool)), this, SLOT(sltPostPublished(int, int, bool)));
    connect(b, SIGNAL(sigError(QString&)), this, SLOT(sltError(QString&)));
    BilboPost *post = toolbox->getFieldsValue();
    if(activePost->postBody()->isEmpty() || activePost->postTitle().isEmpty()){
        if(KMessageBox::warningContinueCancel(this, i18n("Your post title or body is empty!\nAre you sure of pubishing this post?")
           ) == KMessageBox::Cancel)
            return;
    }
    post->setContent(*(activePost->postBody()));
    post->setTitle(activePost->postTitle());
    post->setPrivate(false);
	
    b->publishPost(post);
    statusBar()->showMessage(i18n("publishing new post..."));
    this->setCursor(Qt::BusyCursor);
}

void MainWindow::sltPostPublished(int blog_id, int post_id, bool isPrivate)
{
    kDebug()<<"Post Id: "<< post_id;
    QString blog_name = toolbox->listBlogs.key(blog_id);
    QString msg;
    if(isPrivate){
        msg = i18n("New Draft saved to \"%1\" successfully.\nDo you want to keep it on editor?", blog_name);
        statusBar()->showMessage(i18n("New draft saved to \"%1\"", blog_name) , STATUSTIMEOUT);
    }
    else {
        msg = i18n("New Post published to \"%1\" successfully.\nDo you want to keep it on editor?", blog_name);
        statusBar()->showMessage(i18n("New post published to \"%1\"",blog_name) , STATUSTIMEOUT);
    }
    if(KMessageBox::questionYesNo(this, msg, "Successful") != KMessageBox::Yes)
        sltRemoveCurrentPostEntry();
	
    this->unsetCursor();
}

void MainWindow::sltRemoveCurrentPostEntry()
{
    kDebug();
    if(tabPosts->count()==1){
        sltCreateNewPost();
        previousActivePostIndex = 0;
    }
    tabPosts->removeTab(tabPosts->currentIndex());
    tabPosts->setCurrentIndex(previousActivePostIndex);
}

void MainWindow::sltNewPostSelected(BilboPost * newPost)
{
    kDebug();
    PostEntry *temp=new PostEntry(this);
    tabPosts->addTab(temp,newPost->title());
    connect(temp, SIGNAL(sigTitleChanged( const QString& )), this, SLOT(sltPostTitleChanged( const QString& )));
    
//     temp->setPostTitle(newPost->title());
//     temp->setPostBody(newPost->content());
    temp->setCurrentPost(*newPost);
    temp->setCurrentPostBlogId(toolbox->currentBlogId());
	
    BilboBlog *tmp = __db->getBlogInfo(toolbox->currentBlogId());
    temp->setDefaultLayoutDirection(tmp->direction());
    delete tmp;

//     activePost=temp;
    tabPosts->setCurrentWidget(temp);
}

void MainWindow::sltCurrentBlogChanged(int blog_id)
{
    kDebug();
    if(blog_id==-1){
        kDebug()<<"Blog id do not sets correctly";
        return;
    }
	///suggestion:
	//this->activePost->setCurrentPostBlogId(blog_id);
	
//     BilboBlog *tmp = __db->getBlogInfo(blog_id);
//     this->centralWidget()->setLayoutDirection(tmp->direction());
//     delete tmp;
    BilboBlog *tmp = __db->getBlogInfo(blog_id);
    this->activePost->setDefaultLayoutDirection(tmp->direction());
    delete tmp;
}

void MainWindow::sltSavePostLocally()
{
    kDebug();
}

void MainWindow::sltSaveAsDraft()
{
    kDebug();
    int blog_id = toolbox->currentBlogId();
    if(blog_id==-1){
        KMessageBox::sorry(this, i18n("You have to select a blog to save this post as draft on it."));
        kDebug()<<"Blog id not sets correctly.";
        return;
    }
    Backend *b = new Backend(blog_id);
    connect(b, SIGNAL(sigPostPublished(int, int, bool)), this, SLOT(sltPostPublished(int, int, bool)));
    connect(b, SIGNAL(sigError(QString&)), this, SLOT(sltError(QString&)));
    BilboPost *post = toolbox->getFieldsValue();
    if(activePost->postBody()->isEmpty() || activePost->postTitle().isEmpty()){
        if(KMessageBox::warningContinueCancel(this, i18n("Your post title or body is empty!\nAre you sure of pubishing this post?")
                                             ) == KMessageBox::Cancel)
            return;
    }
    post->setContent(*(activePost->postBody()));
    post->setTitle(activePost->postTitle());
    post->setPrivate(true);
	
    b->publishPost(post);
    statusBar()->showMessage(i18n("Saving draft..."));
    this->setCursor(Qt::BusyCursor);
}

void MainWindow::sltError(QString & errorMessage)
{
    kDebug()<<"Error message: "<<errorMessage;
    KMessageBox::detailedError(this, "An Error ocurred on latest transaction ", errorMessage);
}

#include "mainwindow.moc"
