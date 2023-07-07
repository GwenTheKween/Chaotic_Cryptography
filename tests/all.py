#! /usr/bin/python
from sys import argv
import numpy
import argparse
import matplotlib.pyplot as plt

import utils

tests = ['hist', 'avg', 'std', 'phase']

def gen_histogram(vals, filename):
    plt.hist(vals, bins=256, range = (0,255))
    plt.savefig(filename+'_hist.png')
    plt.show()

def gen_phase_space(vals, filename):
    phase =[[0 for i in range(256)] for j in range(256)]
    for i in range(1, len(vals)):
        phase[vals[i-1]][vals[i]] += 1
    x = []
    y = []
    color = []
    for i in range(256):
        for j in range(256):
            if(phase[i][j] != 0):
                x.append(i)
                y.append(j)
                color.append(phase[i][j])
    plt.scatter(x,y,c=color)
    plt.savefig(filename+'_phase.png')
    plt.show()

def parse_args(args):
    parser = argparse.ArgumentParser(description='generate a histogram based on the given file')
    parser.add_argument('-f', '--file', help='Name of the file to analyze', required=True)
    parser.add_argument('-t', '--tests', help='Which tests to run.', choices=['all']+tests, nargs='+', required=True)
    return parser.parse_args(args)

args = parse_args(argv[1:])
vals = utils.read_file(args.file)

all_t = False
if 'all' in args.tests:
    args.tests = tests

if 'hist' in args.tests:
    gen_histogram(vals, args.file)
if 'avg' in args.tests:
    print('average is:', numpy.mean(vals))
if 'std' in args.tests:
    print('standard deviation is:', numpy.std(vals))
if 'phase' in args.tests:
    gen_phase_space(vals, args.file)
