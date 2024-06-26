#!/usr/bin/python3


import argparse as ap
import sys
import cmd


def p_err(*args, **kwargs):
    ''' Print error function. '''
    print(*args, file=sys.stderr, **kwargs)

if __name__ != '__main__':
    p_err('Error: this script should be run as __main__. Stopping...')
    exit(code=1)


del sys.path[0] # we gotta work at the specified source directory


arg_parser = ap.ArgumentParser()
arg_parser.add_argument('source_directory', type=str)

args = arg_parser.parse_args()

# working at the specified source directory, where the most root level ConfigLists.py resides
# but of course if the specified source directory is correct
sys.path.append(args.source_directory)

CONFIG_FILE = '.config' # the file to save to/load from "CONFIG_NAME=CONFIG_VALUE"-like configuration
CMAKE_CONFIG_FILE = 'config.cmake' # cmake file to store "set(CONFIG_NAME CONFIG_VALUE)"-like configuration

# import main beasts
from scripts.config.format import parse_config_line, config_name_valid, remove_config_prefix
from scripts.config.manager import ConfigManager


class ConfigShell(cmd.Cmd):
    intro = 'Interactive config manager shell'
    prompt = '(config) '
    saved = True

    def __init__(self, manager:ConfigManager, completekey: str="tab", stdin=None, stdout=None):
        super().__init__(completekey, stdin, stdout)
        self.manager = manager

    def do_get(self, args):
        '''
            get <config>
            Get current value of the <config>.
        '''
        if args.strip() == '':
            p_err('Error: \'get\' requires a <config> argument.')
            return

        config_name_original = args.strip().split()[0]
        config_name = self.__check_config_existence(config_name_original)
        if config_name is None:
            return
        if config_name not in self.manager.config.config:
            p_err(f'Config f{config_name_original} is not defined yet.')
            return
        print(f'\t{config_name}={self.manager.get_config()[config_name]}')

    def do_set(self, args):
        '''
            set <config>=<value>
            <config>=<value>
            Set <config>'s <value>.
        '''
        self.__read_config_line(args)

    def do_list(self, _):
        '''
            list (same as 'ls')
            List all available configs.
        '''
        for name, config_item in self.manager.get_lists().items():
            print(f"\t{name} - {config_item['description']}" + \
                    ('' if 'type' not in config_item else f" - {config_item['type'].__name__}"))

    def do_ls(self, _):
        '''
            ls (same as 'list')
            List all available configs.
        '''
        self.do_list(_)

    def do_info(self, args):
        if args.strip() == '':
            p_err('Error: \'info\' requires a <config> argument.')
            return

        config_name_original = args.strip().split()[0]
        config_name = self.__check_config_existence(config_name_original)
        if config_name is None:
            return

        print('\tCONFIG_' + config_name)
        config_item = self.manager.get_lists().get(config_name)
        if config_item is None:
            return

        if 'type' in config_item:
            print(f"\tType: {config_item['type'].__name__}")

        if 'depends' in config_item:
            print(f"\tDepends on: {''.join(f'{config_name} ' for config_name in config_item['depends'])}")

        if 'value_set' in config_item:
            print(f"\tAccepted values: {config_item['value_set']}")

        if 'default_value' in config_item:
            default_value = config_item['default_value']
            if not callable(default_value):
                print(f"\tDefault value: {default_value}")

        if config_name in self.manager.get_config():
            print(f'\tCurrent value: {self.manager.get_config()[config_name]}')

        if 'description' in config_item:
            print('\t' + config_item['description'])

    def do_save(self, _):
        '''
            save
            Save all currently defined configs to config.cmake file.
        '''
        with open(CONFIG_FILE, 'w') as f:
            self.manager.save_config_file(f)
            self.saved = True

    def do_cmake(self, _):
        with open(CMAKE_CONFIG_FILE, 'w') as f:
            self.manager.gen_cmake_config_file(f)
        print(f'Generated {CMAKE_CONFIG_FILE} file.')

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
        if line.strip() == '':
            return ''
        ret = parse_config_line(line)
        if ret is not None:
            return 'set ' + line # commands like CONFIG_NAME=CONFIG_VALUE are interpreted as 'set CONFIG_NAME=CONFIG_VALUE'
        splits = line.split(maxsplit=1)
        return f'{splits[0].lower()} {splits[1] if len(splits) == 2 else ""}'


    def __read_config_line(self, config_line):
        ret = parse_config_line(config_line)
        if ret is None:
            p_err('Error: \'set\' requires a <config>=<value> argument.')
            return
        name, value = ret
        passed, reason = self.manager.define_config_str(name, value)
        if passed:
            if reason != 'same-value':
                self.saved = False
            return
        p_err(f'Error: failed defining config {name} with value {value}: Reason: {reason}')


    def __check_config_existence(self, config_name):
        config_name_original = config_name
        config_name = remove_config_prefix(config_name_original)

        if not config_name_valid(config_name):
            p_err(f'Error: invalid config name {config_name_original}')
            return None
        if config_name not in self.manager.get_lists():
            p_err(f'Error: unknown config - {config_name_original}')
            return None
        return config_name


config_manager = ConfigManager()
config_manager.read_config_lists(args.source_directory)

try:
    with open(CONFIG_FILE, 'r') as f:
        config_manager.load_config_file(f)
except FileNotFoundError:
    print('Fresh configuration setup...')

ConfigShell(config_manager).cmdloop()

