units = {
    'B': 1,
    'Kb': 1 << 10,
    'Mb': 1 << 20,
    'Gb': 1 << 30,
    'Tb': 1 << 40,
    'Pb': 1 << 50,
    'Eb': 1 << 60,
    'Zb': 1 << 70,
    'Yb': 1 << 80,
}

def str_mem_size(nr_bytes):
    B, Kb, Mb, Gb, Tb, Pb, Eb, Zb, Yb = 1, 1 << 10, 1 << 20, 1 << 30, 1 << 40, 1 << 50, 1 << 60, 1 << 70, 1 << 80
    if nr_bytes < Kb:
        return f'{nr_bytes // B}B'
    elif nr_bytes < Mb:
        return f'{nr_bytes // Kb}Kb'
    elif nr_bytes < Gb:
        return f'{nr_bytes // Mb}Mb'
    elif nr_bytes < Tb:
        return f'{nr_bytes // Gb}Gb'
    elif nr_bytes < Pb:
        return f'{nr_bytes // Tb}Tb'
    elif nr_bytes < Eb:
        return f'{nr_bytes // Pb}Pb'
    elif nr_bytes < Zb:
        return f'{nr_bytes // Eb}Eb'
    elif nr_bytes < Yb:
        return f'{nr_bytes // Zb}Zb'
    else:
        return f'{nr_bytes // Yb}Yb'


def _try_to_int(value:str):
    possible_base = 10
    if value.startswith('0x'):
        possible_base = 16
    elif value.startswith('0b'):
        possible_base = 2
    elif value.startswith('0'):
        possible_base = 8
    else:
        return None
    try:
        return int(value, base=possible_base)
    except:
        return None


def _try_to_mem_size(value:str):
    possible_unit = None
    for unit in units.keys():
        if value.endswith(unit):
            possible_unit = unit
            break
    if possible_unit is None:
        return None
    return _try_to_int(value[-1 - len(possible_unit)::-1].strip())


def check_value_type(value:str):
    value = value.strip().lower()
    if value in ('true', 'false'):
        return value != 'false', bool

    if value in ('none', 'null'):
        return None, type(None)

    mem_size = _try_to_mem_size(value)
    if mem_size is not None:
        return mem_size, int

    int_val = _try_to_int(value)
    if int_val is not None:
        return int_val, int

