"""
Generate the default config file
"""
import ConfigParser

cfg = ConfigParser.SafeConfigParser()

sec = 'UI_SECTION'

cfg.add_section(sec)
cfg.set(sec, 'app_height', '700')
cfg.set(sec, 'app_width', '800')
cfg.set(sec, 'top_panel_height', '-1')
cfg.set(sec, 'bottom_panel_height', '-1')

cfg.set(sec, 'w_view_log', 'True')
cfg.set(sec, 'w_view_sky', 'True')
cfg.set(sec, 'w_view_webcam', 'True')
cfg.set(sec, 'w_view_status', 'True')
cfg.set(sec, 'w_view_graphs', 'True')
cfg.set(sec, 'w_view_cmd_input', 'True')

cfg.set(sec, 's_view_location', 'True')
cfg.set(sec, 's_view_command', 'True')
cfg.set(sec, 's_view_azel', 'True')
cfg.set(sec, 's_view_offsets', 'True')
cfg.set(sec, 's_view_ra', 'True')
cfg.set(sec, 's_view_time', 'True')
cfg.set(sec, 's_view_source', 'True')
cfg.set(sec, 's_view_vlsr', 'True')
cfg.set(sec, 's_view_integ', 'True')
cfg.set(sec, 's_view_freq', 'True')
cfg.set(sec, 's_view_if', 'False')
cfg.set(sec, 's_view_bw', 'False')
cfg.set(sec, 's_view_resol', 'False')

cfg.set(sec, 't_view_default', 'True')
cfg.set(sec, 't_view_night', 'False')

cfg.set(sec, 'show_cancel_dialog', 'True')

srt_section = "SRT_SECTION"

cfg.add_section(srt_section)
cfg.set(srt_section, 'simulate_antenna', str(True))
cfg.set(srt_section, 'simulate_receiver', str(True))
cfg.set(srt_section, 'simulate_fft', str(True))

cfg.set(srt_section, 'freq', str(1420.4))
cfg.set(srt_section, 'num_freq', str(256))
cfg.set(srt_section, 'cal_mode', str(2))
cfg.set(srt_section, 'azimuth', str(0.0))
cfg.set(srt_section, 'elevation', str(0.0))
cfg.set(srt_section, 'az_limits', str(0.0))
cfg.set(srt_section, 'el_limits', str(0.0))
cfg.set(srt_section, 'bandwidth', str(2.4))
cfg.set(srt_section, 'beamwidth', str(5))
cfg.set(srt_section, 'sys_temp', str(171))
cfg.set(srt_section, 'tcal', str(290))
cfg.set(srt_section, 'nblock', str(5))
cfg.set(srt_section, 'location', 'unknown')
cfg.set(srt_section, 'az_offset', str(0.0))
cfg.set(srt_section, 'el_offset', str(0.0))


# Writing our configuration file to 'example.cfg'
with open('default.cfg', 'wb') as configfile:
    cfg.write(configfile)
