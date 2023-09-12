import platform
from tools.config.utils import MemLoc, MemSize


def _ARCH_on_value_change(config:dict):
    arch = config['ARCH']
    if arch in ('i386', 'ia32', 'x86_32'):
        config['ARCH'] = 'i386'
    elif arch in ('x86_64', 'ia32e', 'amd64'):
        config['ARCH'] = 'x86_64'


def _ARCH_BITNESS_default_value(config:dict):
    arch = config['ARCH']
    if arch == 'x86_64':
        return 64
    elif arch == 'i386':
        return 32
    else:
        return None


def _ARCH_BITNESS_check_value(bitness:int, config:dict):
    arch = config['ARCH']
    if arch == 'i386' and bitness == 32:
        return True, None
    elif arch == 'x86_64' and bitness == 64:
        return True, None
    else:
        return False, f'Wrong bitness ({bitness}) selected for architecture {arch}'


_4Kb, _2Mb, _4Mb, _1Gb = MemSize('4Kb'), MemSize('2Mb'), MemSize('4Mb'), MemSize('1Gb') # 0x1000, 0x200000, 0x400000, 0x40000000
i386_PAGE_SIZES = (_4Kb, _4Mb)
x86_64_PAGE_SIZES = (_4Kb, _2Mb, _1Gb)


def _PAGE_SIZE_check_value(page_size:int, config:dict):
    """
    Check PAGE_SIZE value based on its dependencies.
    """
    arch = config['ARCH']
    if arch == 'i386':
        page_sizes = i386_PAGE_SIZES
    elif arch == 'x86_64':
        page_sizes = x86_64_PAGE_SIZES
    else:
        return False, 'Unknown ARCH is selected.'

    passed = page_size in page_sizes
    return passed, None if passed else \
        f'Wrong page size for {arch}, allowed page sizes are {"".join([str(page_size) + ", " for page_size in page_sizes])}'


def _VM_SPLIT_default_value(config:dict):
    """
    Get default value of VM_SPLIT based on the current config.
    """
    arch = config['ARCH']
    if arch == 'i386':
        return MemLoc('3Gb')
    elif arch == 'x86_64':
        return MemLoc('128Tb')
    else:
        return None


def _VM_SPLIT_check_page_size_alignment(vm_split:MemLoc, config:dict):
    """
    Check if VM_SPLIT is divisible by the page size
    """
    page_size = config['PAGE_SIZE']

    if vm_split % page_size != 0:
        return False, 'VM_SPLIT must be divisible by PAGE_SIZE'
    else:
        return True, None


def _VM_SPLIT_check_value_bounds_and_ps_alignment(vm_split:MemLoc, config: dict,
                                                min_vm_split:MemLoc, max_vm_split:MemLoc):
    """
    Check if VM_SPLIT is in the acceptable bounds.
    """
    if min_vm_split > vm_split:
        return False, 'VM_SPLIT too low.'
    elif max_vm_split < vm_split:
        return False, 'VM_SPLIT too high.'
    else:
        return _VM_SPLIT_check_page_size_alignment(vm_split, config)


def _VM_SPLIT_check_value(value:MemLoc, config: dict):
    """
    Check VM_SPLIT value based on its dependencies from config.
    """
    arch = config['ARCH']
    if arch in ('i386', 'ia32', 'x86_32'):
        # for i386 the VM_SPLIT is better be in [1Gb, 3Gb] zone
        return _VM_SPLIT_check_value_bounds_and_ps_alignment(
                value, config, MemLoc('1Gb'), MemLoc('3Gb'))
    elif arch in ('x86_64', 'ia32e', 'amd64'):
        # for x86_64 the VM_SPLIT is better be in [64Tb, 192Tb] zone
        return _VM_SPLIT_check_value_bounds_and_ps_alignment(
                value, config, MemLoc('64Tb'), MemLoc('192Tb'))
    else:
        return False, 'Unknown ARCH is selected.'


def _STACK_SIZE_default_value(config: dict):
    return config['PAGE_SIZE'] * 2


def _STACK_SIZE_check_value(stack_size:MemSize, config: dict):
    page_size = config['PAGE_SIZE']
    if stack_size % page_size != 0:
        return False, 'STACK_SIZE must be divisible by PAGE_SIZE.'
    else:
        return True, None


CONFIGS = {
    'ARCH': {
        'description': 'The target architecture the kernel will compile to.',
        'type': str,
        'value_set': {('i386', 'ia32', 'x86_32'), ('x86_64', 'ia32e', 'amd64')},
        'default_value': platform.uname().machine,
        'on_value_change': _ARCH_on_value_change,
    },
    'ARCH_BITNESS': {
        'description': 'Target architecture bitness.',
        'type': int,
        'depends': ['ARCH'],
        'default_value': _ARCH_BITNESS_default_value,
        'value_set': [32, 64],
        'value_checker': _ARCH_BITNESS_check_value,
    },
    'HAVE_TESTS': {
        'description': 'Enabling this will configure and build the tests.',
        'type': bool,
        'default_value': False,
    },
    'PAGE_SIZE': {
        'description': 'Page size in bytes.',
        'type': MemSize,
        'depends': ['ARCH'],
        'default_value': _4Kb,
        'value_checker': _PAGE_SIZE_check_value,
    },
    'VM_SPLIT': {
        'description': 'The position of the virtual memory split between user space and kernel space.',
        'type': MemLoc,
        'depends': ['ARCH', 'PAGE_SIZE'],
        'default_value': _VM_SPLIT_default_value,
        'value_checker': _VM_SPLIT_check_value,
    },
    'STACK_SIZE': {
        'description': 'Kernel stack size.',
        'type': MemSize,
        'depends': ['PAGE_SIZE'],
        'default_value': _STACK_SIZE_default_value,
        'value_checker': _STACK_SIZE_check_value,
    },
    'MULTIBOOT2': {
        'description': 'Enabling this makes kernel multiboot2 specification comliant.',
        'type': bool,
        'default_value': True,
    },
}

SOURCE_CONFIGS = ['arch']
