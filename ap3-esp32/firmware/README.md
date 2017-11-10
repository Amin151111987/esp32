 
# Firmware

This folder contains the current firmware files for the ESP32.

Following projects are available:

## rfc7668_ping_working

This project contains the RFC7668 implementation, based on BTStack.
If compiled & flashed, the ESP32 is running no dedicated firmware, but the device is available
as RFC7668 device (see getting started document how to connect).

The device is responding to ICMPv6 ping requests

## rfc7668_mqtt-sn

This project contains a small MQTT-SN example, which just connects to a broker (hardcoded!)
after the RFC7668 connection is established.
After connecting (no further error checking!), this example publishes ON/OFF messages to a short topic
name (max. length: 2 characters, more characters need a registration for a topic id, see MQTT-SN specification).

## Compiling 

You need to change following settings after a git clone:

* ./components/btstack/component.mk: change the BTSTACK_ROOT define (relative path!!!!)
* Set ESP_IDF variable
* Setup a valid esp-idf 
