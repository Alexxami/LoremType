#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#define COLOR_DIM 8
#define COLOR_NEXT 9

// Función para convertir un valor hexadecimal a un valor RGB en el rango de 0-1000
void hex_to_rgb(int hex, int *r, int *g, int *b) {
    *r = ((hex >> 16) & 0xFF) * 1000 / 255;
    *g = ((hex >> 8) & 0xFF) * 1000 / 255;
    *b = (hex & 0xFF) * 1000 / 255;
}

void init_colors() {
    start_color();

    // Definir colores personalizados
    int r, g, b;

    // Fondo (#1e1e2e)
    hex_to_rgb(0x1e1e2e, &r, &g, &b);
    init_color(10, r, g, b); // Color personalizado para el fondo

    // Verde (#a6e3a1)
    hex_to_rgb(0xa6e3a1, &r, &g, &b);
    init_color(11, r, g, b); // Color personalizado para el verde

    // Rojo (#f38ba8)
    hex_to_rgb(0xf38ba8, &r, &g, &b);
    init_color(12, r, g, b); // Color personalizado para el rojo

    // Amarillo (#f9e2af)
    hex_to_rgb(0xf9e2af, &r, &g, &b);
    init_color(13, r, g, b); // Color personalizado para el amarillo

    // Inicializar pares de colores
    init_pair(1, COLOR_WHITE, 10);  // Texto claro sobre fondo personalizado
    init_pair(2, COLOR_WHITE, 10);  // Texto correcto (blanco) sobre fondo personalizado
    init_pair(3, 12, 10);           // Texto incorrecto (rojo personalizado) sobre fondo personalizado
    init_pair(4, 11, 10);           // Texto correcto (verde personalizado) sobre fondo personalizado
    init_pair(5, 13, 10);           // Amarillo personalizado para el contador
    init_pair(COLOR_DIM, COLOR_BLACK, COLOR_WHITE);  // Botón "next" apagado
    init_pair(COLOR_NEXT, COLOR_BLACK, 11);          // Botón "next" activo (fondo verde personalizado)

    // Establecer el fondo de toda la pantalla
    bkgd(COLOR_PAIR(1)); // Usar el par de colores 1 (fondo personalizado)
}

void print_centered(int y, const char *text) {
    int x = (COLS - strlen(text)) / 2; // Calcular la posición horizontal para centrar el texto
    mvprintw(y, x, "%s", text);
}

void print_text_with_feedback(const char *target, const char *input, int start_y, int start_x) {
    int len = strlen(input);
    for (int i = 0; i < len; i++) {
        if (i < strlen(target) && input[i] == target[i]) {
            attron(COLOR_PAIR(4)); // Verde personalizado para caracteres correctos
            mvaddch(start_y, start_x + i, input[i]);
            attroff(COLOR_PAIR(4));
        } else {
            attron(COLOR_PAIR(3) | A_UNDERLINE); // Rojo personalizado y subrayado para caracteres incorrectos
            mvaddch(start_y, start_x + i, input[i]);
            attroff(COLOR_PAIR(3) | A_UNDERLINE);
        }
    }
    if (len < strlen(target)) {
        attron(COLOR_PAIR(1)); // Blanco para el texto restante
        mvprintw(start_y, start_x + len, "%s", target + len);
        attroff(COLOR_PAIR(1));
    }
}

int main() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(1);

    // Verificar si la terminal soporta colores personalizados
    if (!has_colors() || !can_change_color()) {
        endwin();
        printf("Tu terminal no soporta colores personalizados.\n");
        return 1;
    }

    init_colors();

    const char *texts[] = {
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit.",
        "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.",
        "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris.",
        "Nunc euismod orci non viverra ornare. Vivamus sit amet tellus hendrerit, molestie purus et, hendrerit nunc.",
        "Curabitur pharetra massa mollis arcu mollis, ac condimentum augue pellentesque.",
        "Integer mauris mauris, auctor lacinia sollicitudin in, fringilla vitae tellus.",
        "Morbi eleifend at lectus sed pretium. Nunc posuere tempus pharetra.",
        "Donec viverra sollicitudin neque sed venenatis.",
        "Nullam molestie pulvinar dolor non euismod.",
        "Nullam tempor, massa id imperdiet lacinia, turpis velit feugiat ipsum, at fermentum nisi erat eu nisi.",
    };
    int text_count = sizeof(texts) / sizeof(texts[0]);
    int current_text = 0;

    while (current_text < text_count) {
        clear();
        const char *target = texts[current_text];
        char input[256] = {0};
        int len = 0;
        int finished = 0; // Bandera para indicar si el texto se ha completado correctamente

        // Calcular la posición vertical y horizontal para centrar el texto
        int start_y = LINES / 2 - 1; // Centrar verticalmente
        int start_x = (COLS - strlen(target)) / 2; // Centrar horizontalmente

        // Mostrar el contador de caracteres en la parte superior (en amarillo personalizado)
        attron(COLOR_PAIR(5));
        mvprintw(0, 0, "%d/%d", len, strlen(target));
        attroff(COLOR_PAIR(5));

        // Mostrar el texto objetivo centrado
        attron(COLOR_PAIR(1));
        print_centered(start_y, target);
        attroff(COLOR_PAIR(1));

        while (1) {
            // Mover el cursor a la posición actual del texto ingresado
            move(start_y, start_x + len);

            int ch = getch();
            if (!finished) { // Solo procesar entrada si no se ha completado el texto
                if (ch == 127 || ch == KEY_BACKSPACE) {
                    if (len > 0) {
                        len--;
                        input[len] = '\0';
                    }
                } else if (ch >= 32 && ch <= 126) {
                    if (len < strlen(target)) { // Permitir escribir solo si no se ha alcanzado la longitud del texto objetivo
                        input[len] = ch;
                        len++;
                        input[len] = '\0';
                    }
                }
            }

            // Limpiar la línea del texto ingresado
            move(start_y, start_x);
            clrtoeol();

            // Mostrar el texto con feedback de colores
            print_text_with_feedback(target, input, start_y, start_x);

            // Actualizar el contador de caracteres (en amarillo personalizado)
            attron(COLOR_PAIR(5));
            mvprintw(0, 0, "%d/%d", len, strlen(target));
            attroff(COLOR_PAIR(5));

            // Verificar si el texto se ha completado correctamente
            if (len == strlen(target)) {
                if (strcmp(input, target) == 0) {
                    finished = 1; // Marcar como completado
                    // Mostrar el botón "Next" activo
                    attron(COLOR_PAIR(COLOR_NEXT));
                    print_centered(start_y + 2, "[ Next ]");
                    attroff(COLOR_PAIR(COLOR_NEXT));
                    refresh();

                    // Esperar a que el usuario presione Enter para continuar
                    while (1) {
                        ch = getch();
                        if (ch == '\n') {
                            break;
                        }
                    }
                    break; // Pasar al siguiente texto
                } else {
                    // Si hay errores, mostrar el botón "Next" apagado
                    attron(COLOR_PAIR(COLOR_DIM));
                    print_centered(start_y + 2, "[ Next ]");
                    attroff(COLOR_PAIR(COLOR_DIM));
                }
            } else {
                // Si no se ha completado el texto, mostrar el botón "Next" apagado
                attron(COLOR_PAIR(COLOR_DIM));
                print_centered(start_y + 2, "[ Next ]");
                attroff(COLOR_PAIR(COLOR_DIM));
            }

            refresh();
        }
        current_text++;
    }

    endwin();
    return 0;
}
