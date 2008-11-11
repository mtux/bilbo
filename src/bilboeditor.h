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

// #include <QTabWidget>
#include <QtGui>
// #include <QWebView>
#include "multilinetextedit.h"

class AddEditLink;
class QTabWidget;
class QWebView;
class QAction;
class QToolBar;
//!Class BilboEditor represents the editor part of BilboBlogger
/*!
	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
*/

class BilboEditor : public QTabWidget
{
	Q_OBJECT
	public:
		//!BilboEditor constructor
		/*!
		Creates a new instance of BilboEditor, and calls createUi() function to initialize its widgets.
		*/
    	BilboEditor();

		//!BilboEditor destructor
    	~BilboEditor();
	
		//!Returns the editor current text in html format
		/*!
		Synchronizes htmlEditor and editor tabs, by sending content of the current one to another. then copies the content of htmlEditor into the variable mHtmlContent, and returns it.
		\return a pointer to an String which contains html text
		*/
		QString *htmlContent();
	
	protected slots:
		/*! 
		Changes Italic style of current format.
		 */
		void sltToggleItalic();
		
		/*!
		Changes Bold style of current text.
		 */
		void sltToggleBold();
		
		/*!
		Changes Underline style of current text.
		*/
		void sltToggleUnderline();
		
		/*!
		Changes Strikeout style of current text.
		*/
		void sltToggleStrikeout();
		
		/*!
		Changes the current text font into Courier fontfamily, to represent code style.
		or if text font already be in code style, uses the fontfamily it had before.
		 */
		void sltToggleCode();
		
		/*!
		Increments font size by one degree.
		there are five predefined values for font size: "small", "medium", "large", "x-large", "xx-large". Bilboeditor uses "medium" as default font size.
		 */
		void sltFontSizeIncrease();
		
		/*!
		Decrements font size by one degree.
		there are five predefined values for font size: "small", "medium", "large", "x-large", "xx-large". Bilboeditor uses "medium" as default font size.
		 */
		void sltFontSizeDecrease();
		
		/*!
		 * Creates a new paragraph in cursor position by adding <p> tags.
		 */
		void sltNewParagraph();
		/*!
		Changes Alignment of current paragraph into Right Alignment.
		 */
		void sltAlignRight();
		
		/*!
		Changes Alignment of current paragraph into Left Alignment.
		 */
		void sltAlignLeft();
		
		/*!
		Changes Alignment of current paragraph into Center Alignment.
		 */
		void sltAlignCenter();
		void sltAlignJustify();
		
		/*!
		Switches Layout Direction of current paragraph between RightToLeft and LeftToRight Directions.
		 */
		void sltChangeLayoutDirection();
		
		/*!
		Opens a dialog to set link address. if cursor has selection and it is a link itself, the link address is shown in the dialog to edit.
		 */
		void sltAddEditLink();
		
		/*!
		Sets the link address given in the Caller link dialog as AnchorHref of the current text. if link title is set in the link dialog, current text will change into that.
		 */
		void sltSetLink(QString address, QString target, QString title);
		
		/*!
		Removes link from current text by assigning false to the Anchor property of text format.
		 */
		void sltRemoveLink();
		
		/*!
		Changes the forground color of current text.
		 */
		void sltSelectColor();
		
		/*!
		Changes all properties of current text format into default, except Anchor, AnchorName and AnchorHref properties.
		\sa defaultCharFormat
		 */
		void sltRemoveFormatting();
		void sltAddImage();
		
		/*!
		Sets the content of the current tab  as other tabs' contents, to apply recent changes. this function executes each time the user switches between tabs. 
		 */
		void sltSyncEditors(int index);
		
	private:
		/*!
		Creates actions of Bilbo editor, and assigns each one to a button, then adds each button to barVisual, on the editor tab.
		 */
		void createActions();
		
		/*!
		Creates Widget of BilboEditor.
		then assigns default charachter format of the editor tab to defaultCharFormat variable, to be used in remove formatting operation. then calls createActions function.
		\sa sltRemoveFormatting(), createActions()
		 */
		void createUi();
		
		/*!
		Prepares the html code to be used by editor->setHtml() function.
		 */
		QString htmlToRichtext(const QString& html);
		
		QWidget *tabVisual;
		QWidget *tabHtml;
		QWidget *tabPreview;
		
		QTextEdit *editor;
		//MultiLineTextEdit *editor;
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
		QAction *actNewParagraph;
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
		
		QString *mHtmlContent;
		
		AddEditLink *linkDialog;
		QTextCharFormat defaultCharFormat;
		int prev_index;
};

#endif
