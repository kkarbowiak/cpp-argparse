import argparse


parser = argparse.ArgumentParser()
parser.add_argument('--num', type=int)
parser.add_argument('-n', type=int)

args = parser.parse_args(['-n', '-10'])

print(args)
