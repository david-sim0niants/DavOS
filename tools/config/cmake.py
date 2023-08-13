from config import Config
from utils import MemSize

def convert_config_to_cmake(config:Config, file):
    for name, value in config.config.items():
        config_item = config.lists[name]

        print(type(value))
        if isinstance(value, bool):
            cmake_value = 'ON' if value else 'OFF'
            cmake_type = 'BOOL'
        elif isinstance(value, MemSize):
            cmake_value = str(value.bytes())
            cmake_type = 'STRING'
        else:
            cmake_value = str(value)
            cmake_type = 'STRING'

        description = config_item.get('description', '')
        file.writelines([f'set(CONFIG_{name} {cmake_value} CACHE {cmake_type} "{description}")\n'])
