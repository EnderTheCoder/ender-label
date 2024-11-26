import os

for root, dirs, files in os.walk('./src/dto'):
    for file in files:
        if file.endswith('Dto.hpp'):
            with open(os.path.join(root, file), 'r+') as f:
                contents = str(f.read())
                contents = contents.replace('#include <oatpp/codegen/dto/base_define.hpp>', '')
                f.seek(0)
                f.truncate(0)
                f.write(contents)