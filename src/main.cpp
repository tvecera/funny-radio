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
#include <main.h>

uint16_t moviesFiles[MOVIE_NUM_FILES];
uint16_t lakatosFiles[LAKATOS_NUM_FILES];

struct Playlist moviePlaylist = {MOVIE_NUM_FILES, MOVIE_START_INDEX, 0,
                                 moviesFiles};
struct Playlist lakatosPlaylist = {LAKATOS_NUM_FILES, LAKATOS_START_INDEX, 0,
                                   lakatosFiles};

uint32_t lastPlayTime;
bool inSleep = false;

uint16_t checksum(uint8_t* packet) {
  uint16_t sum = 0;
  for (int i = 1; i < 7; i++) {
    sum += packet[i];
  }
  return -sum;
}

/**
 * 7E FF 06 0F 00 01 01 xx xx EF
 *
 * 7E - start byte
 * FF - version
 * 06 - length
 * XX - command - 0x06 Specify volume (0-30), 0x08 Specify playback mode
 *      (0/1/2/3) Repeat/folder repeat/single repeat/random
 * 00 - if need for feedback, 1: feedback, 0: no feedback
 * XX	- parameter 1 - Query high data byte
 * XX - parameter 2 - Query low data byte
 * XXXX	- accumulation and verification (not include Checksum start byte)
 * EF	-	end byte
 */
void send(uint8_t command, uint16_t parameter = 0) {
  uint8_t out[PACKET_SIZE] = {START_BIT,
                              VERSION_BIT,
                              SIZE_BIT,
                              command,
                              00,
                              static_cast<uint8_t>(parameter >> 8),
                              static_cast<uint8_t>(parameter & 0x00FF),
                              00,
                              00,
                              END_BIT};
  uint16_t sum = checksum(out);
  out[7] = (sum >> 8);
  out[8] = (sum & 0xFF);

  uartWriter.on(PLAYER_TX_PORT, PLAYER_BAUDRATE, Timer0Compare);
  for (uint8_t i = 0; i < PACKET_SIZE; i++) {
    uartWriter.write(out[i]);
  }

  while (uartWriter.outputRemaining()) {
    sleep(SLEEP_MODE_IDLE);
  }

  uartWriter.off();
}

void fisherYatesShuffle(struct Playlist* playlist) {
  for (uint16_t i = 0; i < playlist->files; i++) {
    playlist->songs[i] = i + playlist->startIndex;
  }

  for (uint16_t i = playlist->files - 1; i > 0; i--) {
    uint16_t j;
    j = random(RAND_MAX) % (i + 1);
    if (j != i) {
      uint16_t swap = playlist->songs[j];
      playlist->songs[j] = playlist->songs[i];
      playlist->songs[i] = swap;
    }
  }
}

void playtrack(uint16_t fileIndex) {
  send(0x03, fileIndex);
  delay(DELAY_MAX);
}

void shufflePlay(struct Playlist* playlist) {
  uint16_t fileIndex = playlist->songs[playlist->currentIndex];
  playtrack(fileIndex);
  if (playlist->currentIndex < playlist->files - 1) {
    playlist->currentIndex = playlist->currentIndex + 1;
  } else {
    fisherYatesShuffle(playlist);
    playlist->currentIndex = 0;
  }
}

void setPlaybackSourceSD() {
  send(0x09, 0x01);
  delay(DELAY_MAX);
}

void setPlaybackSourceSleep() {
  send(0x09, 0x03);
  delay(DELAY_MAX);
}

void setVolume(uint16_t volume) {
  send(0x06, volume);
  delay(DELAY_MIN);
}

void sleepPlayer() {
  send(0x0A);
  delay(DELAY_MAX);
}

void playerOff() {
  inSleep = true;
  setPlaybackSourceSleep();
  sleepPlayer();
  uartWriter.off();
}

void playerOn() {
  if (inSleep) {
    inSleep = false;
    setPlaybackSourceSD();
  }
}

void setup() {
  sei();
  delay(500);
  playerOn();
  delay(3000);
  setVolume(VOLUME);

  fisherYatesShuffle(&moviePlaylist);
  fisherYatesShuffle(&lakatosPlaylist);

  pinMode(BUTTON_PORT, INPUT);
  pinMode(PLAYER_BUSY_PORT, INPUT);

  randomSeed(analogRead(0));
  lastPlayTime = millis();
}

void loop() {
  if (digitalRead(BUTTON_PORT) == HIGH) {
    lastPlayTime = millis();
    playerOn();
    if (digitalRead(PLAYER_BUSY_PORT) == HIGH) {
      shufflePlay(&moviePlaylist);
    } else {
      shufflePlay(&lakatosPlaylist);
    }
  } else {
    sleep(SLEEP_MODE_IDLE);
  }

  if (millis() - lastPlayTime > IDDLE_TIME && !inSleep) {
    playerOff();
  }
}
