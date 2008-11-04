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
#ifndef BILBOEDITOR_H
#define BILBOEDITOR_H

#include <QTabWidget>
#include <QtGui>
#include <QWebView>
#include <QTabWidget>

#include "addeditlink.h"
/**
	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
*/

class BilboEditor : public QTabWidget
{
	Q_OBJECT
public:
    BilboEditor();

    ~BilboEditor();
	protected slots:
		void toggleItalic();
		void toggleBold();
		void toggleUnderline();
		void toggleStrikeout();
		void toggleCode();
		void fontSizeIncrease();
		void fontSizeDecrease();
		void alignRight();
		void alignLeft();
		void alignCenter();
		void alignJustify();
		void changeLayoutDirection();
		void addEditLink();
		void setLink(QString address, QString target, QString title);
		void removeLink();
		void selectColor();
		void removeFormatting();
		void sltAddImage();
		
		void syncEditors(int index);
		
	private:
		void createActions();
		void createUi();
		QString htmlToRichtext(const QString& html);
		
		QWidget *tabVisual;
		QWidget *tabHtml;
		QWidget *tabPreview;
		
		QTextEdit *editor;
		QPlainTextEdit *htmlEditor;
		QWebView *preview;
		
		QToolBar *barVisual;
		QToolBar *barHtml;
		QToolBar *barPreview;
		
		QAction *actBold;
		QAction *actItalic;
		QAction *actUnderline;
		QAction *actStrikeout;
		QAction *actCode;
		QAction *actFontIncrease;
		QAction *actFontDecrease;
		QAction *actAlignRight;
		QAction *actAlignLeft;
		QAction *actAlignCenter;
		QAction *actJustify;
		QAction *actRightToLeft;
		QAction *actAddLink;
		QAction *actRemoveLink;
		QAction *actRemoveFormatting;
		QAction *actColorSelect;
		QAction *actAddImage;
		
		QTextCharFormat defaultCharFormat;
		
		AddEditLink *linkDialog;
		
		int prev_index;
};

#endif
