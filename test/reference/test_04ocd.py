import argparse


parser = argparse.ArgumentParser()
parser.add_argument('-c', action='count', default=0)
args = parser.parse_args()
print(args)
