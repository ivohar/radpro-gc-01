def barbie_color(color):
    # Simulate the macro: (((color & (0x1f<<16))<<3) | ((color & (0x3f<<8))<<2) | ((color & 0x1f)<<3))
    return (((color & (0x1f << 16)) << 3) |
            ((color & (0x3f << 8)) << 2) |
            ((color & 0x1f) << 3))

def mr_get_color_565(color):
    r = (color >> 16) & 0xFF
    g = (color >> 8) & 0xFF
    b = color & 0xFF
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)

def linint(x, vals):
    for i in range(len(vals) - 1):
        x1, y1 = vals[i]
        x2, y2 = vals[i + 1]
        if x1 <= x <= x2:
            if x1 != x2:
                return int(y1 + (y2 - y1) * (x - x1) / (x2 - x1))
            else:
                return int(y1)
    return 0

# Arrays from your C code
LogColorArray = [
    (0x00, 0x00),
    (0x20, 0x33),
    (0x40, 0x55),
    (0x60, 0x70),
    (0x80, 0x88),
    (0xA0, 0x9E),
    (0xC0, 0xB3),
    (0xE0, 0xC8),
    (0x100, 0xFF)
]

thirdDeg_colorArray = [
    (0x00, 0x00),
    (0x20, 0x50),
    (0x40, 0x79),
    (0x60, 0x88),
    (0x80, 0x8C),
    (0xA0, 0x90),
    (0xC0, 0x9E),
    (0xE0, 0xBF),
    (0x100, 0xFF)
]

def interpolate_color(color, mode):
    r = (color >> 16) & 0xFF
    g = (color >> 8) & 0xFF
    b = color & 0xFF

    if mode == 3:
        arr = thirdDeg_colorArray
    else:
        arr = LogColorArray

    r = linint(r, arr)
    g = linint(g, arr)
    b = linint(b, arr)
    return (r << 16) | (g << 8) | b

def TransformColor(color, displayFNIRSI):
    if displayFNIRSI == 1:
        color = barbie_color(color)
    elif displayFNIRSI in (2, 3):
        color = interpolate_color(color, displayFNIRSI)
    return color
    #return mr_get_color_565(color)

# Example displayColors array (first column only, fill with your actual values)
displayColors = [
    [0x1a1a1a, 0xf2f0f2, 0x474700],
    [0x666366, 0x9a9a9a, 0x474600],
    [0xf7f7f7, 0x1f1f1f, 0x000000],
    [0xfcfdfc, 0x262726, 0x0a0a00],
    [0xd0d0d0, 0x181818, 0x000000],
    [0xe8e8e8, 0x1f1f1f, 0x000000],
    [0xd0d0d0, 0x202020, 0x000000],
    [0xe8e8e8, 0x282828, 0x000000],
    [0x325b9a, 0x5696c7, 0x003315],
    [0xd6deeb, 0x3b4550, 0x081303],
    [0xb8c0cd, 0x222c37, 0x1a2703],
    [0xffffff, 0x333233, 0x0a0a00],
    [0xd9d8d9, 0x141214, 0x212400],
    [0x2e548e, 0x45789f, 0x002911],
    [0xc5cdd9, 0x2f3740, 0x060f02],
    [0xaab1bd, 0x1b232c, 0x151f02],
    [0xebedeb, 0x292929, 0x080800],
    [0xc8c8c8, 0x101010, 0x1a1d00],
    [0x2a4d83, 0x37607f, 0x00210e],
    [0xb6bdc8, 0x262c33, 0x050b02],
    [0x9da3ae, 0x161c23, 0x111902],
    [0xd9d8d9, 0x212121, 0x060700],
    [0xb8b8b8, 0x0d0f0d, 0x151700],
    [0x333233, 0xe6e6e6, 0x474700],
    [0x707070, 0x9c9c9c, 0x333200],
    [0xdf1b1b, 0xe52626, 0x4d0005],
    [0xffffff, 0x333233, 0x000000],
    [0xb0b6bf, 0x3c5275, 0x002912],
    [0x161718, 0xf2f0f2, 0x474700],
    [0x737473, 0xa3a4a3, 0x474600],
    [0x305792, 0x5779a2, 0x193422],
    [0xb89070, 0xb89070, 0x202000],
    [0xd0b088, 0xd0b088, 0x303000],
    [0x000000, 0x000000, 0x000000],
    [0xffffff, 0xffffff, 0x484800],
]

# Theme: Day - Enhancement Logarithmic
# Theme: Dusk - Linear
# Theme: Night - Enhancement 3rd Degree
# Theme: Barbie - Linear from Day
for fnirsi in range(1, 4):
    print(f"settings.displayFNIRSI = {fnirsi}")
    for row in displayColors:
        color = row[0]
        transformed = TransformColor(color, fnirsi)
        print(f"0x{row[0]:06X}, 0x{transformed:06X}")
    print()