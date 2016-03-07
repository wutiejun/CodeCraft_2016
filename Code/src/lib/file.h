/*****************************************************************************/
/* FILE structure                                                            */
/*****************************************************************************/
#define OS_FILE      FILE

/*****************************************************************************/
/* File IO functions                                                         */
/*****************************************************************************/
#define OS_FREAD(DEST, SIZE, NUM, FILE)    fread((DEST), (SIZE), (NUM), (FILE))
#define OS_FEOF(FILE)                      feof(FILE)
#define OS_FCLOSE(FILE)                    fclose(FILE)
#define OS_STDIN                           stdin
#define OS_STDOUT                          stdout

#define OS_FOPEN(FILE, MODE)                fopen((FILE), (MODE))
#define OS_REMOVE(FILE)                     remove(FILE)
#define OS_RENAME(OLDNAME, NEWNAME)         rename((OLDNAME), (NEWNAME))
#define OS_SETVBUF(STREAM, BUF, TYPE, SIZE)                                  \
                                       setvbuf((STREAM), (BUF), (TYPE), (SIZE))


#define OS_FGETS(DEST, LENGTH, FILE)        fgets((DEST), (LENGTH), (FILE))
#define OS_FFLUSH(FILE)                     fflush(FILE)
#define OS_FWRITE(DT, B, LEN, FILE)         fwrite(DT, B, LEN, FILE)
#define OS_FSEEK(FILE, NUM, QUAL)           fseek(FILE, NUM, QUAL)
#define OS_FTELL(FILE)                      ftell(FILE)


typedef struct nbb_ffile
{
  /***************************************************************************/
  /* File name and size.                                                     */
  /***************************************************************************/
  unsigned char base_name[256];
  int max_size;

#if 0//def WIN32
  /***************************************************************************/
  /* File handle and type.                                                   */
  /***************************************************************************/
  void *fh;
  unsigned char file_type;

#else
  /***************************************************************************/
  /* File handle.                                                            */
  /***************************************************************************/
  OS_FILE *fp;

  /***************************************************************************/
  /* String indicating the mode for opening this file.                       */
  /***************************************************************************/
  unsigned char mode[3];
#endif

} NBB_FFILE;

