import argparse

parser = argparse.ArgumentParser()
parser.add_argument('pos')
args = parser.parse_args(['--help'])
print(args)
