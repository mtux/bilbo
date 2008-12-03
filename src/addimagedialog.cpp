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
//#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kdebug.h>
#include <kio/job.h>

#include "addimagedialog.h"
#include "bilbomedia.h"
#include "constants.h"

AddImageDialog::AddImageDialog(QWidget *parent) :KDialog(parent)
{
	QDialog *dialog = new QDialog(0);
	ui.setupUi(dialog);
	dialog->setAttribute( Qt::WA_DeleteOnClose );
	this->setMainWidget(dialog);
	this->resize(dialog->width(), dialog->height());
	connect(this, SIGNAL(okClicked()), this, SLOT(sltOkClicked()));
}

AddImageDialog::~AddImageDialog()
{
}

void AddImageDialog::sltOkClicked()
{
	KUrl tempUrl = ui.kurlreqLocalUrl->url();
	
	if (!tempUrl.isEmpty()) {
		if (tempUrl.isValid()) {
			media = new BilboMedia();
			QString name = tempUrl.fileName();
			media->setName(name);
			
			if (!tempUrl.isLocalFile()) {
				media->setRemoteUrl(tempUrl.url());
				media->setUploded(true);
				
				KIO::MimetypeJob* typeJob = KIO::mimetype(tempUrl);
				//KIO::TransferJob* tempJob = typeJob;
				//KIO::TransferJob* tempJob = KIO::mimetype(tempUrl,false);
				
				connect(typeJob, SIGNAL(mimetype(KIO::Job*, const QString&)), this, SLOT(sltRemoteFileTypeFound(KIO::Job*, const QString&)));
			} else {
				QFile::copy(tempUrl.toLocalFile(), TEMP_MEDIA_DIR + "/" + name);
				media->setLocalUrl(TEMP_MEDIA_DIR + "/" + name);
				media->setUploded(false);
				
				KMimeType::Ptr typePtr;
				typePtr = KMimeType::findByUrl(tempUrl, 0, true, false);
				name = typePtr.data()->name();
				kDebug() << name ;
				media->setMimeType(name);
				
				Q_EMIT signalAddImage(media);
			}
		} else {
			KMessageBox::error(this,i18n("The selected media address is an invalid url."));
		}
	}
}

void AddImageDialog::sltRemoteFileTypeFound(KIO::Job *job, const QString &type)
{
	kDebug() << type ;
	media->setMimeType(type);
	Q_EMIT signalAddImage(media);
}

#include "addimagedialog.moc"
