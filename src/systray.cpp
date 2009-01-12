/***************************************************************************
 *   This file is part of the Bilbo Blogger.                               *
 *   Copyright (C) 2008-2009 Mehrdad Momeny <mehrdad.momeny@gmail.com>     *
 *   Copyright (C) 2008-2009 Golnaz Nilieh <g382nilieh@gmail.com>          *
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
#include "settings.h"
#include <kmenu.h>
#include <KDE/KLocale>


SysTray::SysTray(QWidget* parent): KSystemTrayIcon(parent)
{
	kDebug();
// 	bilbo = new MainWindow();
	this->setIcon(parent->windowIcon());
    this->setToolTip(i18n("Bilbo Blogger"));
// 	trayMenu = new QMenu;
// 	createActions();
	
// 	connect(this,  SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(sltActivated(QSystemTrayIcon::ActivationReason)) );
}

SysTray::~SysTray()
{
	kDebug();
}

// void SysTray::createActions()
// {
// // actNewPost = actionCollection()->action("new_post");
// }

// void SysTray::sltActivated(QSystemTrayIcon::ActivationReason reason)
// {
// // 	if(reason==2 || reason==3){
// // 		if(bilbo){
// // 			if(bilbo->isVisible())
// // 				bilbo->hide();
// // 			else
// // 				bilbo->show();
// // 		} else {
// // 			bilbo = new MainWindow;
// // 			bilbo->show();
// // 		}
// // 	}
// }

#include "systray.moc"
