import argparse


parser = argparse.ArgumentParser(description='Test description')
parser.add_argument('-a', help='optional arg #1')

args = parser.parse_args()

print(args)