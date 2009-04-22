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

#ifndef POSTENTRIY_H
#define POSTENTRIY_H

#include <QFrame>
#include "bilbopost.h"

class BilboEditor;
class QWidget;
class QGridLayout;
class QLabel;
class QHBoxLayout;
class KLineEdit;
class BilboMedia;
class QProgressBar;
class QTimer;
/**
 * Post Entry Widget
 * contains Editor, and Title box.
 @author Mehrdad Momeny <mehrdad.momeny@gmail.com>
 @author Golnaz Nilieh <g382nilieh@gmail.com>
 */
class PostEntry: public QFrame
{
    Q_OBJECT
public:
    PostEntry( QWidget *parent );
    ~PostEntry();
    QString postTitle() const;

    void setPostTitle( const QString &title );
    void setPostBody( const QString &content, const QString &additionalContent=QString() );

    int currentPostBlogId();
    void setCurrentPostBlogId( int blog_id );

    BilboPost* currentPost();
    void setCurrentPost( const BilboPost &post );

    Qt::LayoutDirection defaultLayoutDirection();
    void setDefaultLayoutDirection( Qt::LayoutDirection direction );

    QMap <QString, BilboMedia*> & mediaList();

    /**
     * Will Upload media files not uploaded yet, and return true if there's any file to upload
     * and False if there isn't any file!
     * @return True if there's any file to upload, otherwise false.
     */
    bool uploadMediaFiles();

    void publishPost ( int blogId, const BilboPost &postData );

    void saveLocally();

Q_SIGNALS:
    /**
     * emitted when title of this entry changed.
     * @param title is a QString which contains new title.
     */
    void sigTitleChanged( const QString &title );
    /**
     * This signal emitted when a post manipulation job e.g. Publishing a new post finished.
     * @param isError If an error occurred on publishing this will be TRUE. Otherwise FLASE
     * @param customMessage A Custom message will be shown on StatusBar.
     */
    void postPublishingDone( bool isError, const QString &customMessage );

    /**
     * This signal is emitted when the post contents (Title or content) is modified!
     */
    void postModified();

    /**
     * This signal is emitted when the post is saved temporarily!
     */
    void postSavedTemporary();

    void postSavedLocally();

    /**
     * To show a message on statusBar
     * @param message Message to be shown
     * @param isPermanent If it's true the message will not have a timeout!
     *  so it will be shown until next message arrived
     */
    void showStatusMessage( const QString& message, bool isPermanent);

    /**
     * This signal is emitted for operations in background, like request of some
     * data from the web.
     * @param isBusy if it's true, the operation is in progress. otherwise, it
     * is finished.
     */
    void sigBusy( bool isBusy );

public Q_SLOTS:
    void settingsChanged();

protected Q_SLOTS:
    void sltMediaFileUploaded( BilboMedia *media );
    void sltError( const QString& errMsg );
    void sltMediaError( const QString &errorMessage, BilboMedia* media );
    void sltPostPublished( int blog_id, BilboPost *post );

private Q_SLOTS:
    void sltTitleChanged( const QString& title );
    void deleteProgressBar();
    void saveTemporary( bool force=false );
    void slotPostModified();
    void slotQuit();

private:
    void createUi();
    void publishPostAfterUploadMediaFiles();
    void setCurrentPostFromEditor();

    QProgressBar *progress;
    BilboEditor *editPostWidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *labelTitle;
    KLineEdit *txtTitle;
    QTimer *mTimer;
    BilboPost mCurrentPost;
    int mCurrentPostBlogId;
    QMap <QString, BilboMedia*> mMediaList;

    int mNumOfFilesToBeUploaded;
    bool isUploadingMediaFilesFailed;
    bool isNewPost;
//     bool mIsModified;
    bool isPostContentModified;
};

#endif