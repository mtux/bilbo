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
#include "mainwindow.h"
#include "global.h"
#include "toolbox.h"
#include "postentry.h"
#include "addeditblog.h"
#include "backend.h"
#include <QSettings>

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
    qDebug("MainWindow::MainWindow");
    previousActivePostIndex = -1;
	createUi();
	
	readConfig();
	createActions();
	toolbox->setVisible(__conf->isToolboxVisibleByDefault);
	if(__conf->isToolboxVisibleByDefault){
		actToggleToolboxVisible->setText(tr("Hide Toolbox"));
	} else {
		actToggleToolboxVisible->setText(tr("Show Toolbox"));
	}
	
	connect(tabPosts, SIGNAL(currentChanged( int )), this, SLOT(sltActivePostChanged(int)));
    connect(toolbox, SIGNAL(sigEntrySelected(BilboPost *)), this, SLOT(sltNewPostSelected(BilboPost*)));
    connect(toolbox, SIGNAL(sigCurrentBlogChanged(int)), this, SLOT(sltCurrentBlogChanged(int)));
}

MainWindow::~MainWindow()
{
    delete toolbox;
    delete activePost;
    delete addBlogPage;
    delete tabPosts;
    delete toolbarPost;
    delete toolbarBlogger;
    delete toolbarAction;
    delete menubar;
    delete menuBilbo;
    delete menuPost;
    delete menuAbout;
    delete statusbar;
    delete btnRemovePost;
    delete actAddBlog;
    delete actUploadAll;
    delete actNewPost;
    delete actPublish;
    delete actSaveLocally;
    delete actSaveDraft;
    delete actDeletePost;
    delete actDeleteLocally;
    delete actAbout;
    delete actQuit;
    delete actToggleToolboxVisible;
}

void MainWindow::readConfig()
{
    qDebug("MainWindow::readConfig");
	QSettings config(CONF_PATH, QSettings::NativeFormat);

	QPoint pos = config.value("pos", QPoint(300, 200)).toPoint();
	QSize size = config.value ( "size", QSize ( 800, 600 ) ).toSize();
	int toolboxWidth = config.value ( "toolbox_width", 300 ).toInt();
    toolbox->setCurrentBlog(config.value("selected_blog", -1).toInt());

	resize(size);
	toolbox->resize(toolboxWidth, toolbox->height());
	move(pos);
}

void MainWindow::writeConfig()
{
    qDebug("MainWindow::writeConfig");
	__conf->isToolboxVisibleByDefault = toolbox->isVisible();
	
	QSettings config(CONF_PATH, QSettings::NativeFormat);
	config.setValue("pos" , pos());
	config.setValue("size", size() );
	config.setValue("toolbox_width", toolbox->width());
    config.setValue("selected_blog", toolbox->currentBlogId());
	config.sync();
}

void MainWindow::createUi()
{
    qDebug("MainWindow::createUi");
// 	this->resize(887, 559);
	this->setWindowTitle("Bilbo Blogger");
	
	tabPosts = new QTabWidget(this);
	tabPosts->setGeometry(QRect(3, 56, 577, 455));
    tabPosts->setElideMode(Qt::ElideRight);
	this->setCentralWidget(tabPosts);
	
	menubar = new QMenuBar(this);
	//menubar->setGeometry(QRect(0, 0, 887, 22));
	menuBilbo = new QMenu("Bilbo",menubar);
	menuPost = new QMenu("Post",menubar);
	menuAbout = new QMenu("About",menubar);
    menuSave = new QMenu("Save", 0);
	this->setMenuBar(menubar);
	
	toolbarPost=new QToolBar(this);
	//toolbarPost->setGeometry(QRect(0,20,311,45));
	//toolbarPost->setAllowedAreas(Qt::TopToolBarArea);
	toolbarPost->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	this->addToolBar(Qt::TopToolBarArea,toolbarPost);
	
	toolbarBlogger=new QToolBar(this);
	//toolbarBlogger->setGeometry(QRect(315,20,261,45));
	toolbarBlogger->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	this->addToolBar(Qt::TopToolBarArea,toolbarBlogger);
	
	toolbarAction = new QToolBar(this);
	toolbarAction->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	this->addToolBar(Qt::TopToolBarArea,toolbarAction);
	
	statusbar = new QStatusBar(this);
	this->setStatusBar(statusbar);
	
    setFocusPolicy(Qt::StrongFocus);
    
    toolbox=new Toolbox(statusbar, this);
    this->addDockWidget(Qt::RightDockWidgetArea,toolbox);
//     toolbox->setMinimumWidth(280);
    
    btnRemovePost = new QToolButton(tabPosts);
    btnRemovePost->setIcon(QIcon(":/media/dialog-close.png"));
    btnRemovePost->setToolTip("Remove current post");
    tabPosts->setCornerWidget(btnRemovePost, Qt::TopRightCorner);
    connect(btnRemovePost, SIGNAL(clicked( bool )), this, SLOT(sltRemoveCurrentPostEntry()));
    
	sltCreateNewPost();
    activePost = qobject_cast<PostEntry*>(tabPosts->currentWidget());
    previousActivePostIndex = 0;
	
	
// 	connect(tabPosts->tabBar(), SIGNAL());
}

void MainWindow::createActions()
{
	actAddBlog=new QAction(QIcon(":/media/format-text-bold.png"),"Add &Blog",this);
	connect(actAddBlog,SIGNAL(triggered( bool )), toolbox, SLOT(sltAddBlog()));
	
	actUploadAll=new QAction(QIcon(":/media/format-text-bold.png"),"&Upload All Changes",this);
	connect(actUploadAll,SIGNAL(triggered( bool )),this,SLOT(sltUploadAllChanges()));
	
	actNewPost=new QAction(QIcon(":/media/tab-new.png"),"&New Post",this);
	actNewPost->setShortcut(tr("Ctrl+N"));
	connect(actNewPost,SIGNAL(triggered( bool )),this,SLOT(sltCreateNewPost()));
	
	actPublish=new QAction(QIcon(":/media/format-text-bold.png"),"&Publish",this);
	connect(actPublish,SIGNAL(triggered( bool )),this,SLOT(sltPublishPost()));
    
    actSave = new QAction(QIcon(":/media/format-text-bold.png"), "Save", this);
    actSave->setMenu(menuSave);
    connect(actSave, SIGNAL(triggered( bool )), this, SLOT(sltSavePostLocally()));
	
	actSaveLocally=new QAction(QIcon(":/media/format-text-bold.png"),"Save Locally",this);
    actSaveLocally->setShortcut(tr("Ctrl+S"));
	connect(actSaveLocally, SIGNAL(triggered( bool )), this, SLOT(sltSavePostLocally()));
	
	actSaveDraft=new QAction(QIcon(":/media/format-text-bold.png"),"Save as Draft",this);
    actSaveDraft->setShortcut(tr("Ctrl+Shift+S"));
	connect(actSaveDraft, SIGNAL(triggered( bool )), this, SLOT(sltSaveAsDraft()));
	
	actDeletePost=new QAction(QIcon(":/media/format-text-bold.png"),"Delete from Server",this);
// 	connect(actDeletePost,SIGNAL(triggered( bool )),activePost,SLOT(sltDelPost()));
	
	actDeleteLocally=new QAction(QIcon(":/media/format-text-bold.png"),"Delete Locally",this);
// 	connect(actDeleteLocally,SIGNAL(triggered( bool )),activePost,SLOT(sltDelLocally()));
	
	actAbout = new QAction(QIcon(":/media/format-text-bold.png"),"About",this);
	connect(actAbout, SIGNAL(triggered( bool )), this, SLOT(sltBilboAbout()));
	
	actQuit = new QAction(QIcon(":/media/format-text-bold.png"), "Quit", this);
	actQuit->setShortcut(tr("Ctrl+Q"));
	connect(actQuit, SIGNAL(triggered( bool )), this, SLOT(sltQuit()));
	
	actToggleToolboxVisible = new QAction(QIcon(":/media/format-text-bold.png"), "Hide Toolbox", this);
	actToggleToolboxVisible->setShortcut(tr("Ctrl+T"));
	connect(actToggleToolboxVisible, SIGNAL(triggered( bool )), this, SLOT(sltToggleToolboxVisible()));
	
	addCreatedActions();
}

void MainWindow::addCreatedActions()
{
// 	saveActions=new QActionGroup(this);
// 	saveActions->addAction(actSaveLocally);
// 	saveActions->addAction(actSaveDraft);
    
    menuSave->addAction(actSaveDraft);
    menuSave->addAction(actSaveLocally);
	
	toolbarPost->addAction(actNewPost);
	toolbarPost->addAction(actSave);
	toolbarPost->addAction(actPublish);
	
	toolbarBlogger->addAction(actAddBlog);
	toolbarBlogger->addAction(actUploadAll);
	
	toolbarAction->addAction(actAbout);
	//toolbarAction->addAction(actQuit);
	
	menubar->addAction(menuBilbo->menuAction());
	menubar->addAction(menuPost->menuAction());
	menubar->addAction(menuAbout->menuAction());
	
	menuBilbo->addAction(actToggleToolboxVisible);
	menuBilbo->addAction(actQuit);
	
	menuPost->addAction(actNewPost);
	menuPost->addSeparator();
	menuPost->addAction(actPublish);
	menuPost->addSeparator();
	menuPost->addAction(actSaveDraft);
	menuPost->addAction(actSaveLocally);
	menuPost->addSeparator();
	menuPost->addAction(actDeletePost);
	menuPost->addAction(actDeleteLocally);
	
	menuAbout->addAction(actAbout);
}

void MainWindow::sltCreateNewPost()
{
    qDebug("MainWindow::sltCreateNewPost");
	PostEntry *temp=new PostEntry(this);
	tabPosts->addTab(temp,"Untitled");
    temp->setCurrentPost();
    temp->setCurrentPostBlogId(toolbox->currentBlogId());
	
	// FIXME these lines added to set direction for new posts, but it generates Segmentation fault at run time!
// 	BilboBlog *tmp = __db->getBlogInfo(toolbox->currentBlogId());
// 	temp->setDefaultLayoutDirection(tmp->direction());
// 	delete tmp;
	
    connect(temp, SIGNAL(sigTitleChanged(const QString& )), this, SLOT(sltPostTitleChanged(const QString&)));
// 	activePost=temp;
	tabPosts->setCurrentWidget(temp);
	if(this->isVisible()==false)
		this->show();
}

void MainWindow::sltUploadAllChanges()
{
}

void MainWindow::sltPostTitleChanged(const QString& title)
{
    qDebug("MainWindow::sltPostTitleChanged");
	tabPosts->setTabText(tabPosts->currentIndex(),title);
}

void MainWindow::sltBilboAbout()
{
	QMessageBox::information(this, "About Bilbo", "Bilbo Blogger.\nLicense: GNU GPL v3\n\nAuthors:\n\tMehrdad Momeny\n\tGolnaz Nilieh");
}

void MainWindow::sltQuit()
{
	qDebug("MainWindow::sltQuit");
	writeConfig();
	qApp->quit();
}

void MainWindow::sltToggleToolboxVisible()
{
	if(toolbox->isVisible()){
		toolbox->hide();
		actToggleToolboxVisible->setText(tr("Show Toolbox"));
	} else {
		toolbox->show();
		actToggleToolboxVisible->setText(tr("Hide Toolbox"));
	}
}

void MainWindow::sltActivePostChanged(int index)
{
    qDebug("MainWindow::sltActivePostChanged");
    qDebug()<<"new post index: "<<index<<"\tPrev Index: "<<previousActivePostIndex;

    activePost = qobject_cast<PostEntry*>( tabPosts->widget(index) );
    PostEntry *prevActivePost = qobject_cast<PostEntry*>( tabPosts->widget( previousActivePostIndex ) );
    if(prevActivePost != 0 && index != previousActivePostIndex){
    prevActivePost->setCurrentPost((*toolbox->getFieldsValue()));
    prevActivePost->setCurrentPostBlogId(toolbox->currentBlogId());
    tabPosts->setTabText(previousActivePostIndex, prevActivePost->postTitle());
    }
    if(activePost != 0){
    toolbox->setFieldsValue(activePost->currentPost());
    toolbox->setCurrentBlog(activePost->currentPostBlogId());
    previousActivePostIndex = index;
    sltPostTitleChanged( activePost->postTitle() );
    } else {
        qDebug() << "MainWindow::sltActivePostChanged: ActivePost is NULL! \
                tabPosts Current index is: " << tabPosts->currentIndex() ;
    }
}

void MainWindow::sltPublishPost()
{
	qDebug("MainWindow::sltPublishPost");
	int blog_id = toolbox->currentBlogId();
	if(blog_id==-1){
		QMessageBox::warning(this, "Blog not sets", "You have to select a blog to publish this post to it.");
		qDebug("MainWindow::sltPublishPost: Blog id not sets correctly.");
		return;
	}
	Backend *b = new Backend(blog_id);
    connect(b, SIGNAL(sigPostPublished(int, int, bool)), this, SLOT(sltPostPublished(int, int, bool)));
	BilboPost *post = toolbox->getFieldsValue();
	if(activePost->postBody()->isEmpty() || activePost->postTitle().isEmpty()){
		if(QMessageBox::warning(this, "Empty fields!",
							 "Your post title or body is empty!\nAre you sure of pubish this post?",
		QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Cancel)
			return;
	}
	post->setContent(*(activePost->postBody()));
	post->setTitle(activePost->postTitle());
	post->setPrivate(false);
	
	b->publishPost(post);
	statusbar->showMessage("publishing new post...");
    this->setCursor(Qt::BusyCursor);
}

void MainWindow::sltPostPublished(int blog_id, int post_id, bool isPrivate)
{
	qDebug("MainWindow::sltPostPublished: Post Id: %d", post_id);
	QString blog_name = toolbox->listBlogs.key(blog_id);
    QString msg;
    if(isPrivate){
        msg = "New Draft saved to \"" + blog_name +
                "\" successfully\nDo you want to keep it on editor?";
        statusbar->showMessage("New draft saved to \""+blog_name + "\"" , STATUSTIMEOUT);
    }
    else {
        msg = "New Post published to \"" + blog_name +
                "\" successfully\nDo you want to keep it on editor?";
        statusbar->showMessage("New post published to \""+blog_name + "\"" , STATUSTIMEOUT);
    }
	if(QMessageBox::information(this, "Successful",  msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
           != QMessageBox::Yes)
		sltRemoveCurrentPostEntry();
	
    this->unsetCursor();
}

void MainWindow::sltRemoveCurrentPostEntry()
{
    qDebug("MainWindow::sltRemoveCurrentPostEntry");
	if(tabPosts->count()==1){
		sltCreateNewPost();
        previousActivePostIndex = 0;
	}
	tabPosts->removeTab(tabPosts->currentIndex());
    tabPosts->setCurrentIndex(previousActivePostIndex);
}

void MainWindow::sltNewPostSelected(BilboPost * newPost)
{
    qDebug("MainWindow::sltNewPostSelected");
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

void MainWindow::keyReleaseEvent(QKeyEvent * event)
{
    if( event->modifiers() == Qt::ControlModifier){
        switch( event->key() ){
        case  Qt::Key_1:
            toolbox->setCurrentPage(0);
            break;
        case Qt::Key_2:
            toolbox->setCurrentPage(1);
            break;
        case Qt::Key_3:
            toolbox->setCurrentPage(2);
            break;
        case Qt::Key_4:
            toolbox->setCurrentPage(3);
            break;
        case Qt::Key_5:
            toolbox->setCurrentPage(4);
            break;
        default:
            QMainWindow::keyPressEvent(event);
            break;
        }
    }
}

void MainWindow::sltCurrentBlogChanged(int blog_id)
{
	//suggestion:
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
    qDebug("MainWindow::sltSavePostLocally");
}

void MainWindow::sltSaveAsDraft()
{
    qDebug("MainWindow::sltSaveAsDraft");
    int blog_id = toolbox->currentBlogId();
    if(blog_id==-1){
        QMessageBox::warning(this, "Blog not sets", "You have to select a blog to save this post as draft to it.");
        qDebug("MainWindow::sltSaveAsDraft: Blog id not sets correctly.");
        return;
    }
    Backend *b = new Backend(blog_id);
    connect(b, SIGNAL(sigPostPublished(int, int, bool)), this, SLOT(sltPostPublished(int, int, bool)));
    BilboPost *post = toolbox->getFieldsValue();
    if(activePost->postBody()->isEmpty() || activePost->postTitle().isEmpty()){
        if(QMessageBox::warning(this, "Empty fields!",
           "Your post title or body is empty!\nAre you sure of pubish this post?",
           QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Cancel)
            return;
    }
    post->setContent(*(activePost->postBody()));
    post->setTitle(activePost->postTitle());
    post->setPrivate(true);
	
    b->publishPost(post);
    statusbar->showMessage("Save new post draft...");
    this->setCursor(Qt::BusyCursor);
}


