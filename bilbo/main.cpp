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

#include "mainwindow.h"
#include <kuniqueapplication.h>
#include "global.h"
#include "constants.h"
#include "settings.h"

#include <kaboutdata.h>
#include <kcmdlineargs.h>
static const char description[] =
    I18N_NOOP( "A KDE Blogging Client" );

int main( int argc, char *argv[] )
{
    qDebug()<<APPNAME<<' '<<VERSION;
    KAboutData about( "bilbo", 0, ki18n( APPNAME ), VERSION, ki18n( description ),
                      KAboutData::License_GPL_V3, ki18n( "(C) 2008-2009 Bilbo Developers" ),
                      KLocalizedString(), "http://bilbo.gnufolks.org",
                      "https://bugs.launchpad.net/bilbo" );
    about.addAuthor( ki18n( "Mehrdad Momeny" ), ki18n( "Core Developer" ), "mehrdad.momeny@gmail.com" );
    about.addAuthor( ki18n( "Golnaz Nilieh" ), ki18n( "Core Developer" ), "g382nilieh@gmail.com" );
    about.addCredit( ki18n( "Roozbeh Shafiee" ), ki18n( "Icon designer" ), "roozbeh@roozbehonline.com");
    about.addCredit( ki18n( "Sajjad Baroodkoo" ), ki18n( "Icon designer" ), "sajjad@graphit.ir");

    about.setTranslator( ki18nc("NAME OF TRANSLATORS", "Your names"),
                         ki18nc("EMAIL OF TRANSLATORS", "Your emails"));
    KCmdLineArgs::init( argc, argv, &about );
//     KCmdLineOptions options;

    KUniqueApplication app;
    global_init();

    MainWindow *bilbo = new MainWindow;
//     bilbo->setAttribute(Qt::WA_DeleteOnClose, false);

    bilbo->show();
    int r = app.exec();

    global_end();
    return r;
}

