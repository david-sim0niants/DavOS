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


    def __trace_dependencies(self, config_name: str, dependency_stack=[]):
        for dependency_config_name in self.lists[config_name].get('depends', []):
            dependency_config_item = self.lists.get(dependency_config_name, None)
            if dependency_config_item is None:
                raise ConfigErr(f'Config {config_name}\
                        has an unknown dependency config f{dependency_config_name}')
            if config_name == dependency_config_name or dependency_config_name in dependency_stack:
                raise ConfigErr(f'Circular dependencies found. Dependency stack: {dependency_stack + [config_name, dependency_config_name]}.')

            dependants:set = dependency_config_item.get('dependants', set())
            dependants.add(config_name)
            dependency_config_item['dependants'] = dependants

            self.__trace_dependencies(dependency_config_name, dependency_stack=dependency_stack + [config_name])


    @staticmethod
    def __flatten_value_set(value_set):
        new_value_set = set()
        for value in value_set:
            if isinstance(value, tuple):
                new_value_set.update(value)
            else:
                new_value_set.add(value)
        return new_value_set


    def __check_value_set__true_if_missing(self, config_item, value):
        '''
            Check value in value set of the config item.
            Return True if the value set is missing (not the value in it).
        '''
        if 'value_set' in config_item:
            value_set = config_item['value_set']
            return value in Config.__flatten_value_set(value_set)
        else:
            return True


    def __run_value_checker__true_if_missing(self, config_item, value):
        '''
            Check value via the value checker of the config item.
            Return True if the value checker is missing.
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


    def __recheck_dependant(self, dependant):
        config_item = self.lists[dependant]
        value = self.config.get(dependant)

        if value is not None:
            if self.__check_value_set__true_if_missing(config_item, value):
                return True
            r = self.__run_value_checker__true_if_missing(config_item, value)
            passed, _ = r
            if passed:
                return True

        if 'default_value' in config_item:
            default_value = Config.__get_default_value(config_item, self.config)
            self.config[dependant] = default_value

        for dependant in config_item.get('dependants', []):
            self.__recheck_dependant(dependant)


    def define_config(self, name, value):
        config_item = self.lists[name]
        if not self.__check_value_set__true_if_missing(config_item, value):
            return False, f'Value provided for the config {name} was not found in its pre-defined value set.'

        r = self.__run_value_checker__true_if_missing(config_item, value)
        passed, reason = r
        if not passed:
            if isinstance(reason, str):
                return False, f'Value checker failed: {reason}'
            else:
                return False, 'Value checker failed.'

        self.config[name] = value
        for dependant in config_item.get('dependants', []):
            self.__recheck_dependant(dependant)

        return True, None

