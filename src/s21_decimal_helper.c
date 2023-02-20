#include "s21_decimal.h"

const unsigned int SD[32] = {0x80000000, 0xc0000000, 0xe0000000, 0xf0000000, \
0xf8000000, 0xfc000000, 0xfe000000, 0xff000000, 0xff800000, 0xffc00000, \
0xffe00000, 0xfff00000, 0xfff80000, 0xfffc0000, 0xfffe0000, 0xffff0000, \
0xffff8000, 0xffffc000, 0xffffe000, 0xfffff000, 0xfffff800, 0xfffffc00, \
0xfffffe00, 0xffffff00, 0xffffff80, 0xffffffc0, 0xffffffe0, 0xfffffff0, \
0xfffffff8, 0xfffffffc, 0xfffffffe, 0xffffffff};

const s21_decimal ones = {{1, 0, 0, 0}};
const long_decimal onesl = {{1, 0, 0, 0, 0, 0, 0}};
const s21_decimal tens = {{10, 0, 0, 0}};
const long_decimal tensl = {{10, 0, 0, 0, 0, 0, 0}};
const s21_decimal zero = {{0, 0, 0, 0}};
const long_decimal zerol = {{0, 0, 0, 0, 0, 0, 0}};


// Выдаёт маску в заданном диапозоне
unsigned int mask(const int start, const int end) {
    unsigned int res = 0;
    for (int i = start; i <= end; setbit(&res, i, 1), i++) {}
    return res;
}

// Узнать какой бит на позиции начиная с 0
int checkbit(const int value, const int position) {
    return (value & (1 << position)) != 0;
}

// Установить нужный бит на нужную позицию начиная с 0
void setbit(unsigned int *value, const int position, const int bit) {
    *value = bit ? (*value | (1 << position)) : (*value & ~(1 << position));
}

// Сравнение 2 битов на указанной позиции
int equalbit(unsigned int a, unsigned int b, int posicion) {
    return checkbit(a, posicion) == checkbit(b, posicion) ? 1 : 0;
}

// Сдвинуть указанное колличество битов на указанный щаг
void sdvig_up(s21_decimal *str, const int max, const int step) {
    unsigned int temp = 0, temp1 = 0;
    for (int i = 0; i < max; i++) {
        if (!(i % 2)) {
            temp = (str->bits[i] & SD[step - 1]) >> (32 - step);
            str->bits[i] <<= step;
            str->bits[i] += temp1;
        } else {
            temp1 = (str->bits[i] & SD[step - 1]) >> (32 - step);
            str->bits[i] <<= step;
            str->bits[i] += temp;
        }
    }
}

// Сдвинуть указанное колличество битов на указанный щаг
void sdvig_down(s21_decimal *str, const int max, const int step) {
    unsigned int temp = 0, temp1 = 0;
    for (int i = max; i >= 0; i--) {
        if (i % 2) {
            temp = (str->bits[i] & ~SD[31 - step - 1]) << (32 - step);
            str->bits[i] >>= step;
            str->bits[i] += temp1;
        } else {
            temp1 = (str->bits[i] & ~SD[31 - step - 1]) << (32 - step);
            str->bits[i] >>= step;
            str->bits[i] += temp;
        }
    }
}

// Сдвинуть указанное колличество битов на указанный щаг в long decimal
void sdvig_up_long(long_decimal *str, const int step) {
    unsigned int temp = 0, temp1 = 0;
    for (int i = 0; i < MAX_LONG_S; i++) {
        if (!(i % 2)) {
            temp = (str->bits[i] & SD[step - 1]) >> (32 - step);
            str->bits[i] <<= step;
            str->bits[i] += temp1;
        } else {
            temp1 = (str->bits[i] & SD[step - 1]) >> (32 - step);
            str->bits[i] <<= step;
            str->bits[i] += temp;
        }
    }
}

// Умножение на 10 по указанному количеству бит
void mul_ten(s21_decimal *a, const int max, const int hex) {
    for (int i = 0; i < hex; i++) {
        s21_decimal b = *a;
        sdvig_up(a, max, 3);
        sdvig_up(&b, max, 1);
        sum_r(*a, b, a, 32 * max);
    }
}

void mul_ten_long(long_decimal *a, const int hex) {
    for (int i = 0; i < hex; i++) {
        long_decimal b = *a;
        sdvig_up_long(a, 3);
        sdvig_up_long(&b, 1);
        sum_long(*a, b, a);
    }
}

// Копирование первого децимала во второй
void craft_decim(const s21_decimal a, s21_decimal *b) {
    b->bits[0] = a.bits[0];
    b->bits[1] = a.bits[1];
    b->bits[2] = a.bits[2];
    b->bits[3] = a.bits[3];
}

// Копирование первого лонг децимала во второй
void craft_long(const long_decimal a, long_decimal *b) {
    b->bits[0] = a.bits[0];
    b->bits[1] = a.bits[1];
    b->bits[2] = a.bits[2];
    b->bits[3] = a.bits[3];
    b->bits[4] = a.bits[4];
    b->bits[5] = a.bits[5];
    b->bits[6] = a.bits[6];
}

// Функция возвращающая экспоненту в виде инт
unsigned i_exp(s21_decimal a) {
    return (a.bits[3] & s21_EXPON2) >> 16;
}

// Функция приведения степеней
void equation_exp(s21_decimal *a, s21_decimal *b, unsigned int e1, unsigned int e2, const int max) {
    !e1 ? e1 = i_exp(*a) : 0;
    !e2 ? e2 = i_exp(*b) : 0;
    if (e1 > e2)
        mul_ten(b, max, e1 - e2);
    else if (e2 > e1)
        mul_ten(a, max, e2 - e1);
}

// Функция приведения степеней
void equation_exp_long(long_decimal *a, long_decimal *b, unsigned int e1, unsigned int e2) {
    if (e1 > e2)
        mul_ten_long(b, e1 - e2);
    else if (e2 > e1)
        mul_ten_long(a, e2 - e1);
}

// Сумирование степеней
void sum_exp(unsigned int a, unsigned int b, unsigned int *res) {
    unsigned int temp = ((a & s21_EXPON2) >> 16) + ((b & s21_EXPON2) >> 16);
    temp = temp > 28 ? 28 : temp;
    *res = temp << 16;
}

// Сделать битовую маску в децим
void mask_div(s21_decimal a, s21_decimal *b, int st, int end) {
    for (; st >= end; st--) {
        setbit(&b->bits[st / 32], st % 32, checkbit(a.bits[st / 32], st % 32));
    }
}

// Удалить экспоненту оставив знак
void not_exp(s21_decimal *a) {
    a->bits[3] = a->bits[3] & SD[0];
}

// Поиск первого значимого бита с указанной позиции
int search_bits(s21_decimal a, int i) {
    return checkbit(a.bits[i / 32], i % 32) || i == 0 ? i : search_bits(a, i - 1);
}

// Поиск первого значимого бита с указанной позиции
int search_bits_long(long_decimal a) {
    int res = 0, i = MAX_LONG;
    for (; i >= 0 && !res; i -= !res) {
        res = checkbit(a.bits[i / 32], i % 32);
    }
    return i;
}

// Округлить децимал c exp знака до n_exp знака после запятой
void round_exp(s21_decimal *value, int exp, int n_exp) {
    s21_decimal a = *value;
    div_ten(value, 127, n_exp - exp);
    s21_decimal t = *value;
    mul_ten(&t, 4, n_exp - exp);
    sub_r(a, t, &a, 127);
    div_ten(&a, 127, n_exp - exp - 1);
    a.bits[0] > 4 ? sum_r(*value, ones, value, 127) : 0;
}

// Округлить децимал c exp знака до n_exp знака после запятой
void round_exp_long(long_decimal *value, int exp, int n_exp) {
    long_decimal a = *value;
    div_ten_long(value, exp - n_exp);
    long_decimal t = *value;
    mul_ten_long(&t, exp - n_exp);
    sub_long(a, t, &a);
    div_ten_long(&a, exp - n_exp - 1);
    a.bits[0] > 4 ? sum_long(*value, onesl, value) : 0;
}

// Сравнение числа с 0
int zeros_eq(s21_decimal a, const int max) {
    int res = 0;
    setbit(&a.bits[3], 31, 0);
    for (int i = 0; i < max; res += a.bits[i], i++) {}
    return !res;
}

// Сравнение числа с 0
int zeros_eq_long(long_decimal a) {
    int res = 0;
    for (int i = 0; i < MAX_LONG_S; res += a.bits[i], i++) {}
    return !res;
}

// Сравнить два децимала на неравенство value_1 > value_2. Сравнение идёт по указанному колличеству битов
int greater(s21_decimal value_1, s21_decimal value_2, int max) {
    max -= 1;
    int check = 0;
    for (int i = max; i >= 0 && !check; i--) {
        int a = checkbit(value_1.bits[i / 32], i % 32);
        int b = checkbit(value_2.bits[i / 32], i % 32);
        a > b ? check = 1 : a < b ? check = -1 : 0;
    }
    return check > 0;
}

// Сравнить два децимала на неравенство value_1 > value_2. Сравнение идёт по указанному колличеству битов
int greater_long(long_decimal value_1, long_decimal value_2) {
    int check = 0, a = 0, b = 0;
    for (int i = MAX_LONG; i >= 0 && !check; i--) {
        a = checkbit(value_1.bits[i / 32], i % 32);
        b = checkbit(value_2.bits[i / 32], i % 32);
        a > b ? check = 1 : a < b ? check = -1 : 0;
    }
    return check > 0;
}

int is_equal_long(const long_decimal value_1, const long_decimal value_2) {
    int res = 1;
    for (int i = MAX_LONG; i >= 0 && res; i--)
        res = equalbit(value_1.bits[i / 32], value_2.bits[i / 32], i % 32);
    return res;
}

// Идеальное вычитание
void sub_r(s21_decimal a, s21_decimal b, s21_decimal *res, int max) {
    max -= 1;
    for (int i = max; i >= 0; i--) {
        int temp_s = checkbit(a.bits[i / 32], i % 32) - checkbit(b.bits[i / 32], i % 32);
        if (temp_s == 0) {
            setbit(&res->bits[i / 32], i % 32, 0);
        } else if (temp_s == 1) {
            setbit(&res->bits[i / 32], i % 32, 1);
        } else if (temp_s == -1) {
            int j = i, g = 0;
            for (; j <= max && !g;) {
                g = checkbit(res->bits[j / 32], j % 32);
                j++;
            }
            setbit(&res->bits[(j - 1) / 32], (j - 1) % 32, 0);
            for (int z = j - 2; z >= i; z--)
                setbit(&res->bits[z / 32], z % 32, 1);
        }
    }
}

// Идеальное вычитание long decimal
void sub_long(long_decimal a, long_decimal b, long_decimal *res) {
    for (int i = MAX_LONG; i >= 0; i--) {
        int temp_s = checkbit(a.bits[i / 32], i % 32) - checkbit(b.bits[i / 32], i % 32);
        if (temp_s == 0) {
            setbit(&res->bits[i / 32], i % 32, 0);
        } else if (temp_s == 1) {
            setbit(&res->bits[i / 32], i % 32, 1);
        } else if (temp_s == -1) {
            int j = i, g = 0;
            for (; j <= MAX_LONG && !g;) {
                g = checkbit(res->bits[j / 32], j % 32);
                j++;
            }
            setbit(&res->bits[(j - 1) / 32], (j - 1) % 32, 0);
            for (int z = j - 2; z >= i; z--)
                setbit(&res->bits[z / 32], z % 32, 1);
        }
    }
}

// Идеальное сложение long decimal
int sum_long(long_decimal a, long_decimal b, long_decimal *res) {
    *res = zerol;
    int temp = 0, result = 0;
    for (int i = 0; i <= MAX_LONG && !result; i++) {
        int temp_s = checkbit(a.bits[i / 32], i % 32) + checkbit(b.bits[i / 32], i % 32);
        if (temp_s == 2 && temp == 1) {
            setbit(&res->bits[i / 32], i % 32, 1);
            temp = 1;
        } else if (temp_s == 2 && temp == 0) {
            setbit(&res->bits[i / 32], i % 32, 0);
            temp = 1;
        } else if (temp_s == 1 && temp == 1) {
            setbit(&res->bits[i / 32], i % 32, 0);
            temp = 1;
        } else if (temp_s == 1 && temp == 0) {
            setbit(&res->bits[i / 32], i % 32, 1);
        } else if (temp_s == 0 && temp == 1) {
            setbit(&res->bits[i / 32], i % 32, 1);
            temp = 0;
        } else {
            setbit(&res->bits[i / 32], i % 32, 0);
        }
        result = i == MAX_LONG && temp;
    }
    *res = result ? zerol : *res;
    return result;
}

// Идеальное сложение
int sum_r(s21_decimal a, s21_decimal b, s21_decimal *res, int max) {
    int temp = 0, result = 0;
    for (int i = 0; i < max && !result; i++) {
        int temp_s = checkbit(a.bits[i / 32], i % 32) + checkbit(b.bits[i / 32], i % 32);
        if (temp_s == 2 && temp == 1) {
            setbit(&res->bits[i / 32], i % 32, 1);
            temp = 1;
        } else if (temp_s == 2 && temp == 0) {
            setbit(&res->bits[i / 32], i % 32, 0);
            temp = 1;
        } else if (temp_s == 1 && temp == 1) {
            setbit(&res->bits[i / 32], i % 32, 0);
            temp = 1;
        } else if (temp_s == 1 && temp == 0) {
            setbit(&res->bits[i / 32], i % 32, 1);
        } else if (temp_s == 0 && temp == 1) {
            setbit(&res->bits[i / 32], i % 32, 1);
            temp = 0;
        } else {
            setbit(&res->bits[i / 32], i % 32, 0);
        }
        result = i == max - 1 && temp;
    }
    *res = result ? zero : *res;
    return result;
}

// Правильное деление на 10
void div_ten(s21_decimal *res, int max, int hex) {
    unsigned int ch = max == 95 ? res->bits[3] : 0;
    for (int i = 0; i < hex; i++) {
        s21_decimal temp = {{0, 0, 0, 0}}, a = *res;
        while (greater(a, tens, max) || s21_is_equal(a, tens)) {
            s21_decimal te = tens, te2 = tens, on = ones;
            int z = 0;
            while (greater(a, te2, max)) {
                mul_ten(&te2, (max / 32) + 1, 1);
                if (greater(a, te2, max)) {
                    craft_decim(te2, &te);
                    z++;
                }
            }
            mul_ten(&on, (max / 32) + 1, z);
            sub_r(a, te, &a, max);
            sum_r(temp, on, &temp, max);
        }
        craft_decim(temp, res);
    }
    res->bits[3] = max == 95 ? ch : res->bits[3];
}

// Правильное деление на 10
void div_ten_long(long_decimal *res, int hex) {
    for (int i = 0; i < hex; i++) {
        long_decimal temp = zerol, a = *res;
        while (greater_long(a, tensl) || is_equal_long(a, tensl)) {
            long_decimal te = tensl, te2 = tensl, on = onesl;
            int z = 0;
            while (greater_long(a, te2)) {
                mul_ten_long(&te2, 1);
                if (greater_long(a, te2)) {
                    craft_long(te2, &te);
                    z++;
                }
            }
            mul_ten_long(&on, z);
            sub_long(a, te, &a);
            sum_long(temp, on, &temp);
        }
        craft_long(temp, res);
    }
}

// Копирует лонг в децимал 1/ децимал в лонг 0
void copy_decimal_to_long(s21_decimal *a, long_decimal *b, const int ch) {
    if (ch) {
        a->bits[0] = b->bits[0];
        a->bits[1] = b->bits[1];
        a->bits[2] = b->bits[2];
        a->bits[3] = b->bits[3];
    } else {
        b->bits[0] = a->bits[0];
        b->bits[1] = a->bits[1];
        b->bits[2] = a->bits[2];
        b->bits[3] = a->bits[3];
    }
}

int check_long(long_decimal a) {
    return !a.bits[3] && !a.bits[4] && !a.bits[5] && !a.bits[6];
}

// Функция деления, использует метод вычитанием. Используется в функциях s21_div и s21_mod
int div_r(s21_decimal a, s21_decimal b, s21_decimal *c, int flag) {
    int e1 = i_exp(a), e2 = i_exp(b), exp = 0, cz = 0;
    int n1 = !!(a.bits[3] & SD[0]), n2 = !!(b.bits[3] & SD[0]), r = 0, res = 0;
    e1 > 28 ? round_exp(&a, e1, 28), e1 = 28 : 0;
    e2 > 28 ? round_exp(&b, e2, 28), e2 = 28 : 0;
    a.bits[3] = 0;
    b.bits[3] = 0;
    long_decimal la = zerol, lb = zerol;
    copy_decimal_to_long(&a, &la, 0);
    copy_decimal_to_long(&b, &lb, 0);
    equation_exp_long(&la, &lb, e1, e2);
    unsigned sd = 0;
    long_decimal ch = zerol, temp = zerol;
    long_decimal on = onesl, t = la;
    div_ten_long(&t, 1);
    while (greater_long(t, lb) || is_equal_long(t, lb)) {
        if (greater_long(la, lb) || is_equal_long(t, lb)) {
            mul_ten_long(&lb, 1);
            cz++;
        }
    }
    mul_ten_long(&on, cz);
    while (exp < 30 && !r && !res) {
        if (greater_long(la, lb) || is_equal_long(la, lb)) {
            if (sd) {
                exp += sd;
                mul_ten_long(&temp, sd);
                sd = 0;
            }
            sub_long(la, lb, &la);
            sum_long(ch, on, &ch);
        } else if (!greater_long(la, lb) && !zeros_eq_long(la)) {
            while (cz && greater_long(lb, la)) {
                div_ten_long(&lb, 1);
                div_ten_long(&on, 1);
                cz--;
            }
            if (!zeros_eq_long(ch)) {
                sum_long(temp, ch, &temp);
                ch = zerol;
            }
            if (flag && !cz) {
                r = 1;
            } else if (!cz) {
                mul_ten_long(&la, 1);
                sd++;
            }
        } else if (zeros_eq_long(la)) {
            sum_long(temp, ch, &temp);
            r = 1;
        }
        res = !res && temp.bits[3] > 0 && !exp;
    }
    int check = 0;
    if (exp > 28) {
        round_exp_long(&temp, exp, 28);
        exp = 28;
        check = zeros_eq_long(temp);
    }
    while (!check_long(temp) && exp && !check && !res) {
        round_exp_long(&temp, exp, exp - 1);
        exp--;
    }
    if (check) {
        *c = zero;
    } else if (!res && flag) {
        copy_decimal_to_long(c, &la, 1);
        c->bits[3] = e1 << 16;
        setbit(&c->bits[3], 31, n1);
    } else if (!res && check_long(temp)) {
        copy_decimal_to_long(c, &temp, 1);
        c->bits[3] = exp << 16;
        setbit(&c->bits[3], 31, n1 && n2 ? 0 : n1 || n2);
    } else {
        res += n1;
    }
    return res;
}

int expon_float(float src) {
    unsigned int src_b = *((unsigned int *) &src);
    return ((src_b & s21_EXPON) >> 23) - 127;
}
