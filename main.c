#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "rt.h"
#include "rt_impl.h"
#include "scope.h"
#include "syntax.h"
#include "value.h"
#include "dump.h"

static char* read_to_string(char const *file);
static mscm_value display(mscm_runtime *rt,
                          mscm_scope *scope,
                          void *ctx,
                          size_t narg,
                          mscm_value *args);

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <files...>\n", argv[0]);
        return 1;
    }

    mscm_runtime *rt = runtime_new();
    mscm_value display_fn = mscm_make_native_function(display, 0, 0);
    mscm_runtime_push(rt, "display", (mscm_value)display_fn);
    mscm_runtime_gc_add(rt, display_fn);

    for (int i = 1; i < argc; ++i) {
        char *content = read_to_string(argv[i]);
        if (!content) {
            fprintf(stderr, "error: could not read file %s\n", argv[i]);
            continue;
        }

        mscm_syntax_node node = mscm_parse(argv[i], content);
        if (!node) {
            fprintf(stderr, "error: could not parse file %s\n", argv[i]);
            free(content);
            continue;
        }

        runtime_eval(rt, node, true);
        mscm_free_syntax_node(node);
        free(content);
    }
    runtime_free(rt);

    return 0;
}

static char* read_to_string(char const *file) {
    FILE *f = fopen(file, "r");
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

static mscm_value display(mscm_runtime *rt,
                          mscm_scope *scope,
                          void *ctx,
                          size_t narg,
                          mscm_value *args) {
    (void)rt;
    (void)scope;
    (void)ctx;

    for (size_t i = 0; i < narg; i++) {
        mscm_value_dump(args[i]);
        putchar(' ');
    }
    putchar('\n');
    return 0;
}
