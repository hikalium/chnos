void api_putstr(unsigned char *s);
void api_end(void);

void CHNMain(void)
{
	api_putstr("Hello World\n");
	api_end();
}
