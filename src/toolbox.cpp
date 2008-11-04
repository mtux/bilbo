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
#include "toolbox.h"

Toolbox::Toolbox(QWidget *parent)
    :QDockWidget(parent)
{
	setupUi(this);
}

void Toolbox::on_btnBlogAdd_clicked()
{
	addEditBlogWindow = new AddEditBlog(-1, this);
	addEditBlogWindow->show();
}

void Toolbox::on_btnBlogEdit_clicked()
{
	addEditBlogWindow = new AddEditBlog(-1, this);///TODO blog_id must set correctly!
	addEditBlogWindow->show();
}
