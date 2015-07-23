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


# Writing our configuration file to 'example.cfg'
with open('default.cfg', 'wb') as configfile:
    cfg.write(configfile)
