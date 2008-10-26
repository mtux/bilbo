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
#include "settings.h"
#include <QDebug>
#include <QSettings>

Settings::Settings()
{
	load();
}


Settings::~Settings()
{
	save();
}

void Settings::save()
{
	qDebug("Settings::save");
	QSettings *set = new QSettings(CONF_PATH, QSettings::NativeFormat);
// 	qDebug(QString(CONF_PATH).toLatin1().data());
	
	set->setValue("show_main_on_start", showMainOnStart);
}

void Settings::load()
{
	qDebug("Settings::load");
	
	QSettings *set = new QSettings;
	
	showMainOnStart = set->value("show_main_on_start", true).toBool();
}


