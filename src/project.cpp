/**
 * project.cpp
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
#include "project.h"
#include <QSqlError>
#include <QDebug>

namespace depgraphV
{
	Project::Project( const QString& filePath, QObject* parent )
		: QObject( parent ),
		  _db( QSqlDatabase::addDatabase( "QSQLITE" ) )
	{
		_db.setDatabaseName( filePath );
		if( !_db.open() )
		{
			qDebug() << _db.lastError();
		}
	}
	//-------------------------------------------------------------------------
	Project::~Project()
	{
		_db.close();
	}
	//-------------------------------------------------------------------------
	Project* Project::newProject( const QString& filePath, QObject* parent )
	{
		Project* p = new Project( filePath, parent );

		//TODO Create here tables, views, etc.

		return p;
	}
	//-------------------------------------------------------------------------
	Project* Project::openProject( const QString& filePath, QObject* parent )
	{
		return new Project( filePath, parent );
	}
}
