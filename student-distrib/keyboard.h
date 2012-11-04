
#ifndef KEYBOARD_H
#define KEYBOARD_H


/*** io constants ***/
#define KEYBOARD_PORT		0x60
#define KEYBOARD_STATUS_PORT	0x64

/*** irq constant ***/
#define KEYBOARD_IRQ		1

#define TERMINAL_BUFFER_MAX_SIZE   1024

/* Called to read from command buffer */
int terminal_read(unsigned char * buf, int cnt);

/* Called to read from command buffer */
void printthebuffer();

/* Called to initialize keyboard before using it. */
void keyboard_init(void);
/* Keyboard Interrupt */
void keyboard_interruption(void);



#endif
