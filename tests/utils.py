import numpy

def read_file(filename):
    """
    Read the given file, which expects a single number in the range 0-255
    per line.
    Returns an array with the numbers
    """
    l = []
    with open(filename) as f:
        l = f.readlines()

    return numpy.array([int(x.strip()) for x in l])
