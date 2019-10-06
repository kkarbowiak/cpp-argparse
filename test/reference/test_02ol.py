import argparse


parser = argparse.ArgumentParser(description='Test description')
parser.add_argument('--long-name', help='optional arg #1')
parser.add_argument('--another-long-name', help='optional arg #2')
parser.add_argument('--third-long-name', help='optional arg #3')

args = parser.parse_args()

print(args)