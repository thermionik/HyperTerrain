//-------------------------------------------------------------------
//  File:     IOFile.h
//  Created:  07/28/00 11:29:AM
//  Author:   Aaron Hilton
//  Comments: Generic IOFile class for accessing files
//            in different forms.  Should support detected
//            pack files as directories.
//-------------------------------------------------------------------
#ifndef __IOFile_h_
#define __IOFile_h_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IOFile
{
public:
	IOFile();
  ~IOFile();

  // Create a file object for use.
  // -  szMode uses the same syntax as stdio fopen.
  // Usage: myFile.Create( "Nifty.dat", "rb" );
  // Opens the file "Nifty.dat" in read only & binary mode.
  bool Create( char *szFilename, char *szMode );
  void Toast();

  // Standard binary IO routines.
  bool Read( void *buffer, UInt32 length );
  bool Write( void *buffer, UInt32 length );

  // Gets the current file position.
  Int32 GetPosition();

  // Usual file position move functions.
  bool SeekFromStart( Int32 length );
  bool SeekFromHere( Int32 length );
  bool SeekFromEnd( Int32 length );

  // End of File status.
  bool IsEOF() { return m_bEOF; }

  // Read from file opperators.
  // Usage example:
  // myFile >> nWidth;  // nWidth is a UInt32 variable.
  IOFile& operator>>(UInt8 &n  ) { Read( &n, sizeof(UInt8 ) ); return *this; }
  IOFile& operator>>(UInt16 &n ) { Read( &n, sizeof(UInt16) ); return *this; }
  IOFile& operator>>(UInt32 &n ) { Read( &n, sizeof(UInt32) ); return *this; }
  IOFile& operator>>(Int8 &n  ) { Read( &n, sizeof(Int8 ) ); return *this; }
  IOFile& operator>>(Int16 &n ) { Read( &n, sizeof(Int16) ); return *this; }
  IOFile& operator>>(Int32 &n ) { Read( &n, sizeof(Int32) ); return *this; }

  // Write to file opperators.
  // Usage example:
  // myFile << nWidth;  // nWidth is a UInt32 variable.
  IOFile& operator<<(UInt8 &n  ) { Write( &n, sizeof(UInt8 ) ); return *this; }
  IOFile& operator<<(UInt16 &n ) { Write( &n, sizeof(UInt16) ); return *this; }
  IOFile& operator<<(UInt32 &n ) { Write( &n, sizeof(UInt32) ); return *this; }
  IOFile& operator<<(Int8 &n  ) { Write( &n, sizeof(Int8 ) ); return *this; }
  IOFile& operator<<(Int16 &n ) { Write( &n, sizeof(Int16) ); return *this; }
  IOFile& operator<<(Int32 &n ) { Write( &n, sizeof(Int32) ); return *this; }

protected:
  FILE *m_pFile;
  bool m_bEOF;
};


#endif // __IOFile_h_
//-------------------------------------------------------------------
//  History:
// - Original Release for the OpenGL Challenge.
//-------------------------------------------------------------------
