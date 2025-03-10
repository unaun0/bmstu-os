#include <stdio.h>
#include <stdlib.h>

#define CLIENT_COUNT 52

// Функция для чтения данных из файла
void read_data_from_file(const char *filename, int data[CLIENT_COUNT]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < CLIENT_COUNT; i++) {
        if (fscanf(file, "%d", &data[i]) != 1) {
            fprintf(stderr, "Error reading data from file %s\n", filename);
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);
}

int main() {
    int fork_data[CLIENT_COUNT];
    int pthread_data[CLIENT_COUNT];
    int select_data[CLIENT_COUNT];

    // Чтение данных из файлов
    read_data_from_file("analyze/fork_client.txt", fork_data);
    read_data_from_file("analyze/pthread_client.txt", pthread_data);
    read_data_from_file("analyze/select_client.txt", select_data);

    // Вывод заголовка таблицы
    printf("+--------+------------+-------------+------------+\n");
    printf("| Client |    Fork    |   Pthread   |   Select   |\n");
    printf("+--------+------------+-------------+------------+\n");

    int fork_sum = 0, pthread_sum = 0, select_sum = 0;
    for (int i = 0; i < CLIENT_COUNT; i++) {
        // Вывод строки таблицы с выравниванием
        printf("| %-6d | %-10d | %-11d | %-10d |\n", i + 1, fork_data[i], pthread_data[i], select_data[i]);
        
        // Суммирование для расчета среднего значения
        fork_sum += fork_data[i];
        pthread_sum += pthread_data[i];
        select_sum += select_data[i];
    }

    // Вывод итогов с красивым форматированием
    printf("+--------+------------+-------------+------------+\n");
    printf("|  Avg   | %-9.2f  | %-9.2f   | %-9.2f  |\n", 
           (float)fork_sum / CLIENT_COUNT, 
           (float)pthread_sum / CLIENT_COUNT, 
           (float)select_sum / CLIENT_COUNT);
    printf("+--------+------------+-------------+------------+\n");
    return 0;
}
