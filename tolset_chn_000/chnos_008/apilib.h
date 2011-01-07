typedef enum _bool { false, true} bool;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef enum _color_8 { black, red, green, yellow, 
			blue, pink, light_blue, white, 
			gray, brown, dark_green, gold, 
			navy_blue, purple, dark_cyan, dark_gray} color_8;

int rand(void);
#define srand(seed) (void) (rand_seed = (seed))
extern unsigned int rand_seed;

void api_putchar(uchar c);
//	hariboteOS:ŒÝŠ·void api_putchar(int c);
void api_putstr(uchar *s);
//	hariboteOS:ŒÝŠ·void api_putstr0(char *s);
void api_end(void);
//	hariboteOS:ŒÝŠ·void api_end(void);
uint api_openwin(uchar *buf, int xsize, int ysize, int col_inv, uchar *title);
//	hariboteOS:ŒÝŠ·int api_openwin(char *buf, int xsiz, int ysiz, int col_inv, char *title);ŒÝŠ·
void api_putstrwin(uint win, int x, int y, color_8 col, int len, uchar *str);
//	hariboteOS:ŒÝŠ·void api_putstrwin(int win, int x, int y, int col, int len, char *str);
void api_boxfilwin(uint win, int x0, int y0, int x1, int y1, color_8 col);
//	hariboteOS:ŒÝŠ·void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);
void api_initmalloc(void);
//	hariboteOS:ŒÝŠ·void api_initmalloc(void);
void *api_malloc(uint size);
//	hariboteOS:ŒÝŠ·char *api_malloc(int size);
void api_free(void *addr, uint size);
//	hariboteOS:ŒÝŠ·void api_free(char *addr, int size);
void api_point(uint win, int x, int y, color_8 col);
//	hariboteOS:ŒÝŠ·void api_point(int win, int x, int y, int col);
void api_refreshwin(uint win, int x0, int y0, int x1, int y1);
//	hariboteOS:ŒÝŠ·void api_refreshwin(int win, int x0, int y0, int x1, int y1);
void api_linewin(uint win, int x0, int y0, int x1, int y1, color_8 col);
//void api_linewin(int win, int x0, int y0, int x1, int y1, int col);
//void api_closewin(int win);
//int api_getkey(int mode);
//int api_alloctimer(void);
//void api_inittimer(int timer, int data);
//void api_settimer(int timer, int time);
//void api_freetimer(int timer);
//void api_beep(int tone);
//int api_fopen(char *fname);
//void api_fclose(int fhandle);
//void api_fseek(int fhandle, int offset, int mode);
//int api_fsize(int fhandle, int mode);
//int api_fread(char *buf, int maxsize, int fhandle);
//int api_cmdline(char *buf, int maxsize);
//int api_getlang(void);
