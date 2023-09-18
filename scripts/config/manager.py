from pathlib import Path

from scripts.config.config import Config
from scripts.config.utils import try_parse_value
from scripts.config.format import parse_config_line
from scripts.config.cmake import convert_config_to_cmake


class ConfigManagerErr(Exception):
    def __init__(self, *args: object) -> None:
        super().__init__(*args)


def _read_config_lists_at(dir, scope):
    ''' Read config lists at specified 'dir' directory. Fill the contents in 'scope'.'''
    path = Path(dir).joinpath('ConfigLists.py')
    with open(path) as f:
        exec(f.read(), scope)

# variables to search for in ConfigLists.py
SCOPE_KEYS = ['CONFIGS', 'SOURCE_CONFIGS']


def _read_config_lists(source_dir):
    ''' Read all config lists starting from 'source_dir' directory. '''
    config_lists = {}
    curr_paths = ['']

    while len(curr_paths) != 0:
        fut_paths = []

        for path in curr_paths:
            scope = {}
            _read_config_lists_at(Path(source_dir).joinpath(path), scope=scope)

            scope = {key: scope[key] for key in SCOPE_KEYS if key in scope}
            config_lists[path] = scope

            fut_paths.extend([Path(path).joinpath(subdir)\
                    for subdir in scope.get('SOURCE_CONFIGS', [])])

        curr_paths = fut_paths

    return config_lists


def _combine_config_lists(config_lists:dict):
    ''' Combine 'CONFIGS' dict from all 'CONFIGS' variables collected from all ConfigLists.py files. '''
    configs = {}
    for path, scope in config_lists.items():
        for name, config_item in scope.get('CONFIGS', {}).items():
            if name in configs:
                raise ConfigManagerErr(f'Found a config with name same as another config name found before. Config {name} at {path}.')
            configs[name] = config_item
    return configs


def _strip_comment(line:str):
    ''' Strip comments from line. Comments start with '# ' (note the space after '#'). '''
    comment_start = line.find(' #')
    if comment_start == -1:
        return line
    else:
        return line[:comment_start]


class ConfigManager:
    '''
        Main class used by the interactive config shell to manage configuration, store/load/generate it.
    '''
    def __init__(self):
        pass

    def read_config_lists(self, source_dir):
        ''' Read config lists starting from 'source_dir'. '''
        config_lists = _combine_config_lists(_read_config_lists(source_dir))
        self.config = Config(config_lists)

    def define_config_str(self, name:str, str_value:str):
        '''
            Define config 'name', assuming 'name' and 'str_value' have been successfully parsed from config line.
            'str_value' will be turned into an actual value with known type.
        '''
        config_item = self.config.lists.get(name, None)
        if config_item is None:
            return False, f'Error: no such config - {config_item}.'

        val_type = config_item.get('type')
        if val_type is not str:
            value, input_val_type = try_parse_value(str_value)
            if val_type is not None and input_val_type is not val_type:
                return False, f'Error: type mismatch between specified type \'{val_type.__name__}\' and type of the input value \'{input_val_type.__name__}\'.'
        else:
            value = str_value

        return self.config.define_config(name, value)

    def get_config(self):
        return self.config.config

    def get_lists(self):
        return self.config.lists

    def load_config_file(self, file):
        ''' Load configuration file. '''
        for line_num, line in enumerate(file):
            if line.endswith('\n'):
                line = line[:-1]
            if line.endswith('\r'):
                line = line[:-1]

            orig_line = line
            line = _strip_comment(line.strip())
            if line.strip() == '':
                continue
            ret = parse_config_line(line)
            if ret is None:
                print(f'Warning: couldn\'t parse this line {line_num}: {orig_line}')
                continue

            name, value = ret
            passed, reason = self.define_config_str(name, value)
            if passed:
                continue
            print(f'Error: failed to define a config {name} with value {value}: Reason: {reason}')


    def save_config_file(self, file):
        ''' Save configuration file. '''
        for name, value in self.config.config.items():
            file.write(f'{name}={value}\n')

    def gen_cmake_config_file(self, file):
        ''' Generate cmake config. '''
        convert_config_to_cmake(self.config, file)
