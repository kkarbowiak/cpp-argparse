import argparse


parser = argparse.ArgumentParser(description='Test description')
parser.add_argument('-a', help='optional arg #1')
parser.add_argument('-d', help='optional arg #2')
parser.add_argument('-b', help='optional arg #3')

args = parser.parse_args()

print(args)