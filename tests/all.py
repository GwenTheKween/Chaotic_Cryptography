#! /usr/bin/python
from sys import argv
import numpy
import argparse
import matplotlib.pyplot as plt

import utils

def gen_histogram(vals, filename):
    plt.hist(vals, bins=256, range = (0,255))
    plt.savefig(filename+'.png')
    plt.show()

def parse_args(args):
    parser = argparse.ArgumentParser(description='generate a histogram based on the given file')
    parser.add_argument('-f', '--file', help='Name of the file to analyze', required=True)
    parser.add_argument('-t', '--tests', help='Which tests to run.', choices=['all', 'hist', 'avg', 'std'], nargs='+', required=True)
    return parser.parse_args(args)

args = parse_args(argv[1:])
vals = utils.read_file(args.file)

all_t = False
if args.tests[0] == 'all':
    all_t = True

if all_t or 'hist' in args.tests:
    gen_histogram(vals, args.file)
if all_t or 'avg' in args.tests:
    print('average is:', numpy.mean(vals))
if all_t or 'std' in args.tests:
    print('standard deviation is:', numpy.std(vals))
