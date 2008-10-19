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
#include "bilboeditor.h"
#include "htmlexporter.h"

BilboEditor::BilboEditor()
{
	
	createUi();
}


BilboEditor::~BilboEditor()
{
}

void BilboEditor::createUi()
{
	///this:
	this->resize(600,400);
	tabVisual = new QWidget(this);
	tabHtml = new QWidget(this);
	tabPreview = new QWidget(this);
	this->addTab(tabVisual, "&Visual Editor");
	this->addTab(tabHtml, "&Html Editor");
	this->addTab(tabPreview, "Post &Preview");
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(syncEditors(int)));
	
	///editor:
	editor = new QTextEdit(0);
	barVisual = new QToolBar(0);
	barVisual->setIconSize(QSize(22, 22));
	barVisual->setToolButtonStyle(Qt::ToolButtonIconOnly);
	QVBoxLayout *vLayout = new QVBoxLayout();
// 	barVisual->show();
	vLayout->addWidget(barVisual);
	vLayout->addWidget(editor);
	tabVisual->setLayout(vLayout);
	
	///htmlEditor:
	htmlEditor = new QPlainTextEdit(0);
	QGridLayout *hLayout = new QGridLayout();
	hLayout->addWidget(htmlEditor);
	tabHtml->setLayout(hLayout);
// 	htmlEditor->set
	
	///preview:
	preview = new QWebView(0);
// 	barPreview = new QToolBar(0);
	QVBoxLayout *pLayout = new QVBoxLayout();
	pLayout->addWidget(preview);
	tabPreview->setLayout(pLayout);
	
	createActions();
	
	

// 	tabVisual->layout()->addWidget(editor);//TODO!
}

void BilboEditor::createActions()
{
	actBold = new QAction(QIcon(":/media/format-text-bold.png"), "Bold", this);
	actBold->setCheckable(true);
	connect(actBold, SIGNAL(triggered(bool)), this, SLOT(toggleBold()));
	barVisual->addAction(actBold);
	
	actItalic = new QAction(QIcon(":/media/format-text-italic.png"), "Italic", this);
	actItalic->setCheckable(true);
	connect(actItalic, SIGNAL(triggered(bool)), this, SLOT(toggleItalic()));
	barVisual->addAction(actItalic);
	
	actUnderline = new QAction(QIcon(":/media/format-text-underline.png"), "Underline", this);
	actUnderline->setCheckable(true);
	connect(actUnderline, SIGNAL(triggered(bool)), this, SLOT(toggleUnderline()));
	barVisual->addAction(actUnderline);
	
	actStrikeout = new QAction(QIcon(":/media/format-text-strikethrough.png"), "Strikeout", this);
	actStrikeout->setCheckable(true);
	connect(actStrikeout, SIGNAL(triggered(bool)), this, SLOT(toggleStrikeout()));
	barVisual->addAction(actStrikeout);
	
	actCode = new QAction(QIcon(":/media/format-text-code.png"), "Code", this);
	actCode->setCheckable(true);
	connect(actCode, SIGNAL(triggered(bool)), this, SLOT(toggleCode()));
	barVisual->addAction(actCode);
	
	actFontIncrease = new QAction(QIcon(":/media/format-font-size-more.png"), "Increase font size", this);
	connect(actFontIncrease, SIGNAL(triggered(bool)), this, SLOT(fontSizeIncrease()));
	barVisual->addAction(actFontIncrease);
	
	actFontDecrease = new QAction(QIcon(":/media/format-font-size-less.png"), "Decrease font size", this);
	connect(actFontDecrease, SIGNAL(triggered(bool)), this, SLOT(fontSizeDecrease()));
	barVisual->addAction(actFontDecrease);
	
	actAlignLeft = new QAction(QIcon(":/media/format-justify-left.png"), "Align left", this);
	connect(actAlignLeft, SIGNAL(triggered(bool)), this, SLOT(alignLeft()));
	barVisual->addAction(actAlignLeft);
	
	actAlignCenter = new QAction(QIcon(":/media/format-justify-center.png"), "Align center", this);
	connect(actAlignCenter, SIGNAL(triggered(bool)), this, SLOT(alignCenter()));
	barVisual->addAction(actAlignCenter);
	
	actAlignRight = new QAction(QIcon(":/media/format-justify-right.png"), "Align right", this);
	connect(actAlignRight, SIGNAL(triggered(bool)), this, SLOT(alignRight()));
	barVisual->addAction(actAlignRight);
	
	actJustify = new QAction(QIcon(":/media/format-justify-fill.png"), "Justify", this);
	connect(actJustify, SIGNAL(triggered(bool)), this, SLOT(alignJustify()));
	barVisual->addAction(actJustify);
	
	barVisual->addSeparator();
	
	actColorSelect = new QAction(QIcon(":/media/format-text-color.png"), "Select Color", this);
	connect(actColorSelect, SIGNAL(triggered(bool)), this, SLOT(selectColor()));
	barVisual->addAction(actColorSelect);
}

void BilboEditor::toggleItalic()
{
	editor->setFontItalic(!editor->fontItalic());
}

void BilboEditor::toggleBold()
{
	if ( editor->fontWeight() >= QFont::Bold )
		editor->setFontWeight( QFont::Normal );
	else
		editor->setFontWeight( QFont::Bold );
}

void BilboEditor::toggleUnderline()
{
	editor->setFontUnderline ( !editor->fontUnderline() );
}

void BilboEditor::toggleStrikeout()
{
	QFont f( editor->currentFont() );
	f.setStrikeOut(!f.strikeOut());
	editor->setCurrentFont(f);
}

void BilboEditor::toggleCode()
{
	//TODO
	static QString preFontFamily;
	if ( editor->fontFamily() != "Courier New,courier" ) {
		preFontFamily = editor->fontFamily();
		editor->setFontFamily("Courier New,courier");
	} else {
		editor->setFontFamily(preFontFamily);
	}
}

void BilboEditor::fontSizeIncrease()
{
	QTextCharFormat format = editor->currentCharFormat ();

	int idx = format.intProperty(QTextFormat::FontSizeAdjustment);
	if ( idx < 3 ) {
		format.setProperty ( QTextFormat::FontSizeAdjustment, QVariant( ++idx ));
		editor->setCurrentCharFormat (format);
	}
}

void BilboEditor::fontSizeDecrease()
{
	QTextCharFormat format = editor->currentCharFormat ();

	int idx = format.intProperty(QTextFormat::FontSizeAdjustment);
	if ( idx > -1 ) {
		format.setProperty ( QTextFormat::FontSizeAdjustment, QVariant( --idx ));
		editor->setCurrentCharFormat (format);
	}
}

void BilboEditor::addEditLink()
{
}

void BilboEditor::removeLink()
{
}

void BilboEditor::selectColor()
{
	QColor c = QColorDialog::getColor(QColor("black"), this);
	const QBrush br(c, Qt::SolidPattern);
	QTextCharFormat ch = editor->currentCharFormat();
	ch.setForeground(br);
	editor->setCurrentCharFormat(ch);
}

void BilboEditor::removeFormatting()
{
}

void BilboEditor::alignRight()
{
}

void BilboEditor::alignLeft()
{
}

void BilboEditor::alignCenter()
{
}

void BilboEditor::alignJustify()
{
}

QString BilboEditor::htmlToRichtext(const QString& html)
{
	QString richText = html;

	richText.remove(QChar('\n'));

	richText.replace(QRegExp("<del>(.*)</del>"), "<s>\\1</s>");

    //Note: Qt Error:
    //      QDocument converts <h1> in [ font-size: xx-large + bold ]
    //      and font-size: xx-large in <h1>
	richText.replace("<h1>", "<span style=\"font-size: xx-large\" >");
	richText.replace("<h2>", "<span style=\"font-size: x-large\" >");
	richText.replace("<h3>", "<span style=\"font-size: large\" >");
    //richText.replace("<h4>", "<span style=\"font-size: medium\" >");
	richText.replace(QRegExp("<h4>(.*)</h4>"), "\\1");
	richText.replace("<h5>", "<span style=\"font-size: small\" >");
	richText.replace(QRegExp("</h[1-5]>"), "</span>");

    //kDebug() << "out" << richText;
    //return richText;
	QString h;
// 	QString basePath = KBloggerMedia::cachePath(); <base href=\"" + basePath + "\" />
	h = "<html><head></head><body><p>" + richText + "</p></body></html>";
	return h;
}

void BilboEditor::syncEditors(int index)
{
// 	editor->document();
	htmlExporter* htmlExp = new htmlExporter();
	
	if(index == 0)
		editor->setHtml(htmlToRichtext(htmlEditor->toPlainText()));
	else if(index == 1)
		htmlEditor->setPlainText(htmlExp->toHtml(editor->document()));
	else if(prev_index == 1){
		editor->setHtml(htmlToRichtext(htmlEditor->toPlainText()));
		preview->setHtml(htmlEditor->toPlainText(), QUrl("#"));
	}
	else if(prev_index == 0){
		htmlEditor->setPlainText(htmlExp->toHtml(editor->document()));
		preview->setHtml(htmlEditor->toPlainText(), QUrl("#"));
	}
	
	prev_index = index;
	delete htmlExp;
}

