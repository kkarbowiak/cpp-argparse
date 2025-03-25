import argparse


parser = argparse.ArgumentParser()
parser.add_argument('--option1', action='store_true')
parser.add_argument('--option2', action='store_false')
parser.add_argument('--option3', action='store_const', const='foo')
parser.add_argument('--option4', action='count')

args = parser.parse_args()

print(args)
