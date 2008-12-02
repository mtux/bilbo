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
//#include <QFileDialog>
#include <kfiledialog.h>

#include "addimagedialog.h"
#include "bilbomedia.h"
#include "constants.h"

AddImageDialog::AddImageDialog(QWidget *parent) :QDialog(parent)
{
	setupUi(this);
	this->kButtonbox->addButton(KGuiItem(i18n("Ok"), KIcon("dialog-ok-apply")), QDialogButtonBox::AcceptRole, this, SLOT(accept()));
	this->kButtonbox->addButton(KGuiItem(i18n("Cancel"), KIcon("dialog-cancel")), QDialogButtonBox::RejectRole, this, SLOT(reject()));
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
	media->setName(name);
	
	if (txtRemoteUrl->isEnabled()) {
		media->setRemoteUrl(mSelectedImageUrl);
		media->setUploded(true);
	} else {
		QFile::copy(mSelectedImageUrl, TEMP_MEDIA_DIR + "/" + name);
		media->setLocalUrl(TEMP_MEDIA_DIR + "/" + name);
		media->setUploded(false);
	}
	Q_EMIT signalAddImage(media);
// 	KDialog::accept();
	QDialog::accept();
}

void AddImageDialog::reject()
{
// 	KDialog::reject();
	QDialog::reject();
}

void AddImageDialog::slotBrowseFiles()
{
	KFileDialog *fileDialog = new KFileDialog(KUrl(), "", this->parentWidget());
	//fileDialog->setFileMode(QFileDialog::ExistingFiles);
	fileDialog->setMode(KFile::File & KFile::LocalOnly & KFile::ExistingOnly);

	if (fileDialog->exec())
	{
		//mSelectedImageUrl = fileDialog->selectedFiles().at(0);
		mSelectedImageUrl = fileDialog->selectedFile();
		txtLocalUrl->setText(mSelectedImageUrl);
		txtRemoteUrl->setDisabled(true);
	}
}

void AddImageDialog::slotDisableLocalPath()
{
	if (txtRemoteUrl->text().isEmpty())
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

#include "addimagedialog.moc"
