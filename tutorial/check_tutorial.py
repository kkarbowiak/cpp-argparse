import glob
import os.path
import re


def main():
    cpp_files = glob.glob("*.cpp")
    check_presence_in_cmakelists(cpp_files)
    check_contents_in_readme(cpp_files)


def check_presence_in_cmakelists(cpp_files):
    with open("CMakeLists.txt", "r") as f:
        cmake_content = f.read()
    for cpp_file in cpp_files:
        cpp_target = os.path.splitext(cpp_file)[0]
        if cpp_target not in cmake_content:
            print(f"Warning: {cpp_target} is not listed in CMakeLists.txt")


def check_contents_in_readme(cpp_files):
    with open("readme.md", "r") as f:
        readme_content = f.read()
    sources = extract_code_blocks(readme_content)
    sources_names = sorted([fname for fname in sources.keys()])
    sym_diff = list(set(cpp_files) ^ set(sources_names))
    if sym_diff:
        print("The following files are mismatched between readme.md and the directory:")
        for fname in sym_diff:
            print(f"  {fname}")
    for cpp_file in cpp_files:
        with open(cpp_file, "r") as f:
            code_in_file = f.read()
        code_in_readme = sources.get(cpp_file, "")
        if code_in_file != code_in_readme:
            print(f"Warning: Content mismatch for {cpp_file} between file and readme.md")


def extract_code_blocks(readme_content):
    # Match: "(`filename.cpp``):\n```c++(...)```"
    pattern = r"\(`([a-zA-Z0-9_]+\.cpp)`\):\s*```c\+\+\s*([\s\S]*?)```"
    return dict(re.findall(pattern, readme_content))


if __name__ == '__main__':
    main()
