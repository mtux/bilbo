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
#include <QSettings>
MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
	createUi();
	readConfig();
	toolbox->setMinimumWidth(280);
	createActions();
	
	if(toolbox->isVisible()){
		actToggleToolboxVisible->setText(tr("Hide Toolbox"));
	} else {
		actToggleToolboxVisible->setText(tr("Show Toolbox"));
	}
	
	connect(tabPosts, SIGNAL(currentChanged( int )), this, SLOT(sltActivePostChanged(int)));
}

MainWindow::~MainWindow()
{
}

void MainWindow::readConfig()
{
	QSettings config(CONF_PATH, QSettings::NativeFormat);

	QPoint pos = config.value("pos", QPoint(300, 200)).toPoint();
	QSize size = config.value ( "size", QSize ( 800, 600 ) ).toSize();
	int toolboxWidth = config.value ( "toolbox_width", 300 ).toInt();
	toolbox->setVisible(config.value ( "toolbox_visible", true ).toBool());

	resize(size);
	toolbox->resize(toolboxWidth, toolbox->height());
	move(pos);
}

void MainWindow::writeConfig()
{
	QSettings config(CONF_PATH, QSettings::NativeFormat);
	config.setValue("pos" , pos());
	config.setValue("size", size() );
	config.setValue("toolbox_width", toolbox->width());
	config.setValue("toolbox_visible", toolbox->isVisible());
}

void MainWindow::createUi()
{
// 	this->resize(887, 559);
	this->setWindowTitle("MainWindow");
	
	tabPosts = new QTabWidget(this);
	tabPosts->setGeometry(QRect(3, 56, 577, 455));
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
	
	toolbox=new Toolbox(this);
	this->addDockWidget(Qt::RightDockWidgetArea,toolbox);
	
	btnRemovePost = new QToolButton(tabPosts);
	btnRemovePost->setIcon(QIcon(":/media/format-text-bold.png"));
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
	
	actNewPost=new QAction(QIcon(":/media/format-text-bold.png"),"&New Post",this);
	actNewPost->setShortcut(tr("Ctrl+N"));
	connect(actNewPost,SIGNAL(triggered( bool )),this,SLOT(sltCreateNewPost()));
	
	actPublish=new QAction(QIcon(":/media/format-text-bold.png"),"&Publish",this);
	connect(actPublish,SIGNAL(triggered( bool )),this,SLOT(sltPublishPost()));
	
	actSaveLocally=new QAction(QIcon(":/media/format-text-bold.png"),"Save Locally",this);
	connect(actSaveLocally,SIGNAL(triggered( bool )),activePost,SLOT(sltSavePostLocally()));
	
	actSaveDraft=new QAction(QIcon(":/media/format-text-bold.png"),"Save as Draft",this);
	connect(actSaveDraft,SIGNAL(triggered( bool )),activePost,SLOT(sltSaveAsDraft()));
	
	actDeletePost=new QAction(QIcon(":/media/format-text-bold.png"),"Delete from Server",this);
	connect(actDeletePost,SIGNAL(triggered( bool )),activePost,SLOT(sltDelPost()));
	
	actDeleteLocally=new QAction(QIcon(":/media/format-text-bold.png"),"Delete Locally",this);
	connect(actDeleteLocally,SIGNAL(triggered( bool )),activePost,SLOT(sltDelLocally()));
	
	actAbout = new QAction(QIcon(":/media/format-text-bold.png"),"About",this);
	connect(actAbout, SIGNAL(triggered( bool )), this, SLOT(sltBilboAbout()));
	
	actQuit = new QAction(QIcon(":/media/format-text-bold.png"), "Quit", this);
	actQuit->setShortcut(tr("Ctrl+Q"));
	connect(actQuit, SIGNAL(triggered( bool )), this, SLOT(sltQuit()));
	
	actToggleToolboxVisible = new QAction(QIcon(":/media/format-text-bold.png"), "Hide Toolbox", this);
	actToggleToolboxVisible->setShortcut(tr("Ctrl+T"));
	connect(actToggleToolboxVisible, SIGNAL(triggered( bool )), this, SLOT(sltToggleToolboxVisible(bool)));
	
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
	PostEntry *temp=new PostEntry(this);
	tabPosts->addTab(temp,"Untitled");
	activePost=temp;
	tabPosts->setCurrentWidget(temp);
	toolbox->resetFields();
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

void MainWindow::sltToggleToolboxVisible(bool v)
{
	if(toolbox->isVisible()){
		toolbox->hide();
		actToggleToolboxVisible->setText(tr("Show Toolbox"));
	} else {
		toolbox->show();
		actToggleToolboxVisible->setText(tr("Hide Toolbox"));
	}
}

void MainWindow::sltActivePostChanged(int )
{
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
	post->setContent(*(activePost->postBody()));
	post->setTitle(activePost->postTitle());
	qDebug(activePost->postTitle().toLatin1().data());
	qDebug(post->title().toLatin1().data());
	post->setPrivate(false);
	
	b->publishPost(post);
}

void MainWindow::sltPostPublished(int blog_id, int post_id)
{
	qDebug("MainWindow::sltPostPublished");
	QMessageBox::information(this, "Successful", "New Post published to \""+toolbox->listBlogs.key(blog_id)+"\" blog successfully");
	sltRemoveCurrentPostEntry();
}

void MainWindow::sltRemoveCurrentPostEntry()
{
	tabPosts->removeTab(tabPosts->currentIndex());
	if(tabPosts->count()==0){
		sltCreateNewPost();
	}
	
}


