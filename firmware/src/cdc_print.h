
#pragma once

namespace {

bool cdc_dtr = false;

void cdc_print_eol(void);
void cdc_print(uint16_t n);


void _cdc_print(uint8_t port, const char *s);

template<typename ...Args> void _cdc_print(uint8_t port, const char *s, Args... args) {
    _cdc_print(port, s);
    _cdc_print(port, args...);
}


void cdc_print(uint8_t port, const char *s);

template<typename ...Args> void cdc_print(uint8_t port, const char *s, Args... args) {
    if (cdc_dtr) {
        _cdc_print(port, s);
        _cdc_print(port, args...);
    }
}


void _cdc_println(uint8_t port, const char *s);

template<typename ...Args> void _cdc_println(uint8_t port, const char *s, Args... args) {
    _cdc_print(port, s);
    _cdc_println(port, args...);
}


void cdc_println(uint8_t port, const char *s);

template<typename ...Args> void cdc_println(uint8_t port, const char *s, Args... args) {
    if (cdc_dtr) {
        _cdc_print(port, s);
        _cdc_println(port, args...);
    }
}

void _cdc_prompt(uint8_t port) {
    udi_cdc_multi_putc(port, '>');
    udi_cdc_multi_putc(port, ' ');
}

void cdc_prompt(uint8_t port) {
    if (cdc_dtr) {
        _cdc_prompt(port);
    }
}

//void cdc_print_l(unsigned long n);

void cdc_print_eol(void) {
    if (!cdc_dtr) return;

    udi_cdc_putc('\r');
    udi_cdc_putc('\n');
}

void cdc_print(uint16_t n) {
    char buf[5];
    uint8_t s = sizeof(buf) - 1;

    if (!cdc_dtr) return;

    do {
        uint16_t c = n % 10;
        n = n / 10;
        buf[s--] = '0' + c;
    } while (n > 0);

    for(s++; s < sizeof(buf); s++) {
        udi_cdc_putc(buf[s]);
    }
}

void _cdc_print(uint8_t port, const char *s) {
    for (; *s; ++s) {
        udi_cdc_multi_putc(port, *s);
    }
}

void cdc_print(uint8_t port, const char *s) {
    if (!cdc_dtr) return;

    _cdc_print(port, s);
}

void _cdc_println(uint8_t port, const char *s) {
    for (; *s; ++s) {
        udi_cdc_multi_putc(port, *s);
    }

    udi_cdc_multi_putc(port, '\r');
    udi_cdc_multi_putc(port, '\n');
}

void cdc_println(uint8_t port, const char *s) {
    if (!cdc_dtr) return;

    _cdc_println(port, s);
}

}
