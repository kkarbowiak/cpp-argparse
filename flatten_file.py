import argparse
import subprocess
import flatten


def main():
    parser = get_args_parser()
    args = parser.parse_args()
    line_nos = get_test_case_line_numbers(args.file)
    process_file(args.file, line_nos.reverse(), args.expected)


def get_args_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument('file', help='file to process')
    parser.add_argument('expected', help='text expected in output')
    return parser


def get_test_case_line_numbers(file_name):
    line_nos = []
    with open(file_name) as f:
        lines = f.readlines()
        for line_no in range(len(lines)):
            if 'TEST_CASE' in lines[line_no]:
                line_nos.append(line_no)
    return line_nos


def process_file(file_name, line_nos, expected):
    for line_no in line_nos:
        flatten.process_file(file_name, line_no)
        build_and_commit(file_name, expected)


def build_and_commit(file_name, expected):
    subprocess.check_call(['cmake', '--build', '.', '-j'])
    output = subprocess.check_output(['./test/unittest/unittest'])
    if expected not in output:
        raise RuntimeError('Expected text not found in output')
    subprocess.check_call(['git', 'add', file_name])
    subprocess.check_call(['git', 'commit', '-m', 'Flatten test case'])


if __name__ == '__main__':
    main()
