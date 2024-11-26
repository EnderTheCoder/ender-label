import os.path
import re


def get_template():
    with open('tools/code_template/Dto.hpp', 'r+') as f:
        content = f.read()
        return content


def get_project_name():
    return 'ender_label'


def camel_to_snake(name):
    name = re.sub('([a-z0-9])([A-Z])', r'\1_\2', name)
    return name.upper()


if __name__ == '__main__':
    dto_name = input('input dto name:')
    dto_path = input('input target path:')

    dto_path = os.path.join(dto_path)
    project_name = get_project_name()
    namespace = project_name
    for x in dto_path.split('/')[1:]:
        namespace += '::' + x

    dto_content = get_template().replace('NAMESPACE', namespace).replace('GUARDNAME', camel_to_snake(dto_name)).replace('DTO_NAME',
                                                                                                          dto_name)
    with open(os.path.join(dto_path, dto_name + '.hpp'), 'x') as f:
        f.write(dto_content)