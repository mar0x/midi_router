#pragma once

#include <artl/digital_out.h>

using led_pwr = artl::digital_out<artl::port::A, 6>;

using led_txusb = artl::digital_out<artl::port::A, 5>;
using led_rxusb = artl::digital_out<artl::port::A, 7>;

using led_rx0 = artl::digital_out<artl::port::B, 0>;
using led_rx1 = artl::digital_out<artl::port::B, 1>;
using led_rx2 = artl::digital_out<artl::port::B, 2>;
using led_rx3 = artl::digital_out<artl::port::B, 3>;

using led_tx0 = artl::digital_out<artl::port::C, 5>;
using led_tx1 = artl::digital_out<artl::port::C, 4>;
using led_tx2 = artl::digital_out<artl::port::D, 0>;
using led_tx3 = artl::digital_out<artl::port::D, 1>;
