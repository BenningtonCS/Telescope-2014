from ConfigParser import SafeConfigParser
import os


class ConfigStateManager(object):
    """
    Class which manages session configuration.
    """
    def __init__(self):
        self.dflt_path = './.config/default.cfg'
        self.cfg_path = './.config/session.cfg'

        self.cfg_file = self.cfg_path if os.path.isfile(self.cfg_path) else self.dflt_path

        # ======================================
        # UI STATE
        # ======================================
        self.ui_section = "UI_SECTION"

        # Application Dimensions
        # ----------------------------
        self.app_height = None
        self.app_width = None

        self.top_panel_height = None
        self.bottom_panel_height = None

        # Views
        # ----------------------------
        # Window Views
        self.w_view_log = None
        self.w_view_sky = None
        self.w_view_webcam = None
        self.w_view_status = None
        self.w_view_graphs = None
        self.w_view_cmd_input = None

        # Status Views
        self.s_view_location = None
        self.s_view_command = None
        self.s_view_azel = None
        self.s_view_offsets = None
        self.s_view_ra = None
        self.s_view_time = None
        self.s_view_source = None
        self.s_view_vlsr = None
        self.s_view_integ = None
        self.s_view_freq = None
        self.s_view_if = None
        self.s_view_bw = None
        self.s_view_resol = None

        # Theme Views
        self.t_view_default = None
        self.t_view_night = None

        # ======================================
        # SRT STATE
        # ======================================
        self.srt_section = "SRT_SECTION"

    def save_session_state(self):
        """
        Save the session state

        :return:
        """
        cfg = SafeConfigParser()

        # self._save_srt_state(cfg)
        self._save_ui_state(cfg)
        self._write_state(cfg)

    def load_session_state(self):
        """
        Load in existing session state. If no previous session state exists
        (first start up), load a default session state.

        :return:
        """
        cfg = SafeConfigParser()
        cfg.read(self.cfg_file)

        self._load_ui_state(cfg)
        self._load_srt_state(cfg)

    def _save_ui_state(self, cfg):
        """
        Save state related to UI layout

        :param cfg:
        :type cfg: SafeConfigParser
        :return:
        """
        cfg.add_section(self.ui_section)
        cfg.set(self.ui_section, 'app_height', str(self.app_height))
        cfg.set(self.ui_section, 'app_width', str(self.app_width))
        cfg.set(self.ui_section, 'top_panel_height', str(self.top_panel_height))
        cfg.set(self.ui_section, 'bottom_panel_height', str(self.bottom_panel_height))

        cfg.set(self.ui_section, 'w_view_log', str(self.w_view_log))
        cfg.set(self.ui_section, 'w_view_sky', str(self.w_view_sky))
        cfg.set(self.ui_section, 'w_view_webcam', str(self.w_view_webcam))
        cfg.set(self.ui_section, 'w_view_status', str(self.w_view_status))
        cfg.set(self.ui_section, 'w_view_graphs', str(self.w_view_graphs))
        cfg.set(self.ui_section, 'w_view_cmd_input', str(self.w_view_cmd_input))

        cfg.set(self.ui_section, 's_view_location', str(self.s_view_location))
        cfg.set(self.ui_section, 's_view_command', str(self.s_view_command))
        cfg.set(self.ui_section, 's_view_azel', str(self.s_view_azel))
        cfg.set(self.ui_section, 's_view_offsets', str(self.s_view_offsets))
        cfg.set(self.ui_section, 's_view_ra', str(self.s_view_ra))
        cfg.set(self.ui_section, 's_view_time', str(self.s_view_time))
        cfg.set(self.ui_section, 's_view_source', str(self.s_view_source))
        cfg.set(self.ui_section, 's_view_vlsr', str(self.s_view_vlsr))
        cfg.set(self.ui_section, 's_view_integ', str(self.s_view_integ))
        cfg.set(self.ui_section, 's_view_freq', str(self.s_view_freq))
        cfg.set(self.ui_section, 's_view_if', str(self.s_view_if))
        cfg.set(self.ui_section, 's_view_bw', str(self.s_view_bw))
        cfg.set(self.ui_section, 's_view_resol', str(self.s_view_resol))

        cfg.set(self.ui_section, 't_view_default', str(self.t_view_default))
        cfg.set(self.ui_section, 't_view_night', str(self.t_view_night))

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
        cfg.read(self.cfg_file)

        # Application Dimensions
        # ----------------------------
        self.app_height = cfg.getint(self.ui_section, 'app_height')
        self.app_width  = cfg.getint(self.ui_section, 'app_width')

        self.top_panel_height    = cfg.getint(self.ui_section, 'top_panel_height')
        self.bottom_panel_height = cfg.getint(self.ui_section, 'bottom_panel_height')

        # Views
        # ----------------------------
        # Window Views
        self.w_view_log       = cfg.getboolean(self.ui_section, 'w_view_log')
        self.w_view_sky       = cfg.getboolean(self.ui_section, 'w_view_sky')
        self.w_view_webcam    = cfg.getboolean(self.ui_section, 'w_view_webcam')
        self.w_view_status    = cfg.getboolean(self.ui_section, 'w_view_status')
        self.w_view_graphs    = cfg.getboolean(self.ui_section, 'w_view_graphs')
        self.w_view_cmd_input = cfg.getboolean(self.ui_section, 'w_view_cmd_input')

        # Status Views
        self.s_view_location = cfg.getboolean(self.ui_section, 's_view_location')
        self.s_view_command  = cfg.getboolean(self.ui_section, 's_view_command')
        self.s_view_azel     = cfg.getboolean(self.ui_section, 's_view_azel')
        self.s_view_offsets  = cfg.getboolean(self.ui_section, 's_view_offsets')
        self.s_view_ra       = cfg.getboolean(self.ui_section, 's_view_ra')
        self.s_view_time     = cfg.getboolean(self.ui_section, 's_view_time')
        self.s_view_source   = cfg.getboolean(self.ui_section, 's_view_source')
        self.s_view_vlsr     = cfg.getboolean(self.ui_section, 's_view_vlsr')
        self.s_view_integ    = cfg.getboolean(self.ui_section, 's_view_integ')
        self.s_view_freq     = cfg.getboolean(self.ui_section, 's_view_freq')
        self.s_view_if       = cfg.getboolean(self.ui_section, 's_view_if')
        self.s_view_bw       = cfg.getboolean(self.ui_section, 's_view_bw')
        self.s_view_resol    = cfg.getboolean(self.ui_section, 's_view_resol')

        # Theme Views
        self.t_view_default = cfg.getboolean(self.ui_section, 't_view_default')
        self.t_view_night   = cfg.getboolean(self.ui_section, 't_view_night')

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
        Write the current state to the config file

        :param cfg:
        :type cfg: SafeConfigParser
        :return:
        """
        with open(self.cfg_path, 'wb') as configfile:
            cfg.write(configfile)

    def reset(self):
        """
        Remove any existing session state, so next startup will use
        the default configuration

        :return:
        """
        if os.path.isfile(self.cfg_path):
            os.remove(self.cfg_path)
