/**
 * main.cpp
 *
 * This source file is part of dep-graphV - An useful tool to analize header
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
#include "buildsettings.h"
#include <QApplication>
#include <stdio.h>

#ifdef WIN32
void toggleConsole( bool enabled, bool pause );
#endif

void printVersion()
{
#ifdef WIN32
	toggleConsole( true, true );
#endif
	printf( "%s version %s\n", APP_NAME, APP_VER );
}

void printHelp()
{
	printVersion();
	printf( "Usage\n\n" );
	printf( "\t%s [options]\n\n", APP_NAME );
	printf( "Options\n" );
	printf( "\t-h (--help) \t\t= Print this help message and quit.\n" );
	printf( "\t-v (--version) \t\t= Print %s version and quit.\n", APP_NAME );
	printf( "\t-l (--with-log)\t\t= Enable log messages (disabled by default).\n" );
}

#ifdef QT_USE_QT5
	void noMessageOutput( QtMsgType, const QMessageLogContext&, const QString& ) {}
#else
	void noMessageOutput( QtMsgType, const char* ) {}
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void toggleConsole( bool enabled, bool pause = false )
{
	bool initially = GetConsoleWindow() != NULL;
	bool success = true;

	if( enabled )
	{
		if( initially )
			return;

		if( AllocConsole() )
		{
			SetConsoleTitle( APP_NAME );
			freopen( "CONOUT$", "w", stdout );
			freopen( "CONOUT$", "w", stderr );
		}
		else
			success = false;
	}
	else if( initially )
	{
		if( pause )
			system( "pause" );

		success = FreeConsole();
	}

	if( !success )
		throw new std::exception();
}

void _atExitFnc()
{
	toggleConsole( false );
}

INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, INT )
{
	atexit( _atExitFnc );
	QApplication app( __argc, __argv );
#else

int main( int argc, char* argv[] )
{
	QApplication app( argc, argv );

#endif

	if( app.arguments().count() > 1 )
	{
		//First of all, check for valid option..
		QStringList validOptions;
		validOptions << "-h" << "--help" << "-v" << "--version" << "-l" << "--with-log";
		for( unsigned short i = 1; i < app.arguments().count(); ++i )
		{
			if( !validOptions.contains( app.arguments()[ i ] ) )
			{
				//Wrong option. Print help and quit
				printf( "WRONG USAGE: Unknown option \"%s\"\n\n", app.arguments()[ i ].toStdString() );
				printHelp();
				return 0;
			}
		}

		if( app.arguments().contains( "--version" ) || app.arguments().contains( "-v" ) )
		{
			printVersion();
			return 0;
		}

		else if( app.arguments().contains( "--help" ) || app.arguments().contains( "-h" ) )
		{
			printHelp();
			return 0;
		}

		else if( !app.arguments().contains( "--with-log" ) && !app.arguments().contains( "-l" ) )
		{
#ifdef QT_USE_QT5
			qInstallMessageHandler( &noMessageOutput );
#else
			qInstallMsgHandler( &noMessageOutput );
#endif
		}
#ifdef WIN32
		else
			toggleConsole( true );
#endif
	}

	app.setApplicationName( APP_NAME );
	app.setApplicationVersion( APP_VER );

	depgraphV::MainWindow w;
	w.show();

	return app.exec();
}
