B, Kb, Mb, Gb, Tb, Pb, Eb, Zb, Yb = 1, 1 << 10, 1 << 20, 1 << 30, 1 << 40, 1 << 50, 1 << 60, 1 << 70, 1 << 80

def str_mem_size(nr_bytes):
    if nr_bytes < Kb:
        return f'{nr_bytes}B'
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
