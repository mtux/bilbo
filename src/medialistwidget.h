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
#ifndef MEDIALISTWIDGET_H
#define MEDIALISTWIDGET_H

#include <klistwidget.h>
#include <ui_editimagebase.h>

/**
	@author 
*/
class KAction;
class MediaListWidget : public KListWidget
{
	Q_OBJECT
	public:
		MediaListWidget(QWidget *parent);
	
		~MediaListWidget();
		
		void contextMenuEvent(QContextMenuEvent *event);
		
		enum ItemType {
			ImageType = 1001,
			OtherType = 1002
		};
		
	Q_SIGNALS:
		void sigRemoveMedia(const int index);
		void sigSetProperties(const int index, const double width, const double height, 
							  const QString title, const QString Alt_text);
		
	protected Q_SLOTS:
		void sltEditProperties();
		void sltSetProperties();
		void sltRemoveMedia();
		
	private:
		KAction *actEdit;
		KAction *actRemove;
		
		Ui::EditImageBase ui;

};

#endif
