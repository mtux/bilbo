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

#include "bilbopost.h"
#include "ui_settingsbase.h"
#include <kxmlguiwindow.h>

class KTabWidget;
class KAction;
// class KToggleAction;
class QToolButton;
// class BilboPost;
class Toolbox;
// class AddEditBlog;
class PostEntry;
class SysTray;
// class Backend;
// class BilboMedia;
/**
Main window of bilbo blogger implementation...

	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
 */
class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    MainWindow();

    ~MainWindow();
    
protected slots:
    void sltCreateNewPost();
    void settingsChanged();
    
    void sltUploadAllChanges();
    void sltPostTitleChanged(const QString& title);
    void sltToggleToolboxVisible();
    void sltActivePostChanged( int tabIndex );
    
    void sltNewPostSelected( BilboPost *newPost );
	
    void sltPublishPost();
    void sltPostPublished( int blog_id, int post_id, bool isPrivate );
	/**
     *    Slot to remove current Post entry from main tab wigdet!
     */
    void sltRemoveCurrentPostEntry();

    void sltCurrentBlogChanged( int blog_id );
    
    void sltSavePostLocally();
    void sltSaveAsDraft();
    
    void sltError( QString &errorMessage );
    
private slots:
    void optionsPreferences();
    
private:
    void setupActions();
    void setupSystemTray();
    void writeConfigs();
    
    Ui::SettingsBase ui_prefs_base;
    
    Toolbox *toolbox;
    QDockWidget *toolboxDock;
    PostEntry *activePost;
	SysTray *systemTray;
    KTabWidget *tabPosts;
    
    KAction *actAddBlog;
    KAction *actUploadAll;
    KAction *actNewPost;
    KAction *actPublish;
    KAction *actSaveLocally;
    KAction *actSaveDraft;
    KAction *actDeletePost;
    KAction *actDeleteLocally;
//     KAction *actAbout;
//     KAction *actQuit;
    KAction *actToggleToolboxVisible;
    KAction *actSave;
    
    QToolButton *btnRemovePost;
    
    int previousActivePostIndex;
};
#endif
