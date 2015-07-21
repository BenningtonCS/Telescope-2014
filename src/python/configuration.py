from ConfigParser import SafeConfigParser
import os


class ConfigStateManager(object):
    """
    Class which manages session configuration.
    """
    def __init__(self):
        self.dflt_path = './.config/default.cfg'
        self.cfg_path = './.config/session.cfg'

    def save_session_state(self):
        """

        :return:
        """
        cfg = SafeConfigParser()

        self._save_srt_state(cfg)
        self._save_ui_state(cfg)

    def load_session_state(self):
        """

        :return:
        """
        cfg = SafeConfigParser()

        cfg_file = self.cfg_path if os.path.isfile(self.cfg_path) else self.dflt_path
        cfg.read(cfg_file)

        self._load_ui_state(cfg)
        self._load_srt_state(cfg)

    def _save_ui_state(self, cfg):
        """
        Save state related to UI layout

        :param cfg:
        :type cfg: SafeConfigParser
        :return:
        """
        section_name = 'UI_SECTION'
        cfg.add_section(section_name)

        cfg.set(section_name, 'key', 'value')

    def _save_srt_state(self, cfg):
        """
        Save state related to srt configurations

        :param cfg:
        :type cfg: SafeConfigParser
        :return:
        """
        section_name = 'SRT_SECTION'
        cfg.add_section(section_name)

        cfg.set(section_name, 'key', 'value')

    def _load_ui_state(self, cfg):
        """
        Load state related to UI layout

        :param cfg:
        :type cfg: SafeConfigParser
        :return:
        """
        pass

    def _load_srt_state(self, cfg):
        """
        Load state related to srt configurations

        :param cfg:
        :type cfg: SafeConfigParser
        :return:
        """
        pass

    def _write_state(self, cfg):
        """

        :param cfg:
        :type cfg: SafeConfigParser
        :return:
        """
        pass
