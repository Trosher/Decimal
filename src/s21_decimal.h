#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    unsigned int bits[4];
    // 0 - 3 ячейки для записи числа
    // 4 коэфицент масштабирования [16 - 23] и знак [31] остальное заполненно 0
    // Коэфицент: число от 0 до 28. Знак: 0 - положительное, 1 - отрицательное
} s21_decimal;

// 224 bits
typedef struct {
    unsigned int bits[7];
} long_decimal;

#define s21_EXPON 0x7f800000
#define s21_MANTIS 0x7fffff
#define s21_EXPON2 0xff0000
#define MAX_F (int)pow(2, 22)
#define M_BIT 0x80000000
#define MAX_LONG 223
#define MAX_LONG_S 7

// Функции прямой работы с битами

/* Установить бит по индексу. Принимает указатель на unsigned, позицию и бит(0, 1).*/
void setbit(unsigned int *value, const int position, const int bit);
/* Узнать бит по индексу. Принимает unsigned число и позицию.*/
int checkbit(const int value, const int position);
/* Сравнить биты на позиции. Принимает два unsigned числа и позицию.*/
int equalbit(unsigned int a, unsigned int b, int posicion);
/* Сдвигает всю конструкцию на указанное колличество битов в большую сторону.
Принимает указатель на s21_decimal сколько ячеек двигать (0, 4) и размер сдвига.*/
void sdvig_up(s21_decimal *str, const int max, const int step);
/* Сдвигает всю конструкцию на указанное колличество битов в большую сторону.
Принимает указатель на long_decimal и размер сдвига.*/
void sdvig_up_long(long_decimal *str, const int step);
/* Делает битовый сдвиг в меньшую сторону в структуре по указанному колличеству ячеек на указзаный шаг.
Принимает структуру s21_decimal и размер сдвига.*/
void sdvig_down(s21_decimal *str, const int max, const int step);
/* Перевод строку ввиде "11101010011" в структуру s21_decimal. Недостоющие биты заполняются 0.*/
void char_from_decim(char *c, s21_decimal *a);
/* Возвращате маску заполненную 1 со start до end позиции (start < end && start || end == (0, 31)).*/
unsigned int mask(const int start, const int end);

// Вспомогательные арифметические функции

/* Сложение двух структур s21_decimal и запись из в третью.
Сложение выполняется при идеальных условиях, без учёта знака, степени по указанному количеству битов.
Возвращает 1 если число выходит за допустимый диапазон.*/
int sum_r(s21_decimal a, s21_decimal b, s21_decimal *res, int max);
/* Сложение двух структур long_decimal и запись из в третью.
Сложение выполняется при идеальных условиях, без учёта знака, степени по указанному количеству битов.
Возвращает 1 если число выходит за допустимый диапазон.*/
int sum_long(long_decimal a, long_decimal b, long_decimal *res);
/* Вычитание двух структур s21_decimal и запись из в третью.
Вычитание выполняется при идеальных условиях a > b, без учёта знака, степени по указанному количеству битов.*/
void sub_r(s21_decimal a, s21_decimal b, s21_decimal *res, int max);
/* Вычитание двух структур long_decimal и запись из в третью.
Вычитание выполняется при идеальных условиях a > b, без учёта знака, степени по указанному количеству битов.*/
void sub_long(long_decimal a, long_decimal b, long_decimal *res);
/* Основное тело деления. Выполняет деление вычитанием с домножением на 10 при необходимости.
flag принимает значения 1 - вернуть остаток от деления, 0 - полностью поделить
Возвращает 1 если число выходит за допустимый диапазон и число положительное, или 2 если отрицательное.*/
int div_r(s21_decimal a, s21_decimal b, s21_decimal *c, int flag);
/* Умножает переданную структуру на 10 по указанному количеству ячеек (1, 3) указанное колличество раз.*/
void mul_ten(s21_decimal *a, const int max, const int hex);
/* Умножает переданную структуру на 10 указанное колличество раз.*/
void mul_ten_long(long_decimal *a, int hex);
/* Делит переданную структуру на 10 по указанному количеству ячеек (1, 3) указанное колличество раз.*/
void div_ten(s21_decimal *res, int max, int hex);
/* Делит переданную структуру на 10 указанное колличество раз.*/
void div_ten_long(long_decimal *res, int hex);

// Булевые функции

/*Возвращает 1 если value_1 > value_2. Сравнение идёт по указанному количеству битов. Не учитывается знак и степень.*/
int greater(s21_decimal value_1, s21_decimal value_2, int max);
/*Возвращает 1 если value_1 > value_2. Не учитывается знак и степень.*/
int greater_long(long_decimal value_1, long_decimal value_2);
/*Возвращет 1 если указанные ячейки (1, 3) переданной структуры равны 0.*/
int zeros_eq(s21_decimal a, int max);
/*Возвращет 1 если переданная структура равна 0.*/
int zeros_eq_long(long_decimal a);
/*Возвращает 1 если a == b.*/
int is_equal_long(const long_decimal a, const long_decimal b);
/* Возвращает 1 если последние 3 ячейки структуры равны 0.*/
int check_long(long_decimal a);

// Прочие вспомогательные функции

/* Копирует структуру а в b.*/
void craft_decim(const s21_decimal a, s21_decimal *b);
/* Копирует структуру а в b.*/
void craft_long(const long_decimal a, long_decimal *b);
/* Копирует структуры если ch 1 то a = b иначе b = a.*/
void copy_decimal_to_long(s21_decimal *a, long_decimal *b, const int ch);
/* Округляет переданную структуру на указанную разницу exp - n_exp. exp > n_exp.*/
void round_exp(s21_decimal *value, int exp, int n_exp);
/* Сравнивает и уравнивает степени переданных конструкций. Уравнение идёт по указанному колличеству ячеек (1, 3).
Если e1 или e2 равны 0, то степень берётся из структур.*/
void equation_exp(s21_decimal *a, s21_decimal *b, unsigned int e1, unsigned int e2, const int max);
/* Округляет переданную структуру на указанную разницу exp - n_exp. exp > n_exp.*/
void round_exp_long(long_decimal *value, int exp, int n_exp);
/* Сравнивает и уравнивает переданные степени переданных конструкций.*/
void equation_exp_long(long_decimal *a, long_decimal *b, unsigned int e1, unsigned int e2);
/* Возвращает степень деления числа децимал.*/
unsigned i_exp(s21_decimal a);
/*Возвращает первое вхождение значимого бита от указанной позиции i > 0 && i < 127.*/
int search_bits(s21_decimal a, int i);
/*Возвращает первое вхождение значимого бита от указанной позиции i > 0 && i < 191.*/
int search_bits_long(long_decimal a);
/* Очищает степень в переданной конструкции, но оставляет знак.*/
void not_exp(s21_decimal *a);
/* Копирует биты из структуры a в структуру b. Копирование в диапазоне st - end.
st > end && st || end == (0, 127).*/
void mask_div(s21_decimal a, s21_decimal *b, int st, int end);
/* Функция возвращает экспоненту поданного числа флоат*/
int expon_float(float src);

// Прочие функции

/* Печать в консоль битов переданного числа слева 0, направо 31. При check == 1 ставит перенос строки в конце.*/
void print_bits(const unsigned int value, const int check);
/* Печать в консоль битов всей структуры дуцимал слева 0, направо 127.*/
void print_full(const s21_decimal value);
/* Печать в консоль битов всей структуры лонг дуцимал слева 0, направо 191.*/
void print_long(const long_decimal value);
/* Печать структуры децимал в консоль в десятичной системе счисления безе перевода в переменные.
При flag 1 третья ячейка воспринмается как техническая и будет сохраненна степень и знак.*/
void decim_from_char(s21_decimal a, int flag);
/* Печать структуры лонг децимал в консоль в десятичной системе счисления без перевода в переменные.*/
void longdecim_from_char(long_decimal a);


int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_is_less(s21_decimal value_1, s21_decimal value_2);
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);
int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);

#endif // SRC_S21_DECIMAL_H_