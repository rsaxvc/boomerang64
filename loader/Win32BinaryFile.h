/*
 * Copyright (C) 2000, The University of Queensland
 * Copyright (C) 2001, Sun Microsystems, Inc
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

/* File: Win32BinaryFile.h
 * Desc: This file contains the definition of the class Win32BinaryFile.
*/

#ifndef __WIN32BINARYFILE_H__
#define __WIN32BINARYFILE_H_

#include "BinaryFile.h"
#include <string>
#include <stdint.h>

/* $Revision: 1.20 $
 * This file contains the definition of the Win32BinaryFile class, and some
 * other definitions specific to the exe version of the BinaryFile object
*/
/* At present, there is no support for a symbol table. Win32 files do
		not use dynamic linking, but it is possible that some files may
		have debug symbols (in Microsoft Codeview or Borland formats),
		and these may be implemented in the future. The debug info may
		even be exposed as another pseudo section
 * 02 Jun 00 - Mike: Added LMMH for 32 bit endianness conversions
 * 16 Apr 01 - Brian: Removed redefinition of the LH macro. LH is now
 *			   defined in BinaryFile.h.
 */

// Given a little endian value x, load its value assuming little endian order
// Note: must be able to take address of x
// Note: Unlike the LH macro in BinaryFile.h, the parameter is not a pointer
#define LMMH(x) ((unsigned)((uint8_t *)(&x))[0] + ((unsigned)((uint8_t *)(&x))[1] << 8) + \
	((unsigned)((uint8_t *)(&x))[2] << 16) + ((unsigned)((uint8_t *)(&x))[3] << 24))
// With this one, x is a pointer to unsigned
#define LMMH2(x) ((unsigned)((uint8_t *)(x))[0] + ((unsigned)((uint8_t *)(x))[1] << 8) + \
	((unsigned)((uint8_t *)(x))[2] << 16) + ((unsigned)((uint8_t *)(x))[3] << 24))

typedef struct {				/* exe file header, just the signature really */
		 uint8_t	sigLo;			/* .EXE signature: 0x4D 0x5A	 */
		 uint8_t	sigHi;
} Header;

//#ifdef WIN32
#pragma pack(1)
//#endif

typedef struct {
  uint8_t sigLo;
  uint8_t sigHi;
  uint16_t sigver;
  uint16_t cputype;
  uint16_t numObjects;
  uint32_t TimeDate;
  uint32_t Reserved1;
  uint32_t Reserved2;
  uint16_t NtHdrSize;
  uint16_t Flags;
  uint16_t Reserved3;
  uint8_t  LMajor;
  uint8_t  LMinor;
  uint32_t Reserved4;
  uint32_t Reserved5;
  uint32_t Reserved6;
  uint32_t EntrypointRVA;
  uint32_t Reserved7;
  uint32_t Reserved8;
  uint32_t Imagebase;
  uint32_t ObjectAlign;
  uint32_t FileAlign;
  uint16_t OSMajor;
  uint16_t OSMinor;
  uint16_t UserMajor;
  uint16_t UserMinor;
  uint16_t SubsysMajor;
  uint16_t SubsysMinor;
  uint32_t Reserved9;
  uint32_t ImageSize;
  uint32_t HeaderSize;
  uint32_t FileChecksum;
  uint16_t Subsystem;
  uint16_t DLLFlags;
  uint32_t StackReserveSize;
  uint32_t StackCommitSize;
  uint32_t HeapReserveSize;
  uint32_t HeapCommitSize;
  uint32_t Reserved10;
  uint32_t nInterestingRVASizes;
  uint32_t ExportTableRVA;
  uint32_t TotalExportDataSize;
  uint32_t ImportTableRVA;
  uint32_t TotalImportDataSize;
  uint32_t ResourceTableRVA;
  uint32_t TotalResourceDataSize;
  uint32_t ExceptionTableRVA;
  uint32_t TotalExceptionDataSize;
  uint32_t SecurityTableRVA;
  uint32_t TotalSecurityDataSize;
  uint32_t FixupTableRVA;
  uint32_t TotalFixupDataSize;
  uint32_t DebugTableRVA;
  uint32_t TotalDebugDirectories;
  uint32_t ImageDescriptionRVA;
  uint32_t TotalDescriptionSize;
  uint32_t MachineSpecificRVA;
  uint32_t MachineSpecificSize;
  uint32_t ThreadLocalStorageRVA;
  uint32_t TotalTLSSize;
} PEHeader;

typedef struct {	// The real Win32 name of this struct is IMAGE_SECTION_HEADER
  char ObjectName[8];	// Name
  uint32_t VirtualSize;
  uint32_t RVA;			// VirtualAddress
  uint32_t PhysicalSize;	// SizeOfRawData
  uint32_t PhysicalOffset;	// PointerToRawData
  uint32_t Reserved1;		// PointerToRelocations
  uint32_t Reserved2;		// PointerToLinenumbers
  uint32_t Reserved3;		// WORD NumberOfRelocations; WORD NumberOfLinenumbers;
  uint32_t Flags;			// Characteristics
} PEObject;

typedef struct {
	uint32_t originalFirstThunk; // 0 for end of array; also ptr to hintNameArray
	uint32_t preSnapDate;		// Time and date the import data was pre-snapped
							// or zero if not pre-snapped
	uint16_t verMajor;			// Major version number of dll being ref'd
	uint16_t verMinor;			// Minor "		 "
	uint32_t name;				// RVA of dll name (asciz)
	uint32_t firstThunk;		// RVA of start of import address table (IAT)
} PEImportDtor;

typedef struct {
	uint32_t	flags;			// Reserved; 0
	uint32_t	stamp;			// Time/date stamp export data was created
	uint16_t	verMajor;		// Version number can be ...
	uint16_t	verMinor;		//	 ... set by user
	uint32_t	name;			// RVA of the ascii string containing the name of
							//	 the DLL
	uint32_t	base;			// Starting ordinal number for exports in this
							//	image. Usually set to 1.
	uint32_t	numEatEntries;	// Number of entries in EAT (Export ADdress Table)
	uint32_t	numNptEntries;	// Number of entries in NPT (Name Pointer Table)
							//	(also #entries in the Ordinal Table)
	uint32_t	eatRVA;			// RVA of the EAT
	uint32_t	nptRVA;			// RVA of the NPT
	uint32_t	otRVA;			// RVA of the OT
} PEExportDtor;

//#ifdef WIN32
#pragma pack(4)
//#endif

class Win32BinaryFile : public BinaryFile
{
public:
					Win32BinaryFile();				// Default constructor
virtual				~Win32BinaryFile();				// Destructor
virtual bool		Open(const char* sName);		// Open the file for r/w; ???
virtual void		Close();						// Close file opened with Open()
virtual void		UnLoad();						// Unload the image
virtual LOAD_FMT	GetFormat() const;			// Get format (i.e.
												// LOADFMT_Win32)
virtual MACHINE		GetMachine() const;			// Get machine (i.e.
												// MACHINE_Pentium)
virtual const char*	getFilename() const { return m_pFileName; }
virtual bool		isLibrary() const;
virtual std::list<const char *> getDependencyList();
virtual ADDRESS		getImageBase();
virtual size_t		getImageSize();

virtual std::list<SectionInfo*>& GetEntryPoints(const char* pEntry = "main");
virtual ADDRESS		GetMainEntryPoint();
virtual ADDRESS		GetEntryPoint();
		uint32_t		getDelta();
virtual const char* SymbolByAddress(ADDRESS dwAddr); // Get sym from addr
virtual ADDRESS		GetAddressByName(const char* name, bool bNoTypeOK = false);		// Find addr given name	 
virtual void		AddSymbol(ADDRESS uNative, const char *pName);
		void		dumpSymbols();					// For debugging

//
//		--		--		--		--		--		--		--		--		--
//
		// Internal information
		// Dump headers, etc
virtual bool		DisplayDetails(const char* fileName, FILE* f = stdout);

protected:

		int			win32Read2(short *ps) const; // Read 2 bytes from native addr
		int			win32Read4(int *pi) const;	 // Read 4 bytes from native addr

public:

virtual int			readNative1(ADDRESS a);         // Read 1 bytes from native addr
virtual int			readNative2(ADDRESS a);			// Read 2 bytes from native addr
virtual int			readNative4(ADDRESS a);			// Read 4 bytes from native addr
virtual QWord		readNative8(ADDRESS a);		// Read 8 bytes from native addr
virtual float		readNativeFloat4(ADDRESS a);	// Read 4 bytes as float
virtual double		readNativeFloat8(ADDRESS a); // Read 8 bytes as float

virtual bool		IsDynamicLinkedProcPointer(ADDRESS uNative);
virtual bool		IsStaticLinkedLibProc(ADDRESS uNative);
virtual ADDRESS		IsJumpToAnotherAddr(ADDRESS uNative);
virtual const char *GetDynamicProcName(ADDRESS uNative);

        bool        IsMinGWsAllocStack(ADDRESS uNative);
		bool		IsMinGWsFrameInit(ADDRESS uNative);
		bool		IsMinGWsFrameEnd(ADDRESS uNative);
		bool		IsMinGWsCleanupSetup(ADDRESS uNative);
		bool		IsMinGWsMalloc(ADDRESS uNative);

virtual std::map<ADDRESS, std::string> &getSymbols() { return dlprocptrs; }

		bool		hasDebugInfo() { return haveDebugInfo; }

protected:
virtual bool  		RealLoad(const char* sName); // Load the file; pure virtual

private:

		bool		PostLoad(void* handle); // Called after archive member loaded
		void		findJumps(ADDRESS curr);// Find names for jumps to IATs

		Header* 	m_pHeader;				// Pointer to header
		PEHeader* 	m_pPEHeader;			// Pointer to pe header
		int			m_cbImage;				// Size of image
		int			m_cReloc;				// Number of relocation entries
		uint32_t*		m_pRelocTable;			// The relocation table
		char *		base;					// Beginning of the loaded image
		// Map from address of dynamic pointers to library procedure names:
		std::map<ADDRESS, std::string> dlprocptrs;
		const char	*m_pFileName;
		bool		haveDebugInfo;
        bool        mingw_main;

};

//#ifdef WIN32
#pragma pack()
//#endif
#endif			// ifndef __WIN32BINARYFILE_H__
