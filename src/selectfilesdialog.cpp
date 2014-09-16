/**
 * selectfilesdialog.cpp
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
#include "selectfilesdialog.h"
#include "ui_selectfilesdialog.h"

namespace depgraphV
{
	SelectFilesDialog::SelectFilesDialog( QWidget* parent )
		: QDialog( parent ),
		_ui( new Ui::SelectFilesDialog )
	{
		_ui->setupUi( this );

		AppConfig* c = Singleton<AppConfig>::instancePtr();

		connect( c, SIGNAL( configRestored() ),
				 this, SLOT( onConfigRestored() )
		);
		connect( c, SIGNAL( headerNameFiltersChanged() ),
				 this, SLOT( onNameFiltersChanged() )
		);
		connect( c, SIGNAL( sourceNameFiltersChanged() ),
				 this, SLOT( onNameFiltersChanged() )
		);

		_folderModel = new FoldersModel( this );
	}
	//-------------------------------------------------------------------------
	SelectFilesDialog::~SelectFilesDialog()
	{
		delete _ui;
	}
	//-------------------------------------------------------------------------
	void SelectFilesDialog::changeEvent( QEvent* event )
	{
		if( event && event->type() == QEvent::LanguageChange )
			_ui->retranslateUi( this );

		QDialog::changeEvent( event );
	}
	//-------------------------------------------------------------------------
	void SelectFilesDialog::onClose( int result )
	{
		if( result )
			_folderModel->commitChanges();
		else
			_folderModel->revertChanges();
	}
	//-------------------------------------------------------------------------
	void SelectFilesDialog::onConfigRestored()
	{
		AppConfig* c = Singleton<AppConfig>::instancePtr();
		onNameFiltersChanged();
		_folderModel->initialize( _ui->treeView, _ui->listView, c->rootFolder() );
	}
	//-------------------------------------------------------------------------
	void SelectFilesDialog::onNameFiltersChanged()
	{
		AppConfig* c = Singleton<AppConfig>::instancePtr();
		QStringList l = c->headerNameFilters() + c->sourceNameFilters();
		_folderModel->setFileNameFilters( l );
	}
}
