import argparse


parser = argparse.ArgumentParser()
parser.add_argument('-a', action='append')
args = parser.parse_args()
print(args)
