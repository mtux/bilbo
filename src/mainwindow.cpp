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
	setupUi(this);
	
	activePost=new PostEntry(this);
	tabPosts->addTab(activePost,"Untitled");
	tabPosts->setCurrentIndex(0);
	
	toolbox=new Toolbox(this);
	this->addDockWidget(Qt::RightDockWidgetArea,toolbox);
	
	createActions();
	
	postToolbar->addAction(newPost);
	postToolbar->addAction(saveLocally);
	postToolbar->addAction(publish);
	
	bloggerToolbar->addAction(addBlog);
	bloggerToolbar->addAction(uploadAll);
	
	menubar->addAction(menuBilbo->menuAction());
	menubar->addAction(menuEdit->menuAction());
	menuBilbo->addAction(actionTest);
	menuBilbo->addSeparator();
	menuBilbo->addAction(actionQuit);
}


MainWindow::~MainWindow()
{
}

void MainWindow::setupUi(QMainWindow *window)
{
	window->resize(887, 559);
	centralwidget = new QWidget(window);
	centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
	window->setCentralWidget(centralwidget);
	
	tabPosts = new QTabWidget(centralwidget);
	tabPosts->setObjectName(QString::fromUtf8("tabPosts"));
	tabPosts->setGeometry(QRect(3, 56, 577, 455));
	
	menubar = new QMenuBar(window);
	menubar->setObjectName(QString::fromUtf8("menubar"));
	menubar->setGeometry(QRect(0, 0, 887, 22));
	menuBilbo = new QMenu(menubar);
	menuBilbo->setObjectName(QString::fromUtf8("menuBilbo"));
	menuEdit = new QMenu(menubar);
	menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
	window->setMenuBar(menubar);
	
	postToolbar=new QToolBar(window);
	postToolbar->setGeometry(QRect(0,20,311,45));
	postToolbar->setAllowedAreas(Qt::TopToolBarArea);
	postToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	
	bloggerToolbar=new QToolBar(window);
	bloggerToolbar->setGeometry(QRect(315,20,261,45));
	bloggerToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	
	statusbar = new QStatusBar(window);
	statusbar->setObjectName(QString::fromUtf8("statusbar"));
	window->setStatusBar(statusbar);
	
	window->setWindowTitle(QApplication::translate("window", "MainWindow", 0, QApplication::UnicodeUTF8));
	menuBilbo->setTitle(QApplication::translate("BilboMain", "Bilbo", 0, QApplication::UnicodeUTF8));
	menuEdit->setTitle(QApplication::translate("BilboMain", "Edit", 0, QApplication::UnicodeUTF8));
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
	
	saveActions=new QActionGroup(this);
	saveActions->addAction(saveLocally);
	saveActions->addAction(saveDraft);
	
	actionQuit=new QAction(this);
	actionTest=new QAction(this);
}

void MainWindow::addNewBlog()
{
}

void MainWindow::createNewPost()
{
	PostEntry *temp=new PostEntry();
	tabPosts->addTab(temp,"Untitled");
	activePost=temp;
}

void MainWindow::uploadAllChanges()
{
}

