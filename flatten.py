import argparse
from anytree import Node, RenderTree

# anytree is installed in a virtual environment
# I created the virtual environment with this command:
# `python3 -m venv cpp-argparse-venv`
# and then installed anytree:
# `cpp-argparse-venv/bin/pip install anytree`
# I am now using this command to run this script:
# `cpp-argparse-venv/bin/python3 flatten.py`


def main():
    parser = get_args_parser()
    args = parser.parse_args()
    process_file(args.file, args.line-1)


def get_args_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument('file', help='file to process')
    parser.add_argument('line', help='line number to start from', type=int)
    return parser


def process_file(file_name, line):
    with open(file_name) as f:
        lines = f.readlines()
        test_case_lines = extract_test_case(lines, line)
        test_case_tree = process_test_case_lines(test_case_lines)


def extract_test_case(lines, line):
    result = []

    if lines[line].startswith('TEST_CASE'):
        while True:
            result.append(lines[line])
            if lines[line].startswith('}'):
                break
            line += 1

    return result


def process_test_case_lines(lines):
    root = None
    level = 0
    for line in lines:
        if line.startswith('TEST_CASE'):
            name = line.split('"')
            print(name[1])


if __name__ == '__main__':
    main()
