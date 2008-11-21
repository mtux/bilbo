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
#ifndef ADDEDITLINK_H
#define ADDEDITLINK_H

#include <QDialog>

#include "ui_addeditlink.h"

/**
Implements a dialog to get user input for link parameters; address, name and target.

	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
 */

class AddEditLink: public QDialog, public Ui::AddEditLinkDialog {
	Q_OBJECT
	public:
		AddEditLink(QWidget *parent = 0);
		void show(QString address="", QString title="");
		
	Q_SIGNALS:
		void addLink( const QString& address, const QString& target, const QString& title);
	public Q_SLOTS:
		virtual void accept(); // OK Button
		virtual void reject(); // Cancel Button
};

#endif
