
#pragma once

namespace {

bool cdc_dtr = false;

void _cdc_print_eol();
void cdc_print_eol();

void _cdc_println();
void cdc_println();

template<typename T> void _cdc_print(T n);
template<typename T> void cdc_print(T n);

void _cdc_print(char c);
void cdc_print(char c);

void _cdc_print(const char *s);
void cdc_print(const char *s);

template<typename T>
void _cdc_println(T n) {
    _cdc_print(n);
    _cdc_print_eol();
}

template<typename T>
void cdc_println(T n) {
    if (cdc_dtr) {
        _cdc_println(n);
    }
}

template<typename T, typename ...Args>
void _cdc_print(T n, Args... args) {
    _cdc_print(n);
    _cdc_print(args...);
}

template<typename T, typename ...Args>
void _cdc_println(T n, Args... args) {
    _cdc_print(n);
    _cdc_println(args...);
}

template<typename T, typename ...Args>
void cdc_println(T n, Args... args) {
    if (cdc_dtr) {
        _cdc_print(n);
        _cdc_println(args...);
    }
}

void _cdc_prompt() {
    _cdc_print('>', ' ');
}

void cdc_prompt() {
    if (cdc_dtr) {
        _cdc_prompt();
    }
}

void _cdc_print_eol() {
    _cdc_print('\r', '\n');
}

void cdc_print_eol() {
    if (cdc_dtr) {
        _cdc_print_eol();
    }
}

void _cdc_println() {
    _cdc_print_eol();
}

void cdc_println() {
    cdc_print_eol();
}

void _cdc_print(char c) {
    udi_cdc_putc(c);
}

void cdc_print(char c) {
    if (cdc_dtr) {
        udi_cdc_putc(c);
    }
}

template<typename T>
struct buf_traits {
};

template<>
struct buf_traits<uint8_t> {
    using type_t = char[3];
};

template<>
struct buf_traits<uint16_t> {
    using type_t = char[5];
};

template<>
struct buf_traits<uint32_t> {
    using type_t = char[10];
};

template<typename T>
void _cdc_print(T n) {
    typename buf_traits<T>::type_t buf;
    uint8_t s = sizeof(buf) - 1;

    do {
        uint8_t c = n % 10;
        n = n / 10;
        buf[s--] = '0' + c;
    } while (n > 0);

    for(s++; s < sizeof(buf); s++) {
        udi_cdc_putc(buf[s]);
    }
}

template<typename T>
void _cdc_print_w(T n, uint8_t w) {
    typename buf_traits<T>::type_t buf;
    uint8_t s = sizeof(buf) - 1;

    do {
        uint8_t c = n % 10;
        n = n / 10;
        buf[s--] = '0' + c;
    } while (n > 0);

    while (s >= sizeof(buf) - w) {
        buf[s--] = ' ';
    }

    for(s++; s < sizeof(buf); s++) {
        udi_cdc_putc(buf[s]);
    }
}

void _cdc_print_w(const char* s, uint8_t w) {
    uint8_t l = strlen(s);

    for (; w > l; --w) {
        udi_cdc_putc(' ');
    }

    _cdc_print(s);
}

template<typename T>
uint8_t cdc_get_width(T n) {
    uint8_t res = 0;

    do {
        n = n / 10;
        ++res;
    } while (n > 0);

    return res;
}

template<typename T>
void cdc_print(T n) {
    if (!cdc_dtr) return;

    _cdc_print(n);
}

void _cdc_print(const char *s) {
    for (; *s; ++s) {
        udi_cdc_putc(*s);
    }
}

void cdc_print(const char *s) {
    if (!cdc_dtr) return;

    _cdc_print(s);
}

}
