#!/usr/bin/python3


print('Let the config begin...')

import argparse as ap
import sys
from .lib import parser

parser = ap.ArgumentParser()
parser.add_argument('filename', type=str, default=None, nargs='?')

args = parser.parse_args()

try:
    file = sys.stdin if args.filename is None else open(args.filename, 'r')
except FileNotFoundError:
    print(f"File under location '{args.filename}' not found.", file=sys.stderr)
    raise

if file == sys.stdin:
    print('Reading from stdin...')


for line in file:
    print(line)
