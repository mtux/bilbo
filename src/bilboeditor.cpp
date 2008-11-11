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

#include <QDebug>

#include "bilboeditor.h"
#include "htmlexporter.h"
#include "addeditlink.h"

// #include <QTabWidget>
BilboEditor::BilboEditor()
{
	createUi();
	editor->setFocus();
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
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(sltSyncEditors(int)));
	prev_index=0;
	
	///editor:
	//editor = new QTextEdit(0);
	editor = new MultiLineTextEdit(0);
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
	
	///defaultCharFormat
	defaultCharFormat = editor->currentCharFormat();
// 	defaultCharFormat.setFontFamily(editor->fontFamily());
// 	defaultCharFormat.setFontWeight(editor->fontWeight());
// 	defaultCharFormat.setFontUnderline(editor->fontUnderline());
// 	defaultCharFormat.setFontItalic(editor->fontItalic());
// 	defaultCharFormat.setFontStrikeOut(false);
// 	//const QBrush br(QPalette::WindowText);
// 	defaultCharFormat.setForeground(editor->document()->de);

	//defaultCharFormat.setFont(editor->currentCharFormat().font());
// 	defaultCharFormat.clearProperty(QTextFormat::FontSizeAdjustment);
	
	const QFont defaultFont = editor->document()->defaultFont();
	defaultCharFormat.setFont(defaultFont);
	defaultCharFormat.setForeground(editor->currentCharFormat().foreground());
	defaultCharFormat.setProperty(QTextFormat::FontSizeAdjustment,QVariant(0));
	
	createActions();
	
	

// 	tabVisual->layout()->addWidget(editor);//TODO!
}

void BilboEditor::createActions()
{
	actBold = new QAction(QIcon(":/media/format-text-bold.png"), "Bold", this);
	actBold->setCheckable(true);
	connect(actBold, SIGNAL(triggered(bool)), this, SLOT(sltToggleBold()));
	barVisual->addAction(actBold);
	
	actItalic = new QAction(QIcon(":/media/format-text-italic.png"), "Italic", this);
	actItalic->setCheckable(true);
	connect(actItalic, SIGNAL(triggered(bool)), this, SLOT(sltToggleItalic()));
	barVisual->addAction(actItalic);
	
	actUnderline = new QAction(QIcon(":/media/format-text-underline.png"), "Underline", this);
	actUnderline->setCheckable(true);
	connect(actUnderline, SIGNAL(triggered(bool)), this, SLOT(sltToggleUnderline()));
	barVisual->addAction(actUnderline);
	
	actStrikeout = new QAction(QIcon(":/media/format-text-strikethrough.png"), "Strikeout", this);
	actStrikeout->setCheckable(true);
	connect(actStrikeout, SIGNAL(triggered(bool)), this, SLOT(sltToggleStrikeout()));
	barVisual->addAction(actStrikeout);
	
	actCode = new QAction(QIcon(":/media/format-text-code.png"), "Code", this);
	actCode->setCheckable(true);
	connect(actCode, SIGNAL(triggered(bool)), this, SLOT(sltToggleCode()));
	barVisual->addAction(actCode);
	
	actFontIncrease = new QAction(QIcon(":/media/format-font-size-more.png"), "Increase font size", this);
	connect(actFontIncrease, SIGNAL(triggered(bool)), this, SLOT(sltFontSizeIncrease()));
	barVisual->addAction(actFontIncrease);
	
	actFontDecrease = new QAction(QIcon(":/media/format-font-size-less.png"), "Decrease font size", this);
	connect(actFontDecrease, SIGNAL(triggered(bool)), this, SLOT(sltFontSizeDecrease()));
	barVisual->addAction(actFontDecrease);
	
	actRemoveFormatting= new QAction(QIcon(":/media/draw-eraser.png"), "Remove formatting", this);
	connect(actRemoveFormatting, SIGNAL(triggered(bool)), this, SLOT(sltRemoveFormatting()));
	barVisual->addAction(actRemoveFormatting);
	
	barVisual->addSeparator();
	
	actNewParagraph = new QAction(QIcon(":/media/new-paragraph.png"),"New Paragraph",this);
	connect(actNewParagraph, SIGNAL(triggered(bool)), this, SLOT(sltNewParagraph()));
	barVisual->addAction(actNewParagraph);
	
	actAlignLeft = new QAction(QIcon(":/media/format-justify-left.png"), "Align left", this);
	connect(actAlignLeft, SIGNAL(triggered(bool)), this, SLOT(sltAlignLeft()));
	barVisual->addAction(actAlignLeft);
	
	actAlignCenter = new QAction(QIcon(":/media/format-justify-center.png"), "Align center", this);
	connect(actAlignCenter, SIGNAL(triggered(bool)), this, SLOT(sltAlignCenter()));
	barVisual->addAction(actAlignCenter);
	
	actAlignRight = new QAction(QIcon(":/media/format-justify-right.png"), "Align right", this);
	connect(actAlignRight, SIGNAL(triggered(bool)), this, SLOT(sltAlignRight()));
	barVisual->addAction(actAlignRight);
	
	actJustify = new QAction(QIcon(":/media/format-justify-fill.png"), "Justify", this);
	connect(actJustify, SIGNAL(triggered(bool)), this, SLOT(sltAlignJustify()));
	barVisual->addAction(actJustify);
	
	actRightToLeft = new QAction(QIcon(":/media/format-text-direction-rtl.png"), "Right to Left", this);
	actRightToLeft->setCheckable(true);
	connect(actRightToLeft, SIGNAL(triggered(bool)), this, SLOT(sltChangeLayoutDirection()));
	barVisual->addAction(actRightToLeft);
	
	actAddLink = new QAction(QIcon(":/media/insert-link.png"), "Add/Edit Link", this);
	connect(actAddLink, SIGNAL(triggered(bool)), this, SLOT(sltAddEditLink()));
	barVisual->addAction(actAddLink);
	
	actRemoveLink = new QAction(QIcon(":/media/remove-link.png"), "Remove Link", this);
	connect(actRemoveLink, SIGNAL(triggered(bool)), this, SLOT(sltRemoveLink()));
	barVisual->addAction(actRemoveLink);
	
	barVisual->addSeparator();
	
	actColorSelect = new QAction(QIcon(":/media/format-text-color.png"), "Select Color", this);
	connect(actColorSelect, SIGNAL(triggered(bool)), this, SLOT(sltSelectColor()));
	barVisual->addAction(actColorSelect);
	
	actAddImage = new QAction(QIcon(":/media/insert-image.png"), "Add Image", this);
	connect(actAddImage, SIGNAL(triggered(bool)), this, SLOT(sltAddImage()));
	barVisual->addAction(actAddImage);
}

void BilboEditor::sltToggleItalic()
{
	editor->setFontItalic(!editor->fontItalic());
}

void BilboEditor::sltToggleBold()
{
	if ( editor->fontWeight() >= QFont::Bold )
		editor->setFontWeight( QFont::Normal );
	else
		editor->setFontWeight( QFont::Bold );
}

void BilboEditor::sltToggleUnderline()
{
	editor->setFontUnderline ( !editor->fontUnderline() );
}

void BilboEditor::sltToggleStrikeout()
{
	QFont f( editor->currentFont() );
	f.setStrikeOut(!f.strikeOut());
	editor->setCurrentFont(f);
}

void BilboEditor::sltToggleCode()
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

void BilboEditor::sltFontSizeIncrease()
{
// 	QTextCharFormat format = editor->currentCharFormat();
// 	
// 	int idx = format.intProperty(QTextFormat::FontSizeAdjustment);
// 	if ( idx < 3 ) {
// 		format2.setProperty(QTextFormat::FontSizeAdjustment, QVariant( ++idx ));
// 		//editor->setCurrentCharFormat(format);
// 		editor->textCursor().mergeCharFormat(format2);
// 	}
	QTextCharFormat format;
	int idx = editor->currentCharFormat().intProperty(QTextFormat::FontSizeAdjustment);
	if ( idx < 3 ) {
		format.setProperty(QTextFormat::FontSizeAdjustment, QVariant( ++idx ));
		editor->textCursor().mergeCharFormat(format);
	}
}

void BilboEditor::sltFontSizeDecrease()
{
// 	QTextCharFormat format = editor->currentCharFormat();
// 
// 	int idx = format.intProperty(QTextFormat::FontSizeAdjustment);
// 	if ( idx > -1 ) {
// 		format.setProperty(QTextFormat::FontSizeAdjustment, QVariant( --idx ));
// 		//editor->setCurrentCharFormat(format);
// 		editor->textCursor().mergeCharFormat(format);
// 	}
	QTextCharFormat format;
	int idx = editor->currentCharFormat().intProperty(QTextFormat::FontSizeAdjustment);
	if ( idx > -1 ) {
		format.setProperty(QTextFormat::FontSizeAdjustment, QVariant( --idx ));
		editor->textCursor().mergeCharFormat(format);
	}
}

void BilboEditor::sltAddEditLink()
{
	linkDialog = new AddEditLink(this);
	connect(linkDialog, SIGNAL(addLink(const QString&, const QString, const QString&)), this, SLOT(sltSetLink(QString, QString, QString)));
	QTextCharFormat f = editor->currentCharFormat();
	if (!f.isAnchor()) {
		linkDialog->show();
	}
	else {
		linkDialog->show(f.anchorHref(), f.anchorName());
	}
}

void BilboEditor::sltSetLink(QString address, QString target, QString title)
{
	QTextCharFormat f = editor->currentCharFormat();
	f.setAnchor(true);
	f.setAnchorHref(address);
	f.setAnchorName(title);
	//f.setUnderlineStyle(QTextCharFormat::SingleUnderline);
	//const QBrush br(Qt::blue);
	//f.setForeground(br);
	//editor->setCurrentCharFormat(f);
	editor->textCursor().mergeCharFormat(f);
}

void BilboEditor::sltRemoveLink()
{
	QTextCharFormat f = editor->textCursor().charFormat();
	f.setAnchor(false);
	//f.setUnderlineStyle(QTextCharFormat::NoUnderline);
	//f.clearForeground();
	//editor->textCursor().setCharFormat(f);
	editor->textCursor().mergeCharFormat(f);
}

void BilboEditor::sltSelectColor()
{
	QColor c = QColorDialog::getColor(QColor("black"), this);
	const QBrush br(c, Qt::SolidPattern);
	//QTextCharFormat ch = editor->currentCharFormat();
	QTextCharFormat ch;
	ch.setForeground(br);
	//editor->setCurrentCharFormat(ch);
	editor->textCursor().mergeCharFormat(ch);
}

void BilboEditor::sltRemoveFormatting()
{
// 	if(editor->textCursor().hasSelection())
// 	{
// 	QTextDocumentFragment fragment=editor->textCursor().selection();
// 	QTextDocument d=
// 	}
// 	else
// 	{
// 	}
 	
	editor->textCursor().mergeCharFormat(defaultCharFormat);
	
// 	QTextCharFormat f = editor->currentCharFormat();
// 	
// 	QMap<int, QVariant>::const_iterator i;
// 		
// 	for(i = f.properties().constBegin(); i != f.properties().constEnd(); ++i)
// 	{
// 		if((i.key() != f.intProperty(QTextCharFormat::AnchorHref)) && (i.key() != f.intProperty(QTextCharFormat::AnchorName)) && (i.key() != f.intProperty(QTextCharFormat::IsAnchor)))
// 			f.setProperty(i.key(), defaultCharFormat.properties().value(i.key()));
// 	}
// 
// 	editor->setCurrentCharFormat(f);
}

void BilboEditor::sltNewParagraph()
{
	editor->textCursor().insertBlock(editor->textCursor().blockFormat(), editor->textCursor().charFormat());
}
void BilboEditor::sltAlignRight()
{
	editor->setAlignment(Qt::AlignRight);
}

void BilboEditor::sltAlignLeft()
{
	editor->setAlignment(Qt::AlignLeft);
}

void BilboEditor::sltAlignCenter()
{
	editor->setAlignment(Qt::AlignHCenter);
}

void BilboEditor::sltAlignJustify()
{
	if (editor->alignment() != Qt::AlignJustify) {
		editor->setAlignment(Qt::AlignJustify);
	}
// 	else
// 		editor->setAlignment();
}

void BilboEditor::sltChangeLayoutDirection()
{
	qDebug("BilboEditor::changeLayoutDirection");
	QTextCursor c = editor->textCursor();
	QTextBlockFormat f = c.blockFormat();
	
	if (f.layoutDirection() != Qt::RightToLeft) {
		f.setLayoutDirection(Qt::RightToLeft);
	} else {
		f.setLayoutDirection(Qt::LeftToRight);
	}
	c.setBlockFormat(f);
	editor->setTextCursor(c);
}


void BilboEditor::sltAddImage()
{
	//Not permanent!
	//Just to test postContentHtml function! :D
	qDebug() << htmlContent() ;
}
// void BilboEditor::insertMedia(KBloggerMedia* media)
// {
// 	kDebug();
// 	if (!media) return;
// 	QString name = media->name();
// 	QString target;
// 	kDebug() << "media->url(): " << media->url();
// 	if ( media->url().isValid() ) {
//         //it's an uploaded file
// 		target = media->url().url();
// 	} else {
//         //it's a local file
// 		target = media->cachedFilename();
// 	}
// 
// 	if ( !media->mimetype().contains("image") ) {
// 		addLink(target, name);
// 	} else {
// 		QTextCursor cursor = visualTextEditor->textCursor();
// 		cursor.insertImage(target);
// 	}
// }

void BilboEditor::sltSyncEditors(int index)
{
// 	editor->document();
	htmlExporter* htmlExp = new htmlExporter();
	
	if(index == 0) {
		///Qt QTextEdit::setHtml() or QTextDocument::toHtml() convert <h1-5> tags to <span> tags
		
		editor->setHtml(htmlToRichtext(htmlEditor->toPlainText()));
		//qDebug()<<htmlEditor->toPlainText()<<endl;
		//qDebug()<<editor->document()->toHtml()<<endl;
	} else if (index == 1) {
		qDebug()<<editor->toHtml()<<endl;
		htmlEditor->setPlainText(htmlExp->toHtml(editor->document()));
	} else if (prev_index == 1) {
		editor->setHtml(htmlToRichtext(htmlEditor->toPlainText()));
		preview->setHtml(htmlEditor->toPlainText(), QUrl("#"));
	} else if (prev_index == 0) {
		htmlEditor->setPlainText(htmlExp->toHtml(editor->document()));
		preview->setHtml(htmlEditor->toPlainText(), QUrl("#"));
	}
	
	prev_index = index;
	delete htmlExp;
}

QString BilboEditor::htmlToRichtext(const QString& html)
{
	QString richText = html;

	richText.remove(QChar('\n'));

	richText.replace(QRegExp("<del>(.*)</del>"), "<s>\\1</s>");

///Comment this part to have both <span> and <h1-5> tags
	 
    //Note: Qt Error:
    //      QDocument converts <h1> in [ font-size: xx-large + bold ]
    //      and font-size: xx-large in <h1>
// 	richText.replace("<h1>", "<span style=\"font-size: xx-large\" >");
// 	richText.replace("<h2>", "<span style=\"font-size: x-large\" >");
// 	richText.replace("<h3>", "<span style=\"font-size: large\" >");
//     //richText.replace("<h4>", "<span style=\"font-size: medium\" >");
// 	richText.replace(QRegExp("<h4>(.*)</h4>"), "\\1");
// 	richText.replace("<h5>", "<span style=\"font-size: small\" >");
// 	richText.replace(QRegExp("</h[1-5]>"), "</span>");

    //kDebug() << "out" << richText;
    //return richText;
	QString h;
// 	QString basePath = KBloggerMedia::cachePath(); <base href=\"" + basePath + "\" />
// 	h = "<html><head></head><body><p>" + richText + "</p></body></html>";
	h = "<html><head></head><body><p>" + richText + "</p></body></html>";
	return h;
}

QString* BilboEditor::htmlContent()
{
	htmlExporter* htmlExp = new htmlExporter();
	
	if (this->currentIndex() == 0) {
		htmlEditor->setPlainText(htmlExp->toHtml(editor->document()));
	} else if (this->currentIndex() == 1) {
			editor->setHtml(htmlToRichtext(htmlEditor->toPlainText()));
	}
	
	delete htmlExp;
	
	mHtmlContent = new QString(htmlEditor->toPlainText());
	return mHtmlContent;
}

