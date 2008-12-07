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
	:KDialog(parent)
{
	//QWidget *dialog = new QWidget;
	QDialog *dialog = new QDialog(0);
	ui.setupUi(dialog);
	dialog->setAttribute( Qt::WA_DeleteOnClose );
	this->setMainWidget(dialog);

	ui.label_2->hide();
	ui.txtTitle->hide();
	ui.comboTarget->hide();
	ui.label_3->hide();
	this->resize(dialog->width(), dialog->height()/2);
	connect(this, SIGNAL(accepted()), this, SLOT(sltAccepted()));
}

void AddEditLink::sltAccepted()
{
	if ( ui.txtAddress->text().isEmpty()) return;
// 	Q_EMIT addLink( ui.txtAddress->text(), (ui.comboTarget->currentIndex()==1 || ui.comboTarget->currentIndex()==1)?"_self":"_blank", ui.txtTitle->text() );
	Q_EMIT addLink( ui.txtAddress->text() );
// 	close();
// 	QDialog::accept();
// // 	hide();
}

void AddEditLink::show(QString address, QString title)
{
	KDialog::show();
	if(!address.isEmpty()){
		ui.txtAddress->setText(address);
		this->setWindowTitle(i18nc("verb, to edit link", "Edit Link"));
	}
	if(!title.isEmpty()){
		ui.txtTitle->setText(title);
	}
}

#include "addeditlink.moc"
