/**
 * Funny radio V1.0
 *
 * Copyright (C) 2020 Tomas Vecera tomas@vecera.dev
 *
 * This code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this code.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

#include <Arduino.h>
#include <TinyPinChangeB.h>
#include <TinyTimer0Compare.h>
#include <TinyUartWriter.h>
#include <avr/sleep.h>

#define MOVIE_NUM_FILES 50
#define MOVIE_START_INDEX 1
#define LAKATOS_NUM_FILES 14
#define LAKATOS_START_INDEX 51
#define VOLUME 16
#define IDDLE_TIME 180000

#define PLAYER_TX_PORT PB1
#define PLAYER_BUSY_PORT PB2
#define BUTTON_PORT PB0
#define PLAYER_BAUDRATE 9600
#define MAX_RECEIVE_TIME 2000
#define DELAY_MIN 50
#define DELAY_MAX 250

#define PACKET_SIZE 10
#define START_BIT 0x7E
#define VERSION_BIT 0xFF
#define SIZE_BIT 0x06
#define END_BIT 0xEF
#define MICRO_SD_BIT 0x3D

struct Playlist {
    uint16_t files;
    uint16_t startIndex;
    uint16_t currentIndex;
    uint16_t * songs;
} playlist;

#endif  // SRC_MAIN_H_
