import argparse


parser = argparse.ArgumentParser()
parser.add_argument('pos', nargs=3)
parser.add_argument('-f', action='store_true')
args = parser.parse_args()
print(args)
