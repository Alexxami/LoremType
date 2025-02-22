#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#define COLOR_DIM 8
#define COLOR_NEXT 9

// Function to convert a hexadecimal value to an RGB value in the range of 0-1000
void hex_to_rgb(int hex, int *r, int *g, int *b) {
    *r = ((hex >> 16) & 0xFF) * 1000 / 255;
    *g = ((hex >> 8) & 0xFF) * 1000 / 255;
    *b = (hex & 0xFF) * 1000 / 255;
}

void init_colors() {
    start_color();

    // Define custom colors
    int r, g, b;

    // bg (#1e1e2e)
    hex_to_rgb(0x1e1e2e, &r, &g, &b);
    init_color(10, r, g, b); // Custom bg

    // Green (#a6e3a1)
    hex_to_rgb(0xa6e3a1, &r, &g, &b);
    init_color(11, r, g, b);

    // Red (#f38ba8)
    hex_to_rgb(0xf38ba8, &r, &g, &b);
    init_color(12, r, g, b);

    // Yellow (#f9e2af)
    hex_to_rgb(0xf9e2af, &r, &g, &b);
    init_color(13, r, g, b); 
   
    // Initialize color pairs
    init_pair(1, COLOR_WHITE, 10);  // Clear text on custom background
    init_pair(2, COLOR_WHITE, 10);  // Correct text (white) on custom background
    init_pair(3, 12, 10);           // Incorrect text (custom red) on custom background
    init_pair(4, 11, 10);           // Correct text (custom green) on custom background
    init_pair(5, 13, 10);           // Custom yellow for counter
    init_pair(COLOR_DIM, COLOR_BLACK, COLOR_WHITE);  // Buttom "next" (off)
    init_pair(COLOR_NEXT, COLOR_BLACK, 11);          // Buttom "next" (on)
   
    // Set the screen background
    bkgd(COLOR_PAIR(1)); 
}

void print_centered(int y, const char *text) {
    int x = (COLS - strlen(text)) / 2; // Calculate horizontal position to center text
    mvprintw(y, x, "%s", text);
}

void print_text_with_feedback(const char *target, const char *input, int start_y, int start_x) {
    int len = strlen(input);
    for (int i = 0; i < len; i++) {
        if (i < strlen(target) && input[i] == target[i]) {
            attron(COLOR_PAIR(4)); // Green for correct character
            mvaddch(start_y, start_x + i, input[i]);
            attroff(COLOR_PAIR(4));
        } else {
            attron(COLOR_PAIR(3) | A_UNDERLINE); // underline red for incorrect character
            mvaddch(start_y, start_x + i, input[i]);
            attroff(COLOR_PAIR(3) | A_UNDERLINE);
        }
    }
    if (len < strlen(target)) {
        attron(COLOR_PAIR(1)); 
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

    // Check if the terminal supports custom colors
    if (!has_colors() || !can_change_color()) {
        endwin();
        printf("Your terminal does not support custom colors.\n");
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
        int finished = 0; 
        int start_y = LINES / 2 - 1; // Centrar verticalmente
        int start_x = (COLS - strlen(target)) / 2; // Centrar horizontalmente

        // Show character counter at the top (in custom yellow)
        attron(COLOR_PAIR(5));
        mvprintw(0, 0, "%d/%d", len, strlen(target));
        attroff(COLOR_PAIR(5));

        // Show target text centered
        attron(COLOR_PAIR(1));
        print_centered(start_y, target);
        attroff(COLOR_PAIR(1));

        while (1) {
            // Move the cursor to the current position of the entered text
            move(start_y, start_x + len);

            int ch = getch();
            if (!finished) { // Only process input if text has not been completed
                if (ch == 127 || ch == KEY_BACKSPACE) {
                    if (len > 0) {
                        len--;
                        input[len] = '\0';
                    }
                } else if (ch >= 32 && ch <= 126) {
                    if (len < strlen(target)) { // Allow typing only if target text length has not been reached
                        input[len] = ch;
                        len++;
                        input[len] = '\0';
                    }
                }
            }

            // Clear the line of entered text
            move(start_y, start_x);
            clrtoeol();

            // Show text with color feedback
            print_text_with_feedback(target, input, start_y, start_x);

            // Update character counter (in custom yellow)
            attron(COLOR_PAIR(5));
            mvprintw(0, 0, "%d/%d", len, strlen(target));
            attroff(COLOR_PAIR(5));

            // Check if the text has been completed correctly
            if (len == strlen(target)) {
                if (strcmp(input, target) == 0) {
                    finished = 1; // Marcar como completado
                    // Show active "Next" button
                    attron(COLOR_PAIR(COLOR_NEXT));
                    print_centered(start_y + 2, "[ Next ]");
                    attroff(COLOR_PAIR(COLOR_NEXT));
                    refresh();

                    // Wait for the user to press Enter to continue
                    while (1) {
                        ch = getch();
                        if (ch == '\n') {
                            break;
                        }
                    }
                    break; // Skip to next text
                } else {
                    // If there are errors, show the "Next" button off
                    attron(COLOR_PAIR(COLOR_DIM));
                    print_centered(start_y + 2, "[ Next ]");
                    attroff(COLOR_PAIR(COLOR_DIM));
                }
            } else {
                // If the text has not been completed, show the "Next" button off
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
