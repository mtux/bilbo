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

BilboEditor::BilboEditor()
{
	
	createUi();
}


BilboEditor::~BilboEditor()
{
}

void BilboEditor::createUi()
{
	this->resize(600,400);
	tabVisual = new QWidget(this);
	tabHtml = new QWidget(this);
	tabPreview = new QWidget(this);
	this->addTab(tabVisual, "&Visual Editor");
	this->addTab(tabHtml, "&Html Editor");
	this->addTab(tabPreview, "Post &Preview");
	
	editor = new QTextEdit(0);
	
	barVisual = new QToolBar(0);
	barVisual->setIconSize(QSize(22, 22));
	barVisual->setToolButtonStyle(Qt::ToolButtonIconOnly);
	QVBoxLayout *vLayout = new QVBoxLayout();
	barVisual->show();
	vLayout->addWidget(barVisual);
	vLayout->addWidget(editor);
	
	tabVisual->setLayout(vLayout);
	
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
}

void BilboEditor::toggleItalic()
{
}

void BilboEditor::toggleBold()
{
}

void BilboEditor::toggleUnderline()
{
}

void BilboEditor::toggleStrikeout()
{
}

void BilboEditor::toggleCode()
{
}

void BilboEditor::fontSizeIncrease()
{
}

void BilboEditor::fontSizeDecrease()
{
}

void BilboEditor::addEditLink()
{
}

void BilboEditor::removeLink()
{
}

void BilboEditor::selectColor()
{
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

