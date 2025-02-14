/*
 * Для компиляции необходимо добавить ключ -lncurses
 * gcc -o snake main.c -lncurses
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <inttypes.h>
#include <wchar.h>

enum {LEFT=1, UP, RIGHT, DOWN, STOP_GAME='q'};
enum {MAX_TAIL_SIZE=1000, START_TAIL_SIZE=2, MAX_FOOD_SIZE=20, FOOD_EXPIRE_SECONDS=16, MAX_SPEED=200, SPEED=20000, SEED_NUMBER=6};

/*
 Хвост этто массив состоящий из координат x,y
 */
struct tail {
    int x;
    int y;
} tail[MAX_TAIL_SIZE];

/*
 Еда массив точек
 x, y - координата где установлена точка
 put_time - время когда данная точка была установлена
 point - внешний вид точки ('$','E'...)
 enable - была ли точка съедена
 */
struct food {
    int x;
    int y;
    int seed_number;
    int speed_expire;
    time_t put_time;
    char point;
    uint8_t enable;
} food[MAX_FOOD_SIZE];

/*
 Голова змейки содержит в себе
 x,y - координаты текущей позиции
 direction - направление движения
 tsize - размер хвоста
 *tail -  ссылка на хвост
 */
struct snake {
    int x;
    int y;
    int speed;
    int direction;
    size_t tsize;
    struct tail *tail;
} snake;

/*
 Движение головы с учетом текущего направления движения
 */
void go(struct snake *head) {
    char ch[]="@";
    int max_x=0, max_y=0;
    getmaxyx(stdscr, max_y, max_x); // macro - размер терминала
    //clear(); // очищаем весь экран
    mvprintw(head->y, head->x, " "); // очищаем один символ
    switch (head->direction) {
        case LEFT:
            if(head->x <= 0) // Циклическое движение, что бы не
                // уходить за пределы экрана
                head->x = max_x;
            mvprintw(head->y, --(head->x), ch);
            break;
        case RIGHT:
            if(head->x >= max_x)
                head->x = 0;
            mvprintw(head->y, ++(head->x), ch);
            break;
        case UP:
            if(head->y <= 1)
                head->y = max_y;
            mvprintw(--(head->y), head->x, ch);
            break;
        case DOWN:
            if(head->y >= max_y)
                head->y = 0;
            mvprintw(++(head->y), head->x, ch);
            break;
        default:
            break;
    }
    refresh();
}
void changeDirection(int32_t *new_direction, int32_t key) {
    switch (key) {
        case KEY_DOWN: // стрелка вниз
            *new_direction = DOWN;
            break;
        case KEY_UP: // стрелка вверх
            *new_direction = UP;
            break;
        case KEY_LEFT: // стрелка влево
            *new_direction = LEFT;
            break;
        case KEY_RIGHT: // стрелка вправо
            *new_direction = RIGHT;
            break;
        default:
            break;
    }
}

int checkDirection(int32_t dir, int32_t key)
{
    if(KEY_DOWN == key&&dir==UP || KEY_UP == key&&dir==DOWN || KEY_LEFT == key&&dir==RIGHT || KEY_RIGHT == key&&dir==LEFT)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void initTail(struct tail t[], size_t size) {
    struct tail init_t={0,0};
    for(size_t i=0; i<size; i++) {
        t[i]=init_t;
    }
}
void initHead(struct snake *head) {
    head->x = 0;
    head->y = 2;
    head->direction = RIGHT;
}
void initFood(struct food f[], size_t size) {
    struct food init = {0,0,0,0,0};
    int max_y=0, max_x=0;
    getmaxyx(stdscr, max_y, max_x);
    for(size_t i=0; i<size; i++) {
        f[i] = init;
    }
}
void init(struct snake *head, struct tail *tail, size_t size) {
    clear(); // очищаем весь экран
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head);
    head->tail = tail; // прикрепляем к голове хвост
    head->tsize = size+1;
}

/*
 Движение хвоста с учетом движения головы
 */
void goTail(struct snake *head) {
    char ch[] = "*";
    mvprintw(head->tail[head->tsize-1].y, head->tail[head->tsize-1].x, " ");
    for(size_t i = head->tsize-1; i>0; i--) {
        head->tail[i] = head->tail[i-1];
        if( head->tail[i].y || head->tail[i].x)
            mvprintw(head->tail[i].y, head->tail[i].x, ch);
    }
    head->tail[0].x = head->x;
    head->tail[0].y = head->y;
}

/*
 Увеличение хвоста на 1 элемент
 */
void addTail(struct snake *head) {
    if(head == NULL || head->tsize>MAX_TAIL_SIZE) {
        mvprintw(0, 0, "Can't add tail");
        return;
    }
    head->tsize++;
    if(head->speed > 140)
        head->speed -=4;
    else
    if(head->speed >110 && head->speed <= 140)
        head->speed -=3;
    else
    if(head->speed > 80 &&  head->speed <= 110)
        head->speed -=2;
    else
    if(head->speed > 30 && head->speed <=100)
        head->speed--;
    switch(head->tsize-2){
        case 15:
            food->seed_number--;
            food->speed_expire -= 2;
            break;
        case 30:
            food->seed_number--;
            food->speed_expire -= 2;
            break;
        case 45:
            food->seed_number--;
            food->speed_expire -= 2;
            break;
        case 60:
            food->seed_number--;
            food->speed_expire -= 2;
            break;
        default:
            break;
    }
}

void printHelp(char *s) {
    mvprintw(0, 0, s);
}

/*
 Обновить/разместить текущее зерно на поле
 */
void putFoodSeed(struct food *fp) {
    int max_x=0, max_y=0;
    char spoint[2] = {0};
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(fp->y, fp->x, " ");
    fp->x = rand() % (max_x - 1) + 1; //Не занимаем правый столбик
    fp->y = rand() % (max_y - 2) + 1; //Не занимаем верхнюю строку
    fp->put_time = time(NULL);
    fp->point = '$';
    fp->enable = 1;
    spoint[0] = fp->point;
    mvprintw(fp->y, fp->x, spoint);
}
// Мигаем зерном, перед тем как оно исчезнет
void blinkFood(struct food fp[], size_t nfood) {
    time_t current_time = time(NULL);
    char spoint[2] = {0}; // как выглядит зерно '$','\0'
    for( size_t i=0; i<nfood; i++ ) {
        if( fp[i].enable && (current_time - fp[i].put_time) > 6 ) {
            spoint[0] = (current_time % 2)? 'S' : 's';
            mvprintw(fp[i].y, fp[i].x, spoint);
        }
    }
}

void repairSeed(struct food f[], size_t nfood, struct snake *head) {
    for( size_t i=0; i<head->tsize; i++ )
        for( size_t j=0; j<nfood; j++ ){
            /* Если хвост совпадает с зерном */
            if( f[j].x == head->tail[i].x && f[j].y == head->tail[i].y && f[i].enable ) {
                mvprintw(0, 0, "Repair tail seed %d",j);
                putFoodSeed(&f[j]);
            }
        }
    for( size_t i=0; i<nfood; i++ )
        for( size_t j=0; j<nfood; j++ ){
            /* Если два зерна на одной точке */
            if( i!=j && f[i].enable && f[j].enable && f[j].x == f[i].x && f[j].y == f[i].y && f[i].enable ) {
                mvprintw(0, 0, "Repair same seed %d",j);
                putFoodSeed(&f[j]);
            }
        }
}

/*
 Разместить еду на поле
 */
void putFood(struct food f[], size_t number_seeds) {
    for(size_t i=0; i<number_seeds; i++) {
        putFoodSeed(&f[i]);
    }
}
void refreshFood(struct food f[], int nfood) {
    refresh();
    int max_x=0, max_y=0;
    char spoint[2] = {0};
    getmaxyx(stdscr, max_y, max_x);
    for(size_t i=0; i<nfood; i++) {
        if( f[i].put_time ) {
            if( !f[i].enable || (time(NULL) - f[i].put_time) > food->speed_expire ) {
                putFoodSeed(&f[i]);
            }
        }
    }
}
_Bool haveEat(struct snake *head, struct food f[]) {
    for(size_t i=0; i<MAX_FOOD_SIZE; i++)
        if( f[i].enable && head->x == f[i].x && head->y == f[i].y  ) {
            f[i].enable = 0;
            return 1;
        }
    return 0;
}
void printLevel(struct snake *head) {
    int max_x=0, max_y=0;
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(0, max_x-10, "LEVEL: %d", head->tsize - 2);
}
void printExit(struct snake *head) {
    int max_x=0, max_y=0;
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(max_y/2, max_x/2-5, "Your LEVEL is %d", head->tsize - 2);
}
_Bool isCrash(struct snake *head) {
    for(size_t i=1; i<head->tsize; i++)
        if(head->x == head->tail[i].x && head->y == head->tail[i].y)
            return 1;
    return 0;
}
int main()
{
    char ch[]="*";
    int x=0, y=0, key_pressed=0;
    init(&snake, tail, START_TAIL_SIZE); //Инициализация, хвост = 3
    initFood(food, MAX_FOOD_SIZE);
    initscr();            // Старт curses mod
    keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д.
    
    raw();                // Откдючаем line buffering
    noecho();            // Отключаем echo() режим при вызове getch
    curs_set(FALSE);    //Отключаем курсор
    printHelp("  Use arrows for control. Press 'q' for EXIT");
    putFood(food, food->seed_number = SEED_NUMBER);// Кладем зерна
    snake.speed = MAX_SPEED;
    food->speed_expire = FOOD_EXPIRE_SECONDS;
    timeout(0);    //Отключаем таймаут после нажатия клавиши в цикле
    while( key_pressed != STOP_GAME ) {
        key_pressed = getch(); // Считываем клавишу
        if(checkDirection(snake.direction, key_pressed)) //Проверка корректности смены направления
        {
            changeDirection(&snake.direction, key_pressed); // Меняем напарвление движения
        }
        if(isCrash(&snake))
            break;
        go(&snake); // Рисуем новую голову
        goTail(&snake); //Рисуем хвост
        if(haveEat(&snake, food)) {
            addTail(&snake);
            printLevel(&snake);
        }
        refreshFood(food, food->seed_number);// Обновляем еду
        repairSeed(food, food->seed_number, &snake);
        blinkFood(food, food->seed_number);
        timeout(snake.speed); // Задержка при отрисовке
    }
    printExit(&snake);
    timeout(SPEED);
    getch();
    endwin(); // Завершаем режим curses mod
    
    return 0;
}
