extern "C" __attribute__((section(".text")))
void main(void)
{
	asm volatile ("hlt");
}
