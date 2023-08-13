import inspect

class ConfigErr(Exception):
    def __init__(self, *args: object) -> None:
        super().__init__(*args)


class Config:
    '''
        Class containing config lists (unvalued configuration items with
        their dependencies, descriptions, types etc.) and key(name)/value dict.
    '''
    def __init__(self, lists:dict) -> None:
        '''
            Initialize a config object from the config lists.
            Note: config lists will be stored as a reference
            and will be modified. Pass a copy of it if to avoid any changes.
        '''
        self.lists = lists

        # create a list of dependants for each config item
        for name, config_item in self.lists.items():
            if 'dependants' not in config_item:
                self.__trace_dependencies(name)

        self.config = {}
        self.__set_initial_default_values()


    def __trace_dependencies(self, config_name: str, dependency_stack=[]):
        for dependency_config_name in self.lists[config_name].get('depends', []):
            dependency_config_item = self.lists.get(dependency_config_name, None)
            if dependency_config_item is None:
                raise ConfigErr(f'Config {config_name}\
                        has an unknown dependency config f{dependency_config_name}')
            if config_name == dependency_config_name or dependency_config_name in dependency_stack:
                raise ConfigErr(f'Circular dependencies found. Dependency stack: \
                        {dependency_stack + [config_name, dependency_config_name]}.')

            dependants:set = dependency_config_item.get('dependants', set())
            dependants.add(config_name)
            dependency_config_item['dependants'] = dependants

            self.__trace_dependencies(dependency_config_name,\
                    dependency_stack=dependency_stack + [config_name])


    def __set_initial_default_values(self):
        for name, config_item in self.lists.items():
            if 'depends' in config_item:
                continue
            self.__set_config(name, Config.__get_default_value(config_item, self.config), config_item)
        for name, config_item in self.lists.items():
            self.__recheck_dependants(config_item)


    @staticmethod
    def __flatten_value_set(value_set):
        new_value_set = set()
        for value in value_set:
            if isinstance(value, tuple):
                new_value_set.update(value)
            else:
                new_value_set.add(value)
        return new_value_set


    def __check_value_set(self, config_item, value):
        '''
            Check value in the value set of the config item.
        '''
        if 'value_set' in config_item:
            value_set = config_item['value_set']
            return value in Config.__flatten_value_set(value_set)
        else:
            return False


    def __run_value_checker(self, config_item, value):
        '''
            Check value via the value checker of the config item.
        '''
        if 'value_checker' in config_item:
            value_checker = config_item['value_checker']
            return value_checker(value, self.config)
        else:
            return True, None


    @staticmethod
    def __check_default_value_type(config_item):
        default_value = config_item['default_value']
        if not callable(default_value):
            if 'type' in config_item:
                config_item_type = config_item['type']
                if not isinstance(config_item_type, type):
                    raise ConfigErr('Config item type must be a python type object.')
                if not isinstance(default_value, config_item_type):
                    raise ConfigErr('Type of the default value of the config item does not match the type specified - {config_item_type}')
            return

        # default_value is callable case
        if len(inspect.signature(default_value).parameters) == 1:
            return
        raise ConfigErr('Dynamic default value must be a single argument callable object')


    @staticmethod
    def __get_default_value(config_item, config):
        Config.__check_default_value_type(config_item)
        default_value = config_item['default_value']
        if callable(default_value):
            return default_value(config)
        else:
            return default_value


    def __recheck_dependant(self, dependant, rechecked_set: set):
        if dependant in rechecked_set:
            return
        rechecked_set.add(dependant)

        config_item = self.lists[dependant]
        value = self.config.get(dependant)

        if value is not None:
            if 'value_set' in config_item and self.__check_value_set(config_item, value):
                return
            if 'value_checker' in config_item:
                passed, _ = self.__run_value_checker(config_item, value)
                if passed:
                    return

        default_value = Config.__get_default_value(config_item, self.config)
        self.__set_config(dependant, default_value, config_item)


    def __recheck_dependants(self, config_item: dict):
        rechecked_set = set()

        current_dependants = set(config_item.get('dependants', []))

        while len(current_dependants) != 0:
            future_dependants = set()
            for dependant in current_dependants:
                self.__recheck_dependant(dependant, rechecked_set=rechecked_set)
                future_dependants = future_dependants.union(self.lists[dependant].get('dependants', []))
            current_dependants = future_dependants


    def define_config(self, name, value):
        if name in self.config and self.config[name] == value:
            return True, 'same-value'

        config_item = self.lists[name]
        if 'value_set' in config_item and not self.__check_value_set(config_item, value):
            return False, f'Value provided for the config {name} was not found in its pre-defined value set.'

        if 'value_checker' in config_item:
            passed, reason = self.__run_value_checker(config_item, value)
            if not passed:
                if isinstance(reason, str):
                    return False, f'Value checker failed: {reason}'
                else:
                    return False, 'Value checker failed.'

        self.__set_config(name, value, config_item)
        self.__recheck_dependants(config_item)

        return True, None


    def __set_config(self, name, value, config_item):
        self.config[name] = value
        if 'on_value_change' in config_item:
            config_item['on_value_change'](self.config)

