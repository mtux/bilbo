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
#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <ksystemtrayicon.h>

class QMenu;
/**
System tray icon of app

	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
*/
class SysTray : public KSystemTrayIcon
{
	Q_OBJECT
public:
    SysTray(QWidget* parent=0);

    ~SysTray();
	
protected Q_SLOTS:
// 	void sltHideShow();
// 	void sltActivated(QSystemTrayIcon::ActivationReason);

private:
// 	void createActions();
	
// 	QMenu *trayMenu;
	
// 	QAction *actQuit;
	QAction *actNewPost;
// 	QAction *actHideShow;
// 	QAction *actAbout;
	
// 	MainWindow *bilbo;

};

#endif
