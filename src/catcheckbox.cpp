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
#include "catcheckbox.h"

CatCheckBox::CatCheckBox(QWidget* parent): QCheckBox(parent)
{
}


CatCheckBox::CatCheckBox(const QString& text, QWidget* parent): QCheckBox(text, parent)
{
}


CatCheckBox::~CatCheckBox()
{
}

// int CatCheckBox::catId()
// {
// 	return mCatId;
// }
// 
// void CatCheckBox::setCatId(int id)
// {
// 	mCatId = id;
// }
// 
// void CatCheckBox::setCatTitle(const QString & title)
// {
// 	mCatTitle = title;
// }
// 
// QString CatCheckBox::catTitle() const
// {
// 	return mCatTitle;
// }

Category CatCheckBox::category() const
{
	return mCat;
}

void CatCheckBox::setCategory(const Category & category)
{
	mCat = category;
	this->setToolTip(mCat.description);
}


