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
#include "bilbomedia.h"
#include <QMimeData>

BilboMedia::BilboMedia()
{
}

BilboMedia::~BilboMedia()
{
}

int BilboMedia::blogId() const
{
	return mBlogId;
}

void BilboMedia::setBlogId(const int blog_id)
{
	mBlogId = blog_id;
}

int BilboMedia::mediaId() const
{
	return mMediaId;
}

void BilboMedia::setMediaId(const int media_id)
{
	mMediaId = media_id;
}

bool BilboMedia::isUploaded() const
{
	return mIsUploaded;
}

void BilboMedia::setUploded(bool uploaded)
{
	mIsUploaded = uploaded;
}

QString BilboMedia::localUrl() const
{
	return mLocalUrl;
}

void BilboMedia::setLocalUrl(const QString& url)
{
	mLocalUrl = url;
}

QString BilboMedia::remoteUrl() const
{
	return mRemoteUrl;
}

void BilboMedia::setRemoteUrl(const QString& url)
{
	mRemoteUrl = url;
}

QString BilboMedia::mimeType() const
{
	return mMimeType;
}

void BilboMedia::setMimeData(const QString &type)
{
	mMimeType = type;
}

QString BilboMedia::name() const
{
	return mName;
}

void BilboMedia::setName(const QString &name)
{
	mName = name;
}
