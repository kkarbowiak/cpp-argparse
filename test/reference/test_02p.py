import argparse


parser = argparse.ArgumentParser(description='Test description')
parser.add_argument('pos1', help='positional arg #1')
parser.add_argument('pos2', help='positional arg #2')
parser.add_argument('pos3', help='positional arg #3')

args = parser.parse_args()

print(args)
