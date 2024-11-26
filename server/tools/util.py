import re


def camel_to_snake(name):
    name = re.sub('([a-z0-9])([A-Z])', r'\1_\2', name)
    return name.upper()


def get_template():
    with open('tools/code_template/Dto.hpp', 'r+') as f:
        content = f.read()
        return content


def get_project_name():
    with open('CMakeLists.txt', 'r+') as f:
        content = f.read()
        pattern = r'set\(([^)]+)\)'
        match = re.search(pattern, content)
        if match:
            return match.group(1).split(' ')[1]
        else:
            raise Exception('Unable to find project name in CMakeList.txt')
