//-------------------------------------------------------------------
//  File:   IOFile.cpp
//  Created:  07/28/00 12:08:PM
//  Author:   Aaron Hilton
//  Comments:
//-------------------------------------------------------------------
#include "std.h"
#include "IOFile.h"

//-----------------------------------
// Constructor/destructors...
//-----------------------------------
IOFile::IOFile()
{
  m_pFile = 0;
  m_bEOF = true;
}

IOFile::~IOFile()
{Toast();}

bool IOFile::Create( char *szFilename, char *szMode )
{
  Toast();  // Nuke any old stuff.

  m_pFile = fopen( szFilename, szMode );

  if( m_pFile )
  {
    m_bEOF = false; // Reset EOF flag, 'cause all is well in file land.
    return true;
  }

  // Didn't create the file object.
  return false;
}

void IOFile::Toast()
{
  if( m_pFile )
  {
    fclose( m_pFile );
    m_pFile = 0;
    m_bEOF = true; // No more file data, aka EOF.
  }
}

// Gets the current file position.
Int32 IOFile::GetPosition()
{
  return ftell( m_pFile );
} 

// Read a chunk of data.
bool IOFile::Read( void *buffer, UInt32 length )
{
  if( m_pFile == 0 ) return false;
  if( fread( buffer, length, 1, m_pFile ) == 1 )
  { // Returning with no problem.
    return true;
  } else
  { // Find out if we failed to read due to EOF condition.
    if( feof( m_pFile ) != 0 )
      m_bEOF = true;
    else
      m_bEOF = false;

    // Returning with file reading failure.
    return false;
  }

  // Some compilers are just plain stupid... :)
  //  They don't detect that all branches already return a value.
  return true;
}

// Write a chunk of data.
bool IOFile::Write( void *buffer, UInt32 length )
{
  if( m_pFile == 0 ) return false;
  if( fwrite( buffer, length, 1, m_pFile ) == 1 )
  { // Returning with no problem.
    return true;
  }

  // Returning with file writing failure.
  return false;
}

bool IOFile::SeekFromStart( Int32 length )
{
  if( fseek( m_pFile, length, SEEK_SET ) == 0 )
    return true;

  return false;
}

bool IOFile::SeekFromHere( Int32 length )
{
  if( fseek( m_pFile, length, SEEK_CUR ) == 0 )
    return true;

  return false;
}

bool IOFile::SeekFromEnd( Int32 length )
{
  if( fseek( m_pFile, length, SEEK_END ) == 0 )
    return true;

  return false;
}

//-------------------------------------------------------------------
//  History:
// - Original Release for the OpenGL Challenge.
//-------------------------------------------------------------------


