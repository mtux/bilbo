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
 
//#include <klocalizedstring.h>
//#include <kdialogbuttonbox.h>
#include "addeditlink.h"

AddEditLink::AddEditLink(QWidget *parent)
    :QDialog(parent)
{
	setupUi(this);
	this->kButtonbox->addButton(KGuiItem(i18n("Ok"), KIcon("dialog-ok-apply")), QDialogButtonBox::AcceptRole, this, SLOT(accept()));
	this->kButtonbox->addButton(KGuiItem(i18n("Cancel"), KIcon("dialog-cancel")), QDialogButtonBox::RejectRole, this, SLOT(reject()));
	this->label_2->hide();
	this->txtTitle->hide();
	this->comboTarget->hide();
	this->label_3->hide();
	this->resize(this->width(), this->height()/2);
}

void AddEditLink::accept()
{
	if ( txtAddress->text().isEmpty()) return;
	Q_EMIT addLink( txtAddress->text(), (comboTarget->currentIndex()==1 || comboTarget->currentIndex()==1)?"_self":"_blank", txtTitle->text() );
	close();
	QDialog::accept();
// 	hide();
}

void AddEditLink::reject()
{
	close();
	QDialog::reject();
// 	hide();
}

void AddEditLink::show(QString address, QString title)
{
	QDialog::show();
	if(!address.isEmpty()){
		txtAddress->setText(address);
		this->setWindowTitle(i18nc("verb, to edit link", "Edit Link"));
	}
	if(!title.isEmpty()){
		txtTitle->setText(title);
	}
}

#include "addeditlink.moc"
