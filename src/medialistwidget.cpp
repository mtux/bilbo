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
#include <kdialog.h>
#include <kdebug.h>

MediaListWidget::MediaListWidget(QWidget *parent): KListWidget(parent)
{
	actEdit = new KAction(i18n("Edit properties"),this);
	connect(actEdit, SIGNAL(triggered( bool )), this, SLOT(sltEditProperties()));
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
			menu.addAction(actEdit);
		}
		menu.addAction(actRemove);
		menu.exec(event->globalPos());
	}
}

void MediaListWidget::sltEditProperties()
{
	QDialog *temp = new QDialog(this);
	KDialog *dialog = new KDialog(this);
	//ui.setupUi(dialog);
	ui.setupUi(temp);
	dialog->setMainWidget(temp);
	dialog->setWindowTitle(temp->windowTitle());
	dialog->resize(temp->width(), temp->height());
	dialog->setWindowModality(Qt::WindowModal);
	dialog->setAttribute( Qt::WA_DeleteOnClose );
	connect(dialog, SIGNAL(okClicked()), this, SLOT(sltSetProperties()));
	dialog->exec();
}
void MediaListWidget::sltSetProperties()
{
	Q_EMIT(sigSetProperties(this->currentRow(), ui.txtWidth->text().toDouble(), 
		   ui.txtHeight->text().toDouble(), ui.txtTitle->text(), ui.txtAltText->text()));
	kDebug() << "signal emmited" ;
}

void MediaListWidget::sltRemoveMedia()
{
	kDebug() << this->currentRow();
	Q_EMIT(sigRemoveMedia(this->currentRow()));
}

#include "medialistwidget.moc"