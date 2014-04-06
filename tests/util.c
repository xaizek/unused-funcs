extern void firstExtern(void);
void secondExtern(void);

void
thirdExtern(void)
{
    firstExtern();
    secondExtern();
}
