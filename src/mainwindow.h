/***************************************************************************
 *   This file is part of the Bilbo Blogger.                               *
 *   Copyright (C) 2008-2009 Mehrdad Momeny <mehrdad.momeny@gmail.com>     *
 *   Copyright (C) 2008-2009 Golnaz Nilieh <g382nilieh@gmail.com>          *
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
#include <kxmlguiwindow.h>

class KTabWidget;
class KAction;
class KToggleAction;
class QProgressBar;
class QToolButton;
class Toolbox;
// class AddEditBlog;
class PostEntry;
class SysTray;
// class Backend;
class BilboMedia;
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

signals:
    void mediaFilesUploaded( int count );
    void settingsChanged();

protected slots:
    void sltCreateNewPost();

    void sltUploadAllChanges();
    void sltPostTitleChanged( const QString& title );
    void sltToggleToolboxVisible( bool isVisible );
    void sltActivePostChanged( int tabIndex );

    void sltNewPostOpened( BilboPost &newPost, int blog_id );

    void sltPublishPost();

    /**
     * Remove @p widget from main tab wigdet (if it's NULL current Post entry will be removed)
     */
    void sltRemovePostEntry( PostEntry *widget=0 );

    void sltCurrentBlogChanged( int blog_id );

    void sltSavePostLocally();
//     void sltSaveAsDraft();

    void sltError( const QString &errorMessage );

    /**
    To open temporary posts and that posts are open at previous quit.
    */
    void loadTempPosts();
    void sltClearCache();
    void uploadMediaObject();

private slots:
    void optionsPreferences();
    void postManipulationDone( bool isError, const QString &customMessage );
    void slotBusy( bool isBusy );
    void slotShowStatusMessage(const QString &message, bool isPermanent);
    void slotMediaObjectUploaded( BilboMedia* );

protected:
    void keyReleaseEvent( QKeyEvent * event );
    bool queryClose();
    /**
    *  Reimplemented to save settings
    */
    bool queryExit();

private:
    void setupActions();
    void setupSystemTray();
    void writeConfigs();
    /**
        Create a new post entry, 
        and return pointer to it's widget (Actually return value is a PostEntry instance)
    */
    QWidget* createPostEntry(int blog_id, const BilboPost& post);

    Toolbox *toolbox;
    QDockWidget *toolboxDock;
    PostEntry *activePost;
    SysTray *systemTray;
    KTabWidget *tabPosts;

    QToolButton *btnRemovePost;
    int previousActivePostIndex;

    int busyNumber;///If this is 0 so there isn't any progress! otherwise there is! so progressbar will show
    QProgressBar *progress;
};
#endif
