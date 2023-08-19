def _x86_PAGE_MAP_LEVEL_default_value(config:dict):
    arch = config['ARCH']
    if arch == 'x86_64':
        return 'PAGE_MAP_LEVEL_4'
    elif arch == 'i386':
        return 'PAGE_MAP_LEVEL_2'


def _x86_PAGEMAP_LEVEL_check_value(value:str, config:dict):
    arch = config['ARCH']
    if arch == 'x86_64':
        if value in ['PAGE_MAP_LEVEL_4', 'PAGE_MAP_LEVEL_5']:
            return True, None
    elif arch == 'i386':
        if value in ['PAGE_MAP_LEVEL_2', 'PAGE_MAP_LEVEL_3_PAE']:
            return True, None
    return False, f'Wrong page map level ({value}) selected for {arch} architecture.'


def _x86_PHYS_ADDR_64_BIT_value_checker(value:bool, config:dict):
    if config['ARCH'] == 'x86_64' and value is False:
        return False, 'Config x86_PHYS_ADDR_64BIT is always true for 64bit x86 architecture.'
    return True, None


CONFIGS = {
    'x86_PAGE_MAP_LEVEL': {
        'description': 'Defines page map level in x86.',
        'type': str,
        'value_set': {
            'PAGE_MAP_LEVEL_2',
            'PAGE_MAP_LEVEL_3_PAE',
            'PAGE_MAP_LEVEL_4',
            'PAGE_MAP_LEVEL_5'
        },
        'value_checker': _x86_PAGEMAP_LEVEL_check_value,
        'default_value': _x86_PAGE_MAP_LEVEL_default_value,
        'depends': ['ARCH']
    },
    'x86_PHYS_ADDR_64BIT': {
        'description': 'Enabling this assumes MAXPHYSADDR > 32.',
        'type': bool,
        'default_value': True,
        'depends': ['ARCH'],
        'value_checker': _x86_PHYS_ADDR_64_BIT_value_checker,
    },
}
