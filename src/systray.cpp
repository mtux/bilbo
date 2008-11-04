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
#include "systray.h"
#include "global.h"

SysTray::SysTray(QObject* parent): QSystemTrayIcon(parent)
{
	bilbo = new MainWindow(0);
	this->setIcon(QIcon("/usr/lib/kde4/share/icons/oxygen/128x128/apps/kde.png"));
	trayMenu = new QMenu;
	createActions();
	
	if(conf->showMainOnStart)
		bilbo->show();
	
	connect(this,  SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(sltActivated(QSystemTrayIcon::ActivationReason)) );
}


SysTray::~SysTray()
{
}

void SysTray::createActions()
{
	this->actNewPost = bilbo->actNewPost;
	
	this->actAbout = bilbo->actAbout;
	
	this->actQuit = bilbo->actQuit;
	
	trayMenu->addAction(this->actNewPost);
	trayMenu->addSeparator();
	trayMenu->addAction(this->actAbout);
	trayMenu->addSeparator();
	trayMenu->addAction(this->actQuit);
	
	this->setContextMenu(trayMenu);
}

void SysTray::sltHideShow()
{
}

void SysTray::sltActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason==2 || reason==3){
		if(bilbo){
			if(bilbo->isVisible())
				bilbo->hide();
			else
				bilbo->show();
		} else {
			bilbo = new MainWindow;
			bilbo->show();
		}
	}
}
