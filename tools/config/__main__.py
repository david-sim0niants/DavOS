#!/usr/bin/python3


print('Let the config begin...')

import argparse as ap
import sys, os
from pathlib import Path

from config import Config, ConfigErr
from parser import parse_config_line
from cmake import convert_config_to_cmake


sys.path.append(os.getcwd())


def read_config_lists_at(dir, scope):
    path = Path(dir).joinpath('ConfigLists.py')
    with open(path) as f:
        exec(f.read(), scope)

SCOPE_KEYS = ['CONFIGS', 'SOURCE_CONFIGS']

def read_config_lists():
    config_lists = {}
    curr_paths = [os.getcwd()]

    while len(curr_paths) != 0:
        fut_paths = []

        for path in curr_paths:
            scope = {}
            read_config_lists_at(path, scope=scope)

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


if __name__ == '__main__':
    arg_parser = ap.ArgumentParser()
    arg_parser.add_argument('filename', type=str, default=None, nargs='?')

    args = arg_parser.parse_args()

    try:
        file = sys.stdin if args.filename is None else open(args.filename, 'r')
    except FileNotFoundError:
        print(f"File under location '{args.filename}' not found.", file=sys.stderr)
        raise

    if file == sys.stdin:
        print('Reading from stdin...')

    print("Reading ConfigLists...")
    config_lists = read_config_lists()
    configs = combine_config_lists(config_lists)

    config = Config(configs)

    for line in file:
        try:
            name, value = parse_config_line(line)
            config.define_config(name, value)
        except Exception as e:
            pass

    with open('config.cmake', 'w') as f:
        convert_config_to_cmake(config.config, f)

