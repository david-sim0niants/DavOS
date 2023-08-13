#!/usr/bin/python3


print('Let the config begin...')

import argparse as ap
import sys
from pathlib import Path
import cmd


if __name__ != '__main__':
    print('Error: this script should be run as __main__. Stopping...', file=sys.stderr)
    exit(code=1)


del sys.path[0]


arg_parser = ap.ArgumentParser()
arg_parser.add_argument('source_directory', type=str)

args = arg_parser.parse_args()

sys.path.append(args.source_directory)


from tools.config.config import Config, ConfigErr
from tools.config.parser import parse_config_line, config_name_valid, remove_config_prefix
from tools.config.cmake import convert_config_to_cmake


def read_config_lists_at(dir, scope):
    path = Path(dir).joinpath('ConfigLists.py')
    with open(path) as f:
        exec(f.read(), scope)


SCOPE_KEYS = ['CONFIGS', 'SOURCE_CONFIGS']


def read_config_lists(source_dir):
    config_lists = {}
    curr_paths = ['']

    while len(curr_paths) != 0:
        fut_paths = []

        for path in curr_paths:
            scope = {}
            read_config_lists_at(Path(source_dir).joinpath(path), scope=scope)

            scope = {key: scope[key] for key in SCOPE_KEYS if key in scope}
            config_lists[path] = scope

            fut_paths.extend([Path(path).joinpath(subdir)\
                    for subdir in scope.get('SOURCE_CONFIGS', [])])

        curr_paths = fut_paths

    return config_lists


def combine_config_lists(config_lists:dict):
    configs = {}
    for path, scope in config_lists.items():
        for name, config_item in scope.get('CONFIGS', {}).items():
            if name in configs:
                raise ConfigErr(f'Found a config with name same as another config name found before. Config {name} at {path}.')
            configs[name] = config_item
    return configs


class ConfigShell(cmd.Cmd):
    intro = 'Interactive config manager shell'
    prompt = '(config) '
    saved = False

    def __init__(self, config:Config, completekey: str="tab", stdin=None, stdout=None):
        super().__init__(completekey, stdin, stdout)
        self.config = config

    def do_get(self, args):
        '''
            get <config>
            Get current value of the <config>.
        '''
        config_name_original = args.strip().split()[0]
        config_name = remove_config_prefix(config_name_original)

        if not config_name_valid(config_name):
            print(f'Error: invalid config name {config_name_original}', file=sys.stderr)
            return
        if config_name not in self.config.lists:
            print(f'Error: unknown config - {config_name_original}', file=sys.stderr)
            return
        if config_name not in self.config.config:
            print(f'Config f{config_name_original} is not defined yet.')
            return
        print(f'{config_name}={self.config.config[config_name]}')

    def do_set(self, args):
        '''
            set <config>=<value>
            <config>=<value>
            Set <config>'s <value>.
        '''
        self.parse_config_line_and_define(args)

    def do_list(self, _):
        '''
            list (same as 'ls')
            List all available configs.
        '''
        for name, config_item in self.config.lists.items():
            print(f"{name} - {config_item['description']}" + \
                    ('' if 'type' not in config_item else f" - {config_item['type'].__name__}"))

    def do_ls(self, _):
        '''
            ls (same as 'list')
            List all available configs.
        '''
        self.do_list(_)

    def do_save(self, _):
        '''
            save
            Save all currently defined configs to config.cmake file.
        '''
        with open('config.cmake', 'w') as f:
            convert_config_to_cmake(self.config, f)
            self.saved = True

    def do_exit(self, _):
        '''
            exit
            Exit from this shell.
        '''
        while not self.saved:
            ans = input('Exit without saving? [y/N] ')
            if 'NO'.startswith(ans.strip().upper()):
                return
            elif 'YES'.startswith(ans.strip().upper()):
                return True
        return True

    def do_eof(self, _):
        return self.do_exit(_)

    def precmd(self, line):
        ret = parse_config_line(line)
        if ret is not None:
            return 'set ' + line
        splits = line.split(maxsplit=1)
        return f'{splits[0].lower()} {splits[1] if len(splits) == 2 else ""}'


    def define_config(self, name:str, value):
            passed, reason = self.config.define_config(name, value)
            if passed:
                if reason != 'same-value':
                    self.saved = False
                return
            print(f'Failed defining config {name} with value {value}: Reason: {reason}')


    def parse_config_line_and_define(self, config_line):
        ret = parse_config_line(config_line)
        if ret is None:
            print('Error: invalid input.', file=sys.stderr)
            return
        name, value = ret
        config_item = self.config.lists.get(name, None)
        if config_item is None:
            print(f'Error: no such config - {config_item}.')
            return

        # TODO: value checks

        self.define_config(name, value)


print("Reading ConfigLists...")
config_lists = read_config_lists(args.source_directory)
configs = combine_config_lists(config_lists)

config = Config(configs)
ConfigShell(config).cmdloop()

