/*
   FSearch - A fast file search utility
   Copyright © 2020 Christian Boxdörfer

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.
   */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "fsearch.h"
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <locale.h>

#define _GNU_SOURCE  // Required for strcasestr
#include <stdio.h>
#include <string.h>
#include "fsearch_database.h"
#include "fsearch_database_entry.h"
#include "fsearch_database_search.h"
#include "fsearch_array.h"
#include "fsearch_query.h"

int
main(int argc, char *argv[]) {
    // 1. Check specifically for our custom --cli flag
    char *search_term = NULL;
    int cli_mode = 0;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--cli") == 0) {
            cli_mode = 1;
            // If there's an argument after --cli, use it as the search term
            if (i + 1 < argc) {
                search_term = argv[i+1];
            } else {
                search_term = ""; // Handle empty search
            }
            break;
        }
    }

    if (cli_mode) {
        // fprintf(stderr, "FSearch CLI Mode active...\n"); // Optional debug
        const char *db_path = "/home/sunshine/.local/share/fsearch/fsearch.db";
        FsearchDatabase *db = db_new(NULL, NULL, NULL, false);
        
        if (db_load(db, db_path, NULL)) {
            DynamicArray *files = db_get_files(db);
            DynamicArray *folders = db_get_folders(db);

            // If search_term is empty, print everything
            // If search_term has text, use strcasestr to filter via FSearch
            
            if (files) {
                uint32_t num = darray_get_num_items(files);
                for (uint32_t i = 0; i < num; i++) {
                    FsearchDatabaseEntry *entry = darray_get_item(files, i);
                    const char *name = db_entry_get_name_raw(entry);
                    if (name && (strlen(search_term) == 0 || strcasestr(name, search_term))) {
                        GString *p = db_entry_get_path_full(entry);
                        if (p) { printf("%s\n", p->str); g_string_free(p, TRUE); }
                    }
                }
            }

            if (folders) {
                uint32_t num = darray_get_num_items(folders);
                for (uint32_t i = 0; i < num; i++) {
                    FsearchDatabaseEntry *entry = darray_get_item(folders, i);
                    const char *name = db_entry_get_name_raw(entry);
                    if (name && (strlen(search_term) == 0 || strcasestr(name, search_term))) {
                        GString *p = db_entry_get_path_full(entry);
                        if (p) { printf("%s\n", p->str); g_string_free(p, TRUE); }
                    }
                }
            }
            db_unref(db);
            fflush(stdout);
            _exit(0); 
        }
        _exit(1);
    }

    // Original GUI Startup...
    setlocale(LC_ALL, "");
    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    g_set_application_name(_("FSearch"));
    g_set_prgname("io.github.cboxdoerfer.FSearch");

    return g_application_run(G_APPLICATION(fsearch_application_new()), argc, argv);
}
