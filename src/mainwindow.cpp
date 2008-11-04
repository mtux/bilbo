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

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
	createUi();
	
	activePost=new PostEntry(this);
	tabPosts->addTab(activePost,"Untitled");
	tabPosts->setCurrentIndex(0);
	
	toolbox=new Toolbox(this);
	this->addDockWidget(Qt::RightDockWidgetArea,toolbox);
	
	createActions();
}


MainWindow::~MainWindow()
{
}

void MainWindow::createUi()
{
	this->resize(887, 559);
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
	
	statusbarMain = new QStatusBar(this);
	this->setStatusBar(statusbarMain);
}

void MainWindow::createActions()
{
	actAddBlog=new QAction(QIcon(":/media/format-text-bold.png"),"Add &Blog",this);
	connect(actAddBlog,SIGNAL(triggered( bool )),this,SLOT(sltAddNewBlog()));
	
	actUploadAll=new QAction(QIcon(":/media/format-text-bold.png"),"&Upload All Changes",this);
	connect(actUploadAll,SIGNAL(triggered( bool )),this,SLOT(sltUploadAllChanges()));
	
	actNewPost=new QAction(QIcon(":/media/format-text-bold.png"),"&New Post",this);
	actNewPost->setShortcut(tr("Ctrl+N"));
	connect(actNewPost,SIGNAL(triggered( bool )),this,SLOT(sltCreateNewPost()));
	
	actPublish=new QAction(QIcon(":/media/format-text-bold.png"),"&Publish",this);
	connect(actPublish,SIGNAL(triggered( bool )),activePost,SLOT(sltPublishPost()));
	
	actSaveLocally=new QAction(QIcon(":/media/format-text-bold.png"),"Save Locally",this);
	connect(actSaveLocally,SIGNAL(triggered( bool )),activePost,SLOT(sltSavePostLocally()));
	
	actSaveDraft=new QAction(QIcon(":/media/format-text-bold.png"),"Save as Draft",this);
	connect(actSaveDraft,SIGNAL(triggered( bool )),activePost,SLOT(sltSaveAsDraft()));
	
	actDeletePost=new QAction(QIcon(":/media/format-text-bold.png"),"Delete from Server",this);
	connect(actDeletePost,SIGNAL(triggered( bool )),activePost,SLOT(sltDelPost()));
	
	actDeleteLocally=new QAction(QIcon(":/media/format-text-bold.png"),"Delete Locally",this);
	connect(actDeleteLocally,SIGNAL(triggered( bool )),activePost,SLOT(sltDelLocally()));
	
	actAbout = new QAction(QIcon(":/media/format-text-bold.png"),"About Us",this);
	connect(actAbout, SIGNAL(triggered( bool )), this, SLOT(sltBilboAbout()));
	
	actQuit = new QAction(QIcon(":/media/format-text-bold.png"), "Quit", this);
	actQuit->setShortcut(tr("Ctrl+Q"));
	connect(actQuit, SIGNAL(triggered( bool )), this, SLOT(sltQuit()));
	
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

void MainWindow::sltAddNewBlog()
{
}

void MainWindow::sltCreateNewPost()
{
	PostEntry *temp=new PostEntry(this);
	tabPosts->addTab(temp,"Untitled");
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
	QMessageBox::information(this, "About Us", "Bilbo Blogger.\n\nAuthors:\n\tMehrdad Momeny\n\tGolnaz Nilieh");
}

void MainWindow::sltQuit()
{
	qApp->quit();
}

