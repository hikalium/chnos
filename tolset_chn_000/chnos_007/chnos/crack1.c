void api_end(void);

void CHNMain(void)
{
	*((char *)0x00102600) = 0;
	api_end();
}
