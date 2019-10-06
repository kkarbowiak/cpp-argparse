import argparse


parser = argparse.ArgumentParser(description='Test description')
parser.add_argument('--long-name', help='optional arg #1')

args = parser.parse_args()

print(args)