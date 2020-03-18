#include "editor_body.h"

/* Команды */
void   editor_exit(command_body cmd_body)
{
    node*   ptail;
    node*   prev_address;
    //не забыть очистить list->file_link;
    if (cmd_body.count_arg == 0)
    {
        if (list->edit_indicator == FALSE)
        {
            //очистить список файла
            if (list->tail != NULL && list->head != NULL)
            {
                ptail = list->head;
                while (ptail != NULL)
                {
                    prev_address = ptail;
                    ptail = ptail->next;
                    free (prev_address->str);
                    free (prev_address);
                }
                list->head = list->tail = NULL;
            }
            //очистить список help
            if (help->tail != NULL && help->head != NULL)
            {
                ptail = help->head;
                while (ptail != NULL)
                {
                    prev_address = ptail;
                    ptail = ptail->next;
                    free (prev_address);
                }
                help->head = help->tail = NULL;
            }
            //очистить имя файла
            if (list->file_link != NULL)
            {
                free (list->file_link);
            }
            //очистить cmd
            command_free(cmd_body);
            printf ("\e[2J\e[3J\e[r");
            printf ("Yeah!\n");
            exit (0);
        }
        else
            cmd_body.error = NO_SAVE;
    }
    else
    {
        if (cmd_body.count_arg == 1 && cmd_body.arg[0].str != NULL && !strcmp (cmd_body.arg[0].str,"force"))
        {
             //очистить список файла
            if (list->tail != NULL && list->head != NULL)
            {
                ptail = list->head;
                while (ptail != NULL)
                {
                    prev_address = ptail;
                    ptail = ptail->next;
                    free (prev_address->str);
                    free (prev_address);
                }
                list->head = list->tail = NULL;
            }
            //очистить список help
            if (help->tail != NULL && help->head != NULL)
            {
                ptail = help->head;
                while (ptail != NULL)
                {
                    prev_address = ptail;
                    ptail = ptail->next;
                    free (prev_address);
                }
                help->head = help->tail = NULL;
            }
            //очистить имя файла
            if (list->file_link != NULL)
            {
                free (list->file_link);
            }
            //очистить cmd
            command_free(cmd_body);
            printf ("\e[2J\e[3J\e[r");
            printf ("Yeah!\n");
            exit (0);
        }
        else
            cmd_body.error = NO_ARG;
    }
    error_analyzer (cmd_body);
}

void    command_free(command_body cmd_body)
{
    int i;

    i = 0;
    if (cmd_body.count_arg)
    {
        while (i < (int)cmd_body.count_arg)
        {
            if (cmd_body.arg[i].str != NULL)
                cmd_body.arg[i].str = NULL;
            i++;
        }
    }
    if (cmd_body.count_arg)
        free (cmd_body.arg);
    cmd_body.count_arg = 0;
    free (cmd_body.str_original);
}

int     transfer_changer(char* str)
{
    char*   p;
    int     count;

    count = 0;
    while ((p = strchr (str, '\n')))
    {
        str = &p[1];
        count++;
    }
    return (count);
}

void    editor_replace(command_body cmd_body)
{
    node*           plist;
    //node*   newlist;
    //node*   pnewlist;
    //char*   node_str;
    char*           found_str;
    char*           found_transfer;
    char*           replace_str_copy;
    int             find_str_size, replace_str_size, new_row_count, prev_row, end_str_size, symbol_indicator; //transfer_count, i, arg_transfer;
    //char*   pfind_str;
    command_body    new_cmd_body;
    char*           new_str;
    char*           end_str;
    char*           f_arg_str;
    char*           r_arg_str;
    //char*   transfer;

    plist = list->head;
    new_row_count = 0;
    symbol_indicator = -1;
    if (cmd_body.count_arg <=5 && cmd_body.count_arg >2)
    {
        if (cmd_body.arg[0].str && !strcmp (cmd_body.arg[0].str,"substring"))
        {
            if (plist != NULL)
            {
                if ((cmd_body.count_arg == 5 && cmd_body.arg[1].num && cmd_body.arg[2].num && cmd_body.arg[1].num <= cmd_body.arg[2].num && cmd_body.arg[2].num <= list->tail->num_row && cmd_body.arg[3].str && cmd_body.arg[4].str) || (cmd_body.count_arg == 3 && cmd_body.arg[1].str && cmd_body.arg[2].str))
                {
                    //Размеры
                    if (cmd_body.count_arg == 5)
                    {
                        find_str_size = strlen (cmd_body.arg[3].str);
                        f_arg_str = cmd_body.arg[3].str;
                        r_arg_str = cmd_body.arg[4].str;
                    }
                    else
                    {
                        find_str_size = strlen (cmd_body.arg[1].str);
                        f_arg_str = cmd_body.arg[1].str;
                        r_arg_str = cmd_body.arg[2].str;
                        cmd_body.arg[1].num = list->head->num_row;
                        cmd_body.arg[2].num = list->tail->num_row;
                    }
                    if (find_str_size != 1)
                    {
                        f_arg_str = quote_checked(f_arg_str);
                    }
                    else
                    {
                        if (strchr (f_arg_str, '^'))
                            symbol_indicator = TRUE;
                        else
                        {
                            if (strchr (f_arg_str, '$'))
                                symbol_indicator = FALSE;
                            else
                                cmd_body.error = NO_ARG;
                        }
                    }
                    r_arg_str = quote_checked(r_arg_str);
                    if (!error_analyzer(cmd_body))
                    {
                        if (f_arg_str && r_arg_str)
                        {
                            list->edit_indicator = TRUE;
                            find_str_size = strlen (f_arg_str);
                            replace_str_size = strlen (r_arg_str);
                            while (plist->num_row != cmd_body.arg[1].num)
                            {
                                plist = plist->next;
                            }
                            found_transfer = strchr (r_arg_str, '\n');
                            if (found_transfer)
                                new_row_count += transfer_changer(r_arg_str);
                            while (plist->num_row <= (cmd_body.arg[2].num))
                            {
                                if ((found_str = strstr (plist->str, f_arg_str)) || find_str_size == 1)
                                {
                                    if (find_str_size == 1)
                                    {
                                        if (symbol_indicator == TRUE)
                                            found_str = plist->str;
                                        if (symbol_indicator == FALSE)
                                            found_str = &(plist->str[strlen (plist->str) - 1]);
                                    }
                                    if (!found_transfer)
                                    {
                                        if (symbol_indicator != TRUE)
                                            *found_str = '\0';
                                        if (symbol_indicator == -1)
                                            new_str = (char*) malloc (strlen (plist->str) + replace_str_size + strlen (&found_str[find_str_size]) + 1);
                                        if (symbol_indicator == TRUE)
                                            new_str = (char*) malloc (strlen (plist->str) + replace_str_size + 1);
                                        if (symbol_indicator == FALSE)
                                            new_str = (char*) malloc (strlen (plist->str) + replace_str_size + 2);
                                        if (symbol_indicator != TRUE)
                                        {
                                            strcpy (new_str, plist->str);
                                            strcat (new_str, r_arg_str);
                                        }
                                        else
                                        {
                                            strcpy (new_str, r_arg_str);
                                            strcat (new_str, plist->str);
                                        }
                                        if (symbol_indicator == -1)
                                            strcat (new_str, &found_str[find_str_size]);
                                        if (symbol_indicator == FALSE)
                                            strcat (new_str, "\n");
                                        free (plist->str);
                                        plist->str = new_str;
                                    }
                                    else
                                    {
                                        replace_str_copy = (char*) malloc (replace_str_size + 1);
                                        strcpy (replace_str_copy, r_arg_str);
                                        found_transfer = strchr (replace_str_copy, '\n');
                                        cmd_body.arg[2].num += new_row_count;
                                        *found_transfer = '\0';
                                        if (symbol_indicator != TRUE)
                                            *found_str = '\0';
                                        if (symbol_indicator == -1)
                                            new_str = (char*) malloc (strlen (plist->str) + strlen (replace_str_copy) + 1);
                                        if (symbol_indicator == TRUE)
                                            new_str = (char*) malloc (strlen (replace_str_copy) + 1);
                                        if (symbol_indicator == FALSE)
                                            new_str = (char*) malloc (strlen (plist->str) + strlen (replace_str_copy) + 2);
                                        if (symbol_indicator != TRUE)
                                        {
                                            strcpy (new_str, plist->str);
                                            strcat (new_str, replace_str_copy);
                                        }
                                        else
                                            strcpy (new_str, replace_str_copy);
                                        if (symbol_indicator == FALSE)
                                            strcat (new_str, "\n");
                                        //insert after
                                        new_cmd_body.name = INSERT_REP;
                                        new_cmd_body.arg = (struct argument_type*) malloc (3 * sizeof (struct argument_type));
                                        new_cmd_body.arg[0].str = "after";
                                        new_cmd_body.arg[1].num = plist->num_row;
                                        //printf ("%d\n", plist->num_row);
                                        new_cmd_body.arg[1].str = NULL;
                                        new_cmd_body.arg[2].str = &found_transfer[1];
                                        new_cmd_body.count_arg = 3;
                                        new_cmd_body.error = OK;
                                        editor_insert(new_cmd_body);
                                        free (new_cmd_body.arg);
                                        if (symbol_indicator == -1)
                                        {
                                            end_str_size = strlen (&found_str[find_str_size]);
                                            end_str = (char*) malloc (end_str_size + 1);
                                            strcpy (end_str, &found_str[find_str_size]);
                                        }
                                        if (symbol_indicator == TRUE)
                                        {
                                            end_str_size = strlen (plist->str);
                                            end_str = (char*) malloc (end_str_size + 1);
                                            strcpy (end_str, plist->str);
                                        }
                                        free (plist->str);
                                        plist->str = new_str;
                                        prev_row = plist->num_row;
                                        while (plist->num_row != (prev_row + new_row_count))
                                            plist = plist->next;
                                        if (symbol_indicator != FALSE)
                                        {
                                            end_str_size += strlen (plist->str);
                                            plist->str = (char*) realloc (plist->str, end_str_size + 1);
                                            strcat (plist->str, end_str);
                                            free (end_str);
                                        }
                                        free (replace_str_copy);
                                    }
                                }
                                plist = plist->next;
                                if (!plist)
                                    break;
                            }
                        }
                        else
                            cmd_body.error = NO_ARG;
                    }
                    cmd_body.error = OK;
                }
                else
                    cmd_body.error = NO_ARG;
            }
            else
                cmd_body.error = EMPTY_FILE;
        }
        else
            cmd_body.error = NO_COMMAND;
    }
    else
        cmd_body.error = NO_ARG;
    error_analyzer (cmd_body);
}

int     num_structure(int num)
{
    int res, i;

    i = 1;
    res = num/10;
    while (res != 0)
    {
        res = res/10;
        i++;
    }
    return (i);
}

int     print_str(char* str, int row_num, int row_num_max, int str_place)
{
    int             str_size, i, j, num_size, term_col, term_col_variance;
    char*           str_num;
    char*           str_tab;
    //struct winsize  w;

    i = 0;
    //ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    //размер нумерации
    if (list->file_numeration == TRUE)
    {
        if (!str_place || list->file_wrap == FALSE)
            num_size = num_structure(row_num_max) - num_structure(row_num);
        else
            num_size = num_structure(row_num_max);
        str_num = (char*) malloc ((num_size + 1) * sizeof (char));
        while (i < num_size)
        {
            str_num[i] = ' ';
            i++;
        }
        str_num[i] = '\0';
    }
    //размер табуляции
    str_tab = (char*) malloc (((list->file_tabwidth) + 1) * sizeof (char));
    i = 0;
    while (i < (list->file_tabwidth))
    {
        str_tab[i] = ' ';
        i++;
    }
    str_tab[i] = '\0';
    //вывод строки
    str_size = strlen (str);
    if (list->file_numeration == TRUE)
    {
        if (!str_place || list->file_wrap == FALSE)
            printf ("\e[48;5;24m%s%d\e[0m", str_num, row_num);
        else
            printf ("\e[48;5;24m%s\e[0m", str_num);
    }
    else
    {
        if (!str_place || list->file_wrap == FALSE)
            printf ("\e[38;5;114m * \e[0m");
        else
            printf ("\e[38;5;114m   \e[0m");
    }
    i = str_place;
    term_col = w.ws_col - (num_structure(row_num_max) + 1);
    term_col_variance = term_col - str_size;
    printf (" ");
    if (term_col_variance < 0 && list->file_wrap == TRUE)
    {
        j = 1;
        while (j < term_col)
        {
            if (i <= str_size)
            {
                if (str[i] != '\t')
                {
                    putchar (str[i]);
                }
                else
                    printf ("%s", str_tab);
            }
            else
            {
                i = 0;
                break;
            }
            i++;
            j++;
        }
    }
    else
    {
        while (i < (term_col-1) + str_place && i <= str_size)
        {
            if (str[i] != '\t')
                putchar (str[i]);
            else
                printf ("%s", str_tab);
            i++;
        }
        if (list->file_wrap == FALSE)
            i = str_size - term_col;
        else
            i = 0;
    }
    if (list->file_numeration == TRUE)
        free (str_num);
    free (str_tab);
    return (i);
}

void    editor_print(command_body cmd_body)
{
    //struct winsize  w;
    struct termios  old_attributes,new_attributes;
    int             i, j, command, symbol, suprenum, str_place, transfer_wrap, max_str_place;
    main_node*      file_list;
    node*           plist;
    node*           pliststart;

    if (cmd_body.name == HELP)
    {
        file_list = list;
        list = help;
    }
    i = 1;
    transfer_wrap = 0;
    max_str_place = 0;
    j = TRUE;
    plist = list->head;
    //str_count = list->tail->num_row;
    //ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if (cmd_body.count_arg > 0 && cmd_body.count_arg <= 3 && cmd_body.arg[0].str != NULL)
    {
        if (cmd_body.count_arg == 1 && !strcmp (cmd_body.arg[0].str,"pages"))
        {
            resize.print_command = command = TRUE;
            if (plist != NULL)
                suprenum = list->tail->num_row;         
        }
        else
        {
            if (cmd_body.count_arg > 1 && cmd_body.count_arg <= 3 && cmd_body.arg[1].num)
            {
                if (!strcmp (cmd_body.arg[0].str,"range"))
                {
                    if (plist != NULL)
                    {
                        resize.print_command = command = FALSE;
                        if (cmd_body.arg[1].num <= list->tail->num_row)
                        {
                            while (plist->num_row != cmd_body.arg[1].num)
                                plist = plist->next;
                            if (cmd_body.count_arg == 2)
                                suprenum = list->tail->num_row;
                                else
                                {
                                    if (cmd_body.count_arg == 3 && cmd_body.arg[2].num && (cmd_body.arg[1].num <= cmd_body.arg[2].num))
                                        suprenum = cmd_body.arg[2].num;
                                    else
                                        cmd_body.error = NO_ARG;
                                }
                        }
                        else
                            cmd_body.error = NO_ARG;
                    }
                    else
                        cmd_body.error = EMPTY_FILE;              
                }
                else
                    if (cmd_body.count_arg == 1)
                        cmd_body.error = NO_COMMAND;
                    else
                        cmd_body.error = NO_ARG;
            }
            else
            {
                if (cmd_body.count_arg == 1 && strcmp (cmd_body.arg[0].str, "range"))
                    cmd_body.error = NO_COMMAND;
                else
                    cmd_body.error = NO_ARG;                    
            }
        }
        //терминальная магия
        if(!isatty (0))
        {
            printf ("Похоже вы перенаправили ввод не из терминала.\n"); 
        }   
        tcgetattr (0, &old_attributes);
        memcpy (&new_attributes, &old_attributes, sizeof (struct termios));
        new_attributes.c_lflag &= ~ECHO;
        new_attributes.c_lflag &= ~ICANON;
        new_attributes.c_cc[VMIN] = 1;
        tcsetattr (0, TCSANOW, &new_attributes);
        
        //Отображение одной страницы
        if (!error_analyzer(cmd_body))
        {
        printf ("\e[%d;0H\e[?25l\e[48;5;23mGavrix:\e[0m [- PAGES VIEW -]", w.ws_row-1);
        if (list->file_link == NULL)
            printf ("\e[%d;%dHNew file ", w.ws_row, w.ws_col-8);
        else
            printf ("\e[%d;%ldH%s ", w.ws_row, w.ws_col - strlen (list->file_link), list->file_link);
        str_place = 0;
        resize.print_supernum = suprenum;
        resize.print_indicator = TRUE;
        while (j == TRUE)
        {
            resize.print_str_place_start = str_place;
            resize.pliststart = resize.plist = pliststart = plist;
            printf ("\e[0;0H\e[2K");
            if (command == TRUE)
            {
                while ((i < w.ws_row-1) && (plist != NULL))
                {
                    printf ("\e[%d;0H\e[2K", i);
                    if (list->file_wrap == TRUE)
                        str_place = print_str(plist->str, plist->num_row, list->tail->num_row, str_place);
                    else
                    {
                        str_place = print_str(plist->str, plist->num_row, list->tail->num_row, transfer_wrap);
                        if (str_place > max_str_place)
                            resize.print_max_str_place = max_str_place = str_place;
                    }
                    if ((!str_place && list->file_wrap == TRUE) || list->file_wrap == FALSE)
                        resize.plist = plist = plist->next;
                    i++;
                }
            }
            if (command == FALSE)
            {    
                while ((i < w.ws_row-1) && (plist != NULL) && plist->num_row <= suprenum)
                {
                    printf ("\e[%d;0H\e[2K", i);
                    if (list->file_wrap == TRUE)
                        str_place = print_str(plist->str, plist->num_row, list->tail->num_row, str_place);
                    else
                    {
                        str_place = print_str(plist->str, plist->num_row, list->tail->num_row, transfer_wrap);
                        if (str_place > max_str_place)
                            resize.print_max_str_place = max_str_place = str_place;
                    }
                    if ((!str_place && list->file_wrap == TRUE) || list->file_wrap == FALSE)
                        resize.plist = plist = plist->next;
                    i++;
                }
            }
            while (i < w.ws_row-1)
            {
                printf ("\e[%d;0H\e[2K", i);
                printf ("\e[38;5;114m * \e[0m\n");
                i++;
            }
            if (plist == NULL || (plist->num_row > suprenum))
                j = FALSE;

            while((symbol=getchar ()) != EOF)
            {
                if (resize.indicator == TRUE)
                {
                    resize.indicator = FALSE;
                    plist = resize.plist;
                    str_place = resize.print_str_place_end;
                }
                if (symbol == ' ')
                {
                    i = 1;
                    resize.print_transfer_wrap = transfer_wrap = 0;
                    resize.print_max_str_place = max_str_place = 0;
                    break; 
                }
                if (symbol == 'q' || symbol == 'Q')
                {
                    j = FALSE;
                    break;
                }
                if (list->file_wrap == FALSE && symbol == 27)
                {
                    if ((symbol=getchar ()) == 91)
                    {
                        symbol=getchar ();
                        if (symbol == 67)
                        {
                            if (max_str_place > transfer_wrap)
                            {
                                i = 1;
                                j = TRUE;
                                resize.plist = plist = pliststart;
                                transfer_wrap++;
                                resize.print_transfer_wrap = transfer_wrap;
                                break;
                            }
                        }
                        if (symbol == 68)
                        {
                            i = 1;
                            j = TRUE;
                            resize.plist = plist = pliststart;
                            if (transfer_wrap)
                            {
                                transfer_wrap--;
                                resize.print_transfer_wrap = transfer_wrap;
                            }
                            break;
                        }
                    }
                }
            }
        }
        }
        tcsetattr (0, TCSANOW, &old_attributes);
        printf ("\e[?25H\e[0;0H");
        command_free(cmd_body);
        if (cmd_body.name == HELP)
            list = file_list;
        resize.print_indicator = FALSE;
        editor_start();
    }
    else
    {
        if (cmd_body.count_arg <= 1)
            cmd_body.error = NO_COMMAND;
        else 
            cmd_body.error = NO_ARG;
    }
    error_analyzer(cmd_body);
}

void    editor_help(command_body cmd_body)
{
    int     count_help_str, i;
    node*   ptail;

    i = 0;
    count_help_str = sizeof (help_str)/sizeof (help_str[0]);
    if (help->tail == NULL)
    {
        while (i < count_help_str)
        {
            if (help->tail != NULL)
                ptail = help->tail;
            help->tail = (node*) malloc (sizeof (node));
            help->tail->str = help_str[i];
            help->tail->num_row = i+1;
            if (help->head == NULL)
            {
                help->tail->prev = help->tail->next = NULL;
                help->head = help->tail;
            }
            else
            {
                help->tail->next = NULL;
                help->tail->prev = ptail;
                help->tail->prev->next = help->tail;
            }
            i++;
        }
    }
    cmd_body.error = OK;
    cmd_body.arg = (struct  argument_type*) malloc (sizeof (struct  argument_type));
    cmd_body.arg[0].str = "pages";
    cmd_body.count_arg = 1;
    editor_print(cmd_body);
}

char*   quote_checked(char* str)
{
    int str_size;

    str_size = strlen (str);
    if (str[0] == str[str_size-1] && str[str_size-1] == '"')
    {
        str[str_size-1] = '\0';
        return &str[1];
    }
    else
        return NULL;
}

void    list_numeration_normalize()
{
    node*   plist;
    int     i;

    plist = list->head;
    i = 1;
    while (plist != NULL)
    {
        plist->num_row = i;
        plist = plist->next;
        i++;
    }
}

void    editor_delete(command_body cmd_body)
{
    node*   plist;
    node*   prevlist;
    int     delete_end_num, delete_now_num;

    plist = list->head;
    if (cmd_body.arg[0].str)
    {
        if (!strcmp (cmd_body.arg[0].str, "range"))
        {  
            if (cmd_body.count_arg <= 3 && cmd_body.count_arg >= 2 && cmd_body.arg[1].num > 0)
            {
                if (plist != NULL)
                {
                    if (list->tail->num_row >= cmd_body.arg[1].num)
                    {
                        if (cmd_body.count_arg == 2)
                            delete_end_num = list->tail->num_row;
                        if (cmd_body.count_arg == 3 && cmd_body.arg[2].num > 0 && cmd_body.arg[2].num >= cmd_body.arg[1].num && (cmd_body.arg[2].num <= list->tail->num_row)) 
                            delete_end_num = cmd_body.arg[2].num;
                        else
                        {
                            if (cmd_body.count_arg == 3)
                                cmd_body.error = NO_ARG;
                        }
                        if (!error_analyzer(cmd_body))
                        {
                            list->edit_indicator = TRUE;
                            while (plist->num_row != cmd_body.arg[1].num)
                                plist = plist->next;
                            delete_now_num = plist->num_row;
                            while (delete_now_num <= delete_end_num)
                            {
                                prevlist = plist;
                                plist = plist->next;
                                if (plist != NULL)
                                    delete_now_num = plist->num_row;
                                else
                                    delete_now_num = delete_now_num + 1;
                                if (list->head == list->tail)
                                {
                                    list->head = list->tail = NULL;
                                    free (prevlist->str);
                                    free (prevlist);
                                }
                                else
                                {
                                    if (prevlist != list->head && prevlist != list->tail)
                                    {
                                        prevlist->prev->next = prevlist->next;
                                        prevlist->next->prev = prevlist->prev;
                                    }
                                    if (prevlist == list->head)
                                    {
                                        prevlist->next->prev = NULL;
                                        list->head = prevlist->next;
                                    }
                                    if (prevlist == list->tail)
                                    {
                                        prevlist->prev->next = NULL;
                                        list->tail = prevlist->prev;
                                    }
                                    free (prevlist->str);
                                    free (prevlist);
                                }                      
                            }
                            list_numeration_normalize();
                        }
                        cmd_body.error = OK;
                    }
                    else
                        cmd_body.error = NO_ARG;
                }
                else
                    cmd_body.error = EMPTY_FILE;   
            }
            else
                cmd_body.error = NO_ARG;
        }
        else
        {
            //comments
            if (!strcmp (cmd_body.arg[0].str, "comments"))
            {
                char*   symbol_str;
                char*   symbol_str2;
                int     indicator, i, quote_indicator, quote_count, block_on_indicator, str_size, str_size_now, j, old_i, old_str_size_now, transfer;
                char*   quote_array;
                char*   block_comment_on;
                char*   block_comment_off;
                char*   list_comment;
                char*   new_str;
                char    lang_quote;
                
                indicator = quote_indicator = FALSE;
                i = quote_count = 0;
                if (cmd_body.arg[1].str)
                {
                    if (plist != NULL)
                    {
                        cmd_body.error = NO_ARG;
                        if (!strcmp (cmd_body.arg[1].str, "pascal"))
                        {
                            block_comment_on = "{";
                            block_comment_off = "}";
                            list_comment = "//";
                            lang_quote = '\'';
                            cmd_body.error = OK;
                        }
                        if (!strcmp (cmd_body.arg[1].str, "c"))
                        {
                            block_comment_on = "/*";
                            block_comment_off = "*/";
                            list_comment = NULL;
                            lang_quote = '"';
                            cmd_body.error = OK;
                        }
                        if (!strcmp (cmd_body.arg[1].str, "c++"))
                        {
                            block_comment_on = NULL;
                            block_comment_off = NULL;
                            list_comment = "//";
                            lang_quote = '"';
                            cmd_body.error = OK;
                        }
                        if (!strcmp (cmd_body.arg[1].str, "shell"))
                        {
                            block_comment_on = NULL;
                            block_comment_off = NULL;
                            list_comment = "#";
                            lang_quote = '"';
                            cmd_body.error = OK;
                        }
                        if (!error_analyzer(cmd_body))
                        {
                            list->edit_indicator = TRUE;
                            //удаление комментария pascal        
                            while (plist != NULL)
                            {
                                symbol_str = strchr (plist->str, lang_quote);
                                if (symbol_str)
                                {
                                    block_on_indicator = indicator;
                                    // if ((symbol_str2 && indicator == TRUE) || (!symbol_str2 && indicator == FALSE))
                                    // {
                                        str_size = strlen (plist->str);
                                        quote_count = 0;
                                        quote_array = NULL;
                                        i = j = 0;
                                        while (plist->str[i] != '\0')
                                        {
                                            if (block_comment_off && block_on_indicator == TRUE && quote_indicator == FALSE && (&(plist->str[i]) == (strstr (&(plist->str[i]), block_comment_off))))
                                                block_on_indicator = FALSE;
                                            if (block_comment_on && block_on_indicator == FALSE && quote_indicator == FALSE && (&(plist->str[i]) == (strstr (&(plist->str[i]), block_comment_on))))
                                                block_on_indicator = TRUE;
                                            if (list_comment && block_on_indicator == FALSE && quote_indicator == FALSE && (&(plist->str[i]) == (strstr (&(plist->str[i]), list_comment))))
                                                break;
                                            if (plist->str[i] == lang_quote && block_on_indicator == FALSE)
                                            {
                                                quote_count++;
                                                if (!quote_array)
                                                    quote_array = (char*) malloc (str_size * sizeof (char));
                                                quote_array[quote_count-1] = (plist->str[i]);
                                                plist->str[i] = '\0';
                                                if (quote_indicator == TRUE)
                                                    quote_indicator = FALSE;
                                                else
                                                    quote_indicator = TRUE;
                                            }
                                            i++;
                                        }
                                        i = 1;
                                        if (quote_count)
                                            quote_array = (char*) realloc (quote_array, quote_count * sizeof(char));
                                        str_size_now = j = 0;
                                        while (i <= (quote_count + 1) && str_size > str_size_now)
                                        {
                                            str_size_now += strlen (&(plist->str[j]));
                                            if (i % 2 != 0)
                                            {
                                                if (list_comment != NULL)
                                                {
                                                    symbol_str = strstr (&(plist->str[j]), list_comment);
                                                    if (symbol_str)
                                                    {
                                                        *symbol_str = '\0';
                                                        str_size -= strlen (&symbol_str[1]) + 1;
                                                        plist->str = (char*) realloc (plist->str, (str_size + 2) * sizeof (char));
                                                        strcat (&(plist->str[j]), "\n");
                                                        break;
                                                    }
                                                }
                                                if (block_comment_on)
                                                    symbol_str = strstr (&(plist->str[j]), block_comment_on);
                                                if (indicator == FALSE && symbol_str)
                                                    indicator = TRUE;
                                                if (indicator == TRUE && block_comment_on && block_comment_off)
                                                {
                                                    symbol_str2 = strstr (&(plist->str[j]), block_comment_off);
                                                    if (symbol_str2 && symbol_str)
                                                    {
                                                        //{ и } на одной строке
                                                        *symbol_str = '\0';
                                                        if (!strcmp (cmd_body.arg[1].str, "c"))
                                                        {
                                                            symbol_str2[1] = '\0';
                                                            symbol_str2 = &symbol_str2[1];
                                                        }
                                                        else
                                                            *symbol_str2 = '\0';
                                                        str_size -= (strlen (&symbol_str[1]) + 2);
                                                        new_str = (char*) malloc (str_size + 1);
                                                        //strcpy (new_str, plist->str);
                                                        old_i = i;
                                                        old_str_size_now = str_size_now;
                                                        i = 0;
                                                        str_size_now = transfer = 0;
                                                        while (i < (quote_count+1))
                                                        {
                                                            strcpy (&new_str[transfer + i], &(plist->str[str_size_now + i]));
                                                            transfer += (strlen (&(plist->str[str_size_now + i])));
                                                            str_size_now += (strlen (&(plist->str[str_size_now + i])));
                                                            if (i == (old_i - 1))
                                                            {
                                                                strcat (&new_str[transfer + i], &symbol_str2[1]);
                                                                transfer += strlen (&symbol_str2[1]);
                                                                old_str_size_now = transfer;
                                                                str_size_now += strlen (&symbol_str2[1]) + strlen (&symbol_str[1]) + 2;
                                                            }
                                                            i++;
                                                        }
                                                        str_size = transfer + i;
                                                        //printf ("size %d\n", str_size);
                                                        i = old_i;
                                                        str_size_now = old_str_size_now;
                                                        free (plist->str);
                                                        plist->str = new_str;
                                                        indicator = FALSE;
                                                    }
                                                    else
                                                    {
                                                        if (symbol_str && !symbol_str2)
                                                        {
                                                            //только {
                                                            symbol_str2 = strchr (plist->str, '\n');
                                                            *symbol_str = '\0';
                                                            plist->str = realloc (plist->str, (strlen (plist->str) + 2) * sizeof (char));
                                                            strcat (plist->str, "\n");
                                                            continue;
                                                        }
                                                    }
                                                }     
                                            }
                                            j = str_size_now + i;
                                            i++;
                                        }
                                        if (quote_count)
                                        {
                                            i = 0;
                                            str_size_now = 0;
                                            str_size_now = strlen (plist->str);
                                            while (i < quote_count && str_size_now < str_size)
                                            {
                                                plist->str[str_size_now + i] = quote_array[i];
                                                i++;
                                                str_size_now += strlen (&(plist->str[str_size_now + i]));
                                            }
                                            free (quote_array);
                                            symbol_str = plist->str;
                                        }
                                        else
                                            symbol_str = NULL;
                                }
                                if (!symbol_str)
                                {
                                    if (list_comment != NULL)
                                    {
                                        symbol_str = strstr (plist->str, list_comment);
                                        if (symbol_str)
                                        {
                                            *symbol_str = '\0';
                                            plist->str = (char*) realloc (plist->str, (strlen(plist->str) + 1) * sizeof (char));
                                        }
                                    }
                                    if (block_comment_on)
                                        symbol_str = strstr (plist->str, block_comment_on);
                                    if (indicator == FALSE)
                                    {
                                        //symbol_str = strstr (plist->str, "{");
                                        if (symbol_str)
                                        {
                                            indicator = TRUE;
                                            //symbol_str2 = strstr (&symbol_str[1], "}");
                                        }
                                    }
                                    if (indicator == TRUE && block_comment_on && block_comment_off)
                                    {
                                        symbol_str2 = strstr (plist->str, block_comment_off);
                                        if (!symbol_str && symbol_str2)
                                        {
                                            //только }
                                            if (!strcmp (cmd_body.arg[1].str, "c"))
                                                symbol_str2 = &symbol_str2[1];
                                            new_str = (char*) malloc ((strlen (&symbol_str2[1]) + 1) * sizeof (char));
                                            strcpy (new_str, &symbol_str2[1]);
                                            free (plist->str);
                                            plist->str = new_str;
                                            indicator = FALSE;
                                            continue;
                                        }
                                        else
                                        {
                                            if (!symbol_str && !symbol_str2)
                                            {
                                                //если нет { }, но на какой-то открыта {
                                                free(plist->str);
                                                plist->str = (char*) malloc (2 * sizeof (char));
                                                strcpy (plist->str, "\n");
                                            }
                                            else
                                            {
                                                if (symbol_str2 && symbol_str)
                                                {
                                                    //{ и } на одной строке
                                                    *symbol_str = '\0';
                                                    if (!strcmp (cmd_body.arg[1].str, "c"))
                                                        symbol_str2 = &symbol_str2[1];
                                                    new_str = (char*) malloc ((strlen (plist->str) + strlen (&symbol_str2[1]) + 1) * sizeof (char));
                                                    strcpy (new_str, plist->str);
                                                    strcat (new_str, &symbol_str2[1]);
                                                    free (plist->str);
                                                    plist->str = new_str;
                                                    indicator = FALSE;
                                                    continue;
                                                }
                                                else
                                                {
                                                    if (symbol_str && !symbol_str2)
                                                    {
                                                        //только {
                                                        symbol_str2 = strchr (plist->str, '\n');
                                                        if (!strcmp (cmd_body.arg[1].str, "c"))
                                                        {
                                                            symbol_str[0] = '\0';
                                                            symbol_str[1] = '\0';
                                                        }
                                                        plist->str = realloc (plist->str, (strlen (plist->str) + 2) * sizeof (char));
                                                        strcat (plist->str, "\n");
                                                        continue;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                plist = plist->next;
                            }
                        }
                        cmd_body.error = OK;
                    }
                    else
                        cmd_body.error = EMPTY_FILE;
                }
                else
                    cmd_body.error = NO_ARG;
            }
            else
                cmd_body.error = NO_COMMAND;
        }       
    }
    else
        cmd_body.error = NO_ARG;
    error_analyzer(cmd_body);
}

char* str_changer(char* cmd, int cmd_size)
{
    int     slash_count, j, i;

    i = j = slash_count = 0;
    while (cmd[i] != '\0')
    {
        //Магия с кавычками
        if (cmd[i] == '\\')
        {
            j = i;
            slash_count = 1;
            while (cmd[j] == '\\')
            {
                j++;
                slash_count++;
            }
            j = i + 1;
            while (slash_count)
            {
                if (cmd[j-1] == cmd[j] && cmd[j] == '\\')
                {
                    memmove (&cmd[j-1], &cmd[j], (cmd_size - j) + 1);
                    cmd_size--;
                }
                else
                {
                    if (cmd[j-1] != cmd[j] && cmd[j-1] == '\\')
                    {
                        if (cmd[j] == 'n')
                        {
                            cmd [j] = '\n';
                            memmove (&cmd[j-1], &cmd[j], (cmd_size - j) + 1);
                            cmd_size--;
                            slash_count--;
                        }
                        else
                        {
                            if (cmd[j] == 't')
                            {
                                cmd [j] = '\t';
                                memmove (&cmd[j-1], &cmd[j], (cmd_size - j) + 1);
                                cmd_size--;
                                slash_count--;
                            }
                            else
                            {
                                if (cmd[j] == 'r')
                                {
                                    cmd [j] = '\r';
                                    memmove (&cmd[j-1], &cmd[j], (cmd_size - j) + 1);
                                    cmd_size--;
                                    slash_count--;
                                }
                                else
                                {
                                    if (cmd[j] == '"')
                                    {
                                        memmove (&cmd[j-1], &cmd[j], (cmd_size - j) + 1);
                                        cmd_size--;
                                        slash_count--;
                                    }
                                }
                            }
                        }
                    }
                }
                j++;
                i++;
                slash_count--;
            }      
        }
        i++;
    }
    return (cmd);
}

void    editor_insert(command_body cmd_body)
{
    node*   plist;
    node*   newlist;
    char*   symbol;
    int     str_count, i, j;
    char*   new_str;
    char*   end_str;

    plist = list->head;
    i = j = str_count = 0;
    if (cmd_body.count_arg >= 1 && cmd_body.arg[0].str && !strcmp (cmd_body.arg[0].str, "symbol"))
    {
        if (cmd_body.count_arg == 4 && cmd_body.arg[1].num > 0 && !cmd_body.arg[2].str && cmd_body.arg[3].str)
        {
            symbol = str_changer(cmd_body.arg[3].str, strlen (cmd_body.arg[3].str));
            if (strlen (symbol) == 1)
            {
                if (plist != NULL)
                {
                    if (cmd_body.arg[1].num <= list->tail->num_row)
                    {
                        while (plist->num_row != cmd_body.arg[1].num)
                            plist = plist->next;
                        str_count = strlen (plist->str);
                        if (cmd_body.arg[2].num > 0 && cmd_body.arg[2].num <= str_count)
                        {
                            list->edit_indicator = TRUE;
                            if (symbol[0] != '\n')
                                new_str = (char*) malloc (sizeof (char) * (str_count + 2));
                            else
                                new_str = (char*) malloc (sizeof (char) * (str_count + 3));
                            while (i != cmd_body.arg[2].num)
                            {
                                new_str[i] = plist->str[i];
                                i++;
                            }
                            new_str[i] = symbol[0];
                            if (symbol[0] == '\n')
                                new_str[i+1] = '\0';
                            j = i;
                            i++;
                            if (symbol[0] == '\n')
                            {
                                end_str = (char*) malloc (strlen (&(plist->str[j])) + 1);
                                strcpy (end_str, &(plist->str[j]));
                            }
                            else
                            {
                                while (i <= str_count+1)
                                {
                                    new_str[i] = plist->str[i-1];
                                    i++;
                                }
                            }
                            free (plist->str);
                            plist->str = new_str;
                        }
                        else
                        {
                            //Выход влево
                            if (cmd_body.arg[2].num <= 0)
                            {
                                list->edit_indicator = TRUE;
                                if (symbol[0] != '\n')
                                    new_str = (char*) malloc (sizeof (char) * (str_count + 2));
                                else
                                    new_str = (char*) malloc (sizeof (char) * (str_count + 3));
                                new_str[0] = symbol[0];
                                if (symbol[0] == '\n')
                                {
                                    new_str[1] = '\0';
                                    end_str = (char*) malloc (strlen (&(plist->str[j])) + 1);
                                    strcpy (end_str, &(plist->str[j]));
                                }
                                else
                                {
                                    while (i <= str_count+1)
                                    {
                                        new_str[i+1] = plist->str[i];
                                        i++;
                                    }
                                }
                                free (plist->str);
                                plist->str = new_str;
                            }
                            else
                                cmd_body.error = NO_ARG;
                        }
                        if (symbol[0] == '\n')
                        {
                            newlist = (node*) malloc (sizeof (node));
                            newlist->str = end_str;
                            newlist->next = plist->next;
                            newlist->prev = plist;
                            plist->next = newlist;
                            if (plist == list->tail)
                                list->tail = newlist;
                            list_numeration_normalize();
                        }
                    }
                    else
                        cmd_body.error = NO_ARG;
                }
                else
                    cmd_body.error = EMPTY_FILE;
            }
            else
                cmd_body.error = NO_ARG;
        }
        else
            cmd_body.error = NO_ARG;
    }
    else
    {
        if (cmd_body.arg[0].str && !strcmp (cmd_body.arg[0].str, "after"))
        {
            node*   prevlist;
            char*   transfer_char;
            char*   end_char;
            char*   str_arg;
            char*   str;
            int     command_indicator;
            
            command_indicator = FALSE;
            if (cmd_body.count_arg >= 2 && cmd_body.count_arg <= 3)
            {
                if ((cmd_body.count_arg == 3 && !cmd_body.arg[1].str && cmd_body.arg[2].str && cmd_body.arg[1].num >= 0 && cmd_body.arg[1].num <= list->tail->num_row) || (cmd_body.count_arg == 2 && cmd_body.arg[1].str))
                {
                    if (plist != NULL)
                    {
                        if (cmd_body.arg[1].str != NULL)
                        {
                            str = quote_checked(cmd_body.arg[1].str);
                            cmd_body.arg[1].num = list->tail->num_row;
                        }
                        else
                        {
                            if (cmd_body.arg[2].str != NULL && cmd_body.name == INSERT)
                                str = quote_checked(cmd_body.arg[2].str);
                            if (cmd_body.name == INSERT_SPEC || cmd_body.name == INSERT_REP)
                                str = cmd_body.arg[2].str;
                        }
                        if (str)
                        {
                            if (cmd_body.arg[1].num)
                            {
                                while (plist->num_row != cmd_body.arg[1].num)
                                    plist = plist->next;
                            }
                            //
                            if (!strcmp (str, "\""))
                            {
                                str = NULL;
                                command_indicator = TRUE;
                                ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
                                printf ("\e[%d;%dH\e[2K", w.ws_row-1, 0);
                                printf ("\e[2K\e[48;5;23mYou[ia]:\e[0m ");
                                str = command_input(str);
                            }
                            prevlist = plist;
                            list->edit_indicator = TRUE;
                            while (strcmp (str, "\"\"\""))
                            {
                                str = str_changer(str, strlen (str));
                                if (cmd_body.name != INSERT_SPEC && (transfer_char = strchr (str, '\n')))
                                {
                                    str_arg = transfer_char = str;
                                    while ((transfer_char = strchr (transfer_char, '\n')))
                                    {
                                        *transfer_char = '\0';
                                        str_count = strlen(str_arg);
                                        newlist = (node*) malloc (sizeof (node));
                                        newlist->str = (char*) malloc ((str_count + 1 + 1) * sizeof (char));
                                        strcpy (newlist->str, str_arg);
                                        end_char = strchr (newlist->str, '\0');
                                        *end_char = '\n';
                                        end_char[1] = '\0';
                                        if (prevlist == list->head && !cmd_body.arg[1].num)
                                        {
                                            list->head = newlist;
                                            prevlist->prev = newlist;
                                            newlist->next = prevlist;
                                            newlist->prev = prevlist->prev;
                                        }
                                        else
                                        {
                                            newlist->next = prevlist->next;
                                            newlist->prev = prevlist;
                                            prevlist->next = newlist;
                                            if (prevlist == list->tail)
                                                list->tail = newlist;
                                        }
                                        str_arg = transfer_char = transfer_char + 1;
                                        prevlist = newlist;
                                    }
                                    str_count = strlen(str_arg);
                                    newlist = (node*) malloc (sizeof (node));
                                    newlist->str = (char*) malloc ((str_count + 1) * sizeof (char));
                                    strcpy (newlist->str, str_arg);
                                    newlist->next = prevlist->next;
                                    newlist->prev = prevlist;
                                    prevlist->next = newlist;
                                    if (prevlist == list->tail)
                                        list->tail = newlist;
                                    prevlist = newlist;
                                }
                                else
                                {
                                    newlist = (node*) malloc (sizeof (node));
                                    newlist->str = (char*) malloc ((strlen (str) + 1) * sizeof (char));
                                    strcpy (newlist->str, str);
                                    if (prevlist == list->head && !cmd_body.arg[1].num)
                                    {
                                        list->head = newlist;
                                        prevlist->prev = newlist;
                                        newlist->next = prevlist;
                                        newlist->prev = prevlist->prev;
                                    }
                                    else
                                    {
                                        if (cmd_body.arg[1].num && cmd_body.arg[1].str)
                                        {
                                            newlist->next = NULL;
                                        }
                                        else
                                            newlist->next = prevlist->next;
                                        newlist->prev = prevlist;
                                        prevlist->next = newlist;
                                        if (prevlist == list->tail)
                                            list->tail = newlist;
                                    }
                                    prevlist = newlist;
                                }
                                if (command_indicator == FALSE)
                                    break;
                                else
                                {
                                    free (str);
                                    str = NULL;
                                    printf ("\e[%d;%dH\e[2K", w.ws_row-1, 0);
                                    printf ("\e[48;5;23mYou[ia]:\e[0m ");
                                    while (str == NULL)
                                        str = command_input(str);
                                }
                            }
                            if (command_indicator == TRUE)
                                free (str);
                            list_numeration_normalize();
                        }
                        else
                            cmd_body.error = NO_ARG;
                    }
                    else
                        cmd_body.error = EMPTY_FILE;
                }
                else
                    cmd_body.error = NO_ARG;
            }
            else
                cmd_body.error = NO_ARG;
            
        }
        else
            cmd_body.error = NO_COMMAND;
    }
    error_analyzer(cmd_body);
}

void    editor_edit(command_body cmd_body)
{
    command_body    new_cmd_body;
    node*           plist;
    char            symbol;
    int             str_count;

    plist = list->head;
    if (cmd_body.count_arg >= 1 && cmd_body.arg[0].str && !strcmp (cmd_body.arg[0].str, "string"))
    {
        if (cmd_body.count_arg == 4 && cmd_body.arg[1].num > 0 && cmd_body.arg[2].num > 0 && cmd_body.arg[3].str)
        {
            str_count = strlen (cmd_body.arg[3].str);
            if (str_count <= 2)
            {
                if (str_count == 2)
                    symbol = *str_changer(cmd_body.arg[3].str, str_count);
                else
                    symbol = cmd_body.arg[3].str[0];
                str_count = 0;
                if (plist != NULL)
                {
                    if (cmd_body.arg[1].num <= list->tail->num_row)
                    {
                        while (plist->num_row != cmd_body.arg[1].num)
                            plist = plist->next;
                        str_count = strlen (plist->str);
                        if (cmd_body.arg[2].num < str_count)
                        {
                            list->edit_indicator = TRUE;
                            if (symbol == '\n')
                            {
                                new_cmd_body.arg = (struct  argument_type*) malloc (3 * sizeof (struct  argument_type));
                                new_cmd_body.arg[0].str = "after";
                                new_cmd_body.arg[1].num = cmd_body.arg[1].num;
                                new_cmd_body.arg[1].str = NULL;
                                new_cmd_body.arg[2].str = &(plist->str[cmd_body.arg[2].num]);
                                new_cmd_body.name = INSERT_SPEC;
                                new_cmd_body.count_arg = 3;
                                new_cmd_body.error = OK;
                                editor_insert(new_cmd_body);
                                plist->str[cmd_body.arg[2].num-1] = '\0';
                                str_count = strlen (plist->str);
                                plist->str = (char*) realloc (plist->str, (str_count + 1) * sizeof (char));
                                free (new_cmd_body.arg);
                            }
                            else
                                plist->str[cmd_body.arg[2].num-1] = symbol;
                        }  
                        else
                            cmd_body.error = NO_ARG;
                    }
                    else
                        cmd_body.error = NO_ARG;
                }
                else
                    cmd_body.error = EMPTY_FILE;
            }
            else
                cmd_body.error = NO_ARG;
        }
        else
            cmd_body.error = NO_ARG;
    }
    else
        cmd_body.error = NO_COMMAND;
    error_analyzer(cmd_body);
}

void    editor_write(command_body cmd_body)
{
    FILE*   my_file;
    node*   ptail;

    ptail = list->head;
    if (cmd_body.count_arg <= 1)
    {
        if (cmd_body.count_arg == 1)
        {
            if (cmd_body.arg[0].str)
            {
                cmd_body.arg[0].str = quote_checked(cmd_body.arg[0].str);
                if (cmd_body.arg[0].str)
                {
                    if (!list->file_link)
                    {
                        list->file_link = (char*) malloc ((strlen (cmd_body.arg[0].str) + 1) * sizeof (char));
                        strcpy (list->file_link, cmd_body.arg[0].str);
                    }
                    if ((my_file = fopen (cmd_body.arg[0].str, "w")) != NULL)
                    {
                        while (ptail != NULL)
                        {
                            fputs (ptail->str, my_file);
                            ptail = ptail->next;
                        }
                        fclose (my_file);  
                        list->edit_indicator = FALSE;
                        cmd_body.error = SUCCESS_WRITE;        
                    }
                }
                else
                    cmd_body.error = NO_ARG;
            }
            else
                cmd_body.error = NO_ARG;
        }
        else
        {
            if (!cmd_body.count_arg && list->file_link)
            {
                if ((my_file = fopen (list->file_link, "w")) != NULL)
                {
                    while (ptail != NULL)
                    {
                        fputs (ptail->str, my_file);
                        ptail = ptail->next;
                    }
                    fclose (my_file);
                    list->edit_indicator = FALSE;
                    cmd_body.error = SUCCESS_WRITE;      
                }       
            }
            else
                cmd_body.error = NO_NAME;
        }
    }
    else
        cmd_body.error = NO_ARG;
    error_analyzer(cmd_body);
}

void    editor_open(command_body cmd_body)
{
    FILE*           my_file;
    unsigned int    i;
    node*           ptail;
    node*           prev_address;
    size_t          len;
    char*           str;

    len = 0;
    str = NULL;
    if (cmd_body.count_arg == 1 && cmd_body.arg[0].str != NULL)
    {
        if (cmd_body.name != TERM_OPEN)
            cmd_body.arg[0].str = quote_checked(cmd_body.arg[0].str);
        if (cmd_body.arg[0].str)
        {
            if ((my_file = fopen (cmd_body.arg[0].str, "r")) != NULL)
            {
                if (list->tail != NULL && list->head != NULL)
                {
                    ptail = list->head;
                    while (ptail != NULL)
                    {
                        prev_address = ptail;
                        ptail = ptail->next;
                        free (prev_address->str);
                        free (prev_address);
                    }
                    list->head = list->tail = NULL;
                }
                if (cmd_body.name == OPEN || cmd_body.name == TERM_OPEN)
                {
                    if (list->file_link != NULL)
                        free (list->file_link);
                    list->file_link = (char*) malloc ((strlen (cmd_body.arg[0].str) + 1) * sizeof (char));
                    strcpy (list->file_link, cmd_body.arg[0].str);
                }
                i = 1;
                while ((getline(&str, &len, my_file)) != -1)
                {
                    if (list->tail != NULL)
                        ptail = list->tail;
                    list->tail = (node*) malloc (sizeof (node));
                    list->tail->str = str;
                    str = NULL;
                    list->tail->num_row = i;
                    i++;
                    if (list->head == NULL)
                    {
                        list->tail->prev = list->tail->next = NULL;
                        list->head = list->tail;
                    }
                    else
                    {
                        list->tail->next = NULL;
                        list->tail->prev = ptail;
                        list->tail->prev->next = list->tail;
                    }
                }
                free (str);
                fclose (my_file);        
            }
            else
                cmd_body.error = NO_FILE;
        }
        else
            cmd_body.error = NO_ARG;
    }
    else
        cmd_body.error = NO_ARG;
    error_analyzer(cmd_body);
}

void    editor_set(command_body cmd_body)
{
    struct winsize  w;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    //set name
    if (cmd_body.count_arg >= 1 && cmd_body.arg[0].str)
    {
        if (!strcmp (cmd_body.arg[0].str,"name"))
        {
            if (cmd_body.count_arg == 2 && cmd_body.arg[1].str)
            {
                if (!(cmd_body.arg[1].str = quote_checked(cmd_body.arg[1].str)))
                    cmd_body.error = NO_ARG;
                else
                {
                    if (list->file_link != NULL)
                        free (list->file_link);
                    list->file_link = (char*) malloc ((strlen (cmd_body.arg[1].str) + 1) * sizeof (char));
                    strcpy (list->file_link, cmd_body.arg[1].str);
                }
            }
            else
            {         
                if (cmd_body.count_arg == 1)
                {
                    if (list->file_link != NULL)
                        free (list->file_link);
                    list->file_link = NULL;
                }
                else
                    cmd_body.error = NO_ARG;
            }
        }
        else
        {
            if (!strcmp (cmd_body.arg[0].str,"numbers"))
            {                  
                if (cmd_body.count_arg == 2 && cmd_body.arg[1].str)
                {
                    if (!strcmp (cmd_body.arg[1].str,"yes"))
                    {
                        list->file_numeration = TRUE;
                        printf ("\e[0;0H\e[2K");
                        printf ("\e[48;5;24m 1 \e[0m\n");
                    }
                    else
                    {
                        if (!strcmp (cmd_body.arg[1].str,"no"))
                        {
                            list->file_numeration = FALSE;
                            printf ("\e[0;0H\e[2K");
                            printf ("\e[38;5;114m * \e[0m\n");
                        }
                        else
                        {
                            cmd_body.error = NO_ARG;
                        }
                        
                    }
                }
                else
                    cmd_body.error = NO_ARG;            
            }
            else
            {
                if (!strcmp (cmd_body.arg[0].str, "tabwidth"))
                {
                    if (!cmd_body.arg[1].str && cmd_body.arg[1].num > 0 && cmd_body.arg[1].num <= 16)
                        list->file_tabwidth = cmd_body.arg[1].num;
                    else
                        cmd_body.error = NO_ARG;
                }
                else
                {
                    if (!strcmp (cmd_body.arg[0].str, "wrap"))
                    {
                        if (cmd_body.count_arg == 2 && cmd_body.arg[1].str)
                        {
                            if (!strcmp (cmd_body.arg[1].str, "yes"))
                            {
                                main_list.file_wrap = FALSE;
                            }
                            else
                            {
                                if (!strcmp (cmd_body.arg[1].str, "no"))
                                {
                                    main_list.file_wrap = TRUE;
                                }
                                else
                                    cmd_body.error = NO_ARG;
                            }
                            
                            
                        }
                        else
                            cmd_body.error = NO_ARG;
                    }
                    else
                        cmd_body.error = NO_COMMAND; 
                } 
            }
                        
        }
    }
    else
        cmd_body.error = NO_ARG;
    if (cmd_body.count_arg > 2 || cmd_body.count_arg == 0)
        cmd_body.error = NO_ARG;     
    error_analyzer(cmd_body);
}

/* Обработка команды */
void    error_timer()
{
    struct termios  old_attributes,new_attributes;
    struct winsize  w;
    char            symbol;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);   
    printf ("\e[%d;%dH[any key]\n", w.ws_row-1, w.ws_col-9);
    //терминальная магия
    if(!isatty (0))
    {
        printf ("Похоже вы перенаправили ввод не из терминала.\n"); 
    }   
    tcgetattr (0, &old_attributes);
    memcpy (&new_attributes, &old_attributes, sizeof (struct termios));
    new_attributes.c_lflag &= ~ECHO;
    new_attributes.c_lflag &= ~ICANON;
    new_attributes.c_cc[VMIN] = 1;
    tcsetattr (0, TCSANOW, &new_attributes);
    while((symbol=getchar ()) != EOF)
    {
       // if (symbol == 'q' || symbol == 'Q')
            break;
    }
    tcsetattr (0, TCSANOW, &old_attributes);
}

int     error_analyzer(command_body cmd_body)
{
    if ((cmd_body.error != OK) && (cmd_body.error != SUCCESS_WRITE))
        printf ("\e[?25l\e[48;5;23mGavrix:\e[0m /Error/ ");
    if (cmd_body.error == SUCCESS_WRITE)
        printf ("\e[?25l\e[48;5;23mGavrix:\e[0m /Yeah!/ ");
    switch (cmd_body.error)
    {
        case NO_COMMAND:
            printf ("No command!");
            break;
        case NO_ARG:
            printf ("Strange arguments!");
            break;   
        case NO_FILE:
            printf ("File not found!");
            break;
        case EMPTY_FILE:
            printf ("File is empty!");
            break;
        case NO_SAVE:
            printf ("File is not saved!");
            break;
        case SUCCESS_WRITE:
            if (cmd_body.count_arg == 1)
                printf ("Successfully saved to %s", cmd_body.arg[0].str);
            if (cmd_body.count_arg == 0 && list->file_link)
                printf ("Successfully saved to %s", list->file_link);
            break; 
        case NO_NAME:
            printf ("No file name!");
            break; 
        default:
            break;
    }
    if (cmd_body.error == OK)
        return 0;
    else
    {
        error_timer();
        printf ("\e[?25h"); //Отображает курсор
        return 1;
    }
}

char*   command_input(char* cmd)
{
    int     i;
    char    symbol;

    i = 1;
    while ((symbol = getchar()) != EOF)
    {
        if (symbol != '\n')
        {
            if (i == 1)
                cmd = (char*) malloc (sizeof (char));
            else
                cmd = (char*) realloc (cmd, i * sizeof (char));
            cmd[i-1] = symbol;
            i++;
        }
        else
            break;
    }
    if (i != 1)
    {
        cmd = (char*) realloc (cmd, i * sizeof (char));
        cmd[i-1] = '\0';
        return (cmd);
    }
    else
        return (NULL);
}

void    command_line()
{
    char*          cmd;
    //struct winsize w;

    cmd = NULL;
    //ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    printf ("\e[%d;%dH\e[2K", w.ws_row, w.ws_col);
    if (list->file_link == NULL)
        printf ("\e[%d;%dHNew file ", w.ws_row, w.ws_col-8);
    else
        printf ("\e[%d;%ldH%s ", w.ws_row, w.ws_col-strlen (list->file_link), list->file_link);
    // printf ("\e[%d;%dH\e[2K", w.ws_row-1, 0);
    // printf ("\e[48;5;23mYou:\e[0m ");
    //scanf ("%[^\n]%*c", cmd);
    while (cmd == NULL)
    {
        printf ("\e[%d;%dH\e[2K", w.ws_row-1, 0);
        printf ("\e[48;5;23mYou:\e[0m ");
        cmd = command_input(cmd);
    }
    printf ("\e[%d;%dH\e[2K", w.ws_row-1, 0);
    if (resize.indicator != TRUE)
        command_parser(cmd);
}

void    command_parser(char* cmd)
{
    command_body    cmd_body;
    char*           end;
    int             arg_num, arg_count_indicator, command_size, cmd_size, transfer, slash_count, quote_indicator;
    size_t          i, j, count_cmd_all;

    i = j = transfer = cmd_body.count_arg = slash_count = 0;
    count_cmd_all = sizeof (cmd_all)/sizeof (cmd_all[0]);
    cmd_size = strlen (cmd);
    cmd_body.str_original = cmd;
    //Пробелы заменяю на '\0'.
    quote_indicator = arg_count_indicator = FALSE;
    if (cmd[0] == ' ')
        cmd_body.count_arg--;
    while (cmd[i] != '\0')
    {
        //Магия с кавычками
        if (cmd[i] == '\\' && quote_indicator == TRUE)
        {
            j = i;
            slash_count = 1;
            while (cmd[j] == '\\')
            {
                j++;
                slash_count++;
            }
            j = i + 1;
            while (slash_count)
            {
                if (cmd[j-1] == cmd[j] && cmd[j] == '\\')
                {
                    memmove (&cmd[j-1], &cmd[j], (cmd_size - j) + 1);
                    cmd_size--;
                }
                else
                {
                    if (cmd[j-1] == '"')
                    {
                        quote_indicator = FALSE;
                    }
                    else
                    {
                        if (cmd[j-1] != cmd[j] && cmd[j-1] == '\\')
                        {
                            if (cmd[j] == 'n')
                            {
                                cmd [j] = '\n';
                                memmove (&cmd[j-1], &cmd[j], (cmd_size - j) + 1);
                                cmd_size--;
                                slash_count--;
                            }
                            else
                            {
                                if (cmd[j] == 't')
                                {
                                    cmd [j] = '\t';
                                    memmove (&cmd[j-1], &cmd[j], (cmd_size - j) + 1);
                                    cmd_size--;
                                    slash_count--;
                                }
                                else
                                {
                                    if (cmd[j] == 'r')
                                    {
                                        cmd [j] = '\r';
                                        memmove (&cmd[j-1], &cmd[j], (cmd_size - j) + 1);
                                        cmd_size--;
                                        slash_count--;
                                    }
                                    else
                                    {
                                        if (cmd[j] == '"')
                                        {
                                            quote_indicator = TRUE;
                                            memmove (&cmd[j-1], &cmd[j], (cmd_size - j) + 1);
                                            cmd_size--;
                                            slash_count--;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                j++;
                i++;
                slash_count--;
            }      
        }
        if (cmd[i] == '"' && quote_indicator == FALSE)
        {
            if (cmd[i-1] != '\\' && i != 0)
                quote_indicator = TRUE;
            if (i == 0)
                quote_indicator = TRUE;
        }
        else
        {
            if (cmd[i] == '"' && quote_indicator == TRUE)
            {
                    quote_indicator = FALSE;
            }
        }
        if (cmd[i] == ' ' && quote_indicator == FALSE)
        {
            if (arg_count_indicator == FALSE)
            {
                arg_count_indicator = TRUE;
                cmd_body.count_arg++;
            }
            cmd[i] = '\0';
        }
        if (cmd[i] != ' ' && cmd[i] != '\0')
            arg_count_indicator = FALSE;
        i++;
    }
    if (arg_count_indicator == TRUE)
        cmd_body.count_arg--;
    //Ищу есть ли введенная команда из cmd_all[] в cmd, если есть присваиваю номер команды из enum в cmd_body
    cmd_body.error = NO_COMMAND;
    while (transfer <= cmd_size)
    {
        if (cmd[transfer] != '\0')
            break;
        else
            transfer++;
    }
    for (j = 0;j < count_cmd_all;j++)
    {
        if (!strcmp (&cmd[transfer], cmd_all[j]))
        {
            cmd_body.name = j;
            cmd_body.error = OK;
            command_size = strlen (&cmd[transfer]);
            transfer += (command_size + 1);
            break;
        }
    }
    if (!error_analyzer(cmd_body))
    {
        if (cmd_body.count_arg > 0)
        {
            if (cmd_body.count_arg <= 5)
            {
                //Перемещаюсь к каждой строчке(аргументу) и присваиваю cmd_body.arg[i]
                // cmd_body.count_arg = j;
                cmd_body.arg = (struct argument_type*) malloc (sizeof (struct argument_type)*cmd_body.count_arg);
                i = 0;
                while (transfer <= cmd_size)
                {
                    while (cmd[transfer] == '\0')
                    {
                        if (transfer < cmd_size)
                            transfer++;
                        if (transfer == cmd_size)
                            break;
                    }
                    if (transfer == cmd_size)
                            break;
                    arg_num = strtol (&cmd[transfer], &end, 10);
                    if (!*end)
                    {
                        cmd_body.arg[i].num = arg_num;
                        cmd_body.arg[i].str = NULL;
                        //printf ("\nnum: i = %ld %d", i, cmd_body.arg[i].num);
                    }
                    else
                    {
                        cmd_body.arg[i].str = &cmd[transfer];
                        //printf ("\nstr: i = %ld %s", i,  cmd_body.arg[i].str);
                    }
                    //printf ("\ncount: arg = %d", cmd_body.count_arg);
                    transfer += (strlen (&cmd[transfer]) + 1);
                    i++;
                }
            }
            else
                cmd_body.error = NO_ARG;
        }
        switch (cmd_body.name)
        {
            case EXIT:
                editor_exit(cmd_body);
                break;
            case OPEN:
                editor_open(cmd_body);
                break;
            case READ:
                editor_open(cmd_body);
                break;
            case WRITE:
                editor_write(cmd_body);
                break;
            case SET:
                editor_set(cmd_body);
                break;
            case PRINT:
                editor_print(cmd_body);
                break;
            case EDIT:
                editor_edit(cmd_body);
                break;
            case INSERT:
                editor_insert(cmd_body);
                break;
            case DELETE:
                editor_delete(cmd_body);
                break;
            case REPLACE:
                editor_replace(cmd_body);
                break;
            case HELP:
                editor_help(cmd_body);
            default:
                break;
        }
        command_free(cmd_body);
    }
    if (cmd_body.error == NO_COMMAND)
    {
        free (cmd);
        cmd_body.str_original = NULL;
    }
    command_line();
}

void    editor_start()
{
   // struct winsize  w;
    int             i;

    //ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    //printf ("\e[2J\e[3J\e[r");
    //printf ("\e[48;5;24m  1\e[0m\n");
    for (i = 1; i < w.ws_row; i++)
        printf ("\e[2K\e[38;5;114m * \e[0m\n");
    if (list->file_numeration == TRUE)
    {
        printf ("\e[0;0H\e[2K");
        printf ("\e[48;5;24m 1 \e[0m\n");
    }
    for (i = 0; i < 6; i++)
        printf ("\e[1;38;5;113m\e[%d;%dH%s\e[0m\n", w.ws_row/2-4+i, w.ws_col/2-17, logo[i]);
    if (resize.indicator == FALSE)
        command_line();
}
/* Конец обработки */
void    sig_print_str()
{
    int     i, str_place;

    i = 1;
    str_place = resize.print_str_place_start;
    resize.print_max_str_place = 0;
    resize.plist = resize.pliststart;
    //resize.print_str_place = 0;
    printf ("\e[2J\e[3J\e[r");
    printf ("\e[%d;0H\e[?25l\e[48;5;23mGavrix:\e[0m [- PAGES VIEW -]", w.ws_row-1);
    if (list->file_link == NULL)
        printf ("\e[%d;%dHNew file ", w.ws_row, w.ws_col-8);
    else
        printf ("\e[%d;%ldH%s ", w.ws_row, w.ws_col - strlen (list->file_link), list->file_link);
    printf ("\e[0;0H\e[2K");

    if (resize.print_command == TRUE)
    {
        while ((i < w.ws_row-1) && (resize.plist != NULL))
        {
            printf ("\e[%d;0H\e[2K", i);
            if (list->file_wrap == TRUE)
                str_place = print_str(resize.plist->str, resize.plist->num_row, list->tail->num_row, str_place);
            else
            {
                str_place = print_str(resize.plist->str, resize.plist->num_row, list->tail->num_row, resize.print_transfer_wrap);
                if (str_place > resize.print_max_str_place)
                    resize.print_max_str_place = str_place;
            }
            if ((!str_place && list->file_wrap == TRUE) || list->file_wrap == FALSE)
                resize.plist = resize.plist->next;
            i++;
        }
    }
    if (resize.print_command == FALSE)
    {    
        while ((i < w.ws_row-1) && (resize.plist != NULL) && (resize.plist->num_row <= resize.print_supernum))
        {
            printf ("\e[%d;0H\e[2K", i);
            if (list->file_wrap == TRUE)
                str_place = print_str(resize.plist->str, resize.plist->num_row, list->tail->num_row, str_place);
            else
            {
                str_place = print_str(resize.plist->str, resize.plist->num_row, list->tail->num_row, resize.print_transfer_wrap);
                if (str_place > resize.print_max_str_place)
                    resize.print_max_str_place = str_place;
            }
            if ((!str_place && list->file_wrap == TRUE) || list->file_wrap == FALSE)
                resize.plist = resize.plist->next;
            i++;
        }
        printf ("%d", i);
    }
    resize.print_str_place_end = str_place;
    while (i < w.ws_row-1)
    {
        printf ("\e[%d;0H\e[2K", i);
        printf ("\e[38;5;114m * \e[0m\n");
        i++;
    }
}

void sig_start()
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    resize.indicator = TRUE;
    if (resize.print_indicator == TRUE)
        sig_print_str();
    else
        resize.indicator = FALSE;
}

int     main(int argc, char* argv[])
{
    if (argc == 2)
    {
        command_body    cmd_body;

        cmd_body.name = TERM_OPEN;
        cmd_body.error = OK;
        cmd_body.arg = (struct  argument_type*) malloc (sizeof (struct  argument_type));
        cmd_body.arg[0].str = argv[1];
        cmd_body.count_arg = 1;
        editor_open(cmd_body);
        free (cmd_body.arg);
        printf ("\e[2J\e[3J\e[r");
    }
    else
    {
        if (argc > 2)
        {
            command_body    cmd_body;

            cmd_body.error = NO_ARG;
            error_analyzer(cmd_body);
        }
    }
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        //signal (SIGWINCH, sig_start);
       // struct sigaction sig;
        

        //struct sigaction act;
       // memset(&act, 0, sizeof(act));
       //memset (&act, '\0', sizeof(act));
        //act.sa_sigaction = &sig_start;
        // sigset_t   set; 
        // sigemptyset(&set);                                                             
        // sigaddset(&set, SIGWINCH); 
        // act.sa_mask = set;
        //sigaction(SIGWINCH, &act, NULL);

        editor_start();    
    return 0;
}