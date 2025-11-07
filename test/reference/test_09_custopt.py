import argparse


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __repr__(self):
        return f"Point({self.x}, {self.y})"


def to_point(str):
    x, y = map(int, str.split(','))
    return Point(x, y)


parser = argparse.ArgumentParser()
parser.add_argument('--point', type=to_point)
parser.add_argument('-p', type=to_point)

args = parser.parse_args()

print(args)
