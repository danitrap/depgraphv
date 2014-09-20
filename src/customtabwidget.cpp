/**
 * customtabwidget.cpp
 *
 * This source file is part of dep-graphV - An useful tool to analize header
 * dependendencies via graphs.
 *
 * This software is distributed under the MIT License:
 *
 * Copyright (c) 2013 - 2014 Francesco Guastella aka romeoxbm
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "customtabwidget.h"
#include "project.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlField>
#ifndef QT_USE_QT5
#	include <QTabBar>
#endif

namespace depgraphV
{
	CustomTabWidget::CustomTabWidget( QWidget* parent )
		: QTabWidget( parent ),
		  _newGraphCount( 0 )
	{
		connect( this, SIGNAL( tabCloseRequested( int ) ), this, SLOT( closeTab( int ) ) );

#ifdef QT_USE_QT5
		connect( this, SIGNAL( tabBarDoubleClicked( int ) ), this, SLOT( renameTab( int ) ) );
#else
		tabBar()->installEventFilter( this );
#endif
	}
	//-------------------------------------------------------------------------
	Graph* CustomTabWidget::currentGraph() const
	{
		if( currentWidget() )
			return static_cast<Graph*>( currentWidget() );

		return 0;
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::newGraph()
	{
		QString graphName = "New Graph " + QString::number( ++_newGraphCount );

		Project* p = Singleton<Project>::instancePtr();
		QSqlTableModel* model = p->tableModel( "graphSettings" );
		QSqlField f( "name", QVariant::String );
		f.setValue( graphName );
		QSqlRecord r;
		r.append( f );
		if( !model->insertRecord( -1, r ) )
		{
			QMessageBox::critical(
						this,
						tr( "New Graph" ),
						tr( "Unable to add a new graph:\n"
							"There's something wrong with your project file." )
			);
			return;
		}

		addTab(
			new Graph( this ),
			graphName
		);
	}
	//-------------------------------------------------------------------------
	void CustomTabWidget::closeTab( int index )
	{
		//TODO Check for changes and warn about them..
		delete widget( index );
	}
	//-------------------------------------------------------------------------
#ifndef QT_USE_QT5
	bool CustomTabWidget::eventFilter( QObject* o, QEvent* evt )
	{
		bool result = QTabWidget::eventFilter( o, evt );

		if( o == tabBar() && evt->type() == QEvent::MouseButtonDblClick )
		{
			QMouseEvent* mouseEvt = static_cast<QMouseEvent*>( evt );
			int index = tabBar()->tabAt( mouseEvt->pos() );
			if( index == -1 )
				return result;

			renameTab( index );
			return true;
		}

		return result;
	}
#endif
	//-------------------------------------------------------------------------
	void CustomTabWidget::renameTab( int index )
	{
		bool ok;
		QString newName = QInputDialog::getText(
							  this,
							  tr( "Change graph name" ),
							  tr( "Type a new graph name" ),
							  QLineEdit::Normal,
							  tabText( index ),
							  &ok
		);

		if( ok && !newName.isEmpty() )
			setTabText( index, newName );
	}
}
