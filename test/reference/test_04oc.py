import argparse


parser = argparse.ArgumentParser()
parser.add_argument('-c', action='count')
args = parser.parse_args()
print(args)
