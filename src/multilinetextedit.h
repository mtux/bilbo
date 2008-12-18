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
#ifndef MULTILINETEXTEDIT_H
#define MULTILINETEXTEDIT_H

//#include <QWidget>
//#include <QtGui>
//#include <QTextEdit>
#include "krichtextedit.h"

//!Class MultiLineTextEdit Implements a TextEdit widget that supports new line charachters
/*!
	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
 */

class MultiLineTextEdit : public KRichTextEdit
{
	Q_OBJECT
	public:
		//!Implements the class constructor.
		/*!
		 * \param *parent is needed for KRichTextEdit constructor, which MultiLineTextEdit 
		 * inherits from.
		 */
		MultiLineTextEdit(QWidget *parent = 0);
		
		//!Implements the class destructor.
		~MultiLineTextEdit();
		
// 	public Q_SLOTS:
// 		void alignRight();
// 		void alignLeft();
		
	protected:
		
		/*!
		 * When MultiLineTextEdit is focused on, checks each presssed key; then replaces 
		 * "Return" characters with QChar::LineSeparator special character.
		 * this function is defined virtual in parent class: KRichTextEdit.
		 */
		void keyPressEvent(QKeyEvent *event);
};

#endif
