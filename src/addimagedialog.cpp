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
#include <QFileDialog>
#include <QFileSystemModel>
#include <QDebug>

#include "addimagedialog.h"
#include "bilbomedia.h"
#include "constants.h"

AddImageDialog::AddImageDialog(QWidget *parent) :QDialog(parent)
{
	setupUi(this);
	connect(btnBrowse, SIGNAL(clicked()), this, SLOT(slotBrowseFiles()));
	connect(txtRemoteUrl, SIGNAL(textEdited( const QString& )), this, SLOT(slotDisableLocalPath()));
}

AddImageDialog::~AddImageDialog()
{
}

void AddImageDialog::accept()
{
	BilboMedia *media = new BilboMedia();
	
	QStringList list1 = selectedImageUrl().split("/", QString::SkipEmptyParts);
	QString name = list1.last();
// 	QFileSystemModel *fs = new QFileSystemModel();
// 	QString type = fs->type(fs->index(selectedImageUrl()));
// 	qDebug() << type;
	media->setName(name);
	
	if (txtRemoteUrl->isEnabled()) {
		media->setRemoteUrl(mSelectedImageUrl);
		media->setAsUploded(true);
	} else {
		QFile::copy(mSelectedImageUrl, TEMP_MEDIA_DIR + name);
		media->setLocalUrl(TEMP_MEDIA_DIR + name);
		media->setAsUploded(false);
	}
	Q_EMIT signalAddImage(media);
	QDialog::accept();
}

void AddImageDialog::reject()
{
	QDialog::reject();
}

void AddImageDialog::slotBrowseFiles()
{
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setFileMode(QFileDialog::ExistingFiles);

	if (fileDialog->exec())
	{
		mSelectedImageUrl = fileDialog->selectedFiles().at(0);
		txtLocalUrl->setText(mSelectedImageUrl);
		txtRemoteUrl->setDisabled(true);
	}
}

void AddImageDialog::slotDisableLocalPath()
{
	if (txtRemoteUrl->text() == "")
	{
		txtLocalUrl->setDisabled(false);
		btnBrowse->setDisabled(false);
	} else {
		txtLocalUrl->setDisabled(true);
		btnBrowse->setDisabled(true);
	}
}

QString AddImageDialog::selectedImageUrl()
{
	if (txtRemoteUrl->isEnabled())
		mSelectedImageUrl = txtRemoteUrl->text();
	return mSelectedImageUrl;
}
