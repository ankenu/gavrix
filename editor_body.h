#ifndef __EDITOR_BODY_H__
#define __EDITOR_BODY_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/wait.h>

#define TRUE    1
#define FALSE   0

enum    command_error
{
    OK,
    SUCCESS_WRITE,
    NO_COMMAND,
    NO_ARG,
    NO_FILE,
    EMPTY_FILE,
    NO_NAME,
    NO_SAVE
};

enum    command_name
{
    EXIT,
    OPEN,
    READ,
    WRITE,
    SET,
    PRINT,
    EDIT,
    INSERT,
    DELETE,
    REPLACE,
    HELP,
    TERM_OPEN,
    INSERT_SPEC,
    INSERT_REP
};

struct   argument_type
{
    char* str;
    int   num;
};

typedef struct
{
    enum    command_name   name;
    struct  argument_type* arg;
    unsigned int           count_arg;
    enum    command_error  error;
    char*                  str_original;
}command_body;

typedef struct type_node
{
    struct type_node*   next;
    struct type_node*   prev;
    char*               str;
    int                 num_row;
}node;

typedef struct 
{
    node*   head;
    node*   tail;
    char*   file_link;
    int     file_numeration;
    int     file_wrap;
    int     file_tabwidth;
    int     edit_indicator;
}main_node;

typedef struct
{
    int     indicator;
    int     print_indicator;
    int     print_command;
    int     print_supernum;
    int     print_transfer_wrap;
    int     print_str_place_end;
    int     print_max_str_place;
    int     print_str_place_start;
    node*   plist;
    node*   pliststart;
}resize_s;


/*Глобальные переменные*/
char*           logo[] = {" _____                      _       ", "|  __ \\                    (_)      ", "| |  \\/  __ _ __   __ _ __  _ __  __", "| | __  / _` |\\ \\ / /| '__|| |\\ \\/ /", "| |_\\ \\| (_| | \\ V / | |   | | >  < ", " \\____/ \\__,_|  \\_/  |_|   |_|/_/\\_\\ "};
char*           help_str[] = {"[HELP]", "I. Text View Commands", "1. set wrap", "This command takes the \"yes\" or \"no\" argument as input and switches the display mode between \"long lines are truncated\" and \"long lines continue\". By default, the \"long lines continue\" mode is set.", "2. set numbers", "This command enables the numbering of the displayed lines. The set numbers yes command includes numbers. The set numbers no command disables. By default, numbers are disabled.", "3. set tabwidth", "This command adjusts the number of spaces used in the display. The default value is 4.", "4. print pages", "The command to view the entire text (from top to bottom and from left to right). View the next screen by pressing the space bar. Exit the command by pressing 'q' or by running out of lines in the text.", "5. print range", "Use this command to view a range of rows. If the second number in the range is not specified, the output is considered to be from the line number specified by the first number to the end of the text. Example command: print range 10 20", "II. Command To Insert Rows", "1. insert after", "Inserts a row or group of rows after the Nth. First, the string number is specified (the format is similar to the previous group of commands), and then in double quotes the inserted string itself. If the number is omitted, it is inserted at the end of the text. If N is 0, it is at the beginning of the text.", "III. String Editing Commands", "1. edit string", "Allows you to replace a character in the N-th line, starting from the M-th position; after specifying the positions, the replaced character follows. Example command: edit string 2 25 k will replace the character in the second line at position 25 the symbol 'k'.", "2. insert symbol", "The command is similar to edit string, but it inserts the character in the desired position, rather than replacing it. If there is an \"exit\" for the range to the right-then insert at the end; if for the range to the left-then insert at the beginning.", "3. replace substring", "The command searches for and replaces substrings in a range of strings. The command name is followed by a range of lines, just as for the print range command (if the range is not specified, then search and replace are performed throughout the text), then two lines in double quotes, possibly each on a new line. The first one is interpreted as a sample string that will be found in the text. The following is interpreted as a string to be replaced in the text. Instead of the sample string, the character '^' may be present-this means that the string should be assign to the beginning of each row from the range, or the '$' character-this means that assign to the end of each row from the range.", "4. delete range", "Removes the row. Specifies a range, similar to the print range command. At the same time the first the value cannot be skipped and cannot be equal to 0.", "5. delete comments", "Must delete all comments in the text. As an argument, the comment type can be passed: pascal, c, c++, shell.", "IV. Technical Commands", "1. exit", "Exits the text editor. If the text is not saved to a file, it is displayed warning and exit is not carried out. If you still need to exit, then enter the exit force command.", "2. read", "Opens the file, and if successful, replaces all the lines of text with it. The file name is not remembered for later saving the edited text to a file. If a name has already been associated with the text, it will not be affected by the command. The file name in the command is specified in double quotes. Example: read \"file.txt\"", "3. open", "Similar to read, except that the file name is remembered for later use save the edited text to it.", "4. write", "Saves the text to a file, if the file name is not specified, then the record occurs in the \"remembered\" file name. If the file name is not associated, an error occurs. If the file name was specified and the file was associated with the text, the association is not changed. If the association was empty, then the association from this team is taken.", "5. set name", "Specifies an association with the file name. As in the read command the association is set by a string in double quote. If the string is empty, the file association is deleted.", "[HELP END]"};
char*           cmd_all[] = {"exit", "open", "read", "write", "set", "print", "edit", "insert", "delete", "replace", "help"};
//char*         file_link;
char*           file_name;
main_node       main_list = {NULL,NULL,NULL,FALSE,FALSE,4, FALSE};
main_node*      list = &main_list;
main_node       help_list = {NULL,NULL,NULL,FALSE,TRUE,4, FALSE};
main_node*      help = &help_list;
resize_s        resize = {FALSE, FALSE, FALSE, 0, 0, 0, 0, 0, NULL, NULL};
struct winsize  w;

void    error_timer();
char*   quote_checked(char* str);
void    list_numeration_normalize();
void    command_free(command_body cmd_body);
char*   command_input(char* cmd);

int     error_analyzer(command_body cmd_body);

void    editor_exit(command_body cmd_body);
void    editor_open(command_body cmd_body);
void    editor_print(command_body cmd_body);
void    editor_help(command_body cmd_body);
void    editor_write(command_body cmd_body);
void    editor_set(command_body cmd_body);
void    editor_insert(command_body cmd_body);
void    editor_replace(command_body cmd_body);
void    editor_edit(command_body cmd_body);
void    editor_insert_symbol(command_body cmd_body);
void    editor_delete(command_body cmd_body);

void    command_line();
void    command_parser();
void    editor_start();

#endif /* __EDITOR_BODY_H__ */