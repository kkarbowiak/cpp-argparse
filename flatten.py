import argparse
import anytree

# anytree is installed in a virtual environment
# I created the virtual environment with this command:
# `python3 -m venv cpp-argparse-venv`
# and then installed anytree:
# `cpp-argparse-venv/bin/pip install anytree`
# I am now using this command to run this script:
# `cpp-argparse-venv/bin/python3 flatten.py`

class TestNode(anytree.AnyNode):
    def __init__(self, text):
        super(TestNode, self).__init__()
        self.text = text
        self.lines = []
        self.indent = 0


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
        process_tree(test_case_tree)


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
    line_no = 0
    while line_no < len(lines):
        line = lines[line_no]
        if line.count('TEST_CASE') > 0:
            name = line.split('"')
            root = TestNode(name[1])
            root.lines.append(line)
        elif line.count('SUBCASE') > 0:
            root.lines.pop()
            line_no = process_subcase_lines(line_no, lines, root)
        elif line.startswith('}'):
            root.lines.append(line)
            return root
        else:
            root.lines.append(line)
        line_no += 1


def process_subcase_lines(start_no, lines, parent):
    start_line = lines[start_no]
    indent = start_line.find('SUBCASE')
    name = start_line.split('"')
    node = TestNode(name[1])
    node.parent = parent
    node.lines.append(start_line)
    node.indent = indent

    line_no = start_no + 1
    while line_no < len(lines):
        line = lines[line_no]
        if line.count('SUBCASE') > 0:
            node.lines.pop()
            line_no = process_subcase_lines(line_no, lines, node)
        elif line.find('}') == indent:
            node.lines.append(line)
            return line_no
        else:
            node.lines.append(line)
        line_no += 1


def process_tree(tree):
    flattened_lines = []
    walker = anytree.Walker()
    for node in anytree.PreOrderIter(tree):
        if node.is_leaf:
            walk = walker.walk(tree, node)
            root_node = walk[1]
            tc_line = root_node.lines[0]
            new_tc_line = tc_line.replace(root_node.text, join_texts(root_node, walk[2]))
            flattened_lines.append(new_tc_line)
            flattened_lines.extend(walk[1].lines[1:-1])  # all test case lines except for closing brace
            for subnode in walk[2]:
                sublines = subnode.lines[2:-1]  # all subcase lines except for subcase and its opening and closing brace
                flattened_lines.extend([subline if subline.startswith('\n') else subline[subnode.indent:] for subline in sublines])
            flattened_lines.extend(walk[1].lines[-1]) # test case closing brace
            flattened_lines.append('\n')
    return flattened_lines


def join_texts(root, nodes):
    text = root.text.removesuffix('...')
    for node in nodes:
        text += ' ' + node.text.removeprefix('...').removesuffix('...')
    return text


if __name__ == '__main__':
    main()
