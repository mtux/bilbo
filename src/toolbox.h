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

#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QDockWidget>
#include <QtGui>
#include <QDir>
#include "category.h"
#include "ui_toolboxbase.h"

class KStatusBar;
class QRadioButton;
class QButtonGroup;
class BilboPost;
class AddEditBlog;
class BilboBlog;
class BlogRadioButton;
class CatCheckBox;
/**
 @author Mehrdad Momeny <mehrdad.momeny@gmail.com>
 @author Golnaz Nilieh <g382nilieh@gmail.com>
 */
class Toolbox: public QWidget, public Ui::ToolboxBase
{
    Q_OBJECT
public:
    Toolbox( QWidget *parent );
    ~Toolbox();

    void reloadBlogList();
    /**
     *    Will set current state of toolbox (Current post) properties on input pointer!
     * @param currentPost input and output of this Function.
     */
    void getFieldsValue( BilboPost &currentPost );
    //void setFieldsValue(const BilboPost& post);
    void setFieldsValue( BilboPost* post = 0 );
    int currentBlogId();
    const BilboBlog *currentBlog();
    void setCurrentBlog( int blog_id );
    void setCurrentPage( int index );
    const QMap<int, BilboBlog*> & blogList() const;
//     void addLocalPostToList(QVariantMap data);

public slots:
    void sltAddBlog();
    void sltBlogAdded( BilboBlog& );
    void sltEditBlog();
    void sltBlogEdited( BilboBlog& );
    void sltRemoveBlog();
    void sltSetCurrentBlog();
    void sltReloadCategoryList();
    void sltLoadCategoryListFromDB( int blog_id );
    void sltUpdateEntries();
    void sltGetEntriesCount( int );
    void sltLoadEntriesFromDB( int blog_id );
    void sltRemoveSelectedEntryFromServer();
    void sltCurrentBlogChanged( int blog_id );
    void resetFields();
    void sltEntrySelected( QListWidgetItem *item );
    void sltEntriesCopyUrl();
    void sltLocalEntrySelected( int row, int column );
    void reloadLocalPosts();
    void sltRemoveLocalEntry();
    void clearEntries();
    void setDateTimeNow();

signals:
    void sigCurrentBlogChanged( int blog_id );
    void sigEntrySelected( BilboPost &post, int blog_id );
    void sigError( const QString& );
    void sigBusy( bool isBusy );

protected slots:
    void slotPostRemoved( int blog_id, const BilboPost &post );

private:
    QStringList selectedCategoriesTitle();
    QList<Category> selectedCategories();
    QList<int> selectedCategoriesId();
    void setSelectedCategories( const QStringList& );
    void setSelectedCategories( const QList<int>& );
    QStringList currentTags();
    void clearCatList();
    void unCheckCatList();
    void setButtonsIcon();

    QButtonGroup listBlogRadioButtons;
    QList<CatCheckBox*> listCategoryCheckBoxes;
    QMap<int, BilboBlog*> mBlogList;
    BlogRadioButton *blogToEdit;
    QDir blogToEditDir;
//  BilboPost *currentPost;
    KStatusBar *statusbar;
};

#endif
