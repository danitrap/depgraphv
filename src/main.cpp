/**
 * main.cpp
 *
 * This source file is part of dep-graph - An useful tool to analize header
 * dependendencies via graphs.
 *
 * This software is distributed under the MIT License:
 *
 * Copyright (c) 2013 Francesco Guastella aka romeoxbm
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
#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

#include <QDebug>

int main( int argc, char *argv[] )
{
	QApplication app( argc, argv );
	app.setApplicationName( "dep-graph" );
	app.setApplicationVersion( "0.2" );

	//Loading translations
	QTranslator qtTranslator;
	QString tPath = QLibraryInfo::location( QLibraryInfo::TranslationsPath );
	qDebug() << tPath;

	qtTranslator.load( QString( "qt_%1" ).arg( QLocale::system().name() ), tPath );
	app.installTranslator( &qtTranslator );

	QTranslator appTranslator;
	appTranslator.load( QString( "core_%1").arg( QLocale::system().name() ) );
	app.installTranslator( &appTranslator );

	depgraph::MainWindow w;
	//Place the mainWindow at center screen
	w.move( QApplication::desktop()->screenGeometry().center() - w.rect().center() );
	w.show();

	return app.exec();
}
