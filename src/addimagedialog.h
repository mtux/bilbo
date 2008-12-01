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
#ifndef ADDIMAGEDIALOG_H
#define ADDIMAGEDIALOG_H

#include <QDialog>
//#include <KDialog>

#include "ui_addimagedialogbase.h"

class BilboMedia;
/**
Implements a dialog to enter address of a local or remote image file.

	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
 */
// TODO change the class to support more than one type of media.
// TODO use KDialog instead of QDialog.
// now when using it default ok-cancel buttons appear on top right corner of the dialog window.

class AddImageDialog: public QDialog, public Ui::AddImageDialogBase
{
	Q_OBJECT
	public:
		/// AddImageDialog constructor.
		/**
		 * Creates a new AddImageDialog instance, and opens it.
		 * @param parent is needed for QDialog constructor, which is the parent class of AddImageDialog.
		 */
		AddImageDialog(QWidget *parent = 0);
		
		/// AddImageDialog destructor.
		~AddImageDialog();
		
		
		/**
		 * Returns the selected image path as an http url or a path in local filesystem.
		 * @return a QString which contains the image path
		 */
		QString selectedImageUrl();
		
	Q_SIGNALS:
		/**
		 * when the dialog is accepted, sends the selected image url to the caller function.
		 * @param url 
		 */
		void signalAddImage(BilboMedia *media);
		
	public Q_SLOTS:
		void accept();
		void reject();
		
	private:
		QString mSelectedImageUrl;
		
	private Q_SLOTS:
		void slotBrowseFiles();
		void slotDisableLocalPath();
};

#endif
