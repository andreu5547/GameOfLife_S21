#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct Cell {
    char old;
    char new;
} Cell;

typedef struct Field {
    Cell **cell;
    int height, width;
} Field;

void print_menu(char *filename);

int init_field(Field *field, int height, int width);

void free_field(Field *field);

int f_load_field(Field *field, FILE *file);

void print_field(Field *field);

int game_loop(Field *field);

void search_life(Field *field);

void backup_field(Field *field);

int count_alive_near(Field *field, int y, int x);

int main(int argc, char *argv[]) {
    const int height = 25, width = 80;
    int error = 0;
    Field field;
    error += init_field(&field, height, width);

    if (argc == 2) {
        if (!error)
            error += f_load_field(&field, stdin);  // Загрузка из stdin
    } else {
        char filename[256];
        print_menu(filename);
        FILE *file = NULL;
        if (!error)
            file = fopen(filename, "r");
        if (!file) error = 1;
        if (!error)
            error += f_load_field(&field, file);
        if (!error)
            fclose(file);
    }
    if (!error)
        game_loop(&field);
    free_field(&field);
    return error;
}

void print_menu(char *filename) {
    printf("=== Игра Жизнь ===\n");
    printf("Выберите начальное состояние игры:\n");
    printf("1. state1.txt\n");
    printf("2. state2.txt\n");
    printf("3. state3.txt\n");
    printf("4. state4.txt\n");
    printf("5. state5.txt\n");
    printf("Введите номер файла или укажите имя: ");
    scanf("%255s", filename);

    if (strcmp(filename, "1") == 0) {
        strcpy(filename, "state1.txt");
    } else if (strcmp(filename, "2") == 0) {
        strcpy(filename, "state2.txt");
    } else if (strcmp(filename, "3") == 0) {
        strcpy(filename, "state3.txt");
    } else if (strcmp(filename, "4") == 0) {
        strcpy(filename, "state4.txt");
    } else if (strcmp(filename, "5") == 0) {
        strcpy(filename, "state5.txt");
    }
}

int init_field(Field *field, int height, int width) {
    field->height = height;
    field->width = width;
    int error = 0;
    field->cell = (Cell **) malloc(height * sizeof(Cell *));
    if (field->cell == NULL) error = 1;
    if (!error)
        for (int i = 0; i < height; ++i) {
            field->cell[i] = (Cell *) malloc(width * sizeof(Cell));
            if (field->cell[i] == NULL) error = 1;
        }
    return error;
}

void free_field(Field *field) {
    for (int i = 0; i < field->height; ++i) {
        if (field->cell[i] != NULL) {
            free(field->cell[i]);
            field->cell[i] = NULL;
        }
    }
    if (field->cell != NULL) {
        free(field->cell);
        field->cell = NULL;
    }
}

int f_load_field(Field *field, FILE *file) {
    int error = 0;
    char buf;
    int cnt_h = 0, cnt_w = 0;
    while ((buf = fgetc(file)) != EOF) {
        if (buf != 10) {
            field->cell[cnt_h][cnt_w].new = buf;
            cnt_w += 1;
        } else {
            cnt_h += 1;
            cnt_w = 0;
        }
        if (cnt_h > field->height || cnt_w > field->width) error = 1;
    }
    return error;
}

void print_field(Field *field) {
    for (int i = 0; i < field->height; ++i) {
        for (int j = 0; j < field->width; ++j) {
            if (field->cell[i][j].new == '1') {
                putchar('O');
            } else {
                putchar(' ');
            }
        }
        putchar(10);
    }
}

int game_loop(Field *field) { // !!!Доделать адекватный вариант выхода
    //char c;
    //while ((c = getchar()) != 'x') {
    while (1) {
        print_field(field);
        search_life(field);
        sleep(1);
        printf("\033[2J");
    }
}

void backup_field(Field *field) {
    for (int i = 0; i < field->height; ++i) {
        for (int j = 0; j < field->width; ++j) {
            field->cell[i][j].old = field->cell[i][j].new;
        }
    }
}

int count_alive_near(Field *field, int y, int x) { // Подсчет количества живых соседей
    int count = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i != 0 || j != 0) {
                int test_y = (y + i + field->height) % field->height;  // Используем "замкнутое" поле
                int test_x = (x + j + field->width) % field->width;
                if (field->cell[test_y][test_x].old == '1') {
                    count += 1;
                }
            }
        }
    }
    return count;
}

void search_life(Field *field) {
    backup_field(field);

    for (int i = 0; i < field->height; ++i) {
        for (int j = 0; j < field->width; ++j) {

            int alive_neighbors = count_alive_near(field, i, j); // Подсчет количества живых соседей

            if (field->cell[i][j].old == '1') { // Клетка живая
                if (alive_neighbors == 2 || alive_neighbors == 3) {
                    field->cell[i][j].new = '1'; // остается жить при 2 ли 3х соседях
                } else {
                    field->cell[i][j].new = '0';  // или смерть
                }
            } else {
                if (alive_neighbors == 3) { // Клетка мертва
                    field->cell[i][j].new = '1'; // зарождается жизнь при 3х соседях
                } else {
                    field->cell[i][j].new = '0';  // или остаётся мёртвой
                }
            }
        }
    }
}
