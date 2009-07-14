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

#include "uploadmediadialog.h"
#include <KDebug>
#include <KFileDialog>
#include <settings.h>
#include <KMessageBox>
#include "bilboblog.h"
#include "bilbomedia.h"
#include "backend.h"
#include <KApplication>
#include <QClipboard>
#include <kio/jobclasses.h>
#include <kio/job.h>

UploadMediaDialog::UploadMediaDialog( QWidget *parent ) :
    KDialog(parent), mCurrentBlog(0)
{
    kDebug();
    QWidget *widget = new QWidget;
    ui.setupUi(widget);
    this->setMainWidget(widget);
    this->setAttribute(Qt::WA_DeleteOnClose);//should delete manually after upload!
    setButtonText(KDialog::Ok, i18n("Upload") );
    setWindowTitle( i18n( "Upload media..." ) );
    ui.kcfg_FtpPath->setText(Settings::ftpServerPath());
    ui.kcfg_httpUrl->setText(Settings::httpUrl());
    setWindowModality(Qt::ApplicationModal);

    connect(ui.kcfg_FileBrowser, SIGNAL(textChanged(const QString)), this, SLOT(mediaChanged(const QString)));
    connect(ui.kcfg_uploadType, SIGNAL(currentIndexChanged(int)), this, SLOT(slotUploadTypeChanged(int)));
}

UploadMediaDialog::~UploadMediaDialog()
{
    kDebug();
    Settings::setFtpServerPath(ui.kcfg_FtpPath->text());
    Settings::setHttpUrl(ui.kcfg_httpUrl->text());
    Settings::self()->writeConfig();
}

void UploadMediaDialog::init( const BilboBlog *currentBlog )
{
    QString mediaPath = KFileDialog::getOpenFileName( KUrl("kfiledialog:///image?global"),
                                                      QString(), this,
                                                      i18n("Select media to upload"));
    if(mediaPath.isEmpty()) {
        deleteLater();
        return;
    }
    if(currentBlog) {
        mCurrentBlog = currentBlog;
        if(mCurrentBlog->supportUploadMedia()){
            ui.kcfg_uploadType->addItem( i18n("Blog API"), BlogAPI );
        }
    }
    ui.kcfg_uploadType->addItem( i18n("FTP"), FTP);
    KUrl mediaUrl(mediaPath);
    ui.kcfg_FileBrowser->setUrl(mediaUrl);
    ui.kcfg_Name->setText(mediaUrl.fileName());
    slotUploadTypeChanged(ui.kcfg_uploadType->currentIndex());
    this->show();
}

void UploadMediaDialog::mediaChanged(const QString newPath)
{
    KUrl newUrl(newPath);
    ui.kcfg_previewer->showPreview( newUrl);
    ui.kcfg_Name->setText( newUrl.fileName() );
}

void UploadMediaDialog::slotUploadTypeChanged(int index)
{
    UploadType type = (UploadType)ui.kcfg_uploadType->itemData(index).toInt();
    if(type == FTP) {
        ui.kcfg_ftpBox->setEnabled(true);
    } else {
        ui.kcfg_ftpBox->setEnabled(false);
    }
}

void UploadMediaDialog::slotButtonClicked(int button)
{
    if(button == KDialog::Ok) {
        UploadType type = (UploadType)ui.kcfg_uploadType->itemData(ui.kcfg_uploadType->currentIndex()).toInt();
        if( type == BlogAPI ) {///Using API!
            BilboMedia *media = new BilboMedia(this);
            KUrl mediaUrl( ui.kcfg_FileBrowser->url() );
            media->setLocalUrl(mediaUrl);
            media->setName( ui.kcfg_Name->text().isEmpty() ? mediaUrl.fileName() : ui.kcfg_Name->text() );
            media->setBlogId( mCurrentBlog->id() );
            media->setMimeType( KMimeType::findByUrl( mediaUrl, 0, true )->name() );
            Backend *b = new Backend( mCurrentBlog->id(), this);
            connect( b, SIGNAL( sigMediaUploaded(BilboMedia*) ),
                    this, SLOT(slotMediaObjectUploaded(BilboMedia*)) );
            connect( b, SIGNAL( sigError(QString)), this, SLOT(slotError(QString)));
            connect( b, SIGNAL( sigMediaError(QString,BilboMedia*) ), this, SLOT(slotError(QString)) );
            b->uploadMedia( media );
            this->hide();
            emit sigBusy(true);
        } else if( type == FTP ) {///Upload via FTP
            if( ui.kcfg_FtpPath->text().isEmpty() ) {
                KMessageBox::sorry(this, i18n("Please insert FTP Url."));
                return;
            }
            KUrl dest;
            dest.setUrl(ui.kcfg_FtpPath->text() , QUrl::TolerantMode);
            if( dest.isValid() ) {
                if( dest.scheme() == "ftp" || dest.scheme() == "sftp" ) {
                    dest.addPath( ui.kcfg_Name->text().isEmpty() ? ui.kcfg_FileBrowser->url().fileName() :
                                                                  ui.kcfg_Name->text() );
                    KIO::FileCopyJob *job = KIO::file_copy(ui.kcfg_FileBrowser->url(), dest);
                    connect(job, SIGNAL(result(KJob *)), this, SLOT(slotMediaObjectUploaded(KJob *)));
                    job->start();
                    this->hide();
                    return;
                }
            }
            KMessageBox::error(this, i18n("Inserted ftp url is not a valid url.\n\
Note that url must start with \"ftp\" or \"sftp\",\
 and end with a / that indicate the directory which file should upload to it."));
        }
    } else {
        close();
    }
}

void UploadMediaDialog::slotMediaObjectUploaded(KJob *job)
{
    emit sigBusy(false);
    if(job->error()) {
        kDebug()<<"Job error: "<<job->errorString();
        slotError(job->errorString());
    } else {
        KIO::FileCopyJob *fcj = qobject_cast<KIO::FileCopyJob*>(job);
        KUrl tmpUrl(ui.kcfg_httpUrl->text());
        QString destUrl;
        if(tmpUrl.isValid()){
            tmpUrl.adjustPath(KUrl::AddTrailingSlash);
            tmpUrl.setFileName(ui.kcfg_Name->text());
            destUrl = tmpUrl.prettyUrl();
        } else {
            destUrl = fcj->destUrl().prettyUrl();
        }
        QString msg;
        if( Settings::copyMediaUrl() ) {
            KApplication::clipboard()->setText( destUrl );
            msg = i18n( "Media uploaded, and URL copied to clipboard.\nYou can find it here:\n%1",
                        destUrl );
        } else {
            msg = i18n( "Media uploaded.\nYou can find it here:\n%1",
                        destUrl );
        }
        KMessageBox::information(this, msg, i18n( "Successfully uploaded" ), QString(), KMessageBox::AllowLink);
        close();
    }
}

void UploadMediaDialog::slotMediaObjectUploaded(BilboMedia *media)
{
    QString msg;
    emit sigBusy(false);
    if( Settings::copyMediaUrl() ) {
        KApplication::clipboard()->setText( media->remoteUrl().prettyUrl() );
        msg = i18n( "Media uploaded, and URL copied to clipboard.\nYou can find it here:\n%1",
                    media->remoteUrl().prettyUrl() );
    } else {
        msg = i18n( "Media uploaded.\nYou can find it here:\n%1",
                    media->remoteUrl().prettyUrl() );
    }
    KMessageBox::information(this, msg, i18n( "Successfully uploaded" ), QString(), KMessageBox::AllowLink);
    close();
}

void UploadMediaDialog::slotError( const QString &msg )
{
    if( KMessageBox::questionYesNo( this, i18n( "Media uploading failed with this result:\n%1\nTry again?", msg) )
        == KMessageBox::Yes ) {
        show();
    } else {
        close();
    }
}

#include "uploadmediadialog.moc"
