#pragma once
#include <stdio.h>
#include <windows.h>
#include <string>
#include <process.h>

extern void send_package(u16 v_x, u16 v_y, u16 v_z, u8 mod);
extern void intial();
extern void shake_hand();
extern void package_parser();