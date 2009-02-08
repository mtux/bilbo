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

#ifndef MULTILINETEXTEDIT_H
#define MULTILINETEXTEDIT_H

//#include <QWidget>
//#include <QtGui>
//#include <QTextEdit>
#include "krichtextedit.h"

// class QNetwokAccessManager;
// class QNetworkReply;
class KJob;

//!Class MultiLineTextEdit Implements a TextEdit widget that supports new line charachters
/*!
 @author Mehrdad Momeny <mehrdad.momeny@gmail.com>
 @author Golnaz Nilieh <g382nilieh@gmail.com>
 */

class MultiLineTextEdit : public KRichTextEdit
{
    Q_OBJECT
public:
    //!Implements the class constructor.
    /*!
     * \param *parent is needed for KRichTextEdit constructor, which MultiLineTextEdit
     * inherits from.
     */
    MultiLineTextEdit( QWidget *parent = 0 );

    //!Implements the class destructor.
    ~MultiLineTextEdit();
    
    static void clearCache();
    
Q_SIGNALS:
    /**
     * when a remote image is downloaded from the web successfully, this signal is emmited.
     * @param imagePath is the image url on the web.
     */
    void sigRemoteImageArrived( const QString imagePath );

protected:

    /*!
     * When MultiLineTextEdit is focused on, checks each presssed key; then replaces
     * "Return" characters with QChar::LineSeparator special character.
     * this function is defined virtual in parent class: KRichTextEdit.
     */
    void keyPressEvent( QKeyEvent *event );
    /**
     * If the name is the url of a remote image, looks for it in cache, and if it doesn't exist, starts downloading of it and returns an empty QVariant object.
     * else if the image is local or has been downloaded before, returns image data as a
     * QVariant object.
     * 
     * @param type is type of the resource. @see QTextDocument::ResourceType.
     * @param name is the resource url.
     * @return a QVariant object which contains resource data, if it could be found by 
     * the function, else, it's an empty QVariant object.
     */
    virtual QVariant loadResource( int type, const QUrl & name );
    
// private:
//     QNetworkAccessManager *netManager;
//     
private Q_SLOTS:
//     void sltReplyFinished( QNetworkReply *reply, const QUrl & name );
    void sltRemoteFileCopied(KJob * job);
    
private:
    static QMap <QString, bool> downloadFinished;
};

// class GetImageThread : public QThread
// {
//     
// public:
//     GetImageThread( KRichTextEdit *parent = 0, const KUrl & image);
// //     ~GetImageThread();
//     
// protected:
//     virtual void run();
//     
// private:
//     KUrl imageUrl;
//     QTextCursor cursor;
// };

#endif
