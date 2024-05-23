import sys


def filename2beta(fn):
    return fn.split('_')[2].zfill(7)


print(filename2beta(sys.argv[1]))
