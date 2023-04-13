import numpy

def read_file(filename):
    """
    Read the given file, which expects a single number in the range 0-255
    per line.
    Returns an array with the numbers
    """
    l = []
    with open(filename) as f:
        for line in f:
            if line[0] == '#' or line[0] == ' ':
                continue
            l.append(int(line.strip()))

    return numpy.array(l)
