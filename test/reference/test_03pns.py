import argparse


parser = argparse.ArgumentParser()
parser.add_argument('pos', nargs='*')
args = parser.parse_args()
print(args)
