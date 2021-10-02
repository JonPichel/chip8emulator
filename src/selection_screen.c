#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <curses.h>
#include <menu.h>

#include "chip8vm.h"
#include "settings.h"

#define IS_ROM(A) (strlen(A) > 4 && strcmp(&A[strlen(A) - 4], ".ch8") == 0)
struct dirent_node {
    struct dirent_node *next_ptr;
    char *name;
};

int dirent_ll(char *dirpath, struct dirent_node **dirent_head);
/*
int is_rom(char *name) {
    return (strlen(name) > 4 && strcmp(name[strlen(name) - 4], ".ch8") == 0);
}
*/

char *select_rom(void) {
    ITEM **items;
    MENU *rom_menu;
    struct dirent_node *head, *tmp;
    char *dirpath = (char *)malloc(PATH_MAX + 1);
    chtype c;
    int entry_num, i, choice;

    dirpath[0] = '\0';
    strcpy(dirpath, ROM_DIR);
    
    choice = 0;
    while (choice != 1) {
        entry_num = dirent_ll(dirpath, &head);
        items = (ITEM **)calloc(entry_num + 1, sizeof(ITEM *));
        items[entry_num] = (ITEM *)NULL;
        i = 0;

        // Add first the directories
        tmp = head;
        do {
            if (!IS_ROM(tmp->name)) {
                items[i++] = new_item(tmp->name, "");
            }
        } while ((tmp = tmp->next_ptr) != NULL);

        // And then rom files
        tmp = head;
        do {
            if (IS_ROM(tmp->name)) {
                items[i++] = new_item(tmp->name, "");
            }
        } while ((tmp = tmp->next_ptr) != NULL);

        rom_menu = new_menu(items);
        set_menu_mark(rom_menu, " * ");
        mvprintw(LINES - 1, 0, "HELLOOOOOO");
        post_menu(rom_menu);
        refresh();

        choice = -1;
        while (choice == -1) {
            c = getch();
            switch (c) {
                case KEY_DOWN:
                    menu_driver(rom_menu, REQ_DOWN_ITEM);
                    break;
                case KEY_UP:
                    menu_driver(rom_menu, REQ_UP_ITEM);
                    break;
                case KEY_NPAGE:
                    menu_driver(rom_menu, REQ_SCR_DPAGE);
                    break;
                case KEY_PPAGE:
                    menu_driver(rom_menu, REQ_SCR_UPAGE);
                    break;
                case 10:
                case KEY_ENTER:
                    choice = IS_ROM(current_item(rom_menu)->name.str) ? 1 : 0;
                    mvprintw(LINES - 1, 0, "HELLLLEFEFEFEF");
                    refresh();
                    strcat(dirpath, "/");
                    strcat(dirpath, current_item(rom_menu)->name.str);
                    break;
            }
        }

        unpost_menu(rom_menu);
        free_menu(rom_menu);
        for (i = 0; i < entry_num; i++) {
            free_item(items[i]);
        }
        free(items);

        while (head != NULL) {
            tmp = head;
            head = head->next_ptr;
            free(tmp->name);
            free(tmp);
        }
    }

    return dirpath;
}

int dirent_ll(char *dirpath, struct dirent_node **dirent_head) {
    DIR *dir_ptr;
    struct dirent *de_ptr;
    struct dirent_node *tmp;
    int entry_num = 0;

    if ((dir_ptr = opendir(dirpath)) == NULL)
        return -1;

    *dirent_head = NULL;
    while ((de_ptr = readdir(dir_ptr)) != NULL) {
        if (strcmp(de_ptr->d_name, ".") == 0)
            continue;
        switch (de_ptr->d_type) {
            case DT_REG:
                if (strcmp(&(de_ptr->d_name[strlen(de_ptr->d_name) - 4]), ".ch8") == 0) {
                    if (entry_num++ == 0) {
                        *dirent_head = malloc(sizeof(struct dirent_node));
                        tmp = *dirent_head;
                    } else {
                        tmp->next_ptr = malloc(sizeof(struct dirent_node));
                        tmp = tmp->next_ptr;
                    }
                    tmp->name = malloc(strlen(de_ptr->d_name) + 1);
                    strcpy(tmp->name, de_ptr->d_name);
                    tmp->next_ptr = NULL;
                }
                break;
            case DT_DIR:
                if (entry_num++ == 0) {
                    *dirent_head = malloc(sizeof(struct dirent_node));
                    tmp = *dirent_head;
                } else {
                    tmp->next_ptr = malloc(sizeof(struct dirent_node));
                    tmp = tmp->next_ptr;
                }
                tmp->name = malloc(strlen(de_ptr->d_name) + 1);
                strcpy(tmp->name, de_ptr->d_name);
                tmp->next_ptr = NULL;
                break;
        }
    }
    closedir(dir_ptr);
    return entry_num;
}
