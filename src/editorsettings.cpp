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
#include <QDir>

#include "editorsettings.h"
#include "constants.h"
#include "multilinetextedit.h"

EditorSettings::EditorSettings( QWidget *parent )
    : QWidget( parent )
{
    setupUi( this );
    btnClearCache->setIcon( KIcon( "edit-clear" ) );
    connect( btnClearCache, SIGNAL( clicked( bool ) ), this, SLOT( sltClearCache() ) );
}

EditorSettings::~EditorSettings()
{
}

void EditorSettings::sltClearCache()
{
    QDir cacheDir( CACHED_MEDIA_DIR );
    QStringListIterator i( cacheDir.entryList() );
    while ( i.hasNext() ) {
        cacheDir.remove( i.next() );
    }
    MultiLineTextEdit::clearCache();
}

#include "editorsettings.moc"
