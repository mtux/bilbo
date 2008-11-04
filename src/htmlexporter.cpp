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

#include "htmlexporter.h"

#include <QString>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextFrame>
#include <QTextBlock>
#include <QTextList>
#include <QTextTable>
#include <QTextFormat>
#include <QVarLengthArray>
#include <QDebug>

htmlExporter::htmlExporter()
{
    //kDebug();
}

htmlExporter::~htmlExporter()
{
    //kDebug();
}

QString htmlExporter::toHtml(const QTextDocument* document )
{
    //kDebug();
    if (document->isEmpty()) return html;
    doc = document;

	qDebug()<<doc->blockCount()<<endl;
	
    const QFont defaultFont = doc->defaultFont();
    defaultCharFormat.setFont(defaultFont);

    emitFrame(doc->rootFrame()->begin());
// 	emitBlock(doc->rootFrame()->begin().currentBlock());
    //sanitizeHtml();
    html.replace(QRegExp("<br[\\s]*/>"), "<br />\n");

    return html;
}


void htmlExporter::emitFrame(QTextFrame::Iterator frameIt)
{
//     kDebug() << "html" << html;
    if (!frameIt.atEnd()) {
// 		qDebug() << "frameIt is not at end" << endl;
        QTextFrame::Iterator next = frameIt;
        ++next;
        if (next.atEnd()
                && frameIt.currentFrame() == 0
                && frameIt.parentFrame() != doc->rootFrame()
                && frameIt.currentBlock().begin().atEnd())
            return;
    }
	

    for (QTextFrame::Iterator it = frameIt;
            !it.atEnd(); ++it) {
// 		qDebug() << "entered for loop" << endl;
        if (QTextFrame *f = it.currentFrame()) {
// 			qDebug() << "Its a frame, not a block" << endl;
            if (QTextTable * table = qobject_cast<QTextTable *>(f)) {
                emitTable(table);
            } else {
// 				qDebug() << "isn't table" << endl;
                html += QLatin1String("\n<table");
                QTextFrameFormat format = f->frameFormat();

                if (format.hasProperty(QTextFormat::FrameBorder))
                    emitAttribute("border", QString::number(format.border()));

                html += QLatin1String(" style=\"-qt-table-type: frame;");
                emitFloatStyle(format.position(), OmitStyleTag);

                if (format.hasProperty(QTextFormat::FrameMargin)) {
                    const QString margin = QString::number(format.margin());
                    emitMargins(margin, margin, margin, margin);
                }

                html += QLatin1Char('\"');

                emitTextLength("width", format.width());
                emitTextLength("height", format.height());

                QBrush bg = format.background();
                if (bg != Qt::NoBrush)
                    emitAttribute("bgcolor", bg.color().name());

                html += QLatin1Char('>');
                html += QLatin1String("\n<tr>\n<td style=\"border: none;\">");
                emitFrame(f->begin());
                html += QLatin1String("</td></tr></table>");
            }
        } else if (it.currentBlock().isValid()) {
// 			qDebug()<< "is valid" << endl;
            emitBlock(it.currentBlock());
        }
    }
}


void htmlExporter::emitTable(const QTextTable *table)
{
	//qDebug() << "emitTable" << html;
    QTextTableFormat format = table->format();

    html += QLatin1String("\n<table");

    if (format.hasProperty(QTextFormat::FrameBorder))
        emitAttribute("border", QString::number(format.border()));

    emitFloatStyle(format.position());
    emitAlignment(format.alignment());
    emitTextLength("width", format.width());

    if (format.hasProperty(QTextFormat::TableCellSpacing))
        emitAttribute("cellspacing", QString::number(format.cellSpacing()));
    if (format.hasProperty(QTextFormat::TableCellPadding))
        emitAttribute("cellpadding", QString::number(format.cellPadding()));

    QBrush bg = format.background();
    if (bg != Qt::NoBrush)
        emitAttribute("bgcolor", bg.color().name());

    html += QLatin1Char('>');

    const int rows = table->rows();
    const int columns = table->columns();

    QVector<QTextLength> columnWidths = format.columnWidthConstraints();
    if (columnWidths.isEmpty()) {
        columnWidths.resize(columns);
        columnWidths.fill(QTextLength());
    }
    Q_ASSERT(columnWidths.count() == columns);

    QVarLengthArray<bool> widthEmittedForColumn(columns);
    for (int i = 0; i < columns; ++i)
        widthEmittedForColumn[i] = false;

    const int headerRowCount = qMin(format.headerRowCount(), rows);
    if (headerRowCount > 0)
        html += QLatin1String("<thead>");

    for (int row = 0; row < rows; ++row) {
        html += QLatin1String("\n<tr>");

        for (int col = 0; col < columns; ++col) {
            const QTextTableCell cell = table->cellAt(row, col);

            // for col/rowspans
            if (cell.row() != row)
                continue;

            if (cell.column() != col)
                continue;

            html += QLatin1String("\n<td");

            if (!widthEmittedForColumn[col]) {
                emitTextLength("width", columnWidths.at(col));
                widthEmittedForColumn[col] = true;
            }

            if (cell.columnSpan() > 1)
                emitAttribute("colspan", QString::number(cell.columnSpan()));

            if (cell.rowSpan() > 1)
                emitAttribute("rowspan", QString::number(cell.rowSpan()));

            const QTextCharFormat cellFormat = cell.format();
            QBrush bg = cellFormat.background();
            if (bg != Qt::NoBrush)
                emitAttribute("bgcolor", bg.color().name());

            html += QLatin1Char('>');

            emitFrame(cell.begin());

            html += QLatin1String("</td>");
        }

        html += QLatin1String("</tr>");
        if (headerRowCount > 0 && row == headerRowCount - 1)
            html += QLatin1String("</thead>");
    }

    html += QLatin1String("</table>");
}


void htmlExporter::emitAttribute(const char *attribute, const QString &value)
{
//     kDebug() << "html" << html;
    html += QLatin1Char(' ');
    html += QLatin1String(attribute);
    html += QLatin1String("=\"");
    html += value;
    html += QLatin1Char('"');
}

QList<htmlExporter::tag> htmlExporter::emitCharFormatStyle(const QTextCharFormat &format)
{
//     kDebug() << "html" << html;
	qDebug("htmlExporter::emitCharFormatStyle");
    QList<htmlExporter::tag> tags;
    bool attributesEmitted = false;
    QLatin1String styleTag("<span style=\"");

    {
        const QString family = format.fontFamily();
        if (!family.isEmpty() && family != defaultCharFormat.fontFamily()) {
            if ( family.right(7) == "courier" ) {
                tags << code;
            } else {
                if (! attributesEmitted ) html += styleTag;
                html += QLatin1String(" font-family:'");
                html += family;
                html += QLatin1String("';");
                attributesEmitted = true;
            }
        }
    }

    if (format.hasProperty(QTextFormat::FontPointSize)
            && format.fontPointSize() != defaultCharFormat.fontPointSize()) {
        if (! attributesEmitted ) html += styleTag;
        html += QLatin1String(" font-size:");
        html += QString::number(format.fontPointSize());
        html += QLatin1String("pt;");
        attributesEmitted = true;
    } else if (format.hasProperty(QTextFormat::FontSizeAdjustment)) {

		///To use <h1-5> tags for font size
//         const int idx = format.intProperty(QTextFormat::FontSizeAdjustment) + 1;
// 
//         switch (idx) {
//         case 0: tags << h5; break;
//             //case 1: tags << h4; break; //NOTE h4 will be the normal text!
//         case 2: tags << h3; break;
//         case 3: tags << h2; break;
//         case 4: tags << h1; break;
//         }
		
		///To use <span> tags for font size

        static const char * const sizeNames[] = {
            "small", "medium", "large", "x-large", "xx-large"
        };
        const char *name = 0;
        const int idx = format.intProperty(QTextFormat::FontSizeAdjustment) + 1;
        if (idx >= 0 && idx <= 4) {
            name = sizeNames[idx];
        }
        if (name) {
        if (! attributesEmitted ) html += styleTag;
            html += QLatin1String(" font-size:");
            html += QLatin1String(name);
            html += QLatin1Char(';');
            attributesEmitted = true;
        }
    }


    if (format.fontWeight() > defaultCharFormat.fontWeight()) {
        tags << strong;
        /*if (! attributesEmitted ) html += styleTag;
        html += QLatin1String(" font-weight:");
        html += QString::number(format.fontWeight() * 8);
        html += QLatin1Char(';');
        attributesEmitted = true;*/
    }

    if (format.fontItalic() != defaultCharFormat.fontItalic()) {
        tags << em;
        /*
        if (! attributesEmitted ) html += styleTag;
        html += QLatin1String(" font-style:");
        html += (format.fontItalic() ? QLatin1String("italic") : QLatin1String("normal"));
        html += QLatin1Char(';');
        attributesEmitted = true;*/
    }

    if (format.fontUnderline() != defaultCharFormat.fontUnderline()) {
        tags << u;
    }


    if (format.fontStrikeOut() != defaultCharFormat.fontStrikeOut()) {
        tags << s;
    }

    if (format.fontOverline() != defaultCharFormat.fontOverline()) {
        if (format.fontOverline()) {
            if (! attributesEmitted ) html += styleTag;
            html += QLatin1String(" text-decoration: overline;");
            attributesEmitted = true;
        }
    }

    /*
    bool hasDecoration = false;
    bool atLeastOneDecorationSet = false;
    QLatin1String decorationTag(" text-decoration:");

    if (format.fontUnderline() != defaultCharFormat.fontUnderline() ||
    format.fontOverline() != defaultCharFormat.fontOverline() ||
    format.fontStrikeOut() != defaultCharFormat.fontStrikeOut() )
    {
      if (! attributesEmitted ) html += styleTag;
      html += decorationTag;
    }

    if (format.fontUnderline() != defaultCharFormat.fontUnderline()) {
        hasDecoration = true;
        if (format.fontUnderline()) {
            html += QLatin1String(" underline");
            atLeastOneDecorationSet = true;
        }
    }

    if (format.fontOverline() != defaultCharFormat.fontOverline()) {
        hasDecoration = true;
        if (format.fontOverline()) {
            html += QLatin1String(" overline");
            atLeastOneDecorationSet = true;
        }
    }

    if (format.fontStrikeOut() != defaultCharFormat.fontStrikeOut()) {
        hasDecoration = true;
        if (format.fontStrikeOut()) {
            html += QLatin1String(" line-through");
            atLeastOneDecorationSet = true;
        }
    }

    if (hasDecoration) {
        if (!atLeastOneDecorationSet)
            html += QLatin1String("none");
        html += QLatin1Char(';');
        attributesEmitted = true;
    }*/
    /* else {
        html.chop(qstrlen(decorationTag.latin1()));
    }*/

//     QBrush linkColor = KColorScheme(QPalette::Active, KColorScheme::Window).foreground(KColorScheme::LinkText);
    if ( format.foreground() != defaultCharFormat.foreground() &&
            format.foreground().style() != Qt::NoBrush) {
//         if ( format.foreground() != linkColor ) {
            if (! attributesEmitted ) html += styleTag;
            html += QLatin1String(" color:");
            html += format.foreground().color().name();
            html += QLatin1Char(';');
            attributesEmitted = true;
//         }
    }

    if (format.background() != defaultCharFormat.background()
            && format.background().style() != Qt::NoBrush) {
        if (! attributesEmitted ) html += styleTag;
        html += QLatin1String(" background-color:");
        html += format.background().color().name();
        html += QLatin1Char(';');
        attributesEmitted = true;
    }

    if (format.verticalAlignment() != defaultCharFormat.verticalAlignment()) { //TODO
        if (! attributesEmitted ) html += styleTag;
        html += QLatin1String(" vertical-align:");

        QTextCharFormat::VerticalAlignment valign = format.verticalAlignment();
        if (valign == QTextCharFormat::AlignSubScript)
            html += QLatin1String("sub");
        else if (valign == QTextCharFormat::AlignSuperScript)
            html += QLatin1String("super");

        html += QLatin1Char(';');
        attributesEmitted = true;
    }

    //Append close span Tag
    if (attributesEmitted) {
        html += QLatin1String("\">");
        tags.prepend(span);
    }

//     kDebug() << "html=>" << html << tags;
    return tags;
}

void htmlExporter::emitTextLength(const char *attribute, const QTextLength &length)
{
//     kDebug() << "html" << html;
    if (length.type() == QTextLength::VariableLength) // default
        return;

    html += QLatin1Char(' ');
    html += QLatin1String(attribute);
    html += QLatin1String("=\"");
    html += QString::number(length.rawValue());

    if (length.type() == QTextLength::PercentageLength)
        html += QLatin1String("%\"");
    else
        html += QLatin1String("\"");
}

void htmlExporter::emitAlignment(Qt::Alignment align)
{
	//qDebug() << "emitAlignment" << html;
    if (align & Qt::AlignLeft)
        return;
    else if (align & Qt::AlignRight)
        html += QLatin1String(" align=\"right\"");
    else if (align & Qt::AlignHCenter)
        html += QLatin1String(" align=\"center\"");
    else if (align & Qt::AlignJustify)
        html += QLatin1String(" align=\"justify\"");
	//qDebug() << "emitAlignment" << html;
}

void htmlExporter::emitFloatStyle(QTextFrameFormat::Position pos, StyleMode mode)
{
//     kDebug() << "html" << html;
    if (pos == QTextFrameFormat::InFlow)
        return;

    if (mode == EmitStyleTag)
        html += QLatin1String(" style=\"float:");
    else
        html += QLatin1String(" float:");

    if (pos == QTextFrameFormat::FloatLeft)
        html += QLatin1String(" left;");
    else if (pos == QTextFrameFormat::FloatRight)
        html += QLatin1String(" right;");
    else
        Q_ASSERT_X(0, "htmlExporter::emitFloatStyle()", "pos should be a valid enum type");

    if (mode == EmitStyleTag)
        html += QLatin1Char('\"');
}

void htmlExporter::emitMargins(const QString &top, const QString &bottom, const QString &left, const QString &right)
{
//     kDebug() << "html" << html;
    html += QLatin1String(" margin-top:");
    html += top;
    html += QLatin1String("px;");

    html += QLatin1String(" margin-bottom:");
    html += bottom;
    html += QLatin1String("px;");

    html += QLatin1String(" margin-left:");
    html += left;
    html += QLatin1String("px;");

    html += QLatin1String(" margin-right:");
    html += right;
    html += QLatin1String("px;");
}

void htmlExporter::emitFragment(const QTextFragment &fragment)
{
//     kDebug() << "html" << html;
    const QTextCharFormat format = fragment.charFormat();

    bool closeAnchor = false;

    if (format.isAnchor()) {
        const QString name = format.anchorName();
        if (!name.isEmpty()) {
            html += QLatin1String("<a name=\"");
            html += name;
            html += QLatin1String("\"></a>");
        }
        const QString href = format.anchorHref();
        if (!href.isEmpty()) {
            html += QLatin1String("<a href=\"");
            html += href;
            html += QLatin1String("\">");
            closeAnchor = true;
        }
    }

    QList<tag> tags = emitCharFormatStyle(format);
	qDebug() << "tags count" << tags.count() << endl;
    for ( int i = 0; i < tags.count(); ++i ) {
        switch ( tags.at(i) ) {
        case span: break; //Jump
        case h1: html += QLatin1String("<h1>"); break;
        case h2: html += QLatin1String("<h2>"); break;
        case h3: html += QLatin1String("<h3>"); break;
        case h4: html += QLatin1String("<h4>"); break;
        case h5: html += QLatin1String("<h5>"); break;
        case strong: html += QLatin1String("<strong>"); break;
        case em: html += QLatin1String("<em>"); break;
        case s: html += QLatin1String("<s>"); break;
        case u: {
            if (!closeAnchor)
                html += QLatin1String("<u>");
        }
        break;
        case code: html += QLatin1String("<code>"); break;
        case sub: html += QLatin1String("<sub>"); break;
        case sup: html += QLatin1String("<sup>"); break;
        }
    }

    /*     QLatin1String styleTag("<span style=\"");
         html += styleTag;
     
         const bool attributesEmitted = emitCharFormatStyle(format);
        if (attributesEmitted)
            html += QLatin1String("\">");
        else
            html.chop(qstrlen(styleTag.latin1()));
    */
    QString txt = fragment.text();
    if (txt.count() == 1 && txt.at(0) == QChar::ObjectReplacementCharacter) {
        if (format.isImageFormat()) {
            QTextImageFormat imgFmt = format.toImageFormat();

            html += QLatin1String("<img");

            if (imgFmt.hasProperty(QTextFormat::ImageName))
                emitAttribute("src", imgFmt.name());

            if (imgFmt.hasProperty(QTextFormat::ImageWidth))
                emitAttribute("width", QString::number(imgFmt.width()));

            if (imgFmt.hasProperty(QTextFormat::ImageHeight))
                emitAttribute("height", QString::number(imgFmt.height()));

            if (QTextFrame *imageFrame = qobject_cast<QTextFrame *>(doc->objectForFormat(imgFmt)))
                emitFloatStyle(imageFrame->frameFormat().position());

            html += QLatin1String(" />");
        }
    } else {
        Q_ASSERT(!txt.contains(QChar::ObjectReplacementCharacter));

        txt = Qt::escape(txt);

        // split for [\n{LineSeparator}]
        QString forcedLineBreakRegExp = QString::fromLatin1("[\\na]");
        forcedLineBreakRegExp[3] = QChar::LineSeparator;

        const QStringList lines = txt.split(QRegExp(forcedLineBreakRegExp));
        for (int i = 0; i < lines.count(); ++i) {
            //if (i > 0)
                //html += QLatin1String("<br />"); // space on purpose for compatibility with Netscape, Lynx & Co.
            html += lines.at(i);
        }
    }

    //Close Tags
    //if (!closeAnchor)
        for ( int i = tags.count(); i > 0; --i ) {
            switch ( tags.at(i - 1) ) {
            case span: html += QLatin1String("</span>"); break; //Jump
            case h1: html += QLatin1String("</h1>"); break;
            case h2: html += QLatin1String("</h2>"); break;
            case h3: html += QLatin1String("</h3>"); break;
            case h4: html += QLatin1String("</h4>"); break;
            case h5: html += QLatin1String("</h5>"); break;
            case strong: html += QLatin1String("</strong>"); break;
            case em: html += QLatin1String("</em>"); break;
            case s: html += QLatin1String("</s>"); break;
            case u: {
                if (!closeAnchor)
                    html += QLatin1String("</u>");
            }
            break;
            case code: html += QLatin1String("</code>"); break;
            case sub: html += QLatin1String("</sub>"); break;
            case sup: html += QLatin1String("</sup>"); break;
            }
        }
    /*    if (attributesEmitted)
            html += QLatin1String("</span>");
    */
    if (closeAnchor)
        html += QLatin1String("</a>");
//     kDebug() << "html=>" << html;
}

void htmlExporter::emitBlockAttributes(const QTextBlock &block)
{
//     kDebug() << "html" << html;
	qDebug("htmlExporter::emitBlockAttributes");
	QTextBlockFormat format = block.blockFormat();
    emitAlignment(format.alignment());

    Qt::LayoutDirection dir = format.layoutDirection();
    if (dir == Qt::LeftToRight) {
        // assume default to not bloat the html too much
        // html += QLatin1String(" dir='ltr'");
    } else {
        html += QLatin1String(" dir='rtl'");
    }

    QLatin1String style(" style=\"");
    html += style;

//     if (block.begin().atEnd()) {
//         html += QLatin1String("-qt-paragraph-type:empty;");
//     }

    emitMargins(QString::number(format.topMargin()),
                QString::number(format.bottomMargin()),
                QString::number(format.leftMargin()),
                QString::number(format.rightMargin()));

    html += QLatin1String(" -qt-block-indent:");
    html += QString::number(format.indent());
    html += QLatin1Char(';');

    html += QLatin1String(" text-indent:");
    html += QString::number(format.indent());
    html += QLatin1String("px;");

    //QTextCharFormat diff = formatDifference(defaultCharFormat, block.charFormat()).toCharFormat();
    //if (!diff.properties().isEmpty())
        //emitCharFormatStyle(diff);

    html += QLatin1Char('"');

    QBrush bg = format.background();
    if (bg != Qt::NoBrush)
        emitAttribute("bgcolor", bg.color().name());
}

void htmlExporter::emitBlock(const QTextBlock &block)
{
//     kDebug() << "html" << html;
    /*    if (block.begin().atEnd()) {
            // ### HACK, remove once QTextFrame::Iterator is fixed
            int p = block.position();
            if (p > 0)
                --p;
            QTextDocumentPrivate::FragmentIterator frag = doc->docHandle()->find(p);
            QChar ch = doc->docHandle()->buffer().at(frag->stringPosition);
            if (ch == QTextBeginningOfFrame
                || ch == QTextEndOfFrame)
                return;
        }
    */
    //html += QLatin1Char('\n');

    // save and later restore, in case we 'change' the default format by
    // emitting block char format information
    QTextCharFormat oldDefaultCharFormat = defaultCharFormat;

    QTextList *list = block.textList();
    if (list) {
// 		qDebug() << "list exists" << endl;
        if (list->itemNumber(block) == 0) { // first item? emit <ul> or appropriate
// 			qDebug() << "first item" << endl;
            const QTextListFormat format = list->format();
            const int style = format.style();
            switch (style) {
            case QTextListFormat::ListDecimal: html += QLatin1String("<ol"); break;
            case QTextListFormat::ListDisc: html += QLatin1String("<ul"); break;
            case QTextListFormat::ListCircle: html += QLatin1String("<ul type=\"circle\""); break;
            case QTextListFormat::ListSquare: html += QLatin1String("<ul type=\"square\""); break;
            case QTextListFormat::ListLowerAlpha: html += QLatin1String("<ol type=\"a\""); break;
            case QTextListFormat::ListUpperAlpha: html += QLatin1String("<ol type=\"A\""); break;
            default: html += QLatin1String("<ul"); // ### should not happen
			//qDebug() << html;
            }
            /*
            if (format.hasProperty(QTextFormat::ListIndent)) {
                html += QLatin1String(" style=\"-qt-list-indent: ");
                html += QString::number(format.indent());
                html += QLatin1String(";\"");
            }*/

            html += QLatin1Char('>');
        }

        html += QLatin1String("<li>");
        /*
                const QTextCharFormat blockFmt = formatDifference(defaultCharFormat, block.charFormat()).toCharFormat();
                if (!blockFmt.properties().isEmpty()) {
                    html += QLatin1String(" style=\"");
                    emitCharFormatStyle(blockFmt);
                    html += QLatin1Char('\"');
         
                    //defaultCharFormat.merge(block.charFormat());
                }
         html += QLatin1Char('>');*/
    }

    const QTextBlockFormat blockFormat = block.blockFormat();
    if (blockFormat.hasProperty(QTextFormat::BlockTrailingHorizontalRulerWidth)) {
        html += QLatin1String("<hr");

        QTextLength width = blockFormat.lengthProperty(QTextFormat::BlockTrailingHorizontalRulerWidth);
        if (width.type() != QTextLength::VariableLength)
            emitTextLength("width", width);
        else
            html += QLatin1Char(' ');

        html += QLatin1String("/>");
        return;
    }

    const bool pre = blockFormat.nonBreakableLines();
    if (pre) {
// 		qDebug() << "NonBreakable lines" << endl;
        if (list)
            html += QLatin1Char('>');
        html += QLatin1String("<pre");

        emitBlockAttributes(block);
        html += QLatin1Char('>');
    }
	else {
		if (list)
			html += QLatin1Char('>');
        html += QLatin1String("<div");
		emitBlockAttributes(block);
		html += QLatin1Char('>');
    }
    /*
        const QTextCharFormat blockCharFmt = block.charFormat();
        const QTextCharFormat diff = formatDifference(defaultCharFormat, blockCharFmt).toCharFormat();
     
         defaultCharFormat.merge(blockCharFmt);
    */
    QTextBlock::Iterator it = block.begin();

    for (; !it.atEnd(); ++it)
	{
// 		qDebug() << "next for" << endl;
        emitFragment(it.fragment());
	}
	
	//qDebug() << html << endl;
    if (pre)
        html += QLatin1String("</pre>");
    else 
	{	
		if (list)
        	html += QLatin1String("</li>");
    	else if ( ! (html.right(7).contains(QRegExp("<br[\\s]*/>[\\n]*"))) )
        	html += QLatin1String("<br />");//"</p>");
		html += QLatin1String("</div>");
	}
    // HACK html.replace( QRegExp("<br[\\s]*/>[\\n]*<br[\\s]*/>[\\n]*"),"<br />&nbsp;<br />" );

    if (list) {
        if (list->itemNumber(block) == list->count() - 1) { // last item? close list
            if (isOrderedList(list->format().style()))
                html += QLatin1String("</ol>");
            else
                html += QLatin1String("</ul>");
        }
    }

    defaultCharFormat = oldDefaultCharFormat;
	//qDebug() << html << endl;
}

QTextFormat htmlExporter::formatDifference(const QTextFormat &from, const QTextFormat &to)
{
    //kDebug();

    QTextFormat diff = to;

    const QMap<int, QVariant> props = to.properties();
    for (QMap<int, QVariant>::ConstIterator it = props.begin(), end = props.end();
            it != end; ++it)
        if (it.value() == from.property(it.key()))
            diff.clearProperty(it.key());

    return diff;
}

bool htmlExporter::isOrderedList(int style)
{
//     kDebug() << "html" << html;
    return style == QTextListFormat::ListDecimal || style == QTextListFormat::ListLowerAlpha
           || style == QTextListFormat::ListUpperAlpha;
}

void htmlExporter::sanitizeHtml()
{
//     kDebug() << "BEFORE" << html;

    int length;
    while ( html.count() != length ) {
        length = html.count();
        html.remove( QRegExp("</[^<]+><[^<|/]+[^/]>"));
    }
//     kDebug() << "AFTER" << html;

}
