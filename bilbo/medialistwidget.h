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

#ifndef MEDIALISTWIDGET_H
#define MEDIALISTWIDGET_H

#include <klistwidget.h>
#include <ui_editimagebase.h>
// #include <ui_editimagewidget.h>

/**
 @author
*/
class KAction;
class MediaListWidget : public KListWidget
{
    Q_OBJECT
public:
    MediaListWidget( QWidget *parent );

    ~MediaListWidget();

    void contextMenuEvent( QContextMenuEvent *event );

    enum ItemType {
        ImageType = 1001,
        OtherType = 1002
    };

Q_SIGNALS:
    void sigRemoveMedia( const int index );
    void sigSetProperties( const int index, const int width, const int height,
                           const QString title, const QString link, const QString Alt_text );

protected Q_SLOTS:
    void sltEditProperties();
    void sltSetProperties();
    void sltCopyUrl();
    void sltRemoveMedia();

private:
    KAction *actEdit;
    KAction *actCopyUrl;
    KAction *actRemove;

    Ui::EditImageBase ui;
//     Ui::EditImageWidget ui;

};

#endif