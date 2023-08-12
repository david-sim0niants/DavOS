import re

valid_chars = re.compile(r'^[a-zA-Z_][a-zA-Z0-9_]*$')

def remove_config_prefix(config_name: str):
    prefix = 'CONFIG_'
    if config_name[:len(prefix)].capitalize() == prefix:
        return config_name[len(prefix):]
    else:
        return config_name

def config_name_valid(name: str):
    return bool(valid_chars.match(name))


def parse_config_line(config_line:str):
    splits = config_line.split('=', maxsplit=1)
    if len(splits) < 2:
        return
    name, value = splits
    name = remove_config_prefix(name)
    if not config_name_valid(name):
        return
    return name, value
