# Rad Pro
# Builds Rad Pro fonts
#
# (C) 2022-2026 Gissio
#
# License: MIT
#

import os
from pathlib import Path
import sys

import fontconv
import textproc


def get_radpro_path():
    tools_path, _ = os.path.split(__file__)

    return tools_path + '/../'


def run_fontconv(source,
                 codepoint_set,
                 dest,
                 name,
                 pixels=None,
                 bpp=None,
                 ascent=None,
                 descent=None,
                 cap_height=None):
    args = []
    args += ['-s', codepoint_set]
    args += ['-n', name]
    if pixels:
        args += ['-p', str(pixels)]
    if bpp:
        args += ['-b', str(bpp)]
    if ascent:
        args += ['-a', str(ascent)]
    if descent:
        args += ['-d', str(descent)]
    if cap_height:
        args += ['-c', str(cap_height)]
    args += [get_radpro_path() + 'fonts/' + source]
    args += [get_radpro_path() + 'platform.io/src/ui/fonts/gc-01/' + dest]
    sys.argv[1:] = args
    fontconv.main()


def get_codepoint_set(text, codepoint_set=''):
    text = ''.join(text)

    codepoints = textproc.parse_codepoint_set(codepoint_set)

    text = textproc.filter_c_strings(text)

    codepoints.update({ord(char) for char in text})

    return ','.join(textproc.build_codepoint_set(codepoints))



# Set font paths based on language
font_small_monochrome = 'Tiny5.bdf'
font_medium_monochrome = 'RadPro-Sans8.bdf'
font_color = 'NotoSans-SemiBold.ttf'


# Languages
for language_file in Path('../platform.io/src/system/strings').glob('*.h'):
    language = language_file.stem

    # Get codepoint sets using textproc
    language_text = open(language_file, 'rt', encoding='utf-8').readlines()

    font_medium_bpp_low_matches = [
        'STRING_NANO',
        'STRING_MICRO',
        'STRING_MILLI',
        'STRING_KILO',
        'STRING_MEGA',
        'STRING_GIGA',
        'STRING_SV',
        'STRING_SVH',
        'STRING_REM',
        'STRING_REMH',
        'STRING_CPM',
        'STRING_CPS',
        'STRING_COUNT',
        'STRING_COUNTS',
    ]

    font_medium_matches = font_medium_bpp_low_matches.copy() + [
        'STRING_VOLT_PER_METER_UNIT',
        'STRING_TESLA_UNIT',
        'STRING_GAUSS_UNIT',
    ]

    medium_text = [line for line in language_text
                   if any(match in line for match in font_medium_matches)]

    medium_1bpp_text = [line for line in language_text
                        if any(match in line for match in font_medium_bpp_low_matches)]
    
    codepoint_set_small = get_codepoint_set(language_text, '0x20-0x7e')
    codepoint_set_monochrome_medium = get_codepoint_set(language_text)
    codepoint_set_color_medium = get_codepoint_set(medium_text)
    codepoint_set_color_medium_1bpp = get_codepoint_set(medium_1bpp_text)

    if language in ['ja', 'ko', 'zh_CN']:
        continue

    if language in ['en']:
        bpp_low = 2
    else:
        bpp_low = 1

    # Language-specific fonts

    # run_fontconv(font_color,
    #              codepoint_set_color_medium_1bpp,
    #              f'font_medium_{language}_color_32_{bpp_low}bpp.h',
    #              'font_medium',
    #              pixels=32,
    #              bpp=bpp_low)


    run_fontconv(font_color,
                 codepoint_set_small,
                 f'font_small_{language}_color_25_{bpp_low}bpp.h',
                 'font_small',
                 pixels=25,
                 bpp=bpp_low)

    run_fontconv(font_color,
                 codepoint_set_small,
                 f'font_small_{language}_color_22_{bpp_low}bpp.h',
                 'font_small',
                 pixels=22,
                 bpp=bpp_low)

    run_fontconv(font_color,
                 codepoint_set_small,
                 f'font_titlebar_{language}_color_21_{bpp_low}bpp.h',
                 'font_titlebar',
                 pixels=21,
                 bpp=bpp_low)
