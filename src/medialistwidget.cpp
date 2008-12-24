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
#include "medialistwidget.h"
#include <QContextMenuEvent>
#include <kmenu.h>
#include <kaction.h>
#include <klocalizedstring.h>
#include <kdebug.h>

MediaListWidget::MediaListWidget(QWidget *parent): KListWidget(parent)
{
	actEditSize = new KAction(i18n("Edit size"),this);
	connect(actEditSize, SIGNAL(triggered( bool )), this, SLOT(sltEditImageSize()));
	actRemove = new KAction(i18n("Remove media"),this);
	connect(actRemove, SIGNAL(triggered( bool )), this, SLOT(sltRemoveMedia()));
}

MediaListWidget::~MediaListWidget()
{
}

void MediaListWidget::contextMenuEvent(QContextMenuEvent *event)
{
	if (this->itemAt(event->pos()))
	{
		KMenu menu(this);
		if (this->itemAt(event->pos())->type() == ImageType)
		{
			menu.addAction(actEditSize);
		}
		menu.addAction(actRemove);
		menu.exec(event->globalPos());
	}
}

void MediaListWidget::sltEditImageSize()
{
}

void MediaListWidget::sltRemoveMedia()
{
	kDebug() << this->currentRow();
	Q_EMIT(sigRemoveMedia(this->currentRow()));
}

#include "medialistwidget.moc"
