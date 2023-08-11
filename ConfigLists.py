import platform
from tools.config.lib.utils import str_mem_size

_4Kb, _2Mb, _4Mb, _1Gb = 0x1000, 0x200000, 0x400000, 0x40000000
i386_PAGE_SIZES = (_4Kb, _4Mb)
x86_64_PAGE_SIZES = (_4Kb, _2Mb, _1Gb)


def _PAGE_SIZE_check_value(value: int, config:dict):
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
    
    return value in page_sizes, \
        f'Wrong page size for {arch}, allowed page sizes are {"".join([str_mem_size(page_size) + ", " for page_size in page_sizes])}'


def _VM_SPLIT_default_value(config: dict):
    """
    Get default value of VM_SPLIT based on the current config.
    """
    arch = config['ARCH']
    if arch in ('i386', 'ia32', 'x86_32'):
        return 0xC0000000
    elif arch in ('x86_64', 'ia32e', 'amd64'):
        return 0x8000000000000000
    else:
        return None


def _VM_SPLIT_check_page_size_alignment(value: int, config: dict):
    """
    Check if VM_SPLIT is divisible by the page size
    """
    page_size = config['PAGE_SIZE']

    if value % page_size != 0:
        return False, 'VM_SPLIT must be divisible by PAGE_SIZE'
    else:
        return True, None


def _VM_SPLIT_check_value_bounds_and_ps_alignment(value: int, config: dict,
                                                min_val: int, max_val: int):
    """
    Check if VM_SPLIT is in the acceptable bounds.
    """
    if min_val > value:
        return False, 'VM_SPLIT too low.'
    elif max_val < value:
        return False, 'VM_SPLIT too high.'
    else:
        return _VM_SPLIT_check_page_size_alignment(value, config)


def _VM_SPLIT_check_value(value: int, config: dict):
    """
    Check VM_SPLIT value based on its dependencies from config.
    """
    arch = config['ARCH']
    if arch in ('i386', 'ia32', 'x86_32'):
        # for i386 the VM_SPLIT is better be in [1Gb, 3Gb] zone
        return _VM_SPLIT_check_value_bounds_and_ps_alignment(
                value, config, 0x40000000, 0xC0000000)
    elif arch in ('x86_64', 'ia32e', 'amd64'):
        # for x86_64 the VM_SPLIT is better be in [64Tb, 192Tb] zone
        return _VM_SPLIT_check_value_bounds_and_ps_alignment(
                value, config, 0x4000000000000000, 0xC000000000000000)
    else:
        return False, 'Unknown ARCH is selected.'


def _STACK_SIZE_default_value(config: dict):
    return config['PAGE_SIZE'] * 2


def _STACK_SIZE_check_value(value: int, config: dict):
    page_size = config['PAGE_SIZE']
    if value % page_size != 0:
        return False, 'STACK_SIZE must be divisible by PAGE_SIZE.'
    else:
        return True, None


CONFIGS = {
    'ARCH': {
        'description': 'The target architecture the kernel will compile to.',
        'type': str,
        'value_set': {('i386', 'ia32', 'x86_32'), ('x86_64', 'ia32e', 'amd64')},
        'default_value': platform.uname().machine,
    },
    'HAVE_TESTS': {
        'description': 'Enabling this will configure and build the tests.',
        'type': bool,
        'default_value': False,
    },
    'PAGE_SIZE': {
        'description': 'Page size in bytes.',
        'type': int,
        'depends': ['ARCH'],
        'default_value': _4Kb,
        'value_checker': _PAGE_SIZE_check_value,
    },
    'VM_SPLIT': {
        'description': 'The position of the virtual memory split between user space and kernel space.',
        'type': int,
        'depends': ['ARCH', 'PAGE_SIZE'],
        'default_value': _VM_SPLIT_default_value,
        'value_checker': _VM_SPLIT_check_value,
    },
    'STACK_SIZE': {
        'description': 'Kernel stack size.',
        'type': int,
        'depends': ['PAGE_SIZE'],
        'default_value': _STACK_SIZE_default_value,
        'value_checker': _STACK_SIZE_check_value,
    },
}

SOURCE_CONFIGS = ['arch']
