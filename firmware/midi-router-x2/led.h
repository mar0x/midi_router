#pragma once

#include <artl/digital_out.h>

using led_pwr = artl::digital_out<artl::port::D, 4>;

//using led_txusb = artl::digital_out<artl::port::D, 4>;
//using led_rxusb = artl::digital_out<artl::port::J, 6>;

using led_rx0 = artl::digital_out<artl::port::C, 5>;
using led_rx1 = artl::digital_out<artl::port::C, 1>;

using led_tx0 = artl::digital_out<artl::port::C, 4>;
using led_tx1 = artl::digital_out<artl::port::C, 0>;
