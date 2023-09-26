#ifndef _x86__SYSTEM_H__
#define _x86__SYSTEM_H__

namespace x86 {

inline void halt()
{
	asm volatile ("hlt");
}

}

#endif
