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

static void
printint(int xx, int base, int sign)
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
void
cprintf(char *fmt, ...)
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

void
panic(char *s)
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

// cursor
int
getcr() {
  int pos;

  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);

  return pos;
}

static void 
changepos(int pos) {
  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
}

static void
cgaputc(int c)
{
  int pos;

  // Cursor position: col + 80*row.
  pos = getcr();

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

  changepos(pos);
  if(c == BACKSPACE)
    crt[pos] = ' ' | 0x0700;
}

void
consputc(int c)
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

void
shiftrinput() {
  int index, next_char, pos;
  pos = getcr();
  changepos(pos + 1);
  index = input.e;
  next_char = input.buf[index % INPUT_BUF];
  while(index < input.end) {
    int tmp = next_char;
    next_char = input.buf[(index + 1) % INPUT_BUF];
    input.buf[(index + 1) % INPUT_BUF] = tmp;
    consputc(input.buf[(index + 1) % INPUT_BUF]);
    index++;
  }
  input.end++;
  changepos(pos);
}

void
shiftlinput() {
  int index, pos;
  pos = getcr();
  index = input.e - 1;
  while(index < input.end) {
    input.buf[index % INPUT_BUF] = input.buf[(index + 1) % INPUT_BUF];
    consputc(input.buf[index % INPUT_BUF]);
    index++;
  }
  consputc(' ');
  input.end--;
  changepos(pos);
}

void gotofirstofline() {
  int pos;
  pos = getcr();
  int change;
  change = pos%80 - 2;
  input.e -= change;
  changepos(pos - change);
}

int isnum(int c) {
  if(c >= '0' && c<='9')
    return 1;
  else
    return 0;
}

void killall(){
  while(input.e != input.w && input.buf[(input.e-1) % INPUT_BUF] != '\n') {
    if(input.e != input.w){
      consputc(BACKSPACE);
      shiftlinput();
      input.e--;
    }
  }
}

void
consoleintr(int (*getc)(void))
{
  int c, doprocdump = 0;

  acquire(&cons.lock);
  while((c = getc()) >= 0){
    switch(c){
    case C('P'):  // Process listing.
      // procdump() locks cons.lock indirectly; invoke later
      doprocdump = 1;
      break;
    case C('U'):  // Kill line.
      killall();
      break;
    case C('H'): case '\x7f':  // Backspace
      if(input.e != input.w){
        consputc(BACKSPACE);
        shiftlinput();
        input.e--;
      }
      break;
    case C('N'):{
      gotofirstofline();

      while (input.e < input.end){
        int pos = getcr();
        if(isnum(input.buf[(input.e) % INPUT_BUF])){
          int pos=getcr();
          changepos(++pos);
          input.e++;
          consputc(BACKSPACE);
          shiftlinput();
          input.e--;
        }
        else{
          input.buf[(input.e) % INPUT_BUF] = input.buf[(input.e) % INPUT_BUF];
          consputc(input.buf[(input.e) % INPUT_BUF]);
          changepos(++pos);
          input.e++;
        }
      }
      break;
    }
    case C('R'): {
      int size = input.end;
      char tmp[INPUT_BUF];
      
      for (int i = 0; i < size; i++)
        tmp[i] = input.buf[(input.e - (i+1)) % INPUT_BUF];

      for (int i = 0; i < size; i++)
        input.buf[(input.e - (size-i)) % INPUT_BUF] = tmp[i];

      for (int i = 0; i < size; i++)
        consputc(BACKSPACE);

      for (int i = 0; i < size; i++)
        consputc(input.buf[(input.e - (size-i)) % INPUT_BUF]);

      break;
    }
    case '\t': {
      int index=-1;
      char* key;
      key = input.buf + input.w; 
      if(sizeCommand<15)
          for (int i = 0; i < sizeCommand; i++)
              if(startswith(key,command[i]))
                  index = i;
      else {
          int endIndex = ((command_num % 15) + 14) % 15 ;
          int i = ( command_num % 15);
          while (i != endIndex)
          {
              if(startswith(key,command[i])) {
                  index = i;
              }
              i++;
              if(i==15) i = 0;
          }
          if(startswith(key,command[i])) index = i;

      }
      if(index!=-1){
        int size = input.e-input.w;
        killall();
        for (int i = 0; i < strlen(command[index])-1; i++)
          consputc(command[index][i]);
      }
      break;
    }
    default:
      if(c != 0 && input.e-input.r < INPUT_BUF){
        c = (c == '\r') ? '\n' : c;
        if(c == '\n' || c == C('D'))
          input.buf[input.end++ % INPUT_BUF] = c;
        else {
          shiftrinput();
          input.buf[input.e++ % INPUT_BUF] = c;
        }

        consputc(c);

        if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
          char* key;
          key = input.buf + input.w; 
          updatehistory(key,input.e-input.w);
          sizeCommand++;
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

int
consoleread(struct inode *ip, char *dst, int n)
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

int
consolewrite(struct inode *ip, char *buf, int n)
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

void
consoleinit(void)
{
  initlock(&cons.lock, "console");

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;
  cons.locking = 1;

  ioapicenable(IRQ_KBD, 0);
}

