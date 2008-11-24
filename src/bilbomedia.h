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
#ifndef BILBOMEDIA_H
#define BILBOMEDIA_H

#include <QString>

/**
Contains needed properties of each media object, like images and videos.

	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
 */
class QMimeData;
class BilboMedia
{
	public:
		///BilboMedia constructor
		BilboMedia();
		
		///BilboMedia destructor
		~BilboMedia();
		
		int blogId() const;
		void setBlogId(const int blog_id);
		
		int mediaId() const;
		void setMediaId(const int media_id);
		
        bool isUploaded() const;
		void setUploded(bool uploaded);
		
        QString localUrl() const;
		void setLocalUrl(const QString &url);
		
        QString remoteUrl() const;
		void setRemoteUrl(const QString &url);
		
        QString mimeType() const;
		void setMimeData(const QString &data);
		
        QString name() const;
		void setName(const QString &name);
		
	private:
		int mBlogId;
		int mMediaId;
		bool mIsUploaded;
		QString mLocalUrl;
		QString mRemoteUrl;
		QString mMimeType;
		QString mName;
		
};

#endif
