/***************************************************************************
 *   Copyright (C) 2008 by Mehrdad Momeny, Golnaz Nilieh   *
 *   mehrdad.momeny@gmail.com, g382nilieh@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
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

#include "systray.h"
#include <QApplication>
// #include "mainwindow.h"
#include "global.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	qDebug("this will be Bilbo Blogger app...");
	
	global_init();
	
// 	MainWindow *window=new MainWindow(0);
// 	window->show();
	  SysTray *s= new SysTray(0);
	  s->show();
//	  BilboEditor *f= new BilboEditor();
//	  f->show();
	
	global_end();
	return app.exec();
}

