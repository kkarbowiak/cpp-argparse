import argparse


parser = argparse.ArgumentParser()
parser.add_argument('-o')
parser.add_argument('-a', action='store_true')
parser.add_argument('-v', action='store_true')
args = parser.parse_args()
print(args)
