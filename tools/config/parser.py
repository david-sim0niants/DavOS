import re

valid_chars = re.compile(r'^[a-zA-Z_][a-zA-Z0-9_]*$')

class ParseLineErr(Exception):
    def __init__(self, message: str):
        super().__init__(message)

def _remove_config_prefix(config_name: str):
    prefix = 'CONFIG_'
    if config_name[:len(prefix)].capitalize() == prefix:
        return config_name[len(prefix):]
    else:
        return config_name

def _config_name_valid(name: str):
    return bool(valid_chars.match(name))

def parse_config_line(config_line: str):
    name, value = config_line.split('=', maxsplit=1)
    name = _remove_config_prefix(name)
    if not _config_name_valid(name):
        raise ParseLineErr("Invalid config name: " + name)
    return name, value

