import argparse
import os.path
from util import get_project_name, get_template, camel_to_snake


def create_dto(dto_name: str, dto_path: str):
    dto_path = os.path.join(dto_path)
    project_name = get_project_name()
    namespace = project_name
    for x in dto_path.split('/')[1:]:
        namespace += '::' + x

    dto_content = get_template().replace('NAMESPACE', namespace).replace('GUARDNAME', camel_to_snake(dto_name)).replace(
        'DTO_NAME',
        dto_name)
    with open(os.path.join(dto_path, dto_name + '.hpp'), 'x') as f:
        f.write(dto_content)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog='Dto Creator',
        description='Create a new dto class hpp file in project.')
    parser.add_argument('-n', '--name', type=str)
    parser.add_argument('-p', '--path', type=str)
    args = parser.parse_args()
    create_dto(args.name, args.path)
