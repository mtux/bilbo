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

#include "addeditlink.h"

AddEditLink::AddEditLink( QWidget *parent )
        : KDialog( parent )
{
    QWidget *dialog = new QWidget( this );
    ui.setupUi( dialog );
    this->setMainWidget( dialog );

    this->resize( dialog->width(), dialog->height() );
    
    connect( this, SIGNAL( accepted() ), this, SLOT( sltAccepted() ) );
    ui.txtAddress->setFocus();
}

void AddEditLink::sltAccepted()
{
    if ( ui.txtAddress->text().isEmpty() ) return;
    QString linkTarget;
    if ( ui.comboTarget->currentIndex() == 1 ) {
        linkTarget = "_self";
    } else if ( ui.comboTarget->currentIndex() == 2 ) {
        linkTarget = "_blank";
    }
    const QString target = linkTarget;

    Q_EMIT addLink( ui.txtAddress->text(), target, ui.txtTitle->text() );
}

void AddEditLink::show( const QString& address, const QString& title, const QString& target )
{
    KDialog::show();
    if ( !address.isEmpty() ) {
        ui.txtAddress->setText( address );
        this->setWindowTitle( i18nc( "verb, to modify an existing link", "Edit Link" ) );
    } else {
        this->setWindowTitle( i18nc( "verb, to insert a link into the text", "Add Link" ) );
    }
    if ( !title.isEmpty() ) {
        ui.txtTitle->setText( title );
    }
    if ( !target.isEmpty() ) {
        if ( target == "_self" ) {
            ui.comboTarget->setCurrentIndex( 1 );
        } else if ( target == "_blank" ) {
            ui.comboTarget->setCurrentIndex( 2 );
        }
    }
}

#include "addeditlink.moc"
