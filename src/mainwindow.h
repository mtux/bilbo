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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui/QToolBar>
//#include "ui_mainwindow.h"
#include "toolbox.h"
#include "postentry.h"
#include "addeditblog.h"

/**
Main window of bilbo blogger implementation...

	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
*/
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
    MainWindow(QWidget* parent);

    ~MainWindow();
	
private slots:
	void addNewBlog();
	void createNewPost();
	void uploadAllChanges();
	void postTitleChanged(const QString& title);
	
private:
	Toolbox *toolbox;
	PostEntry *activePost;
	AddEditBlog *addBlogPage;
	
	QTabWidget *tabPosts;
	QToolBar *postToolbar;
	QToolBar *bloggerToolbar;
	QMenuBar *menubar;
	QMenu *menuBilbo;
	QMenu *menuEdit;
	QStatusBar *statusbar;
	
	QAction *addBlog;
	QAction *uploadAll;
	QAction *newPost;
	QAction *publish;
	QActionGroup *saveActions;
	QAction *saveLocally;
	QAction *saveDraft;
	
	void setupUi();
	void createActions();
	void addCreatedActions();
};

#endif
