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
#include "bilbopost.h"
#include <QSettings>
MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
    qDebug("MainWindow::MainWindow");
    previousActivePostIndex = -1;
	createUi();
    setFocusPolicy(Qt::StrongFocus);
	toolbox=new Toolbox(statusbar, this);
	this->addDockWidget(Qt::RightDockWidgetArea,toolbox);
	toolbox->setMinimumWidth(280);
	
	readConfig();
	createActions();
	toolbox->setVisible(conf->isToolboxVisibleByDefault);
	if(conf->isToolboxVisibleByDefault){
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
	conf->isToolboxVisibleByDefault = toolbox->isVisible();
	
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
	
	activePost=new PostEntry(this);
	tabPosts->addTab(activePost,"Untitled");
	tabPosts->setCurrentIndex(0);
    previousActivePostIndex = 0;
	
	btnRemovePost = new QToolButton(tabPosts);
	btnRemovePost->setIcon(QIcon(":/media/dialog-close.png"));
	btnRemovePost->setToolTip("Remove current post");
	tabPosts->setCornerWidget(btnRemovePost, Qt::TopRightCorner);
	connect(btnRemovePost, SIGNAL(clicked( bool )), this, SLOT(sltRemoveCurrentPostEntry()));
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
	
	actSaveLocally=new QAction(QIcon(":/media/format-text-bold.png"),"Save Locally",this);
// 	connect(actSaveLocally,SIGNAL(triggered( bool )),activePost,SLOT(sltSavePostLocally()));
	
	actSaveDraft=new QAction(QIcon(":/media/format-text-bold.png"),"Save as Draft",this);
// 	connect(actSaveDraft,SIGNAL(triggered( bool )),activePost,SLOT(sltSaveAsDraft()));
	
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
	
	toolbarPost->addAction(actNewPost);
	toolbarPost->addAction(actSaveLocally);
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
    connect(temp, SIGNAL(sigTitleChanged(const QString &title)), this, SLOT(sltPostTitleChanged(const QString&)));
	activePost=temp;
	tabPosts->setCurrentWidget(temp);
	if(this->isVisible()==false)
		this->show();
}

void MainWindow::sltUploadAllChanges()
{
}

void MainWindow::sltPostTitleChanged(const QString& title)
{
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
    PostEntry *prevActivePost = qobject_cast<PostEntry*>( tabPosts->widget( previousActivePostIndex ) );
    if(prevActivePost!=0){
    prevActivePost->setCurrentPost((*toolbox->getFieldsValue()));
    prevActivePost->setCurrentPostBlogId(toolbox->currentBlogId());
    }
    
    toolbox->setFieldsValue(activePost->currentPost());
    toolbox->setCurrentBlog(activePost->currentPostBlogId());
    previousActivePostIndex = index;
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
	connect(b, SIGNAL(sigPostPublished(int, int)), this, SLOT(sltPostPublished(int, int)));
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
	statusbar->showMessage("Request to publish new post sent to server...");
    this->setCursor(Qt::BusyCursor);
}

void MainWindow::sltPostPublished(int blog_id, int post_id)
{
	qDebug("MainWindow::sltPostPublished: Post Id: %d", post_id);
	QString blog_name = toolbox->listBlogs.key(blog_id);
	if(QMessageBox::information(this, "Successful", "New Post published to \"" + blog_name +
			"\" blog successfully\nDo you want to keep it on editor?",
	   		QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
		sltRemoveCurrentPostEntry();
	statusbar->showMessage("New post published to \""+blog_name + "\"" , STATUSTIMEOUT);
    this->unsetCursor();
}

void MainWindow::sltRemoveCurrentPostEntry()
{
    qDebug("MainWindow::sltRemoveCurrentPostEntry");
	tabPosts->removeTab(tabPosts->currentIndex());
	if(tabPosts->count()==0){
		sltCreateNewPost();
	}
	
}

void MainWindow::sltNewPostSelected(BilboPost * newPost)
{
    qDebug("MainWindow::sltNewPostSelected");
    PostEntry *temp=new PostEntry(this);
    tabPosts->addTab(temp,newPost->title());
    
    
    temp->setPostTitle(newPost->title());
    temp->setPostBody(newPost->content());
    temp->setCurrentPost(*newPost);
    temp->setCurrentPostBlogId(toolbox->currentBlogId());

    activePost=temp;
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
    BilboBlog *tmp = db->getBlogInfo(blog_id);
    this->centralWidget()->setLayoutDirection(tmp->direction());
    delete tmp;
}


