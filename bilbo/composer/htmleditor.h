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

#ifndef HTMLEDITOR_H
#define HTMLEDITOR_H

#include <QObject>
#include <QString>

class KAction;
namespace KTextEditor 
{
    class Editor;
    class View;
}
class QWidget;
class QMenu;
class HtmlEditorPrivate;

/**
    @author Mehrdad Momeny <mehrdad.momeny@gmail.com>
    @author Golnaz Nilieh <g382nilieh@gmail.com>
*/
class HtmlEditor : public QObject
{
    Q_OBJECT
public:
    static HtmlEditor* self();

    KTextEditor::View* createView( QWidget* parent );

//     void setContent( KTextEditor::View* view, const QString &text );

    QWidget* configPage( int number, QWidget* parent);

private slots:
    void toggleWordWrap();
    void toggleLineNumber();

private:
    friend class HtmlEditorPrivate;
    HtmlEditor();

    ~HtmlEditor();

    KTextEditor::Editor* mEditor;
};

#endif
