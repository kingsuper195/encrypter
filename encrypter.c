#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


int printHelp(void);

struct arguments
{
    char *args[1];
    int verbose;
    char *output_file;
    char *input_file;
    char *input_text;
    char *alphabet;
};

int main(int argc, char *argv[])
{
    struct arguments arguments;
    arguments.verbose = 0;
    arguments.output_file = "";
    arguments.input_file = "";
    arguments.input_text = "";
    arguments.alphabet = "abcdefghijklmnopqrstuvwxyz";

    int opt;
    while ((opt = getopt(argc, argv, "i:o:t:h")) != -1)
    {
        switch (opt)
        {

        // Help option
        case 'h':
            printHelp();
            return 0;

        // Input file
        case 'i':
            arguments.input_file = optarg;
            break;

        // Output file
        case 'o':
            arguments.output_file = optarg;
            break;

        // Input text
        case 't':
            arguments.input_text = optarg;
            break;
        case '?':
            break;
        default:
            printf("?? getopt returned character code 0%o ??\n", opt);
        }
    }
    if (optind >= argc)
    {
        printf("A key is required. Run \"encrypter --help\" for more.");
        return 1;
    }
    arguments.args[0] = argv[optind++];

    if (optind < argc)
    {
        printf("Unknown extra input: ");
        while (optind < argc)
        {
            printf("%s ", argv[optind++]);
        }
        printf("\nRun \"encrypter --help\" for more.");
        return 1;
    }

    if (strlen(arguments.input_file) == 0 && strlen(arguments.input_text) == 0)
    {
        printf("Please set either an input file or an input text. Run \"encrypter --help\" for more.");
        return 1;
    }

    if (strlen(arguments.input_file) > 0 && strlen(arguments.input_text) > 0)
    {
        printf("Please set either one of an input file or an input text, not both. Run \"encrypter --help\" for more.");
        return 1;
    }

    int intext_len = strlen(arguments.input_text);
    int key_len = strlen(arguments.args[0]);
    char *key;
    if (intext_len > key_len)
    {
        int len = (int)ceil((double)intext_len / (double)key_len);
        key = (char *)malloc((len * key_len + 1) * sizeof(char));
        if (key == NULL)
            return 1;
        key[0] = '\0';
        do
        {
            strcat(key, arguments.args[0]);
            len--;
        } while (len > 0);
    }
    else
    {
        key = (char *)malloc((key_len + 1) * sizeof(char));
        strcpy(key, arguments.args[0]);
    }
    char *encrypted = (char *)malloc((intext_len + 1) * sizeof(char));

    int keychar = 0;
    for(int i = 0; i<intext_len;i++) {
        if(strchr(arguments.alphabet, tolower(arguments.input_text[i])) == NULL) {
            encrypted[i] = arguments.input_text[i];
            continue;
        }
        int upper = isupper(arguments.input_text[i]);
        int tmp = tolower(arguments.input_text[i]) + tolower(key[keychar])-'a';
        while(tmp > 'z') {
            tmp -= 'z'-'a'+1;
        }
        if(upper) tmp = toupper(tmp);
        // printf("%c + %c = %c \n", tolower(arguments.input_text[i]), tolower(key[i]),tmp);
        encrypted[i]= tmp;
        keychar++;
    }
    encrypted[intext_len+1] = '\0';

    printf("%s + %s = %s", arguments.input_text, key, encrypted);
    free(key);
    return 0;
}

int printHelp(void)
{
    printf("Usage: encrypter [OPTION...] KEY\n");
    printf("Encrypter - A C-based command line program to encode strings with the vigenere cipher.\n");
    printf("-f FILE,            Input File (not to be used with -t)\n");
    printf("-o FILE,            Output File\n");
    printf("-t TEXT,            Input Text (not to be used with -f)\n");
    printf("-a LETTERS,         Override alphabet (letters that are encoded)\n\n");
    printf("Text from either -f or -t is encoded with KEY and sent to stdout or the -o option. Letters are all converted to lower case, and are skipped if not in alphbet. The default alphabet includes only a-z.\n");
    return 0;
}