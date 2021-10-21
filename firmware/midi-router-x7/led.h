#pragma once

#include <artl/digital_out.h>

using led_pwr = artl::digital_out<artl::port::D, 5>;

using led_txusb = artl::digital_out<artl::port::D, 4>;
using led_rxusb = artl::digital_out<artl::port::J, 6>;

using led_rx0 = artl::digital_out<artl::port::J, 3>;
using led_rx1 = artl::digital_out<artl::port::J, 2>;
using led_rx2 = artl::digital_out<artl::port::J, 1>;
using led_rx3 = artl::digital_out<artl::port::J, 0>;
using led_rx4 = artl::digital_out<artl::port::J, 7>;
using led_rx5 = artl::digital_out<artl::port::J, 6>;
using led_rx6 = artl::digital_out<artl::port::J, 5>;

using led_tx0 = artl::digital_out<artl::port::A, 2>;
using led_tx1 = artl::digital_out<artl::port::A, 3>;
using led_tx2 = artl::digital_out<artl::port::A, 4>;
using led_tx3 = artl::digital_out<artl::port::A, 5>;
using led_tx4 = artl::digital_out<artl::port::D, 1>;
using led_tx5 = artl::digital_out<artl::port::D, 0>;
using led_tx6 = artl::digital_out<artl::port::F, 5>;

using oen = artl::digital_out<artl::port::K, 1>;
