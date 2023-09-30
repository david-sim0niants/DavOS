#include <stdlib.h>
#include <ctype.h>


template<typename T>
static int ato_T(const char *str)
{
	while (isspace(*str))
		++str;

	bool negative = false;

	if (*str == '+')
		++str;
	else if (*str == '-')
	{
		++str;
		negative = true;
	}

	T result = 0;
	for (; isdigit(*str); ++str)
	{
		int digit = *str - '0';
		result *= 10;
		result -= digit;
	}

	return negative ? result : -result;
}

extern "C" int atoi(const char *str)
{
	return ato_T<int>(str);
}

extern "C" long atol(const char *str)
{
	return ato_T<long>(str);
}

extern "C" long long atoll(const char *str)
{
	return ato_T<long long>(str);
}
