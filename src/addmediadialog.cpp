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

//#include <QFileDialog>
#include <kfiledialog.h>
#include <klineedit.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kdebug.h>
#include <kio/job.h>
// #include <kio/netaccess.h>
// #include <kio/jobuidelegate.h>

#include "addmediadialog.h"
#include "bilbomedia.h"
#include "global.h"
#include "settings.h"

AddMediaDialog::AddMediaDialog( QWidget *parent ) : KDialog( parent )
{
    QDialog *dialog = new QDialog( parent );
    ui.setupUi( dialog );
    
//     QStringList mimeFilter;
//     mimeFilter << "image/gif" << "image/jpeg" << "image/png" ;
//     ui.kurlreqMediaUrl->fileDialog()->setMimeFilter( mimeFilter );
    
    ui.kurlreqMediaUrl->fileDialog()->setWindowTitle( i18n( "Choose a file" ) );
    ui.kurlreqMediaUrl->lineEdit()->setToolTip( i18n( "Type media path here." ) );
    ui.kurlreqMediaUrl->button()->setToolTip( i18n( "Browse" ) );
    
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    this->setMainWidget( dialog );
    this->setWindowTitle( dialog->windowTitle() );
    this->resize( dialog->width(), dialog->height() );
    connect( this, SIGNAL( okClicked() ), this, SLOT( sltOkClicked() ) );
    connect( ui.radiobtnLocalUrl, SIGNAL( toggled( bool ) ), this, 
             SLOT( sltMediaSourceChanged() ) );
    connect( ui.radiobtnRemoteUrl, SIGNAL( toggled( bool ) ), this, 
             SLOT( sltMediaSourceChanged() ) );
}

AddMediaDialog::~AddMediaDialog()
{
}

void AddMediaDialog::sltOkClicked()
{
    KUrl mediaUrl = ui.kurlreqMediaUrl->url();
    kDebug() << "parent ok";
    if ( !mediaUrl.isEmpty() ) {
        if ( mediaUrl.isValid() ) {
            media = new BilboMedia();
            QString name = mediaUrl.fileName();

//  qDebug() << type;
            media->setName( name );

            if ( !mediaUrl.isLocalFile() ) {
//                 if ( Settings::download_remote_media() ) {
//                     kDebug() << "download!";
// 
//                     if ( KIO::NetAccess::exists( mediaUrl.url(), true, NULL ) ) {
// //       KTemporaryFile tmpFile;
//                         KUrl localUrl = KUrl( "file://" + __tempMediaDir + name );
// //       KUrl localUrl = KUrl(tempFile.fileName());
//                         KIO::Job*  copyJob = KIO::file_copy( mediaUrl, localUrl, -1, KIO::Overwrite );
//                         connect( copyJob, SIGNAL( result( KJob * ) ), this,
//                                  SLOT( sltRemoteFileCopied( KJob * ) ) );
//                     } else {
//                         KMessageBox::sorry( this, i18n( "The requested media file doesn't exist, or it isn't readable." ),
//                                             i18n( "File not found" ) );
//                         return;
//                     }
//                 }
                media->setRemoteUrl( mediaUrl.url() );
                media->setUploaded( true );
//                 media->setLocal( false );

                KIO::MimetypeJob* typeJob = KIO::mimetype( mediaUrl, KIO::HideProgressInfo );
                //KIO::TransferJob* tempJob = typeJob;
                //KIO::TransferJob* tempJob = KIO::mimetype(mediaUrl,false);

                connect( typeJob, SIGNAL( mimetype( KIO::Job *, const QString & ) ), this,  SLOT( sltRemoteFileTypeFound( KIO::Job *, const QString & ) ) );
                
                addOtherMediaAttributes();
                
            } else {
                bool copyResult = QFile::copy( mediaUrl.toLocalFile(), __tempMediaDir
                                               + name );
                if ( !copyResult ) {
                    int ret = KMessageBox::questionYesNo( this, i18n( "This file is already  added to Bilbo temp directory, and won't be copied again.\nyou can save the file with different name and try again.\ndo you want to continue using the existing file?" ), i18n( "File already exists" ) );
                    if ( ret == KMessageBox::No ) return;
                }
                media->setLocalUrl( __tempMediaDir + name );
                media->setRemoteUrl( "file://" + __tempMediaDir + name );
                media->setUploaded( false );
//                 media->setLocal( true );

                KMimeType::Ptr typePtr;
                typePtr = KMimeType::findByUrl( mediaUrl, 0, true, false );
                name = typePtr.data()->name();
                kDebug() << name ;
                media->setMimeType( name );
                Q_EMIT sigMediaTypeFound( media );

                addOtherMediaAttributes();
            }
        } else {
            KMessageBox::error( this, i18n( "The selected media address is an invalid url." ) );
        }
    }
}

void AddMediaDialog::sltRemoteFileTypeFound( KIO::Job *job, const QString &type )
{
    kDebug() << type ;
    media->setMimeType( type );
//     if ( !Settings::download_remote_media() ) {
//         media->setLocalUrl( media->remoteUrl() );
//         Q_EMIT signalAddMedia( media );
    Q_EMIT sigMediaTypeFound( media );
//         addOtherMediaAttributes();
//     }
}

// void AddMediaDialog::sltRemoteFileCopied( KJob *job )
// {
//     KIO::FileCopyJob *copyJob = dynamic_cast <KIO::FileCopyJob*>( job );
//     if ( job->error() ) {
//         copyJob->ui()->setWindow( this );
//         copyJob->ui()->showErrorMessage();
//     } else {
//         //KIO::FileCopyJob *copyJob = dynamic_cast <KIO::FileCopyJob*> (job);
//         media->setLocalUrl( copyJob->destUrl().toLocalFile() );
//         Q_EMIT signalAddMedia( media );
//     }
// }

void AddMediaDialog::addOtherMediaAttributes()
{
    kDebug() << "emmiting add media signal";
    Q_EMIT sigAddMedia( media );
}

void AddMediaDialog::sltMediaSourceChanged()
{
    if ( ui.radiobtnRemoteUrl->isChecked() ) {
        ui.kurlreqMediaUrl->button()->setEnabled( false );
    } else {
        ui.kurlreqMediaUrl->button()->setEnabled( true );
    }
}
#include "addmediadialog.moc"
