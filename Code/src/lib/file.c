#include <base.h>
#include <file.h>

char new_ext[] = ".txt";
char bak_ext[] = ".bak";


/**PROC+**********************************************************************/
/* Name:      nbb_open                                                       */
/*                                                                           */
/* Purpose:   Open a flip file                                               */
/*                                                                           */
/* Returns:   TRUE if file opened successfully                               */
/*            FALSE if file open failed                                      */
/*                                                                           */
/* Params:    IN     ffp               - pointer to flip file control block  */
/*            IN     base_name         - base name (including path,          */
/*                                       excluding extension) of file        */
/*            IN     max_size          - maximum size of file in bytes       */
/*            IN     file_type         - one of NBB_TEXT_FILE or             */
/*                                       NBB_BINARY_FILE                     */
/*                                                                           */
/* Operation: Open both trace files for write access                         */
/*                                                                           */
/**PROC-**********************************************************************/

int nbb_open(NBB_FFILE *ffp,
                  char *base_name,
                  int max_size
                  )
{
  /***************************************************************************/
  /* open a flip file for write and append operations                        */
  /***************************************************************************/
  char file_name[256];
  int open_ok;
  int rc;

  /***************************************************************************/
  /* NBB_TRC_ENTRY("nbb_open");                                              */
  /* DO NOT add any tracing in this routine                                  */
  /***************************************************************************/
  open_ok = TRUE;

  /***************************************************************************/
  /* initialize the flip file control block                                  */
  /***************************************************************************/
  strcpy(ffp->base_name, base_name);
  ffp->max_size = max_size;

  /***************************************************************************/
  /* form the current file name and open it                                  */
  /***************************************************************************/
  strcpy(file_name, ffp->base_name);
  strcat(file_name, new_ext);

  /***************************************************************************/
  /* Set up the mode for opening this file.  We store this string and use it */
  /* in future attempts to open a file for this NBB_FFILE.                   */
  /***************************************************************************/
  ffp->mode[0] = 'w';
  ffp->mode[1] = 0;

  if ((ffp->fp = OS_FOPEN(file_name, ffp->mode)) == NULL)
  {
    /*************************************************************************/
    /* FLOW TRACING NOT REQUIRED    Reason: No tracing in this function.     */
    /*************************************************************************/
    /*************************************************************************/
    /* failed to open the file                                               */
    /*************************************************************************/
    open_ok = FALSE;
  }

  /***************************************************************************/
  /* NBB_TRC_EXIT();                                                         */
  /***************************************************************************/

  return(open_ok);

} /* nbb_open */

/**PROC+**********************************************************************/
/* Name:      nbb_write                                                      */
/*                                                                           */
/* Purpose:   Write a data record to a flip file, flipping it if necessary   */
/*                                                                           */
/* Returns:   Nothing                                                        */
/*                                                                           */
/* Params:    IN     data         - pointer to data record                   */
/*            IN     data_len     - data record length                       */
/*            IN     ffp          - pointer to flip file control block       */
/*            IN     allow_flip   - TRUE: flipping is allowed FALSE: must    */
/*                                follow on in same file                     */
/*                                                                           */
/* Operation: Extracts the current length of the file and checks for         */
/*            overflow.  If overflow has occured, the file is closed, the    */
/*            backup file removed, the current file renamed to be the        */
/*            backup, and the file reopened.  Then the end of the file is    */
/*            sought and the data record written.  Doesn't currently         */
/*            firewall against record sizes larger than max file size.       */
/*                                                                           */
/**PROC-**********************************************************************/

void nbb_write(void *data,
                   int data_len,
                   NBB_FFILE *ffp,
                   int allow_flip
                   )
{
  /***************************************************************************/
  /* write data to a flip file (flipping the file if necessary)              */
  /***************************************************************************/
  char new_file[256];
  char bak_file[256];
  char abort_string[288];
  int file_len;
  int rc;

  /***************************************************************************/
  /* NBB_TRC_ENTRY("nbb_write");                                             */
  /* Beware recursion! (do not delete this comment)                          */
  /***************************************************************************/

  /***************************************************************************/
  /* If the output file is not open, quit.                                   */
  /***************************************************************************/
  if (ffp->fp == NULL)
  {
    /*************************************************************************/
    /* FLOW TRACING NOT REQUIRED    Reason: No tracing in this function.     */
    /*************************************************************************/
    goto EXIT_LABEL;
  }

  /***************************************************************************/
  /* Write to the file.                                                      */
  /***************************************************************************/
  OS_FWRITE((const char *)data, 1, data_len, ffp->fp);

  /***************************************************************************/
  /* Only flush the data to disk in debug builds as doing this in a release  */
  /* build would reduce performance significantly.                           */
  /***************************************************************************/

EXIT_LABEL:
  /***************************************************************************/
  /* NBB_TRC_EXIT();                                                         */
  /* Beware recursion! (do not delete this comment)                          */
  /***************************************************************************/

  return;

} /* nbb_write */

/**PROC+**********************************************************************/
/* Name:      nbb_close                                                      */
/*                                                                           */
/* Purpose:   Close a flip file                                              */
/*                                                                           */
/* Returns:   Nothing                                                        */
/*                                                                           */
/* Params:    IN     ffp               - pointer to flip file control block  */
/*                                                                           */
/* Operation: Simply closes the currently open file                          */
/*                                                                           */
/**PROC-**********************************************************************/

void nbb_close(NBB_FFILE *ffp  )
{
  /***************************************************************************/
  /* close a flip file                                                       */
  /***************************************************************************/
  NBB_TRC_ENTRY("nbb_close");

  OS_FCLOSE(ffp->fp);

  NBB_TRC_EXIT();

  return;

} /* nbb_close */

