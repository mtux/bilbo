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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <kstandarddirs.h>

/**
Constants.
 @author Mehrdad Momeny <mehrdad.momeny@gmail.com>
 @author Golnaz Nilieh <g382nilieh@gmail.com>
 */

// #define VERSION "0.1"
#define APPNAME "Bilbo Blogger"
#define DATA_DIR KStandardDirs::locateLocal("data", "bilbo/")
// #define CONF_PATH QString(CONF_DIR)+QString("/bilbo.conf")
#define CONF_DB QString(DATA_DIR)+QString("/bilbo.db")
#define MEDIA_DIR KStandardDirs::locateLocal("data", "bilbo/media/", true)
#define UNKNOWN_BLOG_DIR KStandardDirs::locateLocal("data", "bilbo/Unknown Blog/", true)
#define CACHED_MEDIA_DIR KStandardDirs::locateLocal("cache", "bilbo/media/", true)
#define STATUSTIMEOUT 5000


struct Category {
    QString name;
    QString description;
    QString htmlUrl;
    QString rssUrl;
    QString categoryId;
    QString parentId;
    int id;
    int blog_id;
};

#endif
