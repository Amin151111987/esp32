EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:ESP32-footprints-Shem-Lib
LIBS:rt7285c
LIBS:relays
LIBS:motor_drivers
LIBS:nx3008
LIBS:switches
LIBS:Relay_8x-cache
EELAYER 25 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L C C3
U 1 1 58B959B0
P 3600 1550
F 0 "C3" H 3625 1650 50  0000 L CNN
F 1 "22uF" H 3625 1450 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_4x5.7" H 3638 1400 50  0001 C CNN
F 3 "" H 3600 1550 50  0000 C CNN
	1    3600 1550
	1    0    0    -1  
$EndComp
$Comp
L RM50-xx21 RL1
U 1 1 58B95F2F
P 1150 2700
F 0 "RL1" H 1100 2450 50  0000 L CNN
F 1 "ADW1103" H 1000 3000 50  0000 L CNN
F 2 "Common_Footprints:ADW11" H 1150 2700 50  0001 C CNN
F 3 "" H 1150 2700 50  0001 C CNN
	1    1150 2700
	1    0    0    -1  
$EndComp
$Comp
L RM50-xx21 RL4
U 1 1 58B95FC4
P 1150 5150
F 0 "RL4" H 1450 4850 50  0000 L CNN
F 1 "ADW1103" H 1000 5450 50  0000 L CNN
F 2 "Common_Footprints:ADW11" H 1150 5150 50  0001 C CNN
F 3 "" H 1150 5150 50  0001 C CNN
	1    1150 5150
	1    0    0    -1  
$EndComp
$Comp
L RM50-xx21 RL6
U 1 1 58B9605F
P 1150 6750
F 0 "RL6" H 1450 6450 50  0000 L CNN
F 1 "ADW1103" H 1000 7050 50  0000 L CNN
F 2 "Common_Footprints:ADW11" H 1150 6750 50  0001 C CNN
F 3 "" H 1150 6750 50  0001 C CNN
	1    1150 6750
	1    0    0    -1  
$EndComp
$Comp
L RM50-xx21 RL2
U 1 1 58B96169
P 1150 3500
F 0 "RL2" H 1100 3250 50  0000 L CNN
F 1 "ADW1103" H 1000 3800 50  0000 L CNN
F 2 "Common_Footprints:ADW11" H 1150 3500 50  0001 C CNN
F 3 "" H 1150 3500 50  0001 C CNN
	1    1150 3500
	1    0    0    -1  
$EndComp
$Comp
L RM50-xx21 RL3
U 1 1 58B961EB
P 1150 4300
F 0 "RL3" H 1100 4050 50  0000 L CNN
F 1 "ADW1103" H 1000 4600 50  0000 L CNN
F 2 "Common_Footprints:ADW11" H 1150 4300 50  0001 C CNN
F 3 "" H 1150 4300 50  0001 C CNN
	1    1150 4300
	1    0    0    -1  
$EndComp
$Comp
L RM50-xx21 RL5
U 1 1 58B96240
P 1150 5950
F 0 "RL5" H 1450 5650 50  0000 L CNN
F 1 "ADW1103" H 1000 6250 50  0000 L CNN
F 2 "Common_Footprints:ADW11" H 1150 5950 50  0001 C CNN
F 3 "" H 1150 5950 50  0001 C CNN
	1    1150 5950
	1    0    0    -1  
$EndComp
$Comp
L ESP32-WROOM U2
U 1 1 58B96D67
P 6850 9300
F 0 "U2" H 6150 10550 60  0000 C CNN
F 1 "ESP32-WROOM" H 7350 10550 60  0000 C CNN
F 2 "ESP32-footprints-Lib:ESP32-WROOM" H 7200 10650 60  0001 C CNN
F 3 "" H 6400 9750 60  0001 C CNN
	1    6850 9300
	1    0    0    -1  
$EndComp
$Comp
L NX3008 T10
U 1 1 58B9994E
P 10400 3000
F 0 "T10" H 10400 2200 60  0000 C CNN
F 1 "NX3008CBKS" H 10400 2350 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SC-70-6_Handsoldering" H 10200 3000 60  0001 C CNN
F 3 "" H 10200 3000 60  0001 C CNN
	1    10400 3000
	1    0    0    -1  
$EndComp
$Comp
L NX3008 T9
U 1 1 58B99AD5
P 10400 1550
F 0 "T9" H 10400 750 60  0000 C CNN
F 1 "NX3008CBKS" H 10400 900 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SC-70-6_Handsoldering" H 10200 1550 60  0001 C CNN
F 3 "" H 10200 1550 60  0001 C CNN
	1    10400 1550
	1    0    0    -1  
$EndComp
$Comp
L NX3008 T12
U 1 1 58B99DCB
P 10400 5950
F 0 "T12" H 10400 5150 60  0000 C CNN
F 1 "NX3008CBKS" H 10400 5300 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SC-70-6_Handsoldering" H 10200 5950 60  0001 C CNN
F 3 "" H 10200 5950 60  0001 C CNN
	1    10400 5950
	1    0    0    -1  
$EndComp
$Comp
L NX3008 T8
U 1 1 58B99DD7
P 8650 6000
F 0 "T8" H 8650 5200 60  0000 C CNN
F 1 "NX3008CBKS" H 8650 5350 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SC-70-6_Handsoldering" H 8450 6000 60  0001 C CNN
F 3 "" H 8450 6000 60  0001 C CNN
	1    8650 6000
	1    0    0    -1  
$EndComp
$Comp
L NX3008 T11
U 1 1 58B99DDD
P 10400 4500
F 0 "T11" H 10400 3700 60  0000 C CNN
F 1 "NX3008CBKS" H 10400 3850 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SC-70-6_Handsoldering" H 10200 4500 60  0001 C CNN
F 3 "" H 10200 4500 60  0001 C CNN
	1    10400 4500
	1    0    0    -1  
$EndComp
$Comp
L NX3008 T7
U 1 1 58B99DE9
P 8650 4500
F 0 "T7" H 8650 3700 60  0000 C CNN
F 1 "NX3008CBKS" H 8650 3850 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SC-70-6_Handsoldering" H 8450 4500 60  0001 C CNN
F 3 "" H 8450 4500 60  0001 C CNN
	1    8650 4500
	1    0    0    -1  
$EndComp
$Comp
L NX3008 T5
U 1 1 58B99FBB
P 8650 1550
F 0 "T5" H 8650 750 60  0000 C CNN
F 1 "NX3008CBKS" H 8650 900 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SC-70-6_Handsoldering" H 8450 1550 60  0001 C CNN
F 3 "" H 8450 1550 60  0001 C CNN
	1    8650 1550
	1    0    0    -1  
$EndComp
$Comp
L NX3008 T2
U 1 1 58B99FC1
P 6900 3000
F 0 "T2" H 6900 2200 60  0000 C CNN
F 1 "NX3008CBKS" H 6900 2350 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SC-70-6_Handsoldering" H 6700 3000 60  0001 C CNN
F 3 "" H 6700 3000 60  0001 C CNN
	1    6900 3000
	1    0    0    -1  
$EndComp
$Comp
L NX3008 T4
U 1 1 58B99FC7
P 6900 6000
F 0 "T4" H 6900 5200 60  0000 C CNN
F 1 "NX3008CBKS" H 6900 5350 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SC-70-6_Handsoldering" H 6700 6000 60  0001 C CNN
F 3 "" H 6700 6000 60  0001 C CNN
	1    6900 6000
	1    0    0    -1  
$EndComp
$Comp
L NX3008 T1
U 1 1 58B99FCD
P 6900 1550
F 0 "T1" H 6900 750 60  0000 C CNN
F 1 "NX3008CBKS" H 6900 900 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SC-70-6_Handsoldering" H 6700 1550 60  0001 C CNN
F 3 "" H 6700 1550 60  0001 C CNN
	1    6900 1550
	1    0    0    -1  
$EndComp
$Comp
L NX3008 T6
U 1 1 58B9A4D3
P 8650 3000
F 0 "T6" H 8650 2200 60  0000 C CNN
F 1 "NX3008CBKS" H 8650 2350 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SC-70-6_Handsoldering" H 8450 3000 60  0001 C CNN
F 3 "" H 8450 3000 60  0001 C CNN
	1    8650 3000
	1    0    0    -1  
$EndComp
$Comp
L NX3008 T3
U 1 1 58B9A4D9
P 6900 4500
F 0 "T3" H 6900 3700 60  0000 C CNN
F 1 "NX3008CBKS" H 6900 3850 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SC-70-6_Handsoldering" H 6700 4500 60  0001 C CNN
F 3 "" H 6700 4500 60  0001 C CNN
	1    6900 4500
	1    0    0    -1  
$EndComp
$Comp
L Screw_Terminal_1x06 J4
U 1 1 58B9B377
P 1950 8400
F 0 "J4" H 1950 9050 50  0000 C TNN
F 1 "Screw_Terminal_1x06" V 1800 8400 50  0000 C TNN
F 2 "Connect:bornier6" H 1950 7775 50  0001 C CNN
F 3 "" H 1925 8800 50  0001 C CNN
	1    1950 8400
	-1   0    0    -1  
$EndComp
$Comp
L Screw_Terminal_1x06 J2
U 1 1 58B9B42F
P 1950 3500
F 0 "J2" H 1950 4150 50  0000 C TNN
F 1 "Screw_Terminal_1x06" V 1800 3500 50  0000 C TNN
F 2 "Connect:bornier6" H 1950 2875 50  0001 C CNN
F 3 "" H 1925 3900 50  0001 C CNN
	1    1950 3500
	-1   0    0    -1  
$EndComp
$Comp
L Screw_Terminal_1x06 J3
U 1 1 58B9B4B7
P 1900 5950
F 0 "J3" H 1900 6600 50  0000 C TNN
F 1 "Screw_Terminal_1x06" V 1750 5950 50  0000 C TNN
F 2 "Connect:bornier6" H 1900 5325 50  0001 C CNN
F 3 "" H 1875 6350 50  0001 C CNN
	1    1900 5950
	-1   0    0    -1  
$EndComp
$Comp
L Screw_Terminal_1x02 J1
U 1 1 58B9CD61
P 1100 1400
F 0 "J1" H 1100 1650 50  0000 C TNN
F 1 "Screw_Terminal_1x02" V 950 1400 50  0000 C TNN
F 2 "Connect:bornier2" H 1100 1175 50  0001 C CNN
F 3 "" H 1075 1400 50  0001 C CNN
	1    1100 1400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 58B9E909
P 6650 2100
F 0 "#PWR01" H 6650 1850 50  0001 C CNN
F 1 "GND" H 6650 1950 50  0000 C CNN
F 2 "" H 6650 2100 50  0000 C CNN
F 3 "" H 6650 2100 50  0000 C CNN
	1    6650 2100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 58B9E9AB
P 6650 6550
F 0 "#PWR02" H 6650 6300 50  0001 C CNN
F 1 "GND" H 6650 6400 50  0000 C CNN
F 2 "" H 6650 6550 50  0000 C CNN
F 3 "" H 6650 6550 50  0000 C CNN
	1    6650 6550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR03
U 1 1 58B9EA6A
P 8400 5050
F 0 "#PWR03" H 8400 4800 50  0001 C CNN
F 1 "GND" H 8400 4900 50  0000 C CNN
F 2 "" H 8400 5050 50  0000 C CNN
F 3 "" H 8400 5050 50  0000 C CNN
	1    8400 5050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 58B9EB29
P 10150 5050
F 0 "#PWR04" H 10150 4800 50  0001 C CNN
F 1 "GND" H 10150 4900 50  0000 C CNN
F 2 "" H 10150 5050 50  0000 C CNN
F 3 "" H 10150 5050 50  0000 C CNN
	1    10150 5050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR05
U 1 1 58B9EC8E
P 10150 6500
F 0 "#PWR05" H 10150 6250 50  0001 C CNN
F 1 "GND" H 10150 6350 50  0000 C CNN
F 2 "" H 10150 6500 50  0000 C CNN
F 3 "" H 10150 6500 50  0000 C CNN
	1    10150 6500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR06
U 1 1 58B9ED05
P 8400 6550
F 0 "#PWR06" H 8400 6300 50  0001 C CNN
F 1 "GND" H 8400 6400 50  0000 C CNN
F 2 "" H 8400 6550 50  0000 C CNN
F 3 "" H 8400 6550 50  0000 C CNN
	1    8400 6550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR07
U 1 1 58B9ED7C
P 8400 2100
F 0 "#PWR07" H 8400 1850 50  0001 C CNN
F 1 "GND" H 8400 1950 50  0000 C CNN
F 2 "" H 8400 2100 50  0000 C CNN
F 3 "" H 8400 2100 50  0000 C CNN
	1    8400 2100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR08
U 1 1 58B9EDF3
P 6650 3550
F 0 "#PWR08" H 6650 3300 50  0001 C CNN
F 1 "GND" H 6650 3400 50  0000 C CNN
F 2 "" H 6650 3550 50  0000 C CNN
F 3 "" H 6650 3550 50  0000 C CNN
	1    6650 3550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR09
U 1 1 58B9EE6A
P 6650 5050
F 0 "#PWR09" H 6650 4800 50  0001 C CNN
F 1 "GND" H 6650 4900 50  0000 C CNN
F 2 "" H 6650 5050 50  0000 C CNN
F 3 "" H 6650 5050 50  0000 C CNN
	1    6650 5050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR010
U 1 1 58B9EEE1
P 8400 3550
F 0 "#PWR010" H 8400 3300 50  0001 C CNN
F 1 "GND" H 8400 3400 50  0000 C CNN
F 2 "" H 8400 3550 50  0000 C CNN
F 3 "" H 8400 3550 50  0000 C CNN
	1    8400 3550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR011
U 1 1 58B9EF58
P 10150 2100
F 0 "#PWR011" H 10150 1850 50  0001 C CNN
F 1 "GND" H 10150 1950 50  0000 C CNN
F 2 "" H 10150 2100 50  0000 C CNN
F 3 "" H 10150 2100 50  0000 C CNN
	1    10150 2100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR012
U 1 1 58B9F2FA
P 10150 3550
F 0 "#PWR012" H 10150 3300 50  0001 C CNN
F 1 "GND" H 10150 3400 50  0000 C CNN
F 2 "" H 10150 3550 50  0000 C CNN
F 3 "" H 10150 3550 50  0000 C CNN
	1    10150 3550
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR013
U 1 1 58B9F493
P 7150 2100
F 0 "#PWR013" H 7150 1950 50  0001 C CNN
F 1 "VCC" H 7150 2250 50  0000 C CNN
F 2 "" H 7150 2100 50  0000 C CNN
F 3 "" H 7150 2100 50  0000 C CNN
	1    7150 2100
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR014
U 1 1 58B9F559
P 7150 6550
F 0 "#PWR014" H 7150 6400 50  0001 C CNN
F 1 "VCC" H 7150 6700 50  0000 C CNN
F 2 "" H 7150 6550 50  0000 C CNN
F 3 "" H 7150 6550 50  0000 C CNN
	1    7150 6550
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR015
U 1 1 58B9F5D0
P 8900 5050
F 0 "#PWR015" H 8900 4900 50  0001 C CNN
F 1 "VCC" H 8900 5200 50  0000 C CNN
F 2 "" H 8900 5050 50  0000 C CNN
F 3 "" H 8900 5050 50  0000 C CNN
	1    8900 5050
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR016
U 1 1 58B9F68F
P 10650 5050
F 0 "#PWR016" H 10650 4900 50  0001 C CNN
F 1 "VCC" H 10650 5200 50  0000 C CNN
F 2 "" H 10650 5050 50  0000 C CNN
F 3 "" H 10650 5050 50  0000 C CNN
	1    10650 5050
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR017
U 1 1 58B9F83C
P 10650 6500
F 0 "#PWR017" H 10650 6350 50  0001 C CNN
F 1 "VCC" H 10650 6650 50  0000 C CNN
F 2 "" H 10650 6500 50  0000 C CNN
F 3 "" H 10650 6500 50  0000 C CNN
	1    10650 6500
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR018
U 1 1 58B9F8B3
P 8900 6550
F 0 "#PWR018" H 8900 6400 50  0001 C CNN
F 1 "VCC" H 8900 6700 50  0000 C CNN
F 2 "" H 8900 6550 50  0000 C CNN
F 3 "" H 8900 6550 50  0000 C CNN
	1    8900 6550
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR019
U 1 1 58B9FA4A
P 8900 2100
F 0 "#PWR019" H 8900 1950 50  0001 C CNN
F 1 "VCC" H 8900 2250 50  0000 C CNN
F 2 "" H 8900 2100 50  0000 C CNN
F 3 "" H 8900 2100 50  0000 C CNN
	1    8900 2100
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR020
U 1 1 58B9FB99
P 7150 3550
F 0 "#PWR020" H 7150 3400 50  0001 C CNN
F 1 "VCC" H 7150 3700 50  0000 C CNN
F 2 "" H 7150 3550 50  0000 C CNN
F 3 "" H 7150 3550 50  0000 C CNN
	1    7150 3550
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR021
U 1 1 58B9FC34
P 7150 5050
F 0 "#PWR021" H 7150 4900 50  0001 C CNN
F 1 "VCC" H 7150 5200 50  0000 C CNN
F 2 "" H 7150 5050 50  0000 C CNN
F 3 "" H 7150 5050 50  0000 C CNN
	1    7150 5050
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR022
U 1 1 58B9FCAB
P 8900 3550
F 0 "#PWR022" H 8900 3400 50  0001 C CNN
F 1 "VCC" H 8900 3700 50  0000 C CNN
F 2 "" H 8900 3550 50  0000 C CNN
F 3 "" H 8900 3550 50  0000 C CNN
	1    8900 3550
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR023
U 1 1 58B9FD22
P 10650 2100
F 0 "#PWR023" H 10650 1950 50  0001 C CNN
F 1 "VCC" H 10650 2250 50  0000 C CNN
F 2 "" H 10650 2100 50  0000 C CNN
F 3 "" H 10650 2100 50  0000 C CNN
	1    10650 2100
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR024
U 1 1 58BA01D9
P 10650 3550
F 0 "#PWR024" H 10650 3400 50  0001 C CNN
F 1 "VCC" H 10650 3700 50  0000 C CNN
F 2 "" H 10650 3550 50  0000 C CNN
F 3 "" H 10650 3550 50  0000 C CNN
	1    10650 3550
	-1   0    0    1   
$EndComp
Text GLabel 6900 950  3    60   Input ~ 0
1A1
Text GLabel 6900 2400 3    60   Input ~ 0
1A2
Text GLabel 6900 3900 3    60   Input ~ 0
2A1
Text GLabel 6900 5400 3    60   Input ~ 0
2A2
Text GLabel 8650 950  3    60   Input ~ 0
3A1
Text GLabel 8650 2400 3    60   Input ~ 0
3A2
Text GLabel 8650 3900 3    60   Input ~ 0
4A1
Text GLabel 8650 5400 3    60   Input ~ 0
4A2
Text GLabel 10400 950  3    60   Input ~ 0
5A1
Text GLabel 10400 2400 3    60   Input ~ 0
5A2
Text GLabel 10400 3900 3    60   Input ~ 0
6A1
Text GLabel 10400 5350 3    60   Input ~ 0
6A2
Text GLabel 950  2400 0    60   Input ~ 0
1A1
Text GLabel 950  3000 0    60   Input ~ 0
1A2
Text GLabel 950  3200 0    60   Input ~ 0
2A1
Text GLabel 950  3800 0    60   Input ~ 0
2A2
Text GLabel 950  4000 0    60   Input ~ 0
3A1
Text GLabel 950  4600 0    60   Input ~ 0
3A2
Text GLabel 950  4850 0    60   Input ~ 0
4A1
Text GLabel 950  5450 0    60   Input ~ 0
4A2
Text GLabel 950  5650 0    60   Input ~ 0
5A1
Text GLabel 950  6250 0    60   Input ~ 0
5A2
Text GLabel 950  6450 0    60   Input ~ 0
6A1
Text GLabel 950  7050 0    60   Input ~ 0
6A2
Text GLabel 1000 7900 2    60   Input ~ 0
7A2
Text GLabel 1000 9050 2    60   Input ~ 0
8A2
$Comp
L 74HC595 U3
U 1 1 58BA1567
P 4000 3350
F 0 "U3" H 4150 3950 50  0000 C CNN
F 1 "74HC595" H 4000 2750 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-16_3.9x9.9mm_Pitch1.27mm" H 4000 3350 50  0001 C CNN
F 3 "" H 4000 3350 50  0000 C CNN
	1    4000 3350
	1    0    0    -1  
$EndComp
$Comp
L 74HC595 U4
U 1 1 58BA18AE
P 4000 4650
F 0 "U4" H 4150 5250 50  0000 C CNN
F 1 "74HC595" H 4000 4050 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-16_3.9x9.9mm_Pitch1.27mm" H 4000 4650 50  0001 C CNN
F 3 "" H 4000 4650 50  0000 C CNN
	1    4000 4650
	1    0    0    -1  
$EndComp
$Comp
L 74HC595 U5
U 1 1 58BA1964
P 4000 5950
F 0 "U5" H 4150 6550 50  0000 C CNN
F 1 "74HC595" H 4000 5350 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-16_3.9x9.9mm_Pitch1.27mm" H 4000 5950 50  0001 C CNN
F 3 "" H 4000 5950 50  0000 C CNN
	1    4000 5950
	1    0    0    -1  
$EndComp
$Comp
L R_Small R5
U 1 1 58BA2389
P 6200 4950
F 0 "R5" H 6230 4970 50  0000 L CNN
F 1 "10k" H 6230 4910 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 6200 4950 50  0001 C CNN
F 3 "" H 6200 4950 50  0000 C CNN
	1    6200 4950
	1    0    0    -1  
$EndComp
$Comp
L R_Small R6
U 1 1 58BA3725
P 6200 6450
F 0 "R6" H 6230 6470 50  0000 L CNN
F 1 "10k" H 6230 6410 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 6200 6450 50  0001 C CNN
F 3 "" H 6200 6450 50  0000 C CNN
	1    6200 6450
	1    0    0    -1  
$EndComp
$Comp
L R_Small R4
U 1 1 58BA37F0
P 6150 3450
F 0 "R4" H 6180 3470 50  0000 L CNN
F 1 "10k" H 6180 3410 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 6150 3450 50  0001 C CNN
F 3 "" H 6150 3450 50  0000 C CNN
	1    6150 3450
	1    0    0    -1  
$EndComp
$Comp
L R_Small R3
U 1 1 58BA3A94
P 6150 2000
F 0 "R3" H 6180 2020 50  0000 L CNN
F 1 "10k" H 6180 1960 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 6150 2000 50  0001 C CNN
F 3 "" H 6150 2000 50  0000 C CNN
	1    6150 2000
	1    0    0    -1  
$EndComp
$Comp
L R_Small R7
U 1 1 58BA3F0F
P 7950 2000
F 0 "R7" H 7980 2020 50  0000 L CNN
F 1 "10k" H 7980 1960 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 7950 2000 50  0001 C CNN
F 3 "" H 7950 2000 50  0000 C CNN
	1    7950 2000
	1    0    0    -1  
$EndComp
$Comp
L R_Small R8
U 1 1 58BA410E
P 7950 3450
F 0 "R8" H 7980 3470 50  0000 L CNN
F 1 "10k" H 7980 3410 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 7950 3450 50  0001 C CNN
F 3 "" H 7950 3450 50  0000 C CNN
	1    7950 3450
	1    0    0    -1  
$EndComp
$Comp
L R_Small R9
U 1 1 58BA41D9
P 7950 4950
F 0 "R9" H 7980 4970 50  0000 L CNN
F 1 "10k" H 7980 4910 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 7950 4950 50  0001 C CNN
F 3 "" H 7950 4950 50  0000 C CNN
	1    7950 4950
	1    0    0    -1  
$EndComp
$Comp
L R_Small R10
U 1 1 58BA43B5
P 7950 6450
F 0 "R10" H 7980 6470 50  0000 L CNN
F 1 "10k" H 7980 6410 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 7950 6450 50  0001 C CNN
F 3 "" H 7950 6450 50  0000 C CNN
	1    7950 6450
	1    0    0    -1  
$EndComp
$Comp
L R_Small R12
U 1 1 58BA4591
P 9700 3450
F 0 "R12" H 9730 3470 50  0000 L CNN
F 1 "10k" H 9730 3410 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 9700 3450 50  0001 C CNN
F 3 "" H 9700 3450 50  0000 C CNN
	1    9700 3450
	1    0    0    -1  
$EndComp
$Comp
L R_Small R11
U 1 1 58BA46AC
P 9700 2000
F 0 "R11" H 9730 2020 50  0000 L CNN
F 1 "10k" H 9730 1960 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 9700 2000 50  0001 C CNN
F 3 "" H 9700 2000 50  0000 C CNN
	1    9700 2000
	1    0    0    -1  
$EndComp
$Comp
L R_Small R13
U 1 1 58BA4D20
P 9700 4950
F 0 "R13" H 9730 4970 50  0000 L CNN
F 1 "10k" H 9730 4910 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 9700 4950 50  0001 C CNN
F 3 "" H 9700 4950 50  0000 C CNN
	1    9700 4950
	1    0    0    -1  
$EndComp
$Comp
L R_Small R14
U 1 1 58BA4F3F
P 9700 6400
F 0 "R14" H 9730 6420 50  0000 L CNN
F 1 "10k" H 9730 6360 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 9700 6400 50  0001 C CNN
F 3 "" H 9700 6400 50  0000 C CNN
	1    9700 6400
	1    0    0    -1  
$EndComp
$Comp
L +12V #PWR025
U 1 1 58BA77B2
P 1750 1150
F 0 "#PWR025" H 1750 1000 50  0001 C CNN
F 1 "+12V" H 1750 1290 50  0000 C CNN
F 2 "" H 1750 1150 50  0000 C CNN
F 3 "" H 1750 1150 50  0000 C CNN
	1    1750 1150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR026
U 1 1 58BA78B2
P 2800 2000
F 0 "#PWR026" H 2800 1750 50  0001 C CNN
F 1 "GND" H 2800 1850 50  0000 C CNN
F 2 "" H 2800 2000 50  0000 C CNN
F 3 "" H 2800 2000 50  0000 C CNN
	1    2800 2000
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR027
U 1 1 58BA79ED
P 3600 1250
F 0 "#PWR027" H 3600 1100 50  0001 C CNN
F 1 "VCC" H 3600 1400 50  0000 C CNN
F 2 "" H 3600 1250 50  0000 C CNN
F 3 "" H 3600 1250 50  0000 C CNN
	1    3600 1250
	1    0    0    -1  
$EndComp
Text Label 6100 1550 1    60   ~ 0
1G1L
Text Label 7700 1550 1    60   ~ 0
1G1H
Text Label 6100 3000 1    60   ~ 0
1G2L
Text Label 7700 3000 1    60   ~ 0
1G2H
Text Label 6100 4500 1    60   ~ 0
2G1L
Text Label 7700 4500 1    60   ~ 0
2G1H
Text Label 6100 6000 1    60   ~ 0
2G2L
Text Label 7700 6000 1    60   ~ 0
2G2H
Text Label 7850 1550 1    60   ~ 0
3G1L
Text Label 9450 1550 1    60   ~ 0
3G1H
Text Label 7850 3000 1    60   ~ 0
3G2L
Text Label 9450 3000 1    60   ~ 0
3G2H
Text Label 7850 4500 1    60   ~ 0
4G1L
Text Label 9450 4500 1    60   ~ 0
4G1H
Text Label 7850 6000 1    60   ~ 0
4G2L
Text Label 9450 6000 1    60   ~ 0
4G2H
Text Label 9600 1550 1    60   ~ 0
5G1L
Text Label 11200 1550 1    60   ~ 0
5G1H
Text Label 9600 3000 1    60   ~ 0
5G2L
Text Label 11200 3000 1    60   ~ 0
5G2H
Text Label 9600 4500 1    60   ~ 0
6G1L
Text Label 11200 4500 1    60   ~ 0
6G1H
Text Label 9600 5950 1    60   ~ 0
6G2L
Text Label 11200 5950 1    60   ~ 0
6G2H
Text Label 4700 3400 0    60   ~ 0
1G1L
Text Label 4700 3300 0    60   ~ 0
1G1H
Text Label 4700 3600 0    60   ~ 0
1G2L
Text Label 4700 3500 0    60   ~ 0
1G2H
Text Label 4700 3000 0    60   ~ 0
2G1L
Text Label 4700 2900 0    60   ~ 0
2G1H
Text Label 4700 3200 0    60   ~ 0
2G2L
Text Label 4700 3100 0    60   ~ 0
2G2H
Text Label 4700 4700 0    60   ~ 0
3G1L
Text Label 4700 4600 0    60   ~ 0
3G1H
Text Label 4700 4900 0    60   ~ 0
3G2L
Text Label 4700 4800 0    60   ~ 0
3G2H
Text Label 4700 4300 0    60   ~ 0
4G1L
Text Label 4700 4200 0    60   ~ 0
4G1H
Text Label 4700 4500 0    60   ~ 0
4G2L
Text Label 4700 4400 0    60   ~ 0
4G2H
Text Label 4700 6000 0    60   ~ 0
5G1L
Text Label 4700 5900 0    60   ~ 0
5G1H
Text Label 4700 6200 0    60   ~ 0
5G2L
Text Label 4700 6100 0    60   ~ 0
5G2H
Text Label 4700 5600 0    60   ~ 0
6G1L
Text Label 4700 5500 0    60   ~ 0
6G1H
Text Label 4700 5800 0    60   ~ 0
6G2L
Text Label 4700 5700 0    60   ~ 0
6G2H
Text Label 3300 2900 2    60   ~ 0
SER
Text Label 3300 3100 2    60   ~ 0
SRCLK
Text Label 3300 3200 2    60   ~ 0
SRCLR
Text Label 3300 3400 2    60   ~ 0
RCLK
Text Label 3300 3500 2    60   ~ 0
G
Text Label 3300 4400 2    60   ~ 0
SRCLK
Text Label 3300 4500 2    60   ~ 0
SRCLR
Text Label 3300 4700 2    60   ~ 0
RCLK
Text Label 3300 4800 2    60   ~ 0
G
Text Label 3300 5700 2    60   ~ 0
SRCLK
Text Label 3300 5800 2    60   ~ 0
SRCLR
Text Label 3300 6000 2    60   ~ 0
RCLK
Text Label 3300 6100 2    60   ~ 0
G
Text Label 5900 9300 2    60   ~ 0
SER
Text Label 5900 9600 2    60   ~ 0
SRCLK
Text Label 5900 9700 2    60   ~ 0
SRCLR
Text Label 5900 9400 2    60   ~ 0
RCLK
Text Label 5900 9500 2    60   ~ 0
G
$Comp
L GND #PWR028
U 1 1 58BAFD3D
P 5900 10000
F 0 "#PWR028" H 5900 9750 50  0001 C CNN
F 1 "GND" H 5900 9850 50  0000 C CNN
F 2 "" H 5900 10000 50  0000 C CNN
F 3 "" H 5900 10000 50  0000 C CNN
	1    5900 10000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR029
U 1 1 58BAFE78
P 6400 10350
F 0 "#PWR029" H 6400 10100 50  0001 C CNN
F 1 "GND" H 6400 10200 50  0000 C CNN
F 2 "" H 6400 10350 50  0000 C CNN
F 3 "" H 6400 10350 50  0000 C CNN
	1    6400 10350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR030
U 1 1 58BAFF3B
P 7750 9950
F 0 "#PWR030" H 7750 9700 50  0001 C CNN
F 1 "GND" H 7750 9800 50  0000 C CNN
F 2 "" H 7750 9950 50  0000 C CNN
F 3 "" H 7750 9950 50  0000 C CNN
	1    7750 9950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR031
U 1 1 58BB00A8
P 7750 9850
F 0 "#PWR031" H 7750 9600 50  0001 C CNN
F 1 "GND" H 7750 9700 50  0000 C CNN
F 2 "" H 7750 9850 50  0000 C CNN
F 3 "" H 7750 9850 50  0000 C CNN
	1    7750 9850
	0    -1   -1   0   
$EndComp
$Comp
L VCC #PWR032
U 1 1 58BB0400
P 9950 9000
F 0 "#PWR032" H 9950 8850 50  0001 C CNN
F 1 "VCC" H 9950 9150 50  0000 C CNN
F 2 "" H 9950 9000 50  0000 C CNN
F 3 "" H 9950 9000 50  0000 C CNN
	1    9950 9000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR033
U 1 1 58BB048C
P 10450 9000
F 0 "#PWR033" H 10450 8750 50  0001 C CNN
F 1 "GND" H 10450 8850 50  0000 C CNN
F 2 "" H 10450 9000 50  0000 C CNN
F 3 "" H 10450 9000 50  0000 C CNN
	1    10450 9000
	0    -1   -1   0   
$EndComp
Text Label 9950 9100 2    60   ~ 0
RXD
Text Label 10450 9100 0    60   ~ 0
TXD
Text Label 10450 9200 0    60   ~ 0
DIO4
$Comp
L C C4
U 1 1 58B958B2
P 2100 1650
F 0 "C4" H 2125 1750 50  0000 L CNN
F 1 "10uF" H 2125 1550 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_6.3x5.8" H 2138 1500 50  0001 C CNN
F 3 "" H 2100 1650 50  0000 C CNN
	1    2100 1650
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 58BB1D83
P 1750 1650
F 0 "C1" H 1775 1750 50  0000 L CNN
F 1 "100nF" H 1775 1550 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 1788 1500 50  0001 C CNN
F 3 "" H 1750 1650 50  0000 C CNN
	1    1750 1650
	1    0    0    -1  
$EndComp
$Comp
L RM50-xx11 RL7
U 1 1 58BB5C18
P 1200 7600
F 0 "RL7" H 1650 7750 50  0000 L CNN
F 1 "OZ-SS-103LM1" H 1650 7650 50  0000 L CNN
F 2 "Relays_ThroughHole:Relay_SPDT_Schrack-RT1_RM5mm" H 1200 7600 50  0001 C CNN
F 3 "" H 1200 7600 50  0001 C CNN
	1    1200 7600
	1    0    0    -1  
$EndComp
$Comp
L RM50-xx11 RL8
U 1 1 58BB5F82
P 1200 8750
F 0 "RL8" H 1650 8900 50  0000 L CNN
F 1 "OZ-SS-103LM1" H 1650 8800 50  0000 L CNN
F 2 "Relays_ThroughHole:Relay_SPDT_Schrack-RT1_RM5mm" H 1200 8750 50  0001 C CNN
F 3 "" H 1200 8750 50  0001 C CNN
	1    1200 8750
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR034
U 1 1 58BB8D34
P 1000 7300
F 0 "#PWR034" H 1000 7150 50  0001 C CNN
F 1 "VCC" H 1000 7450 50  0000 C CNN
F 2 "" H 1000 7300 50  0000 C CNN
F 3 "" H 1000 7300 50  0000 C CNN
	1    1000 7300
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR035
U 1 1 58BB8E00
P 1000 8450
F 0 "#PWR035" H 1000 8300 50  0001 C CNN
F 1 "VCC" H 1000 8600 50  0000 C CNN
F 2 "" H 1000 8450 50  0000 C CNN
F 3 "" H 1000 8450 50  0000 C CNN
	1    1000 8450
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q1
U 1 1 58BB8FBF
P 3050 9700
F 0 "Q1" H 3250 9750 50  0000 L CNN
F 1 "MMBF170" H 3250 9650 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 3250 9800 50  0001 C CNN
F 3 "" H 3050 9700 50  0000 C CNN
	1    3050 9700
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q2
U 1 1 58BB90FE
P 4050 9700
F 0 "Q2" H 4250 9750 50  0000 L CNN
F 1 "MMBF170" H 4250 9650 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 4250 9800 50  0001 C CNN
F 3 "" H 4050 9700 50  0000 C CNN
	1    4050 9700
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR036
U 1 1 58BB9432
P 3150 9900
F 0 "#PWR036" H 3150 9650 50  0001 C CNN
F 1 "GND" H 3150 9750 50  0000 C CNN
F 2 "" H 3150 9900 50  0000 C CNN
F 3 "" H 3150 9900 50  0000 C CNN
	1    3150 9900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR037
U 1 1 58BB9504
P 4150 9900
F 0 "#PWR037" H 4150 9650 50  0001 C CNN
F 1 "GND" H 4150 9750 50  0000 C CNN
F 2 "" H 4150 9900 50  0000 C CNN
F 3 "" H 4150 9900 50  0000 C CNN
	1    4150 9900
	1    0    0    -1  
$EndComp
Text GLabel 3150 9500 1    60   Input ~ 0
8A2
Text GLabel 4150 9500 1    60   Input ~ 0
7A2
Text Label 2850 9700 2    60   ~ 0
8G
Text Label 3850 9700 2    60   ~ 0
7G
Text Label 5900 9800 2    60   ~ 0
7G
Text Label 5900 9900 2    60   ~ 0
8G
$Comp
L D D2
U 1 1 58BBFF11
P 650 8750
F 0 "D2" H 650 8850 50  0000 C CNN
F 1 "CD0603-B00340" H 650 8650 50  0000 C CNN
F 2 "Diodes_SMD:D_0603" H 650 8750 50  0001 C CNN
F 3 "" H 650 8750 50  0000 C CNN
	1    650  8750
	0    1    1    0   
$EndComp
$Comp
L D D1
U 1 1 58BC004D
P 650 7600
F 0 "D1" H 650 7700 50  0000 C CNN
F 1 "CD0603-B00340" H 650 7500 50  0000 C CNN
F 2 "Diodes_SMD:D_0603" H 650 7600 50  0001 C CNN
F 3 "" H 650 7600 50  0000 C CNN
	1    650  7600
	0    1    1    0   
$EndComp
Text Label 7750 8750 0    60   ~ 0
TXD
Text Label 7750 8850 0    60   ~ 0
RXD
$Comp
L R R15
U 1 1 58BDA85E
P 1250 10300
F 0 "R15" V 1330 10300 50  0000 C CNN
F 1 "R" V 1250 10300 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 1180 10300 50  0001 C CNN
F 3 "" H 1250 10300 50  0000 C CNN
	1    1250 10300
	1    0    0    -1  
$EndComp
$Comp
L R R16
U 1 1 58BDA9A1
P 1250 10700
F 0 "R16" V 1330 10700 50  0000 C CNN
F 1 "R" V 1250 10700 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 1180 10700 50  0001 C CNN
F 3 "" H 1250 10700 50  0000 C CNN
	1    1250 10700
	1    0    0    -1  
$EndComp
$Comp
L R R18
U 1 1 58BDAA65
P 1550 10700
F 0 "R18" V 1630 10700 50  0000 C CNN
F 1 "R" V 1550 10700 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 1480 10700 50  0001 C CNN
F 3 "" H 1550 10700 50  0000 C CNN
	1    1550 10700
	1    0    0    -1  
$EndComp
$Comp
L R R17
U 1 1 58BDAB30
P 1550 10300
F 0 "R17" V 1630 10300 50  0000 C CNN
F 1 "R" V 1550 10300 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 1480 10300 50  0001 C CNN
F 3 "" H 1550 10300 50  0000 C CNN
	1    1550 10300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR038
U 1 1 58BDAE69
P 1400 10850
F 0 "#PWR038" H 1400 10600 50  0001 C CNN
F 1 "GND" H 1400 10700 50  0000 C CNN
F 2 "" H 1400 10850 50  0000 C CNN
F 3 "" H 1400 10850 50  0000 C CNN
	1    1400 10850
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR039
U 1 1 58BDB39C
P 1400 10150
F 0 "#PWR039" H 1400 10000 50  0001 C CNN
F 1 "VCC" H 1400 10300 50  0000 C CNN
F 2 "" H 1400 10150 50  0000 C CNN
F 3 "" H 1400 10150 50  0000 C CNN
	1    1400 10150
	1    0    0    -1  
$EndComp
Text Notes 1050 9750 0    60   ~ 0
Module ID determination
$Comp
L SW_Push SW1
U 1 1 58C6792C
P 5050 8800
F 0 "SW1" H 5100 8900 50  0000 L CNN
F 1 "SW_Push" H 5050 8740 50  0000 C CNN
F 2 "Buttons_Switches_ThroughHole:SW_PUSH_6mm_h7.3mm" H 5050 9000 50  0001 C CNN
F 3 "" H 5050 9000 50  0001 C CNN
	1    5050 8800
	1    0    0    -1  
$EndComp
$Comp
L SW_Push SW2
U 1 1 58C67D13
P 8250 9750
F 0 "SW2" H 8300 9850 50  0000 L CNN
F 1 "SW_Push" H 8250 9690 50  0000 C CNN
F 2 "Buttons_Switches_ThroughHole:SW_PUSH_6mm_h7.3mm" H 8250 9950 50  0001 C CNN
F 3 "" H 8250 9950 50  0001 C CNN
	1    8250 9750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR040
U 1 1 58C68103
P 8550 9750
F 0 "#PWR040" H 8550 9500 50  0001 C CNN
F 1 "GND" H 8550 9600 50  0000 C CNN
F 2 "" H 8550 9750 50  0000 C CNN
F 3 "" H 8550 9750 50  0000 C CNN
	1    8550 9750
	0    -1   -1   0   
$EndComp
$Comp
L R R20
U 1 1 58C68425
P 8000 9600
F 0 "R20" V 8080 9600 50  0000 C CNN
F 1 "10k" V 8000 9600 50  0000 C CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" V 7930 9600 50  0001 C CNN
F 3 "" H 8000 9600 50  0000 C CNN
	1    8000 9600
	1    0    0    -1  
$EndComp
$Comp
L R R19
U 1 1 58C686FC
P 5450 8650
F 0 "R19" V 5530 8650 50  0000 C CNN
F 1 "10k" V 5450 8650 50  0000 C CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" V 5380 8650 50  0001 C CNN
F 3 "" H 5450 8650 50  0000 C CNN
	1    5450 8650
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR041
U 1 1 58C68E82
P 5900 8700
F 0 "#PWR041" H 5900 8550 50  0001 C CNN
F 1 "VCC" H 5900 8850 50  0000 C CNN
F 2 "" H 5900 8700 50  0000 C CNN
F 3 "" H 5900 8700 50  0000 C CNN
	1    5900 8700
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR042
U 1 1 58C6907D
P 5450 8500
F 0 "#PWR042" H 5450 8350 50  0001 C CNN
F 1 "VCC" H 5450 8650 50  0000 C CNN
F 2 "" H 5450 8500 50  0000 C CNN
F 3 "" H 5450 8500 50  0000 C CNN
	1    5450 8500
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR043
U 1 1 58C69142
P 8000 9450
F 0 "#PWR043" H 8000 9300 50  0001 C CNN
F 1 "VCC" H 8000 9600 50  0000 C CNN
F 2 "" H 8000 9450 50  0000 C CNN
F 3 "" H 8000 9450 50  0000 C CNN
	1    8000 9450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR044
U 1 1 58C69283
P 4700 8800
F 0 "#PWR044" H 4700 8550 50  0001 C CNN
F 1 "GND" H 4700 8650 50  0000 C CNN
F 2 "" H 4700 8800 50  0000 C CNN
F 3 "" H 4700 8800 50  0000 C CNN
	1    4700 8800
	0    1    1    0   
$EndComp
Text Label 9950 9200 2    60   ~ 0
DIO0
Text Label 7950 9750 1    60   ~ 0
DIO0
Text Label 7850 9650 1    60   ~ 0
DIO4
$Comp
L CONN_02X03 P1
U 1 1 58C6DC49
P 10200 9100
F 0 "P1" H 10200 9300 50  0000 C CNN
F 1 "CONN_02X03" H 10200 8900 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03" H 10200 7900 50  0001 C CNN
F 3 "" H 10200 7900 50  0000 C CNN
	1    10200 9100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 5450 1700 4850
Wire Wire Line
	1700 4850 1450 4850
Wire Wire Line
	1700 5650 1700 5500
Wire Wire Line
	1700 5500 1350 5500
Wire Wire Line
	1350 5500 1350 5450
Wire Wire Line
	1450 5650 1600 5650
Wire Wire Line
	1600 5650 1600 5850
Wire Wire Line
	1600 5850 1700 5850
Wire Wire Line
	1700 6050 1600 6050
Wire Wire Line
	1600 6050 1600 6300
Wire Wire Line
	1600 6300 1350 6300
Wire Wire Line
	1350 6300 1350 6250
Wire Wire Line
	1700 6250 1650 6250
Wire Wire Line
	1650 6250 1650 6350
Wire Wire Line
	1650 6350 1450 6350
Wire Wire Line
	1450 6350 1450 6450
Wire Wire Line
	1700 6450 1700 7100
Wire Wire Line
	1700 7100 1350 7100
Wire Wire Line
	1350 7100 1350 7050
Wire Wire Line
	1450 2400 1750 2400
Wire Wire Line
	1750 2400 1750 3000
Wire Wire Line
	1350 3000 1700 3000
Wire Wire Line
	1700 3000 1700 3200
Wire Wire Line
	1700 3200 1750 3200
Wire Wire Line
	1450 3200 1650 3200
Wire Wire Line
	1650 3200 1650 3400
Wire Wire Line
	1650 3400 1750 3400
Wire Wire Line
	1750 3600 1650 3600
Wire Wire Line
	1650 3600 1650 3800
Wire Wire Line
	1650 3800 1350 3800
Wire Wire Line
	1750 3800 1700 3800
Wire Wire Line
	1700 3800 1700 3900
Wire Wire Line
	1700 3900 1450 3900
Wire Wire Line
	1450 3900 1450 4000
Wire Wire Line
	1750 4000 1750 4600
Wire Wire Line
	1750 4600 1350 4600
Wire Wire Line
	1400 7900 1400 8100
Wire Wire Line
	6650 950  7150 950 
Wire Wire Line
	6650 5400 7150 5400
Wire Wire Line
	8400 3900 8900 3900
Wire Wire Line
	10150 3900 10650 3900
Wire Wire Line
	6650 2400 7150 2400
Wire Wire Line
	8400 950  8900 950 
Wire Wire Line
	8400 5400 8900 5400
Wire Wire Line
	10150 5350 10650 5350
Wire Wire Line
	6650 3900 7150 3900
Wire Wire Line
	8400 2400 8900 2400
Wire Wire Line
	10150 950  10650 950 
Wire Wire Line
	10650 2400 10150 2400
Wire Wire Line
	6150 1900 6150 1550
Wire Wire Line
	6150 1550 6100 1550
Wire Wire Line
	6150 2100 6650 2100
Wire Wire Line
	7950 2100 8400 2100
Wire Wire Line
	6150 3550 6650 3550
Wire Wire Line
	7950 3550 8400 3550
Wire Wire Line
	9700 2100 10150 2100
Wire Wire Line
	9700 3550 10150 3550
Wire Wire Line
	9700 5050 10150 5050
Wire Wire Line
	7950 5050 8400 5050
Wire Wire Line
	6200 5050 6650 5050
Wire Wire Line
	6200 6550 6650 6550
Wire Wire Line
	7950 6550 8400 6550
Wire Wire Line
	9700 6500 10150 6500
Wire Wire Line
	6150 3350 6150 3000
Wire Wire Line
	6150 3000 6100 3000
Wire Wire Line
	6200 4850 6200 4500
Wire Wire Line
	6200 4500 6100 4500
Wire Wire Line
	6200 6350 6200 6000
Wire Wire Line
	6200 6000 6100 6000
Wire Wire Line
	7950 6000 7950 6350
Wire Wire Line
	7850 6000 7950 6000
Wire Wire Line
	7950 4850 7950 4500
Wire Wire Line
	7950 4500 7850 4500
Wire Wire Line
	9700 4850 9700 4500
Wire Wire Line
	9700 4500 9600 4500
Wire Wire Line
	9700 6300 9700 5950
Wire Wire Line
	9700 5950 9600 5950
Wire Wire Line
	7950 3350 7950 3000
Wire Wire Line
	7950 3000 7850 3000
Wire Wire Line
	7950 1900 7950 1550
Wire Wire Line
	7950 1550 7850 1550
Wire Wire Line
	9700 1900 9700 1550
Wire Wire Line
	9700 1550 9600 1550
Wire Wire Line
	9700 3350 9700 3000
Wire Wire Line
	9700 3000 9600 3000
Wire Wire Line
	4700 3800 4700 4000
Wire Wire Line
	4700 4000 3300 4000
Wire Wire Line
	3300 4000 3300 4200
Wire Wire Line
	4700 5100 4700 5300
Wire Wire Line
	4700 5300 3300 5300
Wire Wire Line
	3300 5300 3300 5500
Wire Wire Line
	1300 1500 1300 2000
Wire Wire Line
	1300 2000 4750 2000
Wire Wire Line
	2100 1500 2100 1300
Connection ~ 2100 1300
Wire Wire Line
	2100 1800 2100 2000
Connection ~ 2100 2000
Wire Wire Line
	1750 1150 1750 1500
Connection ~ 1750 1300
Wire Wire Line
	1750 1800 1750 2000
Connection ~ 1750 2000
Wire Wire Line
	3600 1250 3600 1400
Wire Wire Line
	3600 2000 3600 1700
Wire Wire Line
	1400 8100 1750 8100
Wire Wire Line
	1750 7900 1750 7300
Wire Wire Line
	1750 7300 1500 7300
Wire Wire Line
	1750 8300 1650 8300
Wire Wire Line
	1650 8300 1650 7250
Wire Wire Line
	1650 7250 1300 7250
Wire Wire Line
	1300 7250 1300 7300
Wire Wire Line
	1750 8500 1750 8450
Wire Wire Line
	1750 8450 1500 8450
Wire Wire Line
	1750 8700 1650 8700
Wire Wire Line
	1650 8700 1650 9050
Wire Wire Line
	1650 9050 1400 9050
Wire Wire Line
	1300 8450 1300 8400
Wire Wire Line
	1300 8400 1700 8400
Wire Wire Line
	1700 8400 1700 8900
Wire Wire Line
	1700 8900 1750 8900
Wire Wire Line
	650  7450 650  7300
Wire Wire Line
	650  7300 1000 7300
Wire Wire Line
	1000 7900 650  7900
Wire Wire Line
	650  7900 650  7750
Wire Wire Line
	650  8600 650  8450
Wire Wire Line
	650  8450 1000 8450
Wire Wire Line
	1000 9050 650  9050
Wire Wire Line
	650  9050 650  8900
Wire Wire Line
	1250 10450 1250 10550
Wire Wire Line
	1550 10450 1550 10550
Wire Wire Line
	1250 10850 1550 10850
Connection ~ 1400 10850
Wire Wire Line
	1250 10150 1550 10150
Connection ~ 1400 10150
Wire Notes Line
	1850 9800 1050 9800
Wire Notes Line
	1050 9800 1050 11100
Wire Notes Line
	1050 11100 1850 11100
Wire Notes Line
	1850 11100 1850 9800
Wire Wire Line
	8550 9750 8450 9750
Wire Wire Line
	8050 9750 7750 9750
Wire Wire Line
	5250 8800 5900 8800
Connection ~ 5450 8800
Wire Wire Line
	4850 8800 4700 8800
Connection ~ 8000 9750
Wire Wire Line
	7750 9650 7850 9650
Connection ~ 3600 1300
$Comp
L LM7806CT U1
U 1 1 59C0FA96
P 2800 1350
F 0 "U1" H 2600 1550 50  0000 C CNN
F 1 "TR10S3V3" H 2800 1550 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-220_Vertical" H 2800 1450 50  0001 C CIN
F 3 "" H 2800 1350 50  0001 C CNN
	1    2800 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 1600 2800 2000
Connection ~ 2800 2000
Wire Wire Line
	3200 1300 4750 1300
Wire Wire Line
	2400 1300 1300 1300
NoConn ~ 5900 8900
NoConn ~ 5900 9000
NoConn ~ 5900 9100
NoConn ~ 5900 9200
NoConn ~ 7750 8550
NoConn ~ 7750 8650
NoConn ~ 7750 8950
NoConn ~ 7750 9050
NoConn ~ 7750 9150
NoConn ~ 7750 9250
NoConn ~ 7750 9350
NoConn ~ 7750 9450
NoConn ~ 7750 9550
NoConn ~ 6500 10350
NoConn ~ 6600 10350
NoConn ~ 6700 10350
NoConn ~ 6800 10350
NoConn ~ 6900 10350
NoConn ~ 7000 10350
NoConn ~ 7100 10350
NoConn ~ 7200 10350
NoConn ~ 7300 10350
NoConn ~ 4700 6400
$Comp
L C C2
U 1 1 59C11EB6
P 3900 1550
F 0 "C2" H 3925 1650 50  0000 L CNN
F 1 "100nF" H 3925 1450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 3938 1400 50  0001 C CNN
F 3 "" H 3900 1550 50  0000 C CNN
	1    3900 1550
	1    0    0    -1  
$EndComp
$Comp
L C C5
U 1 1 59C12732
P 4200 1550
F 0 "C5" H 4225 1650 50  0000 L CNN
F 1 "100nF" H 4225 1450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 4238 1400 50  0001 C CNN
F 3 "" H 4200 1550 50  0000 C CNN
	1    4200 1550
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 59C1280B
P 4500 1550
F 0 "C6" H 4525 1650 50  0000 L CNN
F 1 "100nF" H 4525 1450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 4538 1400 50  0001 C CNN
F 3 "" H 4500 1550 50  0000 C CNN
	1    4500 1550
	1    0    0    -1  
$EndComp
$Comp
L C C7
U 1 1 59C128E1
P 4750 1550
F 0 "C7" H 4775 1650 50  0000 L CNN
F 1 "100nF" H 4775 1450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 4788 1400 50  0001 C CNN
F 3 "" H 4750 1550 50  0000 C CNN
	1    4750 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 1300 3900 1400
Wire Wire Line
	4750 1300 4750 1400
Connection ~ 3900 1300
Wire Wire Line
	4200 1400 4200 1300
Connection ~ 4200 1300
Wire Wire Line
	4500 1400 4500 1300
Connection ~ 4500 1300
Wire Wire Line
	4750 2000 4750 1700
Connection ~ 3600 2000
Wire Wire Line
	3900 1700 3900 2000
Connection ~ 3900 2000
Wire Wire Line
	4200 1700 4200 2000
Connection ~ 4200 2000
Wire Wire Line
	4500 1700 4500 2000
Connection ~ 4500 2000
$EndSCHEMATC