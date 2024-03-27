project(
  'airwindows-lv2',
  'c',
  default_options : [
    'c_std=c99',
    'default_library=static',
    'buildtype=release',
    'optimization=3'
  ],
  license : 'MIT',
  version : '28.0'
)
bundle_name = 'Airwindows.lv2'

cc = meson.get_compiler('c')

m_dep = cc.find_library('m', required : false)
lv2_dep = dependency('lv2')

plugins = [
  'Acceleration',
  'Acceleration2',
  'ADClip7',
  'ADClip8',
  'Air2',
  'Air3',
  'BassAmp',
  'BassDrive',
  'BassKit',
  'Baxandall',
  'BigAmp',
  'BitShiftGain',
  'BitShiftPan',
  'BrightAmbience',
  'BrightAmbience2',
  'BrightAmbience3',
  'BussColors4',
  'Cabs',
  'Capacitor',
  'Capacitor2',
  'Channel8',
  'Channel9',
  'ClipOnly',
  'ClipOnly2',
  'ClipSoftly',
  'Compresaturator',
  'Console0Buss',
  'Console0Channel',
  'Console6Buss',
  'Console6Channel',
  'Console7Buss',
  'Console7Cascade',
  'Console7Channel',
  'Console7Crunch',
  'Console8BussHype',
  'Console8BussIn',
  'Console8BussOut',
  'Console8ChannelHype',
  'Console8ChannelIn',
  'Console8ChannelOut',
  'Console8LiteBuss',
  'Console8LiteChannel',
  'Console8SubHype',
  'Console8SubIn',
  'Console8SubOut',
  'ConsoleLABuss',
  'ConsoleLAChannel',
  'Creature',
  'curve',
  'DCVoltage',
  'DeBess',
  'Desk',
  'Distance2',
  'Drive',
  'DrumSlam',
  'Dyno',
  'EdIsDim',
  'EQ',
  'EverySlew',
  'EveryTrim',
  'Facet',
  'FinalClip',
  'FireAmp',
  'Flipity',
  'Focus',
  'Fracture',
  'Galactic',
  'GlitchShifter',
  'GoldenSlew',
  'Golem',
  'GrindAmp',
  'HermeTrim',
  'HighGlossDither',
  'Hull2',
  'Hypersonic',
  'Infinity',
  'Infinity2',
  'Inflamer',
  'Infrasonic',
  'Interstage',
  'IronOxideClassic2',
  'kPlateA',
  'kPlateB',
  'kPlateC',
  'kPlateD',
  'LeadAmp',
  'LeftoMono',
  'LilAmp',
  'Luxor',
  'Mackity',
  'MatrixVerb',
  'MidAmp',
  'MidSide',
  'Mojo',
  'Monitoring',
  'MoNoam',
  'MV',
  'Nikola',
  'PitchNasty',
  'PhaseNudge',
  'PlatinumSlew',
  'PocketVerbs',
  'Point',
  'PowerSag',
  'PowerSag2',
  'Pressure5',
  'PurestConsoleBuss',
  'PurestConsoleChannel',
  'PurestConsole2Buss',
  'PurestConsole2Channel',
  'PurestConsole3Buss',
  'PurestConsole3Channel',
  'PurestDrive',
  'PurestGain',
  'RawGlitters',
  'RawTimbers',
  'Recurve',
  'ResEQ',
  'ResEQ2',
  'Reverb',
  'RightoMono',
  'SampleDelay',
  'ShortBuss',
  'SideDull',
  'Sidepass',
  'Sinew',
  'Slew',
  'SlewOnly',
  'Spiral',
  'Spiral2',
  'Srsly2',
  'StarChild',
  'SubsOnly',
  'ToTape5',
  'ToTape6',
  'TPDFDither',
  'TransDesk',
  'Tube',
  'Tube2',
  'TubeDesk',
  'Ultrasonic',
  'UltrasonicLite',
  'UltrasonicMed',
  'Verbity',
  'Vibrato',
  'XBandpass',
  'XLowpass',
  'XNotch',
  'XRegion',
  'ZOutputStage',
]

# Figure out install_dir
lv2dir = get_option('lv2dir')
if lv2dir == ''
  prefix = get_option('prefix')
  if target_machine.system() == 'darwin' and prefix == '/'
    lv2dir = '/Library/Audio/Plug-Ins/LV2'
  elif target_machine.system() == 'windows' and prefix == 'C:/'
    lv2dir = 'C:/Program Files/Common Files/LV2'
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
