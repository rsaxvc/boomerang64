/*
 * Copyright (C) 2000, The University of Queensland
 * Copyright (C) 2001, Sun Microsystems, Inc
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

/* File: DOS4GWBinaryFile.h
 * Desc: This file contains the definition of the class DOS4GWBinaryFile.
*/

#ifndef __DOS4GWBINARYFILE_H__
#define __DOS4GWBINARYFILE_H_

#include "BinaryFile.h"
#include <string>
#include <stdint.h>

/* $Revision: 1.1 $
 * This file contains the definition of the DOS4GWBinaryFile class, and some
 * other definitions specific to the exe version of the BinaryFile object
*/
/* At present, this loader supports the OS2 file format (also known as
 * the Linear eXecutable format) as much as I've found necessary to 
 * inspect old DOS4GW apps.  This loader could also be used for decompiling
 * Win9x VxD files or, of course, OS2 binaries, but you're probably better off
 * making a specific loader for each of these.
 * 24 Jan 05 - Trent: created.
 */

// Given a little endian value x, load its value assuming little endian order
// Note: must be able to take address of x
// Note: Unlike the LH macro in BinaryFile.h, the paraeter is not a pointer
#define LMMH(x) ((unsigned)((uint8_t  *)(&x))[0] + ((unsigned)((uint8_t  *)(&x))[1] << 8) + \
	((unsigned)((uint8_t  *)(&x))[2] << 16) + ((unsigned)((uint8_t  *)(&x))[3] << 24))
// With this one, x IS a pounsigneder
#define LMMH2(x) ((unsigned)((uint8_t  *)(x))[0] + ((unsigned)((uint8_t  *)(x))[1] << 8) + \
	((unsigned)((uint8_t  *)(x))[2] << 16) + ((unsigned)((uint8_t  *)(x))[3] << 24))
#define LMMHw(x) ((unsigned)((uint8_t  *)(&x))[0] + ((unsigned)((uint8_t  *)(&x))[1] << 8))


typedef struct {				/* exe file header, just the signature really */
		 uint8_t 	sigLo;			/* .EXE signature: 0x4D 0x5A	 */
		 uint8_t 	sigHi;
} Header;

//#ifdef WIN32
#pragma pack(1)
//#endif

typedef struct {
  uint8_t  sigLo;
  uint8_t  sigHi;
  uint8_t  byteord;
  uint8_t  wordord;
  uint32_t formatlvl;
  uint16_t cputype;
  uint16_t ostype;
  uint32_t modulever;
  uint32_t moduleflags;
  uint32_t modulenumpages;
  uint32_t eipobjectnum;
  uint32_t eip;
  uint32_t espobjectnum;
  uint32_t esp;
  uint32_t pagesize;
  uint32_t pageoffsetshift;
  uint32_t fixupsectionsize;
  uint32_t fixupsectionchksum;
  uint32_t loadersectionsize;
  uint32_t loadersectionchksum;
  uint32_t objtbloffset;
  uint32_t numobjsinmodule;
  uint32_t objpagetbloffset;
  uint32_t objiterpagesoffset;
  uint32_t resourcetbloffset;
  uint32_t numresourcetblentries;
  uint32_t residentnametbloffset;
  uint32_t entrytbloffset;
  uint32_t moduledirectivesoffset;
  uint32_t nummoduledirectives;
  uint32_t fixuppagetbloffset;
  uint32_t fixuprecordtbloffset;
  uint32_t importtbloffset;
  uint32_t numimportmoduleentries;
  uint32_t importproctbloffset;
  uint32_t perpagechksumoffset;
  uint32_t datapagesoffset;
  uint32_t numpreloadpages;
  uint32_t nonresnametbloffset;
  uint32_t nonresnametbllen;
  uint32_t nonresnametblchksum;
  uint32_t autodsobjectnum;
  uint32_t debuginfooffset;
  uint32_t debuginfolen;
  uint32_t numinstancepreload;
  uint32_t numinstancedemand;
  uint32_t heapsize;
} LXHeader;

typedef struct {
  uint32_t VirtualSize;
  uint32_t RelocBaseAddr;
  uint32_t ObjectFlags;
  uint32_t PageTblIdx;
  uint32_t NumPageTblEntries;
  uint32_t Reserved1;
} LXObject;

typedef struct {
  uint32_t pagedataoffset;
  uint16_t datasize;
  uint16_t flags;
} LXPage;

// this is correct for internal fixups only
typedef struct {
    unsigned char src;
    unsigned char flags;
    short srcoff;
//    unsigned char object;         // these are now variable length
//    unsigned short trgoff;
} LXFixup;

//#ifdef WIN32
#pragma pack(4)
//#endif

class DOS4GWBinaryFile : public BinaryFile
{
public:
				DOS4GWBinaryFile();				// Default constructor
  virtual		~DOS4GWBinaryFile();				// Destructor
  virtual bool	Open(const char* sName);		// Open the file for r/w; ???
  virtual void	Close();						// Close file opened with Open()
  virtual void	UnLoad();						// Unload the image
  virtual LOAD_FMT GetFormat() const;			// Get format (i.e.
												// LOADFMT_DOS4GW)
  virtual MACHINE GetMachine() const;			// Get machine (i.e.
												// MACHINE_Pentium)
  virtual const char *getFilename() const { return m_pFileName; }
  virtual bool isLibrary() const;
  virtual std::list<const char *> getDependencyList();
  virtual ADDRESS getImageBase();
  virtual size_t getImageSize();

  virtual std::list<SectionInfo*>& GetEntryPoints(const char* pEntry = "main");
  virtual ADDRESS GetMainEntryPoint();
  virtual ADDRESS GetEntryPoint();
  uint32_t getDelta();
  virtual const char* SymbolByAddress(ADDRESS dwAddr); // Get sym from addr
  virtual ADDRESS GetAddressByName(const char* name,
	bool bNoTypeOK = false);					// Find addr given name	 
  virtual void AddSymbol(ADDRESS uNative, const char *pName);

//
//		--		--		--		--		--		--		--		--		--
//
		// Internal information
		// Dump headers, etc
virtual bool	DisplayDetails(const char* fileName, FILE* f = stdout);

protected:

		int dos4gwRead2(short *ps) const; // Read 2 bytes from native addr
		int dos4gwRead4(int *pi) const;	 // Read 4 bytes from native addr

public:

virtual int readNative1(ADDRESS a);         // Read 1 bytes from native addr
virtual int readNative2(ADDRESS a);			// Read 2 bytes from native addr
virtual int readNative4(ADDRESS a);			// Read 4 bytes from native addr
virtual QWord readNative8(ADDRESS a);	// Read 8 bytes from native addr
virtual float readNativeFloat4(ADDRESS a);	// Read 4 bytes as float
virtual double readNativeFloat8(ADDRESS a); // Read 8 bytes as float

virtual bool	IsDynamicLinkedProcPointer(ADDRESS uNative);
virtual bool	IsDynamicLinkedProc(ADDRESS uNative);
virtual const char *GetDynamicProcName(ADDRESS uNative);

	virtual std::map<ADDRESS, std::string> &getSymbols() { return dlprocptrs; }

  protected:
	virtual bool  RealLoad(const char* sName); // Load the file; pure virtual

  private:

		bool	PostLoad(void* handle); // Called after archive member loaded

		Header* m_pHeader;				// Pointer to header
		LXHeader* m_pLXHeader;			// Pointer to lx header
        LXObject* m_pLXObjects;         // Pointer to lx objects
        LXPage*   m_pLXPages;           // Pointer to lx pages
		int		m_cbImage;				// Size of image
		//int		m_cReloc;				// Number of relocation entries
		//uint32_t*	m_pRelocTable;			// The relocation table
		char *	base;					// Beginning of the loaded image
		// Map from address of dynamic pointers to library procedure names:
		std::map<ADDRESS, std::string> dlprocptrs;
		const char *m_pFileName;

};

//#ifdef WIN32
#pragma pack()
//#endif
#endif			// ifndef __DOS4GWBINARYFILE_H__
