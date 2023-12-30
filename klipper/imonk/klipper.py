try:
    from configfile import error as ConfigError
    from configfile import ConfigWrapper
    from gcode import CommandError
    from gcode import GCodeCommand
    from extras.bus import MCU_SPI
    from configfile import PrinterConfig
except:
    class ConfigError(Exception):
        pass
    class CommandError(Exception):
        pass
    class GCodeCommand:
        pass
    class ConfigWrapper:
        def get(self, param) -> str:
            pass

        def get_name(self) -> str:
            pass
    class MCU_SPI:
        pass
    class PrinterConfig:
        pass