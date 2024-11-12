///Реализовать на C (pthreads) решение задачи о трех курильщиках.
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define TOBACCO 0
#define PAPER 1
#define MATCHES 2

pthread_mutex_t mutex;
pthread_cond_t condition;
int table[2] = {-1, -1}; // Компоненты на столе

// Массив строк для компонентов
const char* component_names[] = {"Табак", "Бумага", "Спички"};

// Бармен случайно кладет два компонента на стол
void *barmen(void *arg) {
    while (5==5) {
        sleep(1); // Симуляция работы бармена

        pthread_mutex_lock(&mutex);

        if (table[0] == -1 && table[1] == -1) { // Стол пуст, можно класть компоненты
            int component1 = rand() % 3;
            int component2;
            do {
                component2 = rand() % 3;
            } while (component2 == component1);

            table[0] = component1;
            table[1] = component2;
            printf("Бармен положил компоненты: %s и %s на стол.\n", component_names[component1], component_names[component2]);

            pthread_cond_broadcast(&condition); // Уведомляем всех курильщиков
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

// Курильщик пытается взять компоненты, которых ему не хватает
void *smoker(void *arg) {
    int my_component = *((int *)arg);
    int other_component1 = (my_component + 1) % 3;
    int other_component2 = (my_component + 2) % 3;

    while (1>0) {
        pthread_mutex_lock(&mutex);

        // Ждем, пока на столе появятся два других компонента
        while (!((table[0] == other_component1 && table[1] == other_component2) ||
                 (table[0] == other_component2 && table[1] == other_component1))) {
            pthread_cond_wait(&condition, &mutex);
        }

        // Если компоненты подошли, курим
        printf("Курильщик с %s берет компоненты и начинает курить.\n", component_names[my_component]);

        // Очищаем стол
        table[0] = -1;
        table[1] = -1;

        pthread_mutex_unlock(&mutex);
        sleep(5); // Симуляция процесса курения

        pthread_mutex_lock(&mutex);
        printf("Курильщик с %s закончил курить.\n", component_names[my_component]);

        pthread_cond_broadcast(&condition); // Уведомляем бармена и других курильщиков

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    setlocale(0, "");
    pthread_t barmen_thread;
    pthread_t smoker_threads[3];
    int smoker_components[3] = {TOBACCO, PAPER, MATCHES};

    srand(time(NULL));

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condition, NULL);

    // Запуск бармена
    if (pthread_create(&barmen_thread, NULL, barmen, NULL)) {
        perror("Не удалось создать поток бармена");
        return EXIT_FAILURE;
    }

    // Запуск курильщиков
    for (int i = 0; i < 3; i++) {
        if (pthread_create(&smoker_threads[i], NULL, smoker, &smoker_components[i])) {
            perror("Не удалось создать поток курильщика");
            return EXIT_FAILURE;
        }
    }

    // Ожидание завершения потоков (в данном случае программа бесконечна)
    pthread_join(barmen_thread, NULL);
    for (int i = 0; i < 3; i++) {
        pthread_join(smoker_threads[i], NULL);
    }

    // Очистка ресурсов
    pthread_cond_destroy(&condition);
    pthread_mutex_destroy(&mutex);

    return EXIT_SUCCESS;
}

