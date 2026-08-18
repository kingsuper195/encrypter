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
    while ((opt = getopt(argc, argv, "f:o:t:h")) != -1)
    {
        switch (opt)
        {

        // Help option
        case 'h':
            printHelp();
            return 0;

        // Input file
        case 'f':
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
    char *inp;
    if (strlen(arguments.input_text) > 0)
    {
        inp = (char *)malloc((strlen(arguments.input_text) + 1) * sizeof(char));
        if (inp == NULL)
        {
            printf("Memory Error!!\n");
            return 1;
        }
        strcpy(inp, arguments.input_text);
    }
    else
    {
        FILE *input_file;
        input_file = fopen(arguments.input_file, "r");
        if (input_file == NULL)
        {
            printf("The input file could not be opened.\n");
            return 2;
        }
        fseek(input_file, 0L, SEEK_END);
        int sz = ftell(input_file);
        rewind(input_file);
        inp = (char *)malloc((sz + 1) * sizeof(char));
        inp[0] = '\0';
        if (inp == NULL)
        {
            printf("Memory Error!!\n");
            return 1;
        }
        do
        {
            // Taking input single character at a time
            char c = fgetc(input_file);

            // Checking for end of file
            if (feof(input_file))
                break;

            sprintf(inp + strlen(inp), "%c", c);
        } while (1);
        fclose(input_file);
        // return 0;
    }
    int inp_len = strlen(inp);
    int key_len = strlen(arguments.args[0]);
    char *key;
    if (inp_len > key_len)
    {
        int len = (int)ceil((double)inp_len / (double)key_len);
        key = (char *)malloc((len * key_len + 1) * sizeof(char));
        if (key == NULL)
        {
            printf("Memory Error!!\n");
            return 1;
        }
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
        if (key == NULL)
        {
            printf("Memory Error!!\n");
            return 1;
        }
        strcpy(key, arguments.args[0]);
    }
    char *encrypted = (char *)malloc((inp_len + 1) * sizeof(char));
    if (encrypted == NULL)
    {
        printf("Memory Error!!\n");
        return 1;
    }
    int keychar = 0;
    for (int i = 0; i < inp_len; i++)
    {
        if (strchr(arguments.alphabet, tolower(inp[i])) == NULL)
        {
            encrypted[i] = inp[i];
            continue;
        }
        int upper = isupper(inp[i]);
        int tmp = tolower(inp[i]) + tolower(key[keychar]) - 'a';
        while (tmp > 'z')
        {
            tmp -= 'z' - 'a' + 1;
        }
        if (upper)
            tmp = toupper(tmp);
        // printf("%c + %c = %c \n", tolower(inp[i]), tolower(key[i]),tmp);
        encrypted[i] = tmp;
        keychar++;
    }
    encrypted[inp_len] = '\0';
    if (strlen(arguments.output_file) > 0)
    {
        FILE *output_file;
        output_file = fopen(arguments.output_file, "w");
        if (output_file == NULL)
        {
            printf("The output file could not be opened.\n");
            return 2;
        }
        for (int i = 0; encrypted[i] != '\0'; i++) {
            fputc(encrypted[i],output_file);
        }
        fclose(output_file);
    }
    else
    {
        printf("%s", encrypted);
    }
    free(inp);
    free(key);
    free(encrypted);
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