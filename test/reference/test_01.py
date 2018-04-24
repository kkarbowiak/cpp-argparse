import argparse


parser = argparse.ArgumentParser(description='Test description')
parser.add_argument('pos1', help='positional arg #1')

args = parser.parse_args()

print(args)
