import glob
import os.path


def main():
    cpp_files = sorted(glob.glob("*.cpp"))
    check_presence_in_cmakelists(cpp_files)
    check_presence_in_readme(cpp_files)
    check_contents_in_readme(cpp_files)


def check_presence_in_cmakelists(cpp_files):
    with open("CMakeLists.txt", "r") as f:
        cmake_content = f.read()
    for cpp_file in cpp_files:
        cpp_target = os.path.splitext(cpp_file)[0]
        if cpp_target not in cmake_content:
            print(f"Warning: {cpp_target} is not listed in CMakeLists.txt")


def check_presence_in_readme(cpp_files):
    with open("readme.md", "r") as f:
        readme_content = f.read()
    for cpp_file in cpp_files:
        if cpp_file not in readme_content:
            print(f"Warning: {cpp_file} is not mentioned in readme.md")


def check_contents_in_readme(cpp_files):
    with open("readme.md", "r") as f:
        readme_content = f.read()
    for cpp_file in cpp_files:
        with open(cpp_file, "r") as f:
            cpp_content = f.read()
        if cpp_content not in readme_content:
            print(f"Warning: Contents of {cpp_file} are not included in readme.md")


if __name__ == '__main__':
    main()
