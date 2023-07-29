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
    },
    'x86_PHYS_ADDR_64BIT': {
        'description': 'Enabling this assumes MAXPHYSADDR > 32.',
        'type': bool,
    },
}

DEPENDS = ['ARCH']
IGNORE_COND = lambda config: config['ARCH'] not in ('x86_64', 'i386')
