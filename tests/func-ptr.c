void
func(void)
{
}

int
main(void)
{
    void (*f)(void) = func;
    void (*f2)(void) = &func;
    f();
    return 0;
}
