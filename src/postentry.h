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
#ifndef POSTENTRIY_H
#define POSTENTRIY_H

#include <QFrame>

//#include "ui_post.h"
#include "bilboeditor.h"
// #include "postproperties.h"

/**
	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
 */
//class PostEntry: public QFrame, public Ui::PostWidget {
class PostEntry: public QFrame {
Q_OBJECT
public:
    PostEntry(QWidget *parent);
// 	PostProperties *prop;
		
protected slots:
	void sltPublishPost();
	void sltSavePostLocally();
	void sltSaveAsDraft();
	void sltDelPost();
	void sltDelLocally();
	
private:
	BilboEditor *editPostWidget;
	QGridLayout *gridLayout;
	QHBoxLayout *horizontalLayout;
	QLabel *titleLabel;
	QLineEdit *txtTitle;
	QWidget *wPost;
	
	void setupUi(QWidget *parentWidget);
};

#endif
