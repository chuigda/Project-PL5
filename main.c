#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "rt.h"
#include "rt_impl.h"
#include "scope.h"
#include "syntax.h"
#include "value.h"
#include "dump.h"

#ifdef _WIN32
#   include <windows.h>
#else
#   include <dlfcn.h>
#   include <unistd.h>
#endif /* _WIN32 */

static char* read_to_string(char const *file);
static bool ends_with(char const *str, char const *suffix);

typedef void (*mscm_ext_loader)(mscm_runtime *rt);

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <files...>\n", argv[0]);
        return 1;
    }

    mscm_runtime *rt = runtime_new();
    for (int i = 1; i < argc; ++i) {
        if (ends_with(argv[i], ".dll") || ends_with(argv[i], ".so")) {
#ifdef _WIN32
            HANDLE h = LoadLibraryA(argv[i]);
            if (!h) {
                fprintf(stderr,
                        "error: could not load library %s\n",
                        argv[i]);
                continue;
            }

            mscm_ext_loader loader =
                (mscm_ext_loader)GetProcAddress(h, "mscm_load_ext");
            if (!loader) {
                fprintf(stderr,
                        "error: could not locate mscm_load_ext in %s\n",
                        argv[i]);
                FreeLibrary(h);
                continue;
            }
#else
            void *h = dlopen(argv[i], RTLD_LAZY | RTLD_DEEPBIND);
            if (!h) {
                fprintf(stderr,
                        "error: could not load library %s\n",
                        argv[i]);
                continue;
            }

            mscm_ext_loader loader =
                (mscm_ext_loader)dlsym(h, "mscm_load_ext");
            if (!loader) {
                fprintf(stderr,
                        "error: could not locate mscm_load_ext in %s\n",
                        argv[i]);
                dlclose(h);
                continue;
            }
#endif /* _WIN32 */
            loader(rt);
        }
        else {
            char *content = read_to_string(argv[i]);
            fprintf(stderr, "DEBUG: content = %s\n", content);
            if (!content) {
                fprintf(stderr,
                        "error: could not read file %s\n",
                        argv[i]);
                continue;
            }

            mscm_syntax_node node = mscm_parse(argv[i], content);
            if (!node) {
                fprintf(stderr,
                        "error: could not parse file %s\n",
                        argv[i]);
                free(content);
                continue;
            }

            runtime_eval(rt, node, true);
            mscm_free_syntax_node(node);
            free(content);
        }
    }
    runtime_free(rt);

    return 0;
}

static char* read_to_string(char const *file) {
    FILE *f = fopen(file, "rb");
    if (!f) {
        return 0;
    }

    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(len + 1);
    if (!buf) {
        fclose(f);
        return 0;
    }

    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}

static bool ends_with(char const *str, char const *suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (str_len < suffix_len) {
        return false;
    }
    return !strcmp(str + str_len - suffix_len, suffix);
}
