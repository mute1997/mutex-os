#include <drivers/keyboard.h>
#include <asm/types.h>
#include <std/printk.h>
#include <mm.h>

char key_inputs[256];
char scancodes[0xff];
char shifted_scancodes[0xff];
int is_pressed_shift = NOT_PRESS;
int is_pressed_enter = NOT_PRESS;

extern char log_buf[LOG_BUF_SIZE][256];
extern unsigned long bottom;

void init_keyboard() {
  memset(scancodes, NULL, sizeof(scancodes));

  int alphabet[] = {
    0x1e, 0x30, 0x2e, 0x20, 0x12,
    0x21, 0x22, 0x23, 0x17, 0x24,
    0x25, 0x26, 0x32, 0x31, 0x18,
    0x19, 0x10, 0x13, 0x1f, 0x14,
    0x16, 0x2f, 0x11, 0x2d, 0x15,
    0x2c};

  int numbers[] = {0x0b, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a};

  // enter
  scancodes[0x1c] = '\n';

  // alphabet
  char a = 'a';
  int index;
  for (int i=0;i<26;i++) {
    index = alphabet[i];
    scancodes[index] = a;
    a++;
  }

  // number
  char n = '0';
  for (int i=0;i<10;i++) {
    index = numbers[i];
    scancodes[index] = n;
    n++;
  }

  // number
  shifted_scancodes[0x0b] = ')';
  shifted_scancodes[0x02] = '!';
  shifted_scancodes[0x03] = '@';
  shifted_scancodes[0x04] = '#';
  shifted_scancodes[0x05] = '$';
  shifted_scancodes[0x06] = '%';
  shifted_scancodes[0x07] = '^';
  shifted_scancodes[0x08] = '&';
  shifted_scancodes[0x09] = '*';
  shifted_scancodes[0x0a] = '(';

  // space
  scancodes[0x39] = ' ';
}

char get_char_from_scancode(int scancode) {
  char ret;

  // shift
  if (scancode == 0x2a || scancode == 0x36) {
    is_pressed_shift = PRESS;
  } else if (scancode == 0xaa || scancode == 0xb6) {
    is_pressed_shift = NOT_PRESS;
  }

  // enter
  if (scancode == 0x1c) {
    is_pressed_enter = PRESS;
  } else if (scancode == 0x9c) {
    is_pressed_enter = NOT_PRESS;
    memset(key_inputs, NULL, sizeof(key_inputs));
  }

  // backspace
  if (scancode == 0x0e) {
    if (strlen(key_inputs) > 0) {
      key_inputs[strlen(key_inputs)-1] = NULL;
      log_buf[bottom][strlen(log_buf[bottom])-1] = NULL;
      output_log_buf();
    }
  }

  if (is_pressed_shift) ret = shifted_scancodes[scancode];
  else ret = scancodes[scancode];

  if (ret != '\n') {
    key_inputs[strlen(key_inputs)] = ret;
  }

  return ret;
}
