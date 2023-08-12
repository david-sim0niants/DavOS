def convert_config_to_cmake(config:dict, file):
    for name, value in config.items():
        file.writelines([f'set(CONFIG_{name} {value})\n'])
