/*
 * Copyright (C) 1998-2001, The University of Queensland
 * Copyright (C) 2001, Sun Microsystems, Inc
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

/* File: ExeBinaryFile.h
 * Desc: This file contains the definition of the class ExeBinaryFile.
*/

/* $Revision: 1.5 $
 * This file contains the definition of the ExeBinaryFile class, and some other
    definitions specific to the exe version of the BinaryFile object/
   At present, there is no support for a symbol table. Exe files do
    not use dynamic linking, but it is possible that some files may
    have debug symbols (in Microsoft Codeview or Borland formats),
    and these may be implemented in the future. The debug info may
    even be exposed as another pseudo section 
 * 16 Mar 01 - Mike: Added the needed definitions of new virtual functions
 * 17 Mar 01 - Mike: Removed EXE_RELOCATION (no longer fake a PSP)
 * 16 Apr 01 - Brian: Moved definition of the LH macro to BinaryFile.h.
*/

#ifndef __EXEBINARYFILE_H__
#define __EXEBINARYFILE_H_

#include <stdint.h>
#include "BinaryFile.h"

typedef struct {            /*        PSP structure                 */
    uint16_t int20h;           /* interrupt 20h                        */
    uint16_t eof;              /* segment, end of allocation block     */
    uint8_t  res1;              /* reserved                             */
    uint8_t  dosDisp[5];        /* far call to DOS function dispatcher  */
    uint8_t  int22h[4];         /* vector for terminate routine         */
    uint8_t  int23h[4];         /* vector for ctrl+break routine        */
    uint8_t  int24h[4];         /* vector for error routine             */
    uint8_t  res2[22];          /* reserved                             */
    uint16_t segEnv;           /* segment address of environment block */
    uint8_t  res3[34];          /* reserved                             */
    uint8_t  int21h[6];         /* opcode for int21h and far return     */
    uint8_t  res4[6];           /* reserved                             */
    uint8_t  fcb1[16];          /* default file control block 1         */
    uint8_t  fcb2[16];          /* default file control block 2         */
    uint8_t  res5[4];           /* reserved                             */
    uint8_t  cmdTail[0x80];     /* command tail and disk transfer area  */
} PSP;

typedef struct {            /*      EXE file header          */
     uint8_t    sigLo;          /* .EXE signature: 0x4D 0x5A     */
     uint8_t    sigHi;
     uint16_t  lastPageSize;   /* Size of the last page         */
     uint16_t  numPages;       /* Number of pages in the file   */
     uint16_t  numReloc;       /* Number of relocation items    */
     uint16_t  numParaHeader;  /* # of paragraphs in the header */
     uint16_t  minAlloc;       /* Minimum number of paragraphs  */
     uint16_t  maxAlloc;       /* Maximum number of paragraphs  */
     uint16_t  initSS;         /* Segment displacement of stack */
     uint16_t  initSP;         /* Contents of SP at entry       */
     uint16_t  checkSum;       /* Complemented checksum         */
     uint16_t  initIP;         /* Contents of IP at entry       */
     uint16_t  initCS;         /* Segment displacement of code  */
     uint16_t  relocTabOffset; /* Relocation table offset       */
     uint16_t  overlayNum;     /* Overlay number                */
} exeHeader;

class ExeBinaryFile : public BinaryFile
{
public:
                ExeBinaryFile();                // Default constructor
  virtual void  UnLoad();                       // Unload the image
  virtual bool  Open(const char* sName);        // Open the file for r/w; pv
  virtual void  Close();                        // Close file opened with Open()
  virtual bool  PostLoad(void* handle);         // For archive files only
  virtual LOAD_FMT GetFormat() const;           // Get format (i.e. LOADFMT_EXE)
  virtual MACHINE GetMachine() const;           // Get machine (i.e. MACHINE_PENTIUM)
  virtual const char *getFilename() const { return m_pFileName; }

  virtual bool isLibrary() const;
  virtual std::list<const char *> getDependencyList();
  virtual ADDRESS getImageBase();
  virtual size_t getImageSize();

  virtual char* SymbolByAddr(ADDRESS a);

    // Analysis functions
    virtual std::list<SectionInfo*>& GetEntryPoints(const char* pEntry = "main");
    virtual ADDRESS GetMainEntryPoint();
    virtual ADDRESS GetEntryPoint();

//
//  --  --  --  --  --  --  --  --  --  --  --
//
                // Internal information
    // Dump headers, etc
virtual bool    DisplayDetails(const char* fileName, FILE* f = stdout);

  protected:
    virtual bool  RealLoad(const char* sName); // Load the file; pure virtual
private:


    exeHeader*  m_pHeader;               // Pointer to header
    uint8_t *   m_pImage;                   // Pointer to image
    int         m_cbImage;                  // Size of image
    int         m_cReloc;                   // Number of relocation entries
    uint32_t*   m_pRelocTable;              // The relocation table
    const char* m_pFileName;
};

#endif      // ifndef __EXEBINARYFILE_H__
