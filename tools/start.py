import os

project_name = input('input a project name to start:')

for root, dirs, files in os.walk(''):
    for file in files:
        if file.endswith('.hpp') or file.endswith('.cpp') or file == 'CMakeLists.txt':
            with open(os.path.join(root, file), 'r+') as f:
                contents = str(f.read())
                contents = contents.replace('OATPP_BASE_PROJECT', project_name.upper()).replace('oatpp_base_project', project_name.lower())
                f.seek(0)
                f.truncate(0)
                f.write(contents)
