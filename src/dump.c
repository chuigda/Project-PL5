#include <stdio.h>
#include <inttypes.h>

#include "dump.h"
#include "syntax.h"
#include "value.h"

static void syntax_dump_impl(mscm_syntax_node syntax_node, int indent);
static void print_indent(int indent);

void mscm_sytnax_dump(mscm_syntax_node syntax_node) {
    puts("{");
    syntax_dump_impl(syntax_node, 1);
    puts("}");
}

void mscm_value_dump(mscm_value value) {
    if (!value) {
        printf("null");
        return;
    }

    switch (value->type) {
        case MSCM_TYPE_INT: {
            mscm_int *i = (mscm_int*)value;
            printf("%" PRId64, i->value);
            break;
        }
        case MSCM_TYPE_FLOAT: {
            mscm_float *f = (mscm_float*)value;
            printf("%f", f->value);
            break;
        }
        case MSCM_TYPE_STRING: {
            mscm_string *s = (mscm_string*)value;
            printf("\"%s\"", s->buf);
            break;
        }
        case MSCM_TYPE_SYMBOL: {
            mscm_string *s = (mscm_string*)value;
            printf("'%s", s->buf);
            break;
        }
        case MSCM_TYPE_PAIR: {
            mscm_pair *p = (mscm_pair*)value;
            putchar('(');
            mscm_value_dump(p->fst);
            putchar(' ');
            mscm_value_dump(p->snd);
            putchar(')');
            break;
        }
        case MSCM_TYPE_FUNCTION: {
            printf("<function %p>", (void*)value);
            break;
        }
        case MSCM_TYPE_HANDLE: {
            mscm_handle *h = (mscm_handle*)value;
            if (h->dtor) {
                printf("<handle ptr=%p, dtor=%p, mark=%p>",
                       h->ptr, (void*)h->dtor, (void*)h->marker);
            }
            else {
                printf("<handle ptr=%p>", h->ptr);
            }
            break;
        }
        case MSCM_TYPE_NATIVE: {
            mscm_native_function *f = (mscm_native_function*)value;
            printf("<native function %p, ctx=%p,"
                   " ctx_dtor=%p, ctx_marker=%p>",
                   (void*)f->fnptr, f->ctx,
                   (void*)f->ctx_dtor, (void*)f->ctx_marker);
            break;
        }
        default:
            printf("<unknown value %p>", (void*)value);
            break;
    }
}

static void syntax_dump_impl(mscm_syntax_node node, int indent) {
    if (!node) {
        print_indent(indent);
        puts("(NULL)");
    }

    switch (node->kind) {
        case MSCM_SYN_VALUE: {
            mscm_value_node *value_node = (mscm_value_node*)node;
            print_indent(indent); printf("kind: \"value\",\n");
            print_indent(indent); printf("value: ");
            mscm_value_dump(value_node->value);
            putchar('\n');
            break;
        }
        case MSCM_SYN_IDENT: {
            mscm_ident *ident = (mscm_ident*)node;
            print_indent(indent); printf("kind: \"ident\",\n");
            print_indent(indent);
            printf("ident: \"%s\",\n", ident->ident);
            break;
        }
        case MSCM_SYN_APPLY: {
            mscm_apply *apply = (mscm_apply*)node;
            print_indent(indent); printf("kind: \"apply\",\n");
            print_indent(indent); printf("callee: {\n");
            syntax_dump_impl(apply->callee, indent + 1);
            print_indent(indent); printf("},\n");
            print_indent(indent); printf("args: {\n");
            syntax_dump_impl(apply->args, indent + 1);
            print_indent(indent); printf("},\n");
            break;
        }
        case MSCM_SYN_BEGIN: {
            mscm_begin *begin = (mscm_begin*)node;
            print_indent(indent); printf("kind: \"begin\",\n");
            print_indent(indent); printf("content: {\n");
            syntax_dump_impl(begin->content, indent + 1);
            print_indent(indent); printf("},\n");
            break;
        }
        case MSCM_SYN_LAMBDA: case MSCM_SYN_DEFUN: {
            mscm_func_def *func_def = (mscm_func_def*)node;
            if (node->kind == MSCM_SYN_DEFUN) {
                print_indent(indent); printf("kind: \"defun\",\n");
                print_indent(indent);
                printf("name: \"%s\",\n", func_def->func_name);
            }
            else {
                print_indent(indent); printf("kind: \"lambda\",\n");
            }
            print_indent(indent); printf("param_names: [\n");
            mscm_ident *param_name = func_def->param_names;
            while (param_name) {
                print_indent(indent + 1);
                printf("\"%s\",\n", param_name->ident);
                param_name = (mscm_ident*)param_name->next;
            }
            print_indent(indent); printf("],\n");
            print_indent(indent); printf("body: {\n");
            syntax_dump_impl(func_def->body, indent + 1);
            print_indent(indent); printf("},\n");
            break;
        }
        case MSCM_SYN_COND: {
            mscm_cond *cond = (mscm_cond*)node;
            print_indent(indent); printf("kind: \"cond\",\n");
            print_indent(indent); printf("cond_list: {\n");
            syntax_dump_impl(cond->cond_list, indent + 1);
            print_indent(indent); printf("},\n");
            print_indent(indent); printf("then_list: {\n");
            syntax_dump_impl(cond->then_list, indent + 1);
            print_indent(indent); printf("},\n");
            break;
        }
        case MSCM_SYN_IF: {
            mscm_if *if_ = (mscm_if*)node;
            print_indent(indent); printf("kind: \"if\",\n");
            print_indent(indent); printf("cond: {\n");
            syntax_dump_impl(if_->cond, indent + 1);
            print_indent(indent); printf("},\n");
            print_indent(indent); printf("then: {\n");
            syntax_dump_impl(if_->then, indent + 1);
            print_indent(indent); printf("},\n");
            print_indent(indent); printf("otherwise: {\n");
            syntax_dump_impl(if_->otherwise, indent + 1);
            print_indent(indent); printf("},\n");
            break;
        }
    }

    if (node->next) {
        print_indent(indent); printf("next: {\n");
        syntax_dump_impl(node->next, indent + 1);
        print_indent(indent); printf("},\n");
    }
}

static void print_indent(int indent) {
    for (int i = 0; i < indent; ++i) {
        printf("    ");
    }
}
