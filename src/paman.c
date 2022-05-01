/*! 
 *  \brief     Password manager
 *  \details   Paman is a CLI tool to manage your passwords.
 *  \author    Julio Cesar Kochhann
 *  \version   0.1.0
 *  \date      Apr 2022
 *  \copyright MIT License
 */
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "paman.h"

int main(int argc, char* argv[])
{
    FILE* fp = fopen(FILENAME, "a+");
    assert(fp != NULL);

    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, ":a:c:dehls:v")) != -1) {
        switch (opt) {
            case 'a':           /*!< Insert credential at the end of file fp (append) */
                insert(fp, optarg);
                break;
            case 'c': ;         /*! Convert file <optarg> to cipher/text */
                FILE* cf = fopen(optarg, "r+");
                assert(cf != NULL);
                put_file(cf, cf);
                break;
            case 'd':           /*! Delete the database file */
                printf("Do you want to delete " FILENAME "? (Y/n): ");
                if (getchar() == 'Y') {
                    fclose(fp);
                    assert(!remove(FILENAME));
                    printf(FILENAME " deleted.\n");
                }
                break;
            case 'e': ;         /*!< Export the database file to plain text */
                FILE* of = fopen(FILENAME "_plain.txt", "w");
                assert(of != NULL);
                put_file(fp, of);
                break;
            case 'h':
                print_help();
                break;
            case 'l':           /*! List all credentials */
                put_file(fp, stdout);
                break;
            case 's':           /*! Search for credentials that contain \a optarg */
                search(fp, optarg);
                break;
            case 'v':
                print_version();
                break;
            case ':':
                fprintf (stderr, "option '-%c' requires an argument.\n", optopt);
                print_help();
                exit(EXIT_FAILURE);
                break;
            case '?':
                fprintf (stderr, "unknown option '-%c'.\n", optopt);
                print_help();
                exit(EXIT_FAILURE);
                break;
        }
    }

    if (argc == 1)
        print_help();

    exit(EXIT_SUCCESS);
}

/*! \fn void cipher(char* buf)
 *  \brief Encrypt or decrypt a string.
 *  \param buf a character pointer.
 *  \return void.
 */
void cipher(char* buf)
{
    while (*buf) {
        *buf ^= CIPHER_KEY;
        buf++;
    }
}

/*! \fn void put_file(FILE* fp, FILE* stream) 
 *  \brief Encrypt/decrypt a file and write it to a stream.
 *  \param fp a FILE pointer.
 *  \param stream a FILE pointer.
 *  \return void.
 */
void put_file(FILE* fp, FILE* stream)
{
    char* file;

    read_file(fp, &file);
    cipher(file);
    fputs(file, stream);
    free(file);
}

/*! \fn int read_file(FILE* fp, char** buf)
 *  \brief Read a file into a buffer.
 *  \param fp a FILE pointer.
 *  \param buf a pointer to a character pointer.
 *  \return an integer.
 */
int read_file(FILE* fp, char** buf)
{
    char c;
    int  n = 0;             // number of characters read
    int  buf_sz = 1024;

    assert(fp != NULL);

    (*buf) = (char *) malloc(buf_sz * sizeof(char));

    c = fgetc(fp);

    while (!feof(fp)) {
        if (n == buf_sz)
            (*buf) = (char *) realloc((*buf), (buf_sz *= 2) * sizeof(char));

        assert((*buf) != NULL);

        (*buf)[n++] = c;
        c = fgetc(fp);
    }

    (*buf)[n] = '\0';

    rewind(fp);
    
    return n;
}

/*! \fn void insert(FILE* fp, char* str)
 *  \brief Insert a credential into the database.
 *  \param fp a FILE pointer.
 *  \param str a character pointer.
 *  \exception credential must be unique.
 *  \return void.
 */
void insert(FILE* fp, char* str)
{
    char* domain   = strtok( str, ":" );
    char* username = strtok(NULL, "\0");
    char* password = rand_ps();

    if (username == NULL) {
        fprintf(stderr, "error: username not set.\n");
        print_help();
        exit(EXIT_FAILURE);
    }

    char* cred = (char *) malloc(strlen(domain) + strlen(username) + PASS_LEN + 4);

    sprintf(cred, "%s %s", domain, username);

    if ( search(fp, cred) ) {
        fprintf(stderr, "error: credential must be unique.\n");
        exit(EXIT_FAILURE);
    }

    sprintf(cred, "%s %s %s\n", domain, username, password);
    puts(password);
    cipher(cred);
    fprintf(fp, "%s", cred);
}

/*! \fn int search(FILE* fp, char* str)
 *  \brief Find credentials that contain the string.
 *  \param fp a FILE pointer.
 *  \param str a character pointer.
 *  \return an integer.
 */
int search(FILE* fp, char* str)
{
    char* file;
    char* temp;
    int   n = 0;        // number of matches found

    read_file(fp, &file);
    cipher(file);
    temp = file;

    file = strtok(file, "\n");          // Split file at newlines

    while(file != NULL) {
        if (strstr(file, str)) {
            puts(file);
            n++;
        }
        
        file = strtok(NULL, "\n");
    }

    free(temp);

    return n;
}

/*! \fn char* rand_ps(void)
 *  \brief Generate a string of random characters of PASS_LEN length.
 *  \return a character pointer to a static buffer.
 */
char* rand_ps(void)
{
    static char pswd[PASS_LEN+1];

    srand(time(0));     // Use current time as seed for random generator

    int c, i;
    for (i = 0; i < PASS_LEN; i++) {
        while (isgraph( (c = rand() % ASCII_MAX) ) == 0)
        ;
        pswd[i] = c;
    }

    pswd[i] = '\0';

    return pswd;
}