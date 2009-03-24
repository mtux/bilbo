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

#include "blogsettings.h"
#include "addeditblog.h"
#include <kdebug.h>
#include <KMessageBox>
#include "dbman.h"

BlogSettings::BlogSettings( QWidget *parent )
    : QWidget( parent )
{
    kDebug();
    setupUi( this );

    connect( btnAdd, SIGNAL( clicked() ), this, SLOT( addAccount() ) );
    connect( btnEdit, SIGNAL( clicked() ), this, SLOT( editAccount() ) );
    connect( btnRemove, SIGNAL( clicked() ), this, SLOT( removeAccount() ) );
    connect( blogsTable, SIGNAL( currentItemChanged( QTableWidgetItem *, QTableWidgetItem * ) ),
             this, SLOT( blogsTablestateChanged() ) );

    btnAdd->setIcon( KIcon( "list-add" ) );
    btnEdit->setIcon( KIcon( "edit-rename" ) );
    btnRemove->setIcon( KIcon( "list-remove" ) );
    loadBlogsList();
}

BlogSettings::~BlogSettings()
{
    
}

void BlogSettings::addBlog()
{
    AddEditBlog *addEditBlogWindow = new AddEditBlog( -1, this );
    addEditBlogWindow->setAttribute( Qt::WA_DeleteOnClose );
    addEditBlogWindow->exec();
    connect( addEditBlogWindow, SIGNAL( sigBlogAdded( BilboBlog& ) ), this, SLOT( slotBlogAdded( BilboBlog& ) ) );
}

void BlogSettings::slotBlogAdded( BilboBlog &blog )
{
    int newRow = blogsTable->rowCount();
    blogsTable->insertRow( newRow );
    QTableWidgetItem *item1 = new QTableWidgetItem( blog.title() );
    item1->setData( 32, blog.id() );//blog_id
    blogsTable->setItem( newRow, 0, item1 );
    QTableWidgetItem *item2 = new QTableWidgetItem( blog.blogUrl() );
//     item2->setData( 32, ... );
    blogsTable->setItem( newRow, 1, item2 );
}

void BlogSettings::editBlog()
{
    kDebug();
    if( blogsTable->selectedItems().count() <= 0 )
        return;
    int blog_id = blogsTable->item( blogsTable->currentRow(), 0 )->data( 32 ).toInt();
    AddEditBlog *addEditBlogWindow = new AddEditBlog( blog_id, this );
    addEditBlogWindow->setAttribute( Qt::WA_DeleteOnClose );
    connect( addEditBlogWindow, SIGNAL( sigBlogEdited( BilboBlog& ) ), this, SLOT( slotBlogEdited( BilboBlog& ) ) );
    addEditBlogWindow->exec();
}

void BlogSettings::slotBlogEdited( BilboBlog &blog )
{
    blogsTable->item( blogsTable->currentRow(), 0 )->setText( blog.title() );
    blogsTable->item( blogsTable->currentRow(), 1 )->setText( blog.blogUrl() );
}

void BlogSettings::removeBlog()
{
    if( blogsTable->selectedItems().count() <= 0 )
        return;
    if(KMessageBox::warningYesNo(this, i18n("Are you sure of removing selected blog?")) 
        == KMessageBox::No)
        return;
    int blog_id = blogsTable->item( blogsTable->currentRow(), 0 )->data( 32 ).toInt();
    if ( DBMan::self()->removeBlog( blog_id ) ) {
        blogsTable->removeRow( blogsTable->currentRow() );
    } else {
        ///cannot remove
    }
}

void BlogSettings::loadBlogsList()
{
    QList<BilboBlog*> list = DBMan::self()->blogList().values();
    int count = list.count();
    for(int i=0; i<count; ++i) {
        slotBlogAdded( *list[i] );
    }
}

void BlogSettings::blogsTablestateChanged()
{
    if ( blogsTable->currentRow() >= 0 ) {
        btnEdit->setEnabled( true );
        btnRemove->setEnabled( true );
    } else {
        btnEdit->setEnabled( false );
        btnRemove->setEnabled( false );
    }
}

// #include "blogsettings.moc"