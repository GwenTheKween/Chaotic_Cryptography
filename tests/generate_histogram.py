#!/bin/python
from sys import argv
import numpy as np
import matplotlib.pyplot as plt
import argparse

def gen_histogram(filename, mode):
    l = []
    with open(filename) as f:
        l = f.readlines()
    
    vals = [int(x.strip()) for x in l]
    if(mode == 'default'):
        plt.hist(vals)
    elif(mode == 'PRNG'):
        plt.hist(vals, bins=256, range = (0,255))
    plt.savefig(filename+'.png')
    plt.show()

if __name__ == '__main__':
    def parse_args(args):
        parser = argparse.ArgumentParser(description='generate a histogram based on the given file')
        parser.add_argument('--file', help='Name of the file to analyze', required=True)
        parser.add_argument('--mode', help='How this script should analyze the file. Using PRNG will make the histogram ranges be [0-255] and have 256 bins', choices=['default', 'PRNG'], default='default')
        return parser.parse_args(args)

    args = parse_args(argv[1:])

    gen_histogram(args.file, args.mode)
