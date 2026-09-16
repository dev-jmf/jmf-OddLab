#include "tree_n.h"
#include "web_server.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 960
#define HEIGHT 640

static void draw_text(Display *display, Window window, GC gc, int x, int y,
                      const char *text)
{
    XDrawString(display, window, gc, x, y, text, (int)strlen(text));
}

static void draw_tree(Display *display, Window window, GC gc, const TreeNode *tree,
                      int x, int y, int width, int level)
{
    size_t i;
    int child_width;

    if (tree == NULL) {
        return;
    }
    XFillArc(display, window, gc, x - 8, y - 8, 16, 16, 0, 360 * 64);
    child_width = tree->child_count == 0 ? width : width / (int)tree->child_count;
    for (i = 0; i < tree->child_count; ++i) {
        int child_x = x - width / 2 + child_width / 2 + (int)i * child_width;
        int child_y = y + 64;
        XDrawLine(display, window, gc, x, y + 8, child_x, child_y - 8);
        draw_tree(display, window, gc, tree->children[i], child_x, child_y,
                  child_width, level + 1);
    }
    (void)level;
}

static int read_n(const char *input)
{
    char *end;
    long value = strtol(input, &end, 10);
    if (end == input || *end != '\0' || value < 1 || value > 12) {
        return 0;
    }
    return (int)value;
}

int main(void)
{
    const char *display_name = getenv("DISPLAY");
    Display *display = XOpenDisplay(display_name);
    Window window;
    GC gc;
    XEvent event;
    TreeSimulation simulation;
    char input[16] = "3";
    char status[160] = "Digite n (1-12) e clique em Executar";
    int running = 1;

    if (display == NULL) {
        if (getenv("CODESPACES") != NULL && strcmp(getenv("CODESPACES"), "true") == 0) {
            return web_server_run(8080);
        }
        fprintf(stderr, "Nao foi possivel abrir o display X11.\n");
        if (display_name == NULL || display_name[0] == '\0') {
            fprintf(stderr, "A variavel DISPLAY nao esta definida. Execute em uma sessao grafica ou configure um servidor X11.\n");
        } else {
            fprintf(stderr, "DISPLAY=%s nao esta acessivel.\n", display_name);
        }
        return EXIT_FAILURE;
    }
    window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0,
                                 WIDTH, HEIGHT, 1, BlackPixel(display, 0),
                                 WhitePixel(display, 0));
    XStoreName(display, window, "TREE(n) - simulador exploratorio");
    XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask |
                                      StructureNotifyMask);
    XMapWindow(display, window);
    gc = XCreateGC(display, window, 0, NULL);
    simulation_init(&simulation);

    while (running) {
        XNextEvent(display, &event);
        if (event.type == DestroyNotify) {
            running = 0;
        } else if (event.type == KeyPress) {
            char key[8] = {0};
            KeySym symbol;
            int length = XLookupString(&event.xkey, key, sizeof(key) - 1, &symbol, NULL);
            if (symbol == XK_Return) {
                int n = read_n(input);
                if (n > 0 && simulation_run(&simulation, n)) {
                    snprintf(status, sizeof(status), "n=%d | arvores aceitas: %zu | candidatos: %zu",
                             n, simulation.count, simulation.candidates_checked);
                } else {
                    snprintf(status, sizeof(status), "Use um valor inteiro entre 1 e 12");
                }
            } else if (symbol == XK_BackSpace && strlen(input) > 0) {
                input[strlen(input) - 1] = '\0';
            } else if (length == 1 && key[0] >= '0' && key[0] <= '9' && strlen(input) < 2) {
                size_t length_now = strlen(input);
                if (length_now == 1 && input[0] == '0') input[0] = '\0';
                input[strlen(input)] = key[0];
                input[strlen(input) + 1] = '\0';
            }
        } else if (event.type == ButtonPress && event.xbutton.x >= 150 &&
                   event.xbutton.x <= 300 && event.xbutton.y >= 70 &&
                   event.xbutton.y <= 115) {
            int n = read_n(input);
            if (n > 0 && simulation_run(&simulation, n)) {
                snprintf(status, sizeof(status), "n=%d | arvores aceitas: %zu | candidatos: %zu",
                         n, simulation.count, simulation.candidates_checked);
            } else {
                snprintf(status, sizeof(status), "Use um valor inteiro entre 1 e 12");
            }
        } else if (event.type == Expose) {
            XClearWindow(display, window);
            draw_text(display, window, gc, 40, 42, "TREE(n) - simulador exploratorio");
            draw_text(display, window, gc, 40, 92, "n:");
            XDrawRectangle(display, window, gc, 65, 72, 65, 32);
            draw_text(display, window, gc, 75, 94, input);
            XDrawRectangle(display, window, gc, 150, 70, 150, 45);
            draw_text(display, window, gc, 180, 98, "Executar");
            draw_text(display, window, gc, 40, 145, status);
            draw_text(display, window, gc, 40, 180, "Cada circulo representa uma arvore aceita; cores representam rotulos.");
            if (simulation.count > 0) {
                size_t i;
                for (i = 0; i < simulation.count && i < 40; ++i) {
                    int column = (int)(i % 10);
                    int row = (int)(i / 10);
                    int x = 110 + column * 90;
                    int y = 255 + row * 150;
                    draw_tree(display, window, gc, simulation.trees[i], x, y,
                              90, 0);
                }
            }
        }
    }
    simulation_clear(&simulation);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return EXIT_SUCCESS;
}