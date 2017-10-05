class sockeng
{
	private:

	long *hosts;

	int num;
	int numpasses;
	int left;
	unsigned long timeout;
	long start;
	int delay;

	public:
	sockeng(unsigned long to, unsigned long s, unsigned long e,
			int numpasses, int dl);
	~sockeng(void);

	int ping(HWND pParam);

	static long getaddr(char *string);
	static USHORT checksum(USHORT *buffer, int size);
};
