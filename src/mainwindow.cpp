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
	setupUi();
	
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

void MainWindow::setupUi()
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
	
	postToolbar=new QToolBar(this);
	//postToolbar->setGeometry(QRect(0,20,311,45));
	//postToolbar->setAllowedAreas(Qt::TopToolBarArea);
	postToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	this->addToolBar(Qt::TopToolBarArea,postToolbar);
	
	bloggerToolbar=new QToolBar(this);
	//bloggerToolbar->setGeometry(QRect(315,20,261,45));
	bloggerToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	this->addToolBar(Qt::TopToolBarArea,bloggerToolbar);
	
	actionToolbar = new QToolBar(this);
	actionToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	this->addToolBar(Qt::TopToolBarArea,actionToolbar);
	
	statusbar = new QStatusBar(this);
	this->setStatusBar(statusbar);
}

void MainWindow::createActions()
{
	addBlog=new QAction(QIcon(":/media/format-text-bold.png"),"Add &Blog",this);
	connect(addBlog,SIGNAL(triggered( bool )),this,SLOT(addNewBlog()));
	
	uploadAll=new QAction(QIcon(":/media/format-text-bold.png"),"&Upload All Changes",this);
	connect(addBlog,SIGNAL(triggered( bool )),this,SLOT(uploadAllChanges()));
	
	newPost=new QAction(QIcon(":/media/format-text-bold.png"),"&New Post",this);
	connect(newPost,SIGNAL(triggered( bool )),this,SLOT(createNewPost()));
	
	publish=new QAction(QIcon(":/media/format-text-bold.png"),"&Publish",this);
	connect(publish,SIGNAL(triggered( bool )),activePost,SLOT(publishPost()));
	
	saveLocally=new QAction(QIcon(":/media/format-text-bold.png"),"Save Locally",this);
	connect(saveLocally,SIGNAL(triggered( bool )),activePost,SLOT(savePostLocally()));
	
	saveDraft=new QAction(QIcon(":/media/format-text-bold.png"),"Save as Draft",this);
	connect(saveDraft,SIGNAL(triggered( bool )),activePost,SLOT(saveAsDraft()));
	
	deletePost=new QAction(QIcon(":/media/format-text-bold.png"),"Delete from Server",this);
	connect(deletePost,SIGNAL(triggered( bool )),activePost,SLOT(sltDelPost()));
	
	deleteLocally=new QAction(QIcon(":/media/format-text-bold.png"),"Delete Locally",this);
	connect(deleteLocally,SIGNAL(triggered( bool )),activePost,SLOT(sltDelLocally()));
	
	aboutUs = new QAction(QIcon(":/media/format-text-bold.png"),"About Us",this);
	connect(aboutUs, SIGNAL(triggered( bool )), this, SLOT(sltAboutUs()));
	
	actQuit = new QAction(QIcon(":/media/format-text-bold.png"), "Quit", this);
	connect(actQuit, SIGNAL(triggered( bool )), this, SLOT(sltQuit()));
	
	addCreatedActions();
	
}

void MainWindow::addCreatedActions()
{
	saveActions=new QActionGroup(this);
	saveActions->addAction(saveLocally);
	saveActions->addAction(saveDraft);
	
	postToolbar->addAction(newPost);
	postToolbar->addAction(saveLocally);
	postToolbar->addAction(publish);
	
	bloggerToolbar->addAction(addBlog);
	bloggerToolbar->addAction(uploadAll);
	
	actionToolbar->addAction(aboutUs);
	//actionToolbar->addAction(actQuit);
	
	menubar->addAction(menuBilbo->menuAction());
	menubar->addAction(menuPost->menuAction());
	menubar->addAction(menuAbout->menuAction());
	
	menuBilbo->addAction(actQuit);
	
	menuPost->addAction(newPost);
	menuPost->addSeparator();
	menuPost->addAction(publish);
	menuPost->addSeparator();
	menuPost->addAction(saveDraft);
	menuPost->addAction(saveLocally);
	menuPost->addSeparator();
	menuPost->addAction(deletePost);
	menuPost->addAction(deleteLocally);
	
	menuAbout->addAction(aboutUs);
}

void MainWindow::addNewBlog()
{
}

void MainWindow::createNewPost()
{
	PostEntry *temp=new PostEntry(this);
	tabPosts->addTab(temp,"Untitled");
	activePost=temp;
	tabPosts->setCurrentWidget(temp);
}

void MainWindow::uploadAllChanges()
{
}

void MainWindow::postTitleChanged(const QString& title)
{
	tabPosts->setTabText(tabPosts->currentIndex(),title);
}

void MainWindow::sltAboutUs()
{
	QMessageBox::information(this, "About Us", "Bilbo Blogger.\n\nAuthors:\n\tMehrdad Momeny\n\tGolnaz Nilieh");
}

void MainWindow::sltQuit()
{
	qApp->quit();
}

