#ifndef PAMAN_H
#define PAMAN_H

/*! 
 *  Define ASCII range
*/
#define ASCII_MAX       128
#define CIPHER_KEY      ASCII_MAX+42        /*!< Define cipher key */
#define PASS_LEN        16                  /*!< Define Max. password lenght */
#define VERSION         "0.1.0"             /*!< Define paman version */
#define FILENAME        ".paman_database"   /*!< Define paman database file name */

char* cipher(char* buf);
void  put_file(FILE* fp, FILE* stream);
int   read_file(FILE* fp, char** buf);
void  insert(FILE* fp, char* str);
int   find(FILE* fp, char* str);
char* rand_ps(void);

/*! 
 *  Prints the program version on stdout
*/
#define print_version()     puts("paman version "VERSION)
#define print_help()        puts("usage: paman [-a <site>:<username>]\n \
            [-c <filename>]\n \
            [-q <search-query>]\n \
            [-e] [-h] [-l] [-r] [-v]")
            /*!< Prints program help on stdout */

#endif