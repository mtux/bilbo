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

//#include <QDebug>
#include <QtGui>
#include "multilinetextedit.h"

MultiLineTextEdit::MultiLineTextEdit(QWidget *parent) : KRichTextEdit(parent)
{
}

MultiLineTextEdit::~MultiLineTextEdit()
{
}

// void MultiLineTextEdit::alignRight()
// {
// 	if (this->textCursor().blockFormat().layoutDirection() == Qt::RightToLeft) {
// 		KRichTextEdit::alignLeft();
// 	} else {
// 		KRichTextEdit::alignRight();
// 	}
// }
// 
// void MultiLineTextEdit::alignLeft()
// {
// 	if (this->textCursor().blockFormat().layoutDirection() == Qt::RightToLeft) {
// 		KRichTextEdit::alignRight();
// 	} else {
// 		KRichTextEdit::alignLeft();
// 	}
// }

void MultiLineTextEdit::keyPressEvent(QKeyEvent *event)
{
	//qDebug("MultiLineTextEdit::keyPressEvent");
	int tempKey = event->key();
	if (tempKey == Qt::Key_Return && event->modifiers() != Qt::ShiftModifier)
	{
		this->textCursor().insertText(QString(QChar::LineSeparator));
		//qDebug() << "Enter Pressed" ;

	} else {
		//dynamic_cast <QTextEdit*>(this) ->keyPressEvent(event);
		//dynamic_cast <QWidget*>(this) ->keyPressEvent(event);
		KRichTextEdit::keyPressEvent(event);

	}
}

#include <multilinetextedit.moc>
