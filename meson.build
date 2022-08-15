project(
  'airwindows-lv2',
  'c',
  default_options : [
    'c_std=gnu99',
    'default_library=static',
    'buildtype=release',
    'optimization=3'
  ],
  license : 'MIT',
  version : '7.0'
)
bundle_name = 'Airwindows.lv2'

cc = meson.get_compiler('c')

m_dep = cc.find_library('m', required : false)
lv2_dep = dependency('lv2')

plugins = [
  'Acceleration',
  'Acceleration2',
  'ADClip7',
  'Baxandall',
  'BitShiftGain',
  'Capacitor',
  'Capacitor2',
  'Channel8',
  'Channel9',
  'ClipOnly',
  'ClipOnly2',
  'Console6Buss',
  'Console6Channel',
  'Console7Buss',
  'Console7Cascade',
  'Console7Channel',
  'Console7Crunch',
  'DCVoltage',
  'DeBess',
  'Desk',
  'Dyno',
  'EdIsDim',
  'EveryTrim',
  'Focus',
  'Galactic',
  'HermeTrim',
  'Hypersonic',
  'Infrasonic',
  'Interstage',
  'IronOxideClassic2',
  'LeftoMono',
  'Mackity',
  'MidSide',
  'Mojo',
  'MoNoam',
  'MV',
  'Nikola',
  'PocketVerbs',
  'Pressure5',
  'PurestConsoleBuss',
  'PurestConsoleChannel',
  'PurestConsole2Buss',
  'PurestConsole2Channel',
  'PurestDrive',
  'PurestGain',
  'RightoMono',
  'SideDull',
  'Sidepass',
  'Slew',
  'SlewOnly',
  'Spiral',
  'Srsly2',
  'StarChild',
  'SubsOnly',
  'ToTape6',
  'TransDesk',
  'Tube',
  'Tube2',
  'TubeDesk',
  'Ultrasonic',
  'UltrasonicLite',
  'UltrasonicMed',
  'Vibrato'
]

# Figure out install_dir
lv2dir = get_option('lv2dir')
if lv2dir == ''
  prefix = get_option('prefix')
  if target_machine.system() == 'darwin' and prefix == '/'
    lv2dir = '/Library/Audio/Plug-Ins/LV2'
  elif target_machine.system() == 'windows' and prefix == 'C:/'
    lv2dir = 'C:/Program Files/Common/LV2'
  else
    lv2dir = prefix / get_option('libdir') / 'lv2'
  endif
endif
bundle_install_dir = lv2dir / bundle_name

conf_data = configuration_data()
conf_data_init = false

foreach plugin_name : plugins
  lib = shared_library(
    plugin_name,
    'src' / plugin_name / plugin_name + '.c',
    dependencies : [lv2_dep, m_dep],
    gnu_symbol_visibility : 'hidden',
    install: true,
    install_dir: bundle_install_dir,
    name_prefix : ''
  )

  if not conf_data_init
    conf_data.set('CMAKE_SHARED_LIBRARY_SUFFIX', '.' + lib.full_path().split('.')[-1])
    conf_data_init = true
  endif

  configure_file(
    input : 'src' / plugin_name / plugin_name + '.ttl',
    output : plugin_name + '.ttl',
    copy : true,
    install : true,
    install_dir : bundle_install_dir
  )
endforeach

configure_file(
  input : 'src' / 'manifest.ttl.in',
  output : 'manifest.ttl',
  configuration : conf_data,
  install : true,
  install_dir : bundle_install_dir
)