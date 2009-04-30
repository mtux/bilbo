/****************************************************************************
**
** Copyright (C) 1992-2006 Trolltech ASA. All rights reserved.
** Copyright (C) 2007 Antonio Aloisio <gnuton@gnuton.org>
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef HTML_EXPORTER_H
#define HTML_EXPORTER_H

#include <QTextFrame>
#include <QList>

class QTextDocument;
class QTextBlock;
class QString;
class QTextFrameFormat;
class StyleMode;
class QTextTable;
class QTextCharFormat;
class QTextLength;


class htmlExporter
{
public:
    htmlExporter();
    ~htmlExporter();

    QString toHtml( const QTextDocument* document );

    void setDefaultCharFormat( QTextCharFormat charFormat );
    void setDefaultBlockFormat( QTextBlockFormat blockFormat );

private:
    enum StyleMode { EmitStyleTag, OmitStyleTag };
//     enum tag { span = 0, h1, h2, h3, h4, h5, strong, em, s, u, code, sub, sup };
    enum tag { span = 0, strong, em, s, u, code, sub, sup };

    void emitFrame( QTextFrame::Iterator frameIt );
    void emitTable( const QTextTable *table );
    void emitAttribute( const char *attribute, const QString &value );
    QList<tag>  emitCharFormatStyle( const QTextCharFormat &charFormat, 
                                     const QTextBlockFormat &blockFormat );
    void emitTextLength( const char *attribute, const QTextLength &length );
    void emitAlignment( Qt::Alignment align );
    void emitFloatStyle( QTextFrameFormat::Position pos, StyleMode mode = EmitStyleTag );
    void emitMargins( const QString &top, const QString &bottom, const QString &left, const QString &right );
    void emitFragment( const QTextFragment &fragment, const QTextBlockFormat &blockFormat);
    bool isOrderedList( int style );
    void emitBlockAttributes( const QTextBlock &block );
    void emitBlock( const QTextBlock &block );
    QTextFormat formatDifference( const QTextFormat &from, const QTextFormat &to );
    void sanitizeHtml();

    QString html;
    const QTextDocument* doc;
    //QTextCharFormat defaultCharFormat;
    QTextCharFormat mDefaultCharFormat;
    QTextBlockFormat mDefaultBlockFormat;
};

#endif
