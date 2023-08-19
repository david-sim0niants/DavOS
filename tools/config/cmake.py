from tools.config.config import Config
from tools.config.utils import MemSize

def convert_config_to_cmake(config:Config, file):
    for name, value in config.config.items():
        config_item = config.lists[name]

        if isinstance(value, bool):
            cmake_value = 'ON' if value else 'OFF'
            cmake_type = 'BOOL'
        elif isinstance(value, MemSize):
            cmake_value = '0x' + hex(value.bytes())[2:].upper()
            cmake_type = 'STRING'
        else:
            cmake_value = str(value)
            cmake_type = 'STRING'

        description = config_item.get('description', '')
        file.writelines([f'set(CONFIG_{name} {cmake_value})\n'])# CACHE {cmake_type} "{description}")\n'])
