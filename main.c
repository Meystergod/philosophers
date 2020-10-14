#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

/**
 *\brief Структура, описывающая философа
 *\Поля: Имя, время потребеления пищи
 **/
typedef struct philosopher
{
    char* phillname;
    int timetoeat;
} philosopher_t;

/**
 *\brief Структура, описывающая стол
 *\Поля: Массив вилок и массив стульев, представленные мьютексами
 **/
typedef struct table
{
    pthread_mutex_t chairs[5];
    pthread_mutex_t forks[5];
} table_t;

/**
 *\brief Структура для передачи параметров в поток
 **/
typedef struct philosopher_arguments
{
    philosopher_t *philosopher;
    table_t *table;
} philosopher_arguments_t;

/**
 *\brief Функция инициализации философа
 **/
void init_philosopher(philosopher_t *philosopher, char *phillname_, int timetoeat_)
{
    philosopher->phillname = phillname_;
    philosopher->timetoeat = timetoeat_;
}

/**
 *\brief Функция инициализации стола
 **/
void init_table(table_t *table, const int num_of_phills)
{
    for (int i = 0; i < num_of_phills; i++)
    {
        pthread_mutex_init(&table->forks[i], NULL);
    }
    for (int i = 0; i < num_of_phills; i++)
    {
        pthread_mutex_init(&table->chairs[i], NULL);
    }
}

/**
 *\brief Функция, моделирующая ужин философов
 **/
void* eating(void *args)
{
    philosopher_arguments_t *arg = (philosopher_arguments_t*) args;
    philosopher_t *philosopher = arg->philosopher;
    table_t *table = arg->table;

    while (true)
    {
        time_t t;
        int left_fork = 0;
        int right_fork = 0;

        left_fork = rand() % 5;
        if (left_fork != 4)
        {
            right_fork = left_fork + 1;
        }


        printf("%s вошел в столовую и собирается ужинать. Ему необходимы вилки №%d и №%d\n",
        philosopher->phillname, left_fork, right_fork);
        pthread_mutex_lock(&table->chairs[left_fork]);
        pthread_mutex_lock(&table->forks[left_fork]);
        pthread_mutex_lock(&table->forks[right_fork]);


        printf("%s начинает обедать, используя вилки №%d и №%d\n",
        philosopher->phillname, left_fork, right_fork);

        struct timespec passTime;
        passTime.tv_sec = philosopher->timetoeat;
        passTime.tv_nsec = 0;
        nanosleep(&passTime, NULL);
        pthread_mutex_unlock(&table->forks[left_fork]);
        pthread_mutex_unlock(&table->forks[right_fork]);
        pthread_mutex_unlock(&table->chairs[left_fork]);

        printf("%s закончил обедать и положил вилки №%d и №%d\n",
        philosopher->phillname, left_fork, right_fork);
        passTime.tv_sec = 6;
        passTime.tv_nsec = 0;
        nanosleep(&passTime, NULL);
    }
}

/**
 *\brief Главная функция
 **/
int main()
{
    int timetoeat_;
    char check = '\0';
    const int num_of_phills = 5;
    int i;

    printf ("Введите время, выделенное на обед философа (в секундах): ");
    while (scanf("%d%c", &timetoeat_, &check) != 2 || timetoeat_ <= 0 || check != '\n')
    {
        printf("Некорректное значение. Попробуйте снова: ");
        rewind(stdin);
    }

    pthread_t threads[num_of_phills];
    philosopher_t philosophers[num_of_phills];
    philosopher_arguments_t arguments[num_of_phills];
    table_t table;
    init_table(&table, num_of_phills);
    init_philosopher(&philosophers[0], "1 философ", timetoeat_);
    init_philosopher(&philosophers[1], "2 философ", timetoeat_);
    init_philosopher(&philosophers[2], "3 философ", timetoeat_);
    init_philosopher(&philosophers[3], "4 философ", timetoeat_);
    init_philosopher(&philosophers[4], "5 философ", timetoeat_);

    for (i = 0; i < num_of_phills; i++)
    {
        arguments[i].philosopher = &philosophers[i];
        arguments[i].table = &table;
    }

    for (i = 0; i < num_of_phills; i++)
    {
        pthread_create(&threads[i], NULL, eating, &arguments[i]);
    }

    for (i = 0; i < num_of_phills; i++)
    {
        pthread_join(threads[i], NULL); 
    }
    return 0;
}
