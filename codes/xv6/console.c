// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "prefix_predict.h"

static void consputc(int);


static int panicked = 0;

static struct {
  struct spinlock lock;
  int locking;
} cons;

static void printint(int xx, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    consputc(buf[i]);
}
//PAGEBREAK: 50

// Print to the console. only understands %d, %x, %p, %s.
void cprintf(char *fmt, ...)
{
  int i, c, locking;
  uint *argp;
  char *s;

  locking = cons.locking;
  if(locking)
    acquire(&cons.lock);

  if (fmt == 0)
    panic("null fmt");

  argp = (uint*)(void*)(&fmt + 1);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      printint(*argp++, 10, 1);
      break;
    case 'x':
    case 'p':
      printint(*argp++, 16, 0);
      break;
    case 's':
      if((s = (char*)*argp++) == 0)
        s = "(null)";
      for(; *s; s++)
        consputc(*s);
      break;
    case '%':
      consputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c);
      break;
    }
  }

  if(locking)
    release(&cons.lock);
}

void panic(char *s)
{
  int i;
  uint pcs[10];

  cli();
  cons.locking = 0;
  // use lapiccpunum so that we can call panic from mycpu()
  cprintf("lapicid %d: panic: ", lapicid());
  cprintf(s);
  cprintf("\n");
  getcallerpcs(&s, pcs);
  for(i=0; i<10; i++)
    cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for(;;)
    ;
}

//PAGEBREAK: 50
#define BACKSPACE 0x100
#define CRTPORT 0x3d4
static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory

int get_pos() {
  int pos;

  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);

  return pos;
}

static void change_pos(int pos) {
  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
}

static void cgaputc(int c)
{
  int pos;

  // Cursor position: col + 80*row.
  pos = get_pos();

  if(c == '\n')
    pos += 80 - pos%80;
  else if(c == BACKSPACE){
    if(pos > 0) --pos;
  } else
    crt[pos++] = (c&0xff) | 0x0700;  // black on white

  if(pos < 0 || pos > 25*80)
    panic("pos under/overflow");

  if((pos/80) >= 24){  // Scroll up.
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
    pos -= 80;
    memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
  }

  change_pos(pos);
  if(c == BACKSPACE)
    crt[pos] = ' ' | 0x0700;
}

void consputc(int c)
{
  if(panicked){
    cli();
    for(;;)
      ;
  }

  if(c == BACKSPACE){
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else
    uartputc(c);
  cgaputc(c);
}

#define INPUT_BUF 128
struct {
  char buf[INPUT_BUF];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
  uint end; //end index
} input;

#define C(x)  ((x)-'@')  // Control-x

void shift_input_right() {
  int index, next_char, pos;
  pos = get_pos();
  change_pos(pos + 1);
  index = input.e;
  next_char = input.buf[index % INPUT_BUF];
  while(index < input.end) {
    int temp = next_char;
    next_char = input.buf[(index + 1) % INPUT_BUF];
    input.buf[(index + 1) % INPUT_BUF] = temp;
    consputc(input.buf[(index + 1) % INPUT_BUF]);
    index++;
  }
  input.end++;
  change_pos(pos);
}

void shift_input_left() {
  int index, pos;
  pos = get_pos();
  index = input.e - 1;
  while(index < input.end) {
    input.buf[index % INPUT_BUF] = input.buf[(index + 1) % INPUT_BUF];
    consputc(input.buf[index % INPUT_BUF]);
    index++;
  }
  consputc(' ');
  input.end--;
  change_pos(pos);
}

void go_to_first_of_line() {
  int pos;
  pos = get_pos();
  int change;
  change = pos%80 - 2;
  input.e -= change;
  change_pos(pos - change);
}

int is_num(int c) {
  return (c >= '0' && c<='9') ? 1 : 0;
}

void kill_line(){
  while(input.e != input.w && input.buf[(input.e-1) % INPUT_BUF] != '\n') {
    if(input.e != input.w){
      consputc(BACKSPACE);
      shift_input_left();
      input.e--;
    }
  }
}
void back_space() {
    if(input.e != input.w){
    consputc(BACKSPACE);
    shift_input_left();
    input.e--;
  }
}
void delete_num_of_line() {
  go_to_first_of_line();
  while (input.e < input.end){
    int pos = get_pos();
    if(is_num(input.buf[(input.e) % INPUT_BUF])){
      int pos=get_pos();
      change_pos(++pos);
      input.e++;
      consputc(BACKSPACE);
      shift_input_left();
      input.e--;
    }
    else{
      input.buf[(input.e) % INPUT_BUF] = input.buf[(input.e) % INPUT_BUF];
      consputc(input.buf[(input.e) % INPUT_BUF]);
      change_pos(++pos);
      input.e++;
    }
  }
}
void print_word(char* key) {
  char temp[INPUT_BUF];
  strncpy(temp,key,INPUT_BUF);
  kill_line();
  for (int i = 0; i < strlen(temp)-1; i++){
    input.buf[input.e % INPUT_BUF] = temp[i];
    consputc(input.buf[input.e % INPUT_BUF]);
    input.e++;
    input.end++;
  }
}
void print_word_reverse(char* key) {
  char temp[INPUT_BUF];
  strncpy(temp,key,INPUT_BUF);
  kill_line();
  for (int i = strlen(temp)-1; i >= 0; i--) {
    input.buf[input.e % INPUT_BUF] = temp[i];
    consputc(input.buf[input.e % INPUT_BUF]);
    input.e++;
    input.end++;
  }
}
void reverse_line() {
  char* key = input.buf + input.w; 
  print_word_reverse(key);
}
int predict_in_command_list() {
  int index = -1;
  char* key = input.buf + input.w; 
  if(size_command<MAX_COMMAND_NUM)
      for (int i = size_command; i >= 0; i--)
          if(starts_with(key,command[i]))
              index = i;
  else {
      int endIndex = ((command_num % MAX_COMMAND_NUM) + (MAX_COMMAND_NUM - 1))
      % MAX_COMMAND_NUM ;
      int i = ( command_num % MAX_COMMAND_NUM);
      while (i != endIndex)
      {
          if(starts_with(key,command[i])) {
              index = i;
          }
          i++;
          if(i == MAX_COMMAND_NUM) i = 0;
      }
      if(starts_with(key,command[i])) index = i;
  }
  return index;
}
void predict_process() {
  int index = predict_in_command_list();
  if(index!=-1) 
    print_word(command[index]);
}
void consoleintr(int (*getc)(void))
{
  int c, doprocdump = 0;

  acquire(&cons.lock);
  while((c = getc()) >= 0){
    switch(c){
    case C('P'):  // Process listing.
      // procdump() locks cons.lock indirectly; invoke later
      doprocdump = 1;
      break;
    case C('U'):
      kill_line();
      break;
    case C('H'): case '\x7f':
      back_space();
      break;
    case C('N'):{
      delete_num_of_line();
      break;
    }
    case C('R'): {
      reverse_line();
      break;
    }
    case '\t': {
      predict_process();
      break;
    }
    default:
      if(c != 0 && input.e-input.r < INPUT_BUF){
        c = (c == '\r') ? '\n' : c;
        if(c == '\n' || c == C('D'))
          input.buf[input.end++ % INPUT_BUF] = c;
        else {
          shift_input_right();
          input.buf[input.e++ % INPUT_BUF] = c;
        }

        consputc(c);

        if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
          char* key;
          key = input.buf + input.w;
          update_history(key, input.e - input.w);
          size_command++;
          command_num++;
          input.e = input.end;
          input.w = input.e;
          wakeup(&input.r);
        }
      }
      break;
    }
  }
  release(&cons.lock);
  if(doprocdump) {
    procdump();  // now call procdump() wo. cons.lock held
  }
}

int consoleread(struct inode *ip, char *dst, int n)
{
  uint target;
  int c;

  iunlock(ip);
  target = n;
  acquire(&cons.lock);
  while(n > 0){
    while(input.r == input.w){
      if(myproc()->killed){
        release(&cons.lock);
        ilock(ip);
        return -1;
      }
      sleep(&input.r, &cons.lock);
    }
    c = input.buf[input.r++ % INPUT_BUF];
    if(c == C('D')){  // EOF
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        input.r--;
      }
      break;
    }
    *dst++ = c;
    --n;
    if(c == '\n')
      break;
  }
  release(&cons.lock);
  ilock(ip);

  return target - n;
}

int consolewrite(struct inode *ip, char *buf, int n)
{
  int i;

  iunlock(ip);
  acquire(&cons.lock);
  for(i = 0; i < n; i++)
    consputc(buf[i] & 0xff);
  release(&cons.lock);
  ilock(ip);

  return n;
}

void consoleinit(void)
{
  initlock(&cons.lock, "console");

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;
  cons.locking = 1;

  ioapicenable(IRQ_KBD, 0);
}

