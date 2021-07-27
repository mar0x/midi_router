#pragma once

namespace midi {

extern volatile uint8_t rx_ready;

void init();
void splitter();

bool recv(uint8_t &port, uint8_t &data);
uint8_t send(uint8_t port, const uint8_t *buf, uint8_t size);

}
