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



class QTabWidget;
class QToolButton;
class BilboPost;
class Toolbox;
class AddEditBlog;
class PostEntry;
class Backend;
/**
Main window of bilbo blogger implementation...

	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
*/
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
    MainWindow(QWidget* parent=0);

    ~MainWindow();
	
	QAction *actAddBlog;
	QAction *actUploadAll;
	QAction *actNewPost;
	QAction *actPublish;
	//QActionGroup *saveActions;
	QAction *actSaveLocally;
	QAction *actSaveDraft;
	QAction *actDeletePost;
	QAction *actDeleteLocally;
	QAction *actAbout;
	QAction *actQuit;
	QAction *actToggleToolboxVisible;
	
protected slots:
	void sltCreateNewPost();
	void sltUploadAllChanges();
	void sltPostTitleChanged(const QString& title);
	void sltBilboAbout();
	void sltQuit();
	void sltToggleToolboxVisible(bool);
	void sltActivePostChanged( int );
	
	void sltPublishPost();
	void sltPostPublished( int, int );
	/**
	 *    Slot to remove current Post entry from main tab wigdet!
	 */
	void sltRemoveCurrentPostEntry();
// 	void sltSavePostLocally();
// 	void sltSaveAsDraft();
// 	void sltDelPost();
// 	void sltDelLocally();
	
private:
	void createUi();
	void createActions();
	void addCreatedActions();
	void readConfig();
	void writeConfig();

	
	Toolbox *toolbox;
	PostEntry *activePost;
	AddEditBlog *addBlogPage;
	
	QTabWidget *tabPosts;
	QToolBar *toolbarPost;
	QToolBar *toolbarBlogger;
	QToolBar *toolbarAction;
	QMenuBar *menubar;
	QMenu *menuBilbo;
	QMenu *menuPost;
	QMenu *menuAbout;
	QStatusBar *statusbar;
	QToolButton *btnRemovePost;
	
};

#endif
