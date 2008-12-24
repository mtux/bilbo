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
#ifndef CATCHECKBOX_H
#define CATCHECKBOX_H

#include <QCheckBox>

/**
Extend QCheckBox to add property needed for Category checkboxes.

	@author 
*/
class Q_GUI_EXPORT CatCheckBox : public QCheckBox
{
	Q_OBJECT
public:
    CatCheckBox(QWidget* parent=0);
    
    CatCheckBox(const QString& text, QWidget* parent=0);

    ~CatCheckBox();
	
	int catId();
	void setCatId(int id);
	
	QString catTitle() const;
	void setCatTitle(const QString& title);
	
private:
	int mCatId;
	QString mCatTitle;

};

#endif