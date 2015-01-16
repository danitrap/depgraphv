/**
 ******************************************************************************
 *                _                                        _
 *             __| | ___ _ __         __ _ _ __ __ _ _ __ | |__/\   /\
 *            / _` |/ _ \ '_ \ _____ / _` | '__/ _` | '_ \| '_ \ \ / /
 *           | (_| |  __/ |_) |_____| (_| | | | (_| | |_) | | | \ V /
 *            \__,_|\___| .__/       \__, |_|  \__,_| .__/|_| |_|\_/
 *                      |_|          |___/          |_|
 *
 ******************************************************************************
 *
 * project.cpp
 *
 * This source file is part of dep-graphV - An useful tool to analize header
 * dependendencies via graphs.
 *
 * This software is distributed under the MIT License:
 *
 * Copyright (c) 2013 - 2015 Francesco Guastella aka romeoxbm
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
#include "depgraphv_pch.h"
#include "project.h"
#include "helpers.h"

namespace depgraphV
{
	const QString Project::_defaultExtension = ".dProj";

	#define LATEST_VER	1
	#define MAGIC		( ( 'A' << 24 ) + ( 'V' << 16 ) + ( 'G' << 8 ) + 'G' )

	//-------------------------------------------------------------------------
	Project::Project( const QString& filePath, QObject* parent )
		: QObject( parent ),
		  Singleton<Project>(),
		  _fullPath( filePath ),
		  _version( LATEST_VER ),
		  _model( new QStandardItemModel( this ) ),
		  _delegate( new CustomItemDelegate( this ) ),
		  _mapper( new QDataWidgetMapper( this ) ),
		  _hasUnsubmittedChanges( false ),
		  _modified( false )
	{
		_updateProjectProperties();

		//Setting up QDataWidgetMapper
		_mapper->setModel( _model );
		_mapper->setSubmitPolicy( QDataWidgetMapper::ManualSubmit );
		_mapper->setItemDelegate( _delegate );

		QStringList f;
		f	<< "name"

			//HandleRootsDialog-specific column
			<< "selectedFolders"

			//TODO
			//Root folder being used by both SelectFilesDialog and HandleRootsDialog
			//<< "rootFolder"

			//Scan Mode
			<< "scanByFolders"
			<< "scanRecursively"
			<< "includeHiddenFolders"

			//Header Filters
			<< "hdr_parseEnabled"
			<< "hdr_standardFiltersEnabled"
			<< "hdr_currentStandardFilter"
			<< "hdr_customFilters"

			//Source Filters
			<< "src_parseEnabled"
			<< "src_standardFiltersEnabled"
			<< "src_currentStandardFilter"
			<< "src_customFilters"

			//Graph settings
			<< "layoutAlgorithm"
			<< "highQualityAA"
			<< "rendererType"

			<< "graphModel"

			//"Autogenerated" fields from graphAttribs.xml
			<< "graph_splines"
			<< "graph_nodesep"
			<< "vertex_shape"
			<< "vertex_style"
			<< "edge_minlen"
			<< "edge_style";

		for( int i = 0; i < f.size(); i++ )
			_fields.insert( f[ i ], i );

		_model->setHorizontalHeaderLabels( f );

		connect( _delegate, SIGNAL( editingStarted() ),
				 this, SLOT( _onDataChanged() )
		);
	}
	//-------------------------------------------------------------------------
	Project::~Project()
	{
	}
	//-------------------------------------------------------------------------
	Project* Project::create( QObject* parent )
	{
		return new Project( "", parent );
	}
	//-------------------------------------------------------------------------
	Project* Project::open( QObject* parent, const QString& fileName )
	{
		QString file = fileName;
		if( file.isEmpty() )
		{
			QString filter = tr( "Project Files (*%1)" ).arg( _defaultExtension );
			file = QFileDialog::getOpenFileName(
							   static_cast<QWidget*>( parent ),
							   tr( "Create a new project..." ),
							   QDir::homePath(),
							   filter,
							   &filter
			);
		}

		Project* p = 0;
		if( Helpers::addExtension( file, _defaultExtension ) )
			p = new Project( file, parent );

		return p;
	}
	//-------------------------------------------------------------------------
	void Project::addMapping( QWidget* w, const QString& fieldName,
							  const QString& prefix, const QVariant& defaultValue )
	{
		addMapping( w, fieldIndex( fieldName, prefix ), defaultValue );
	}
	//-------------------------------------------------------------------------
	void Project::addMapping( QWidget* w, int fieldIndex, const QVariant& defaultValue )
	{
		Q_ASSERT( fieldIndex != -1 && "Invalid field index!" );

		if( !_defaultValues.contains( fieldIndex ) && defaultValue.isValid() )
			_defaultValues.insert( fieldIndex, defaultValue );

		_mapper->addMapping( w, fieldIndex );
	}
	//-------------------------------------------------------------------------
	void Project::addMapping( QRadioButton* radios[ 2 ], const QString& fieldName,
							  const QString& prefix, bool defaultValue )
	{
		addMapping( radios, fieldIndex( fieldName, prefix ), defaultValue );
	}
	//-------------------------------------------------------------------------
	void Project::addMapping( QRadioButton* radios[ 2 ], int fieldIndex, bool defaultValue )
	{
		Q_ASSERT( fieldIndex != -1 && "Invalid field index" );

		if( !_defaultValues.contains( fieldIndex ) )
			_defaultValues.insert( fieldIndex, defaultValue );

		BinaryRadioWidget* radioMapper = new BinaryRadioWidget( radios );
		_mapper->addMapping( radioMapper, fieldIndex );
	}
	//-------------------------------------------------------------------------
	int Project::fieldIndex( const QString& fieldName, const QString& prefix ) const
	{
		QString name = prefix + fieldName;
		return _fields.value( name, -1 );
	}
	//-------------------------------------------------------------------------
	QVariant Project::value( int recordIndex, const QString& fieldName,
							 const QString& prefix ) const
	{
		return value( recordIndex, fieldIndex( fieldName, prefix ) );
	}
	//-------------------------------------------------------------------------
	QVariant Project::value( int recordIndex, int fieldIndex ) const
	{
		Q_ASSERT( fieldIndex != -1 && "Invalid field index" );
		QStandardItem* i = _model->item( recordIndex, fieldIndex );
		if( i )
			return i->data( Qt::DisplayRole );

		return QVariant();
	}
	//-------------------------------------------------------------------------
	void Project::setValue( const QVariant& value, int recordIndex,
							const QString& fieldName, const QString& prefix )
	{
		setValue( value, recordIndex, fieldIndex( fieldName, prefix ) );
	}
	//-------------------------------------------------------------------------
	void Project::setValue( const QVariant& value, int recordIndex, int fieldIndex )
	{
		Q_ASSERT( fieldIndex != -1 && "Invalid field index" );
		QStandardItem* i = _model->item( recordIndex, fieldIndex );
		if( !i )
		{
			i = new QStandardItem();
			_model->setItem( recordIndex, fieldIndex, i );
		}

		i->setData( value.isValid() ? value : _defaultValues.value( fieldIndex ),
					Qt::DisplayRole
		);
	}
	//-------------------------------------------------------------------------
	QVariant Project::currentValue( const QString& fieldName, const QString& prefix ) const
	{
		return value( _mapper->currentIndex(), fieldIndex( fieldName, prefix ) );
	}
	//-------------------------------------------------------------------------
	QVariant Project::currentValue( int fieldIndex ) const
	{
		return value( _mapper->currentIndex(), fieldIndex );
	}
	//-------------------------------------------------------------------------
	void Project::setCurrentValue( const QVariant& value, const QString& fieldName,
								   const QString& prefix )
	{
		setValue( value, _mapper->currentIndex(), fieldIndex( fieldName, prefix ) );
	}
	//-------------------------------------------------------------------------
	void Project::setCurrentValue( const QVariant& value, int fieldIndex )
	{
		setValue( value, _mapper->currentIndex(), fieldIndex );
	}
	//-------------------------------------------------------------------------
	int Project::recordCount() const
	{
		return _model->rowCount();
	}
	//-------------------------------------------------------------------------
	Graph* Project::graph( int index ) const
	{
		if( index >= 0 && index < _graphs.count() )
			return _graphs[ index ];

		return 0;
	}
	//-------------------------------------------------------------------------
	Graph* Project::currentGraph() const
	{
		int idx = _mapper->currentIndex();
		if( idx == -1 )
		{
			_mapper->toFirst();
			idx = 0;
		}

		return graph( idx );
	}
	//-------------------------------------------------------------------------
	QStringList Project::nameFilters( FilesModel::FileGroup g )
	{
		QStringList result;
		QString fieldNames[ 4 ] =
		{
			"parseEnabled",
			"standardFiltersEnabled",
			"currentStandardFilter",
			"customFilters"
		};

		const int count = g == FilesModel::All ? 2 : 1;
		int init = 0;
		if( g == FilesModel::Src )
			init = 1;

		for( int i = init; i < count; i++ )
		{
			QString prefix = ( (FilesModel::FileGroup)i ) == FilesModel::Hdr ? "hdr_" : "src_";
			if( !currentValue( fieldNames[ 0 ], prefix ).toBool() )
				continue;

			if( currentValue( fieldNames[ 1 ], prefix ).toBool() )
				result << currentValue( fieldNames[ 2 ], prefix ).toString();
			else
			{
				result += currentValue( fieldNames[ 3 ], prefix ).toString()
						.replace( ' ', "" ).split( ";" );
			}
		}

		return result;
	}
	//-------------------------------------------------------------------------
	bool Project::load()
	{
		QFile f( _fullPath );
		if( !f.open( QIODevice::ReadOnly ) )
			return false;

		QByteArray compressedData = f.readAll();
		f.close();

		QDataStream stream( qUncompress( compressedData ) );
		int fileMagic;
		stream >> fileMagic;
		if( fileMagic != MAGIC )
		{
			QMessageBox::warning(
						static_cast<QWidget*>( parent() ),
						tr( "Load Project" ),
						tr( "It seems you're trying to load an invalid project file;\nloading aborted!" )
			);
			return false;
		}

		stream >> _version;
		if( _version < LATEST_VER )
		{
			//TODO
			//Warn about old file format and/or upgrade to latest version
		}

		int rows;
		stream >> rows;

		_delegate->disableConnections( true );
		for( int r = 0; r < rows; r++ )
		{
			for( int c = 0; c < _model->columnCount(); c++ )
			{
				bool skip;
				stream >> skip;
				if( skip )
					continue;

				QStandardItem* i = new QStandardItem();
				i->read( stream );
				if( c == 0 )
					_newGraph( i );
				else
					_model->setItem( r, c, i );
			}
		}
		_delegate->disableConnections( false );
		emit graphCountChanged( rows );

		return true;
	}
	//-------------------------------------------------------------------------
	bool Project::save()
	{
		return saveAs( _fullPath );
	}
	//-------------------------------------------------------------------------
	bool Project::saveAs( const QString& filePath )
	{
		_fullPath = filePath;
		if( _fullPath.isEmpty() )
		{
			QString filter = tr( "Project Files (*%1)" ).arg( _defaultExtension );
			_fullPath = QFileDialog::getSaveFileName(
							   static_cast<QWidget*>( parent() ),
							   tr( "Create a new project..." ),
							   QDir::homePath(),
							   filter,
							   &filter
			);

			if( !Helpers::addExtension( _fullPath, _defaultExtension ) )
			{
				_fullPath = "";
				_updateProjectProperties();
				return false;
			}
		}

		QFile f( _fullPath );
		if( f.open( QIODevice::WriteOnly ) )
		{
			QByteArray data;
			QDataStream stream( &data, QIODevice::WriteOnly );
			stream << MAGIC;
			stream << _version;
			stream << _model->rowCount();

			for( int r = 0; r < _model->rowCount(); r++ )
			{
				for( int c = 0; c < _model->columnCount(); c++ )
				{
					QStandardItem* i = _model->item( r, c );
					if( i )
					{
						stream << false;
						i->write( stream );
					}
					else
						stream << true;
				}
			}
			f.write( qCompress( data, 9 ) );
			f.close();

			_triggerModified( false );
		}

		_updateProjectProperties();
		return !_modified;
	}
	//-------------------------------------------------------------------------
	void Project::createGraph()
	{
		_newGraph();

		//Set default values
		foreach( int k, _defaultValues.keys() )
			setValue( QVariant(), _model->rowCount() - 1, k );

		_triggerModified( true );
	}
	//-------------------------------------------------------------------------
	void Project::removeGraph( int index )
	{
		_model->removeRow( index );
		_graphs.remove( index );
		emit graphRemoved( index );
		emit graphCountChanged( _model->rowCount() );
		_triggerModified( true );
	}
	//-------------------------------------------------------------------------
	void Project::renameGraph( int index, const QString& newName )
	{
		setValue( newName, index, "name" );
		emit graphRenamed( index, newName );
		_triggerModified( true );
	}
	//-------------------------------------------------------------------------
	void Project::submitChanges()
	{
		if( !_hasUnsubmittedChanges )
			return;

		if( !_mapper->submit() )
		{
			//TODO WARNING MESSAGE
			qDebug() << "Can't submit!";
			return;
		}

		_triggerUnsubmittedChanges( false );
		_triggerModified( true );
	}
	//-------------------------------------------------------------------------
	void Project::revertChanges()
	{
		if( !_hasUnsubmittedChanges )
			return;

		_mapper->revert();
		_triggerUnsubmittedChanges( false );
	}
	//-------------------------------------------------------------------------
	void Project::_onDataChanged()
	{
		_triggerUnsubmittedChanges( true );
	}
	//-------------------------------------------------------------------------
	void Project::_updateProjectProperties()
	{
		if( !_fullPath.isEmpty() )
		{
			QFileInfo f( _fullPath );
			_name = f.baseName();
			_path = f.absolutePath();
		}
		else
			_name = "Unnamed";
	}
	//-------------------------------------------------------------------------
	void Project::_triggerModified( bool value )
	{
		_modified = value;
		emit modified( value );
	}
	//-------------------------------------------------------------------------
	void Project::_triggerUnsubmittedChanges( bool value )
	{
		_hasUnsubmittedChanges = value;
		emit unsubmittedChanges( value );
	}
	//-------------------------------------------------------------------------
	void Project::_newGraph( QStandardItem* i )
	{
		Graph* g = new Graph( static_cast<QWidget*>( parent() ) );
		_graphs << g;

		QString graphName;
		bool triggerGraphCountChangedSignal = false;

		if( !i )
		{
			graphName = QString( tr( "New Graph %1" ) ).arg( QString::number( _graphs.count() ) );
			i = new QStandardItem( graphName );
			triggerGraphCountChangedSignal = true;
		}
		else
			graphName = i->data( Qt::DisplayRole ).toString();

		_model->appendRow( i );
		emit graphCreated( graphName, g );

		if( triggerGraphCountChangedSignal )
			emit graphCountChanged( _model->rowCount() );
	}
}
