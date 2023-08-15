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

    'B': 1,
    'KB': 1 << 10,
    'MB': 1 << 20,
    'GB': 1 << 30,
    'TB': 1 << 40,
    'PB': 1 << 50,
    'EB': 1 << 60,
    'ZB': 1 << 70,
    'YB': 1 << 80,
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


def _try_parse_int(value:str):
    if value.startswith('0x'):
        possible_base = 16
    elif value.startswith('0b'):
        possible_base = 2
    elif value.startswith('0'):
        possible_base = 8
    else:
        possible_base = 10
    try:
        return int(value, base=possible_base)
    except:
        return None


def try_parse_value(value:str):
    value = value.strip()
    lowered_value = value.lower()
    if lowered_value in ('true', 'false'):
        return lowered_value != 'false', bool

    if lowered_value in ('none', 'null'):
        return None, type(None)

    try:
        mem_size = MemSize(value)
        return mem_size, MemSize
    except MemSizeErr:
        pass

    int_val = _try_parse_int(value)
    if int_val is not None:
        return int_val, int

    try:
        res = eval(value)
        return res, type(res)
    except:
        return value, str


class MemSizeErr(Exception):
    def __init__(self, *args):
        super().__init__(*args)


class MemSize:
    value = 0
    unit = 'B'

    def __init__(self, *args, **kwargs):
        if len(args) == 0:
            self.__get_unit_from_kwargs(**kwargs)
            return

        if isinstance(args[0], int):
            self.value = args[0]
        elif isinstance(args[0], str):
            str_mem_size = args[0].strip()
            ret = MemSize.__try_parse_mem_size(str_mem_size)
            if ret is None:
                raise MemSizeErr(f'Failed to parse string {args[0]} to a memory size.')
            self.value, self.unit = ret
            return

        if len(args) > 1 and isinstance(args[1], str) and args[1] in units:
            self.unit = args[1]
            return
        else:
            self.__get_unit_from_kwargs(**kwargs)


    @staticmethod
    def __try_parse_mem_size(str_mem_size:str):
        possible_unit = None
        for unit in units.keys():
            if str_mem_size.endswith(unit):
                possible_unit = unit
                break
        if possible_unit is None:
            return None
        value = _try_parse_int(str_mem_size[:-len(possible_unit)].strip())
        if value is None:
            return None
        return value, possible_unit


    def __get_unit_from_kwargs(self, **kwargs):
        self.unit = kwargs.get('unit', 'B')
        MemSize.__check_unit(self.unit)


    @staticmethod
    def __check_unit(unit:str):
        if unit not in units:
            raise MemSizeErr(f'Unknown memory size unit - {unit}')


    @staticmethod
    def highest_unit_for_value(value:int):
        units_order = ['B', 'Kb', 'Mb', 'Gb', 'Tb', 'Pb', 'Eb', 'Zb', 'Yb']
        prev_unit = 'B'
        for unit in units_order:
            if value < units[unit]:
                return prev_unit
            prev_unit = unit
        return prev_unit


    def to_unit(self, unit:str):
        MemSize.__check_unit(unit)
        return MemSize(self.value * units[self.unit] // units[unit], unit)

    def bytes(self):
        return self.value * units[self.unit];

    def __str__(self):
        return f'{self.value}{self.unit}'


    def __common_unit(self, other):
        min_unit_index, min_unit_value = min(enumerate([units[self.unit], units[other.unit]]), key=lambda x: x[1])
        min_unit = self.unit if min_unit_index == 0 else other.unit
        return min_unit, min_unit_value


    @staticmethod
    def __check_other_mem_size(other, op):
        if not isinstance(other, MemSize):
            raise MemSizeErr(f'Cannot {op} with an object of type \'{type(other).__name__}\'')


    def __add__(self, other):
        MemSize.__check_other_mem_size(other, 'add')
        common_unit, common_unit_value = self.__common_unit(other)
        total = (self.bytes() + other.bytes())
        return MemSize(total // common_unit_value, common_unit)

    def __sub__(self, other):
        MemSize.__check_other_mem_size(other, 'subtract')
        common_unit, common_unit_value = self.__common_unit(other)
        total = (self.bytes() - other.bytes())
        return MemSize(total // common_unit_value, common_unit)

    def __mul__(self, other):
        if isinstance(other, int):
            return MemSize(self.value * other, self.unit)
        MemSize.__check_other_mem_size(other, 'multiply')
        total = self.bytes() * other.bytes()
        return MemSize(total, MemSize.highest_unit_for_value(total))

    def __floordiv__(self, other):
        if isinstance(other, int):
            return MemSize(self.bytes() // other)
        MemSize.__check_other_mem_size(other, 'divide')
        return self.bytes() // other.bytes()

    def __mod__(self, other):
        if isinstance(other, int):
            return self.bytes() % other
        MemSize.__check_other_mem_size(other, 'modulo')
        return self.bytes() % other.bytes()

    def __eq__(self, other):
        MemSize.__check_other_mem_size(other, 'compare')
        return self.bytes() == other.bytes()

    def __ne__(self, other):
        MemSize.__check_other_mem_size(other, 'compare')
        return self.bytes() != other.bytes()

    def __gt__(self, other):
        MemSize.__check_other_mem_size(other, 'compare')
        return self.bytes() >  other.bytes()

    def __ge__(self, other):
        MemSize.__check_other_mem_size(other, 'compare')
        return self.bytes() >= other.bytes()

    def __lt__(self, other):
        MemSize.__check_other_mem_size(other, 'compare')
        return self.bytes() <  other.bytes()

    def __le__(self, other):
        MemSize.__check_other_mem_size(other, 'compare')
        return self.bytes() <= other.bytes()


MemLoc = MemSize
