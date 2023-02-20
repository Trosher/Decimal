#include "s21_decimal.h"

const s21_decimal zeroz = {{0, 0, 0, 0}};
const long_decimal zerolz = {{0, 0, 0, 0, 0, 0, 0}};

/* Функция сложения.*/
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    *result = zeroz;
    int res = 0, e1 = i_exp(value_1), e2 = i_exp(value_2);
    e1 > 28 ? round_exp(&value_1, e1, 28), e1 = 28 : 0;
    e2 > 28 ? round_exp(&value_2, e2, 28), e2 = 28 : 0;
    int exp = e1 > e2 ? e1 : e2;
    unsigned int n1 = checkbit(value_1.bits[3], 31), n2 = checkbit(value_2.bits[3], 31);
    value_1.bits[3] = 0;
    value_2.bits[3] = 0;
    long_decimal temp = zerolz, a = zerolz, b = zerolz;
    copy_decimal_to_long(&value_1, &a, 0);
    copy_decimal_to_long(&value_2, &b, 0);
    equation_exp_long(&a, &b, e1, e2);
    if (n1 != n2) {
        if (greater_long(a, b)) {
            sub_long(a, b, &temp);
        } else {
            sub_long(b, a, &temp);
            n1 = !n1;
        }
    } else {
        res = sum_long(a, b, &temp);
    }
    while (!check_long(temp) && exp) {
        round_exp_long(&temp, exp, exp - 1);
        exp--;
    }
    if (!res && check_long(temp)) {
        copy_decimal_to_long(result, &temp, 1);
        result->bits[3] = ((exp << 16) + (n1 << 31));
    } else {
        res = 1 + n1;
    }
    return res;
}

/* Функция вычитания.*/
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    *result = zeroz;
    int res = 0, e1 = i_exp(value_1), e2 = i_exp(value_2);
    unsigned int n1 = checkbit(value_1.bits[3], 31), n2 = checkbit(value_2.bits[3], 31);
    e1 > 28 ? round_exp(&value_1, e1, 28), e1 = 28 : 0;
    e2 > 28 ? round_exp(&value_2, e2, 28), e2 = 28 : 0;
    int exp = e1 > e2 ? e1 : e2;
    value_1.bits[3] = 0;
    value_2.bits[3] = 0;
    long_decimal temp = zerolz, a = zerolz, b = zerolz;
    copy_decimal_to_long(&value_1, &a, 0);
    copy_decimal_to_long(&value_2, &b, 0);
    equation_exp_long(&a, &b, e1, e2);
    if (n1 != n2) {
        sum_long(a, b, &temp);
    } else {
        if (greater_long(a, b)) {
            sub_long(a, b, &temp);
        } else {
            sub_long(b, a, &temp);
            n1 = !n1;
        }
    }
    while (!check_long(temp) && exp) {
        round_exp_long(&temp, exp, exp - 1);
        exp--;
    }
    if (check_long(temp)) {
        copy_decimal_to_long(result, &temp, 1);
        result->bits[3] = ((exp << 16) + (n1 << 31));
    } else {
        res = 1 + n1;
    }
    return res;
}

/* Функция умножения.*/
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    *result = zeroz;
    int e1 = i_exp(value_1), e2 = i_exp(value_2);
    int n1 = checkbit(value_1.bits[3], 31), n2 = checkbit(value_2.bits[3], 31);
    n1 = n1 && n2 ? 0 : n1 || n2;
    e1 > 28 ? round_exp(&value_1, e1, 28), e1 = 28 : 0;
    e2 > 28 ? round_exp(&value_2, e2, 28), e2 = 28 : 0;
    int exp = e1 + e2;
    value_1.bits[3] = 0;
    value_2.bits[3] = 0;
    long_decimal temp = zerolz, a = zerolz, b = zerolz;
    copy_decimal_to_long(&value_1, &a, 0);
    copy_decimal_to_long(&value_2, &b, 0);
    int check = zeros_eq(value_1, 3) || zeros_eq(value_2, 3), res = 0;
    for (int i = search_bits_long(a); i >= 0 && !check && !res; i--) {
        checkbit(a.bits[i / 32], i % 32) ? sum_long(b, temp, &temp) : 0;
        res = !res && temp.bits[3] > 0 && !exp;
        i ? sdvig_up_long(&temp, 1) : 0;
    }
    if (!check && !res && exp > 28) {
        round_exp_long(&temp, exp, 28);
        exp = 28;
        check = zeros_eq_long(temp);
    }
    while (!check_long(temp) && exp) {
        round_exp_long(&temp, exp, exp - 1);
        exp--;
    }
    if (!check && check_long(temp)) {
        copy_decimal_to_long(result, &temp, 1);
        result->bits[3] = exp << 16;
        setbit(&result->bits[3], 31, n1);
    } else if (!check) {
        res = 1 + n1;
    }
    return res;
}

/* Функция деления.*/
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    *result = zeroz;
    int res = zeros_eq(value_2, 3) ? 3 : 0;
    if (!res)
        res = div_r(value_1, value_2, result, 0);
    return res;
}

/* Функция взятия остатка от деления.*/
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    *result = zeroz;
    int res = zeros_eq(value_2, 3) ? 3 : 0;
    if (!res)
        div_r(value_1, value_2, result, 1);
    return res;
}

/* Функция сравнения value_1 < value_2.*/
int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
    return !s21_is_equal(value_1, value_2) ? !s21_is_greater(value_1, value_2) : 0;
}

/* Функция сравнения value_1 <= value_2.*/
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
    return s21_is_less(value_1, value_2) || s21_is_equal(value_1, value_2);
}

/* Функция сравнения value_1 > value_2.*/
int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
    int res = !s21_is_equal(value_1, value_2), check = 0;
    int n1 = checkbit(value_1.bits[3], 31), n2 = checkbit(value_2.bits[3], 31);
    n1 > n2 ? res = 0 : n1 < n2 ? check = 1 : 0;
    int e1 = i_exp(value_1), e2 = i_exp(value_2);
    e1 > 28 ? round_exp(&value_1, e1, 28), e1 = 28 : 0;
    e2 > 28 ? round_exp(&value_2, e2, 28), e2 = 28 : 0;
    value_1.bits[3] = 0;
    value_2.bits[3] = 0;
    long_decimal a = zerolz, b = zerolz;
    copy_decimal_to_long(&value_1, &a, 0);
    copy_decimal_to_long(&value_2, &b, 0);
    equation_exp_long(&a, &b, e1, e2);
    if (res && !check)
        check = n1 && n2 ? !greater_long(a, b) : greater_long(a, b);
    return check && res;
}

/* Функция сравнения value_1 >= value_2.*/
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
    return s21_is_greater(value_1, value_2) || s21_is_equal(value_1, value_2);
}

/* Функция сравнения value_1 = value_2.*/
int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
    int res = 1;
    for (int i = 127; i >= 0 && res; i--)
        res = equalbit(value_1.bits[i / 32], value_2.bits[i / 32], i % 32);
    return res;
}

/* Функция сравнения value_1 != value_2.*/
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
    return !s21_is_equal(value_1, value_2);
}

/* Функция конвертации.*/
int s21_from_int_to_decimal(int src, s21_decimal *dst) {
    *dst = zeroz;
    int neg = 0;
    checkbit(src, 31) ? neg = 1, src *= -1 : 0;
    unsigned int src_b = *((unsigned int*) &src);
    setbit(&dst->bits[3], 31, neg);
    for (int i = 0; i < 31; i++)
        setbit(&dst->bits[0], i, checkbit(src_b, i));
    return 0;
}

/* Функция конвертации.*/
int s21_from_float_to_decimal(float src, s21_decimal *dst) {
    *dst = zeroz;
    int check = (fabs(src) < 1e-28f || fabs(src) > powf(2, 96)) || (isnan(src) || isinf(src));
    if (!check) {
        int neg = src < 0;
        double src_d = (double)fabs(src);
        unsigned int scale = 0;
        for (; scale <= 28 && !((int)src_d / MAX_F) && (int)src_d != src_d; src_d *= 10, scale++) {}
        src = (float)round(src_d);
        for (; scale > 0 && !((int)src % 10); src /= 10, scale--) {}
        unsigned int src_b = *((unsigned int *) &src);
        int exp = expon_float(src);
        setbit(&dst->bits[exp / 32], exp % 32, src != 0);
        exp--;
        for (int i = 22; i >= 0; exp--, i--)
            checkbit(src_b, i) ? setbit(&dst->bits[exp / 32], exp % 32, 1) : 0;
        dst->bits[3] = scale << 16 | neg << 31;
    }
    return check;
}

/* Функция конвертации.*/
int s21_from_decimal_to_int(s21_decimal src, int *dst) {
    int negativ = checkbit(src.bits[3], 31) ? -1 : 1;
    long double temp = 0, z = i_exp(src);
    for (int i = 0; i < 96; temp += pow(2, i) * checkbit(src.bits[i / 32], i % 32), i++) {}
    temp *= powl(10.l, -z);
    int res = temp > ~M_BIT;
    *dst = (int)(temp * negativ * !res);
    return res;
}

/* Функция конвертации.*/
int s21_from_decimal_to_float(s21_decimal src, float *dst) {
    int negativ = checkbit(src.bits[3], 31) ? -1 : 1;
    long double temp = 0, z = i_exp(src);
    for (int i = 0; i < 96; temp += pow(2, i) * checkbit(src.bits[i / 32], i % 32), i++) {}
    *dst = (float)(temp * powl(10.f, -z)) * negativ;
    return 0;
}

/* Функция округления до -INF.*/
int s21_floor(s21_decimal value, s21_decimal *result) {
    int neg = !!(value.bits[3] & M_BIT);
    s21_truncate(value, result);
    s21_decimal b = {{neg, 0, 0, 0}};
    sum_r(*result, b, result, 96);
    return 0;
}

/* Функция округления до ближайшего целого.*/
int s21_round(s21_decimal value, s21_decimal *result) {
    int exp = i_exp(value);
    s21_truncate(value, result);
    s21_decimal a = *result;
    mul_ten(&a, 3, exp);
    sub_r(value, a, &value, 96);
    div_ten(&value, 95, exp - 1);
    s21_decimal b = {{value.bits[0] > 4, 0, 0, 0}};
    sum_r(*result, b, result, 96);
    return 0;
}

/* Функция отбрасывающая дробную часть.*/
int s21_truncate(s21_decimal value, s21_decimal *result) {
    int exp = i_exp(value);
    not_exp(&value);
    div_ten(&value, 95, exp);
    craft_decim(value, result);
    return 0;
}

/* Функция перевода знака на противоположный.*/
int s21_negate(s21_decimal value, s21_decimal *result) {
    *result = value;
    setbit(&result->bits[3], 31, !checkbit(result->bits[3], 31));
    return 0;
}
