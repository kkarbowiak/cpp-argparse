import argparse


parser = argparse.ArgumentParser()
parser.add_argument('-o', nargs=3)
args = parser.parse_args()
print(args)
