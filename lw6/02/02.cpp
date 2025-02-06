#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <string.h>

#define BYTE_COUNT 256

typedef struct {
    unsigned char *data;
    size_t start;
    size_t end;
    size_t length;
    int histogram[BYTE_COUNT];
} ThreadArg;

// Потоковая функция для подсчёта частичной гистограммы
void *compute_histogram(void *arg) {
    ThreadArg *thread_arg = (ThreadArg *)arg;
    unsigned char *data = thread_arg->data + thread_arg->start;
    size_t length = thread_arg->end - thread_arg->start;

    // Инициализация локальной гистограммы
    memset(thread_arg->histogram, 0, sizeof(thread_arg->histogram));

    // Подсчёт частот
    for (size_t i = 0; i < length; i++) {
        thread_arg->histogram[data[i]]++;
    }
    return nullptr;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s FILE_NAME NUM_THREADS\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *file_name = argv[1];
    int num_threads = atoi(argv[2]);

    if (num_threads <= 0) {
        fprintf(stderr, "NUM_THREADS must be a positive integer.\n");
        return EXIT_FAILURE;
    }

    // Открываем файл
    int fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Получаем размер файла
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("Error determining file size");
        close(fd);
        return EXIT_FAILURE;
    }

    // Отображаем файл в память
    unsigned char *fileData = static_cast<unsigned char *>(mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (fileData == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return EXIT_FAILURE;
    }

    // Закрываем файловый дескриптор, так как он больше не нужен
    close(fd);

    // Создаём потоки
    pthread_t threads[num_threads];
    ThreadArg thread_args[num_threads];
    size_t segment_size = file_size / num_threads;
    size_t remaining = file_size % num_threads;

    // Инициализируем потоки
    for (int i = 0; i < num_threads; i++) {
        thread_args[i].data = fileData;
        thread_args[i].start = i * segment_size;
        thread_args[i].end = (i + 1) * segment_size;
        if (i == num_threads - 1) {
            thread_args[i].end += remaining; // Последний поток обрабатывает остаток
        }
        thread_args[i].length = thread_args[i].end - thread_args[i].start;

        pthread_create(&threads[i], nullptr, compute_histogram, &thread_args[i]);
    }

    // Ожидаем завершения потоков
    int final_histogram[BYTE_COUNT] = {0};
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], nullptr);

        // Объединяем частичные гистограммы
        for (int j = 0; j < BYTE_COUNT; j++) {
            final_histogram[j] += thread_args[i].histogram[j];
        }
    }

    // Выводим итоговую гистограмму
    for (int i : final_histogram) {
        printf("%d\n", i);
    }

    // Освобождаем память
    munmap(fileData, file_size);
    return EXIT_SUCCESS;
}
