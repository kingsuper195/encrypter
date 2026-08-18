#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>

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
    // Default Arguments
    struct arguments arguments;
    arguments.verbose = 0;
    arguments.output_file = "";
    arguments.input_file = "";
    arguments.input_text = "";
    arguments.alphabet = "abcdefghijklmnopqrstuvwxyz";

    // Use getopt to find out which args are used
    int opt;
    while ((opt = getopt(argc, argv, "f:o:t:a:h")) != -1)
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

        // Custom alphabet
        case 'a':
            arguments.alphabet = optarg;
            break;

        case '?':
            break;
        default:
            printf("?? getopt returned character code 0%o ??\n", opt);
        }
    }
    // If the getopt function searched every index
    if (optind >= argc)
    {
        printf("A key is required. Run \"encrypter -h\" for more.\n");
        return EIO;
    }

    // Set the key
    arguments.args[0] = argv[optind++];

    // If there are extra perams
    if (optind < argc)
    {
        printf("Unknown extra input: ");
        while (optind < argc)
        {
            printf("%s ", argv[optind++]);
        }
        printf("\nRun \"encrypter -h\" for more.\n");
        return EIO;
    }

    // If no input data was provided
    if (strlen(arguments.input_file) == 0 && strlen(arguments.input_text) == 0)
    {
        printf("Please set either an input file or an input text. Run \"encrypter -h\" for more.\n");
        return EIO;
    }

    // If both a string and a file were provided for input
    if (strlen(arguments.input_file) > 0 && strlen(arguments.input_text) > 0)
    {
        printf("Please set either one of an input file or an input text, not both. Run \"encrypter -h\" for more.\n");
        return EIO;
    }

    // Initalize the input text, based on either the file or the string
    char *inp;
    if (strlen(arguments.input_text) > 0)
    {
        // A string was provided
        inp = (char *)malloc((strlen(arguments.input_text) + 1) * sizeof(char));
        if (inp == NULL)
        {
            printf("Memory Error!!\n");
            return errno;
        }
        strcpy(inp, arguments.input_text);
    }
    else
    {
        // A file was provided
        FILE *input_file;
        input_file = fopen(arguments.input_file, "r");
        if (input_file == NULL)
        {
            printf("The input file could not be opened.\n");
            return errno;
        }
        // Find length of file
        fseek(input_file, 0L, SEEK_END);
        int sz = ftell(input_file);
        rewind(input_file);

        // Set inp to the file's data
        inp = (char *)malloc((sz + 1) * sizeof(char));
        inp[0] = '\0';
        if (inp == NULL)
        {
            printf("Memory Error!!\n");
            return errno;
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
    }

    // Get the length of inp and key
    int inp_len = strlen(inp);
    int key_len = strlen(arguments.args[0]);
    char *key;

    // Repeat the key if it is too short for the text
    if (inp_len > key_len)
    {
        int len = (int)ceil((double)inp_len / (double)key_len);
        key = (char *)malloc((len * key_len + 1) * sizeof(char));
        if (key == NULL)
        {
            int malerrno = errno;
            printf("Memory Error!!\n");
            free(inp);
            return malerrno;
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
            int malerrno = errno;
            printf("Memory Error!!\n");
            free(inp);
            return malerrno;
        }
        strcpy(key, arguments.args[0]);
    }

    // Initialize the encrypted variable
    char *encrypted = (char *)malloc((inp_len + 1) * sizeof(char));
    if (encrypted == NULL)
    {
        int malerrno = errno;
        printf("Memory Error!!\n");
        free(inp);
        free(key);
        return malerrno;
    }
    int keychar = 0;

    for (int i = 0; i < inp_len; i++)
    {
        // If the character is not in the alphabet, copy it directly to the str, then continue on.
        if (strchr(arguments.alphabet, tolower(inp[i])) == NULL)
        {
            encrypted[i] = inp[i];
            continue;
        }
        // Record whether or not char is uppercase
        int upper = isupper(inp[i]);

        // Get the indexes of the inp and key chars in alphabet
        long int inp_id = (long int)(strchr(arguments.alphabet, tolower(inp[i])) - arguments.alphabet);
        long int key_id = (long int)(strchr(arguments.alphabet, tolower(key[keychar])) - arguments.alphabet);

        // Add them togther
        long int inttmp = inp_id + key_id;

        // Account for overflow
        while (inttmp >= strlen(arguments.alphabet))
        {
            inttmp -= strlen(arguments.alphabet);
        }

        // Append to str
        char tmp = arguments.alphabet[inttmp];
        if (upper)
            tmp = toupper(tmp);
        encrypted[i] = tmp;
        keychar++;
    }

    // Add a NUL str terminator
    encrypted[inp_len] = '\0';

    // If there is an output file given, write to it, else write to stdout
    if (strlen(arguments.output_file) > 0)
    {
        FILE *output_file;
        output_file = fopen(arguments.output_file, "w");
        if (output_file == NULL)
        {
            printf("The output file could not be opened.\n");
            return errno;
        }
        for (int i = 0; encrypted[i] != '\0'; i++)
        {
            fputc(encrypted[i], output_file);
        }
        fclose(output_file);
    }
    else
    {
        printf("%s\n", encrypted);
    }

    // Free alloc'd stuffs.
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