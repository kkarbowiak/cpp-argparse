import argparse


parser = argparse.ArgumentParser()
parser.add_argument('num', type=int)

args = parser.parse_args(['-10'])

print(args)
