static void firstStatic(void);

static void secondStatic(void);

void firstExtern(void);

extern void secondExtern(void);

static void
firstStatic(void)
{
}

static void
secondStatic(void)
{
}

void
firstExtern(void)
{
}

void
secondExtern(void)
{
}

int
main(void)
{
    firstExtern();
    secondStatic();
    return 0;
}
