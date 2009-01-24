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
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdebug.h>
// #include <kio/job.h>
// #include <kio/jobuidelegate.h>

#include "addimagedialog.h"
#include "bilbomedia.h"
// #include "global.h"
// #include "settings.h"

AddImageDialog::AddImageDialog(QWidget* parent): AddMediaDialog(parent)
{
    QStringList mimeFilter;
    mimeFilter << "image/gif" << "image/jpeg" << "image/png" ;
    ui.kurlreqMediaUrl->fileDialog()->setMimeFilter( mimeFilter );
    
    editFrame = new QFrame(this);
    editFrame->setFrameShape(QFrame::StyledPanel);
    editFrame->setFrameShadow(QFrame::Raised);
//     editImageWidget = new QWidget( this );
//     editImageWidget = new QWidget( editFrame );
    editImageWidgetUi.setupUi( editFrame );
//     ui.gridLayout->addWidget( editImageWidget, 1, 0, 1, 2 );
    ui.gridLayout_2->addWidget( editFrame, 1, 0, 3, 1 );
//     ui.gridLayout->update();
//     connect( this, SIGNAL( okClicked() ), this, SLOT( sltOkClicked() ) );
}


AddImageDialog::~AddImageDialog()
{
}


// void AddImageDialog::sltOkClicked()
// {
//     kDebug() << "ok zadim :D";
//     AddMediaDialog::sltOkClicked();
//     
//     KUrl imageUrl = ui.kurlreqMediaUrl->url();
//     if ( !imageUrl.isEmpty() && imageUrl.isValid() && !imageUrl.isLocalFile() ) {
//         if ( Settings::download_remote_media() ) {
//             kDebug() << "download!";
//             
//             KUrl localUrl = KUrl( "file://" + __tempMediaDir + imageUrl.fileName() );
//             KIO::Job*  copyJob = KIO::file_copy( imageUrl, localUrl, -1, KIO::Overwrite );
//             connect( copyJob, SIGNAL( result( KJob * ) ), this, 
//                     SLOT( sltRemoteFileCopied( KJob * ) ) );
//         }
//     }
// }

// void AddImageDialog::sltRemoteFileCopied(KJob* job)
// {
// //     AddMediaDialog::sltRemoteFileCopied(job);
//     KIO::FileCopyJob *copyJob = dynamic_cast <KIO::FileCopyJob*>( job );
//     if ( job->error() ) {
//         copyJob->ui()->setWindow( this );
//         copyJob->ui()->showErrorMessage();
//     } else {
//         //KIO::FileCopyJob *copyJob = dynamic_cast <KIO::FileCopyJob*> (job);
//         media->setLocalUrl( copyJob->destUrl().toLocalFile() );
//         addOtherMediaAttributes();
//     }
// }

void AddImageDialog::addOtherMediaAttributes()
{
    if ( media->mimeType().contains( "image" ) ) {
        kDebug() << "emitting add image signal";
        Q_EMIT sigAddImage( media, editImageWidgetUi.spinboxWidth->value(), 
                            editImageWidgetUi.spinboxHeight->value(), 
                            editImageWidgetUi.txtTitle->text(), 
                            editImageWidgetUi.txtAltText->text() );
    } else {
        KMessageBox::error( this, i18n( "The selected media is not an image file, or its format isn't supported." ) );
    }
}

#include "addimagedialog.moc"
