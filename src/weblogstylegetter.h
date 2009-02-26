/***************************************************************************

   Copyright (C) 2007-2008-2008 by Christian Weilbach <christian_weilbach@web.de>
   Copyright (C) 2007-2008 Antonio Aloisio <gnuton@gnuton.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef WEBLOGSTYLEGETTER_H
#define WEBLOGSTYLEGETTER_H
#include <QObject>
#include <QByteArray>
#include <QString>

/**
  @brief
  This class will:
  1. Create a temporary post
  2. Send post
  3. Read the post's url
  4. Get & save the html page
 */

namespace KIO
{
class Job;
}
class KJob;

// namespace KBlogger
// {

class BilboPost;
class Backend;

class WeblogStyleGetter: public QObject
{
    Q_OBJECT
public:
    WeblogStyleGetter( const int blogid, QObject* parent );
    ~WeblogStyleGetter();

    static QString styledHtml( const int blogid,
                              const QString& title,
                              const QString& content );
Q_SIGNALS:
    void sigStyleFetched();

private Q_SLOTS:
//     void slotPostSent();
    void sltTempPostPublished( int blogId, BilboPost* post );
    void sltTempPostFetched( BilboPost* post );
    void sltTempPostRemoved( int blog_id, const BilboPost &post );
    void sltHtmlCopied( KJob* job );
    void sltError( const QString & errMsg );

private:
    void generateRandomPostStrings();

    BilboPost *mTempPost;
    QString mCachePath;
    KIO::Job *mJob;
    QString mPostTitle;
    QString mPostContent;
    QWidget *mParent;

    Backend *b;
};

// } //namespace
#endif
