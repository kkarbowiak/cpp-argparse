import argparse


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
        print(test_case_lines)


def extract_test_case(lines, line):
    result = []

    if lines[line].startswith('TEST_CASE'):
        while True:
            result.append(lines[line])
            if lines[line].startswith('}'):
                break
            line += 1

    return result


if __name__ == '__main__':
    main()
