#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

int printHelp(void);
// const char *argp_program_version = "encrypter 0.1";
// static char doc[] =
//     "Encrypter - A C-based command line program to encode strings with the vigenere cipher.";

// static char args_doc[] = "KEY";

// static struct argp_option options[] =
//     {
//         {"file", 'f', "FILE", 0, "Input File (not to be used with --text)"},
//         {"output", 'o', "FILE", 0, "Output File"},
//         {"text", 't', "TEXT", 0, "Input Text (not to be used with --file)"},
//         {"verbose", 'v', 0, 0, "Produce verbose output"},
//         {0}};

struct arguments
{
    char *args[1];
    int verbose;
    char *output_file;
    char *input_file;
    char *input_text;
};

// static error_t parse_opt(int key, char *arg, struct argp_state *state)
// {
//     struct arguments *arguments = state->input;

//     switch (key)
//     {
//     case 'v':
//         arguments->verbose = 1;
//         break;
//     case 'o':
//         arguments->output_file = arg;
//         break;
//     case 't':
//         arguments->input_text = arg;
//         break;
//     case 'f':
//         arguments->input_file = arg;
//         break;
//     case ARGP_KEY_ARG:
//         if (state->arg_num >= 2)
//             /* Too many arguments. */
//             argp_usage(state);

//         arguments->args[state->arg_num] = arg;

//         break;

//     case ARGP_KEY_END:
//         if (state->arg_num < 2)
//             /* Not enough arguments. */
//             argp_usage(state);
//         break;

//     default:
//         return ARGP_ERR_UNKNOWN;
//     }
//     return 0;
// }

// static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char *argv[])
{
    struct arguments arguments;
    arguments.verbose = 0;
    arguments.output_file = "";
    arguments.input_file = "";
    arguments.input_text = "";

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

    printf("KEY = %s\nOUTPUT_FILE = %s\nINPUT_FILE = %s\nINPUT_TEXT = %s\n",
           arguments.args[0],
           arguments.output_file,
           arguments.input_file,
           arguments.input_text);
    return 0;
}

int printHelp(void)
{
    printf("Usage: encrypter [OPTION...] KEY\n");
    printf("Encrypter - A C-based command line program to encode strings with the vigenere cipher.\n");
    printf("-f FILE,            Input File (not to be used with -t)\n");
    printf("-o FILE,            Output File\n");
    printf("-t TEXT,            Input Text (not to be used with -f)\n");
    return 0;
}