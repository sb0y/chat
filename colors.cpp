#include "colors.h"

/*struct ColorX {
    const char *c;
    int r;
    int g;
    int b;
    const char *n;
};*/

const char *random_colors[] = {
    "\xCD\x5C\x5C", // {}  205, 92, 92   ,"IndianRed" },
    "\xF0\x80\x80", // {}  240, 128, 128 ,"LightCoral" },
    "\xFA\x80\x72", // {}  250, 128, 114 ,"Salmon" },
    "\xE9\x96\x7A", // {}  233, 150, 122 ,"DarkSalmon" },
    "\xFF\xA0\x7A", // {}  255, 160, 122 ,"LightSalmon" },
    "\xDC\x14\x3C", // {}  220, 20, 60   ,"Crimson" },
    "\xFF\x00\x00", // {}  255, 0, 0     ,"Red" },
    "\xB2\x22\x22", // {}  178, 34, 34   ,"FireBrick" },
    "\x8B\x00\x00", // {}  139, 0, 0     ,"DarkRed" },
    "\xFF\xC0\xCB", // {}  255, 192, 203 ,"Pink" },
    "\xFF\xB6\xC1", // {}  255, 182, 193 ,"LightPink" },
    "\xFF\x69\xB4", // {}  255, 105, 180 ,"HotPink" },
    "\xFF\x14\x93", // {}  255, 20, 147  ,"DeepPink" },
    "\xC7\x15\x85", // {}  199, 21, 133  ,"MediumVioletRed" },
    "\xDB\x70\x93", // {}  219, 112, 147 ,"PaleVioletRed" },
    "\xFF\xA0\x7A", // {}  255, 160, 122 ,"LightSalmon" },
    "\xFF\x7F\x50", // {}  255, 127, 80  ,"Coral" },
    "\xFF\x63\x47", // {}  255, 99, 71   ,"Tomato" },
    "\xFF\x45\x00", // {}  255, 69, 0    ,"OrangeRed" },
    "\xFF\x8C\x00", // {}  255, 140, 0   ,"DarkOrange" },
    "\xFF\xA5\x00", // {}  255, 165, 0   ,"Orange" },
    "\xFF\xD7\x00", // {}  255, 215, 0   ,"Gold" },
    "\xFF\xFF\x00", // {}  255, 255, 0   ,"Yellow" },
    "\xFF\xFF\xE0", // {}  255, 255, 224 ,"LightYellow" },
    "\xFF\xFA\xCD", // {}  255, 250, 205 ,"LemonChiffon" },
    "\xFA\xFA\xD2", // {}  250, 250, 210 ,"LightGoldenrodYellow" },
    "\xFF\xEF\xD5", // {}  255, 239, 213 ,"PapayaWhip" },
    "\xFF\xE4\xB5", // {}  255, 228, 181 ,"Moccasin" },
    "\xFF\xDA\xB9", // {}  255, 218, 185 ,"PeachPuff" },
    "\xEE\xE8\xAA", // {}  238, 232, 170 ,"PaleGoldenrod" },
    "\xF0\xE6\x8C", // {}  240, 230, 140 ,"Khaki" },
    "\xBD\xB7\x6B", // {}  189, 183, 107 ,"DarkKhaki" },
    "\xE6\xE6\xFA", // {}  230, 230, 250 ,"Lavender" },
    "\xD8\xBF\xD8", // {}  216, 191, 216 ,"Thistle" },
    "\xDD\xA0\xDD", // {}  221, 160, 221 ,"Plum" },
    "\xEE\x82\xEE", // {}  238, 130, 238 ,"Violet" },
    "\xDA\x70\xD6", // {}  218, 112, 214 ,"Orchid" },
    "\xFF\x00\xFF", // {}  255, 0, 255   ,"Fuchsia" },
    "\xFF\x00\xFF", // {}  255, 0, 255   ,"Magenta" },
    "\xBA\x55\xD3", // {}  186, 85, 211  ,"MediumOrchid" },
    "\x93\x70\xDB", // {}  147, 112, 219 ,"MediumPurple" },
    "\x8A\x2B\xE2", // {}  138, 43, 226  ,"BlueViolet" },
    "\x94\x00\xD3", // {}  148, 0, 211   ,"DarkViolet" },
    "\x99\x32\xCC", // {}  153, 50, 204  ,"DarkOrchid" },
    "\x8B\x00\x8B", // {}  139, 0, 139   ,"DarkMagenta" },
    "\x80\x00\x80", // {}  128, 0, 128   ,"Purple" },
    "\x4B\x00\x82", // {}  75, 0, 130    ,"Indigo" },
    "\x6A\x5A\xCD", // {}  106, 90, 205  ,"SlateBlue" },
    "\x48\x3D\x8B", // {}  72, 61, 139   ,"DarkSlateBlue" },
    "\xFF\xF8\xDC", // {}  255, 248, 220 ,"Cornsilk" },
    "\xFF\xEB\xCD", // {}  255, 235, 205 ,"BlanchedAlmond" },
    "\xFF\xE4\xC4", // {}  255, 228, 196 ,"Bisque" },
    "\xFF\xDE\xAD", // {}  255, 222, 173 ,"NavajoWhite" },
    "\xF5\xDE\xB3", // {}  245, 222, 179 ,"Wheat" },
    "\xDE\xB8\x87", // {}  222, 184, 135 ,"BurlyWood" },
    "\xD2\xB4\x8C", // {}  210, 180, 140 ,"Tan" },
    "\xBC\x8F\x8F", // {}  188, 143, 143 ,"RosyBrown" },
    "\xF4\xA4\x60", // {}  244, 164, 96  ,"SandyBrown" },
    "\xDA\xA5\x20", // {}  218, 165, 32  ,"Goldenrod" },
    "\xB8\x86\x0B", // {}  184, 134, 11  ,"DarkGoldenRod" },
    "\xCD\x85\x3F", // {}  205, 133, 63  ,"Peru" },
    "\xD2\x69\x1E", // {}  210, 105, 30  ,"Chocolate" },
    "\x8B\x45\x13", // {}  139, 69, 19   ,"SaddleBrown" },
    "\xA0\x52\x2D", // {}  160, 82, 45   ,"Sienna" },
    "\xA5\x2A\x2A", // {}  165, 42, 42   ,"Brown" },
    "\x80\x00\x00", // {}  128, 0, 0     ,"Maroon" },
    "\x00\x00\x00", // {}  0, 0, 0       ,"Black" },
    "\x80\x80\x80", // {}  128, 128, 128 ,"Gray" },
    "\xC0\xC0\xC0", // {}  192, 192, 192 ,"Silver" },
    "\xFF\xFF\xFF", // {}  255, 255, 255 ,"White" },
    "\xFF\x00\xFF", // {}  255, 0, 255   ,"Fuchsia" },
    "\x80\x00\x80", // {}  128, 0, 128   ,"Purple" },
    "\xFF\x00\x00", // {}  255, 0, 0     ,"Red" },
    "\x80\x00\x00", // {}  128, 0, 0     ,"Maroon" },
    "\xFF\xFF\x00", // {}  255, 255, 0   ,"Yellow" },
    "\x80\x80\x00", // {}  128, 128, 0   ,"Olive" },
    "\x00\xFF\x00", // {}  0, 255, 0     ,"Lime" },
    "\x00\x80\x00", // {}  0, 128, 0     ,"Green" },
    "\x00\xFF\xFF", // {}  0, 255, 255   ,"Aqua" },
    "\x00\x80\x80", // {}  0, 128, 128   ,"Teal" },
    "\x00\x00\xFF", // {}  0, 0, 255     ,"Blue" },
    "\x00\x00\x80", // {}  0, 0, 128     ,"Navy" },
    "\xAD\xFF\x2F", // {}  173, 255, 47  ,"GreenYellow" },
    "\x7F\xFF\x00", // {}  127, 255, 0   ,"Chartreuse" },
    "\x7C\xFC\x00", // {}  124, 252, 0   ,"LawnGreen" },
    "\x00\xFF\x00", // {}  0, 255, 0     ,"Lime" },
    "\x32\xCD\x32", // {}  50, 205, 50   ,"LimeGreen" },
    "\x98\xFB\x98", // {}  152, 251, 152 ,"PaleGreen" },
    "\x90\xEE\x90", // {}  144, 238, 144 ,"LightGreen" },
    "\x00\xFA\x9A", // {}  0, 250, 154   ,"MediumSpringGreen" },
    "\x00\xFF\x7F", // {}  0, 255, 127   ,"SpringGreen" },
    "\x3C\xB3\x71", // {}  60, 179, 113  ,"MediumSeaGreen" },
    "\x2E\x8B\x57", // {}  46, 139, 87   ,"SeaGreen" },
    "\x22\x8B\x22", // {}  34, 139, 34   ,"ForestGreen" },
    "\x00\x80\x00", // {}  0, 128, 0     ,"Green" },
    "\x00\x64\x00", // {}  0, 100, 0     ,"DarkGreen" },
    "\x9A\xCD\x32", // {}  154, 205, 50  ,"YellowGreen" },
    "\x6B\x8E\x23", // {}  107, 142, 35  ,"OliveDrab" },
    "\x80\x80\x00", // {}  128, 128, 0   ,"Olive" },
    "\x55\x6B\x2F", // {}  85, 107, 47   ,"DarkOliveGreen" },
    "\x66\xCD\xAA", // {}  102, 205, 170 ,"MediumAquamarine" },
    "\x8F\xBC\x8F", // {}  143, 188, 143 ,"DarkSeaGreen" },
    "\x20\xB2\xAA", // {}  32, 178, 170  ,"LightSeaGreen" },
    "\x00\x8B\x8B", // {}  0, 139, 139   ,"DarkCyan" },
    "\x00\x80\x80", // {}  0, 128, 128   ,"Teal" },
    "\x00\xFF\xFF", // {}  0, 255, 255   ,"Aqua" },
    "\x00\xFF\xFF", // {}  0, 255, 255   ,"Cyan" },
    "\xE0\xFF\xFF", // {}  224, 255, 255 ,"LightCyan" },
    "\xAF\xEE\xEE", // {}  175, 238, 238 ,"PaleTurquoise" },
    "\x7F\xFF\xD4", // {}  127, 255, 212 ,"Aquamarine" },
    "\x40\xE0\xD0", // {}  64, 224, 208  ,"Turquoise" },
    "\x48\xD1\xCC", // {}  72, 209, 204  ,"MediumTurquoise" },
    "\x00\xCE\xD1", // {}  0, 206, 209   ,"DarkTurquoise" },
    "\x5F\x9E\xA0", // {}  95, 158, 160  ,"CadetBlue" },
    "\x46\x82\xB4", // {}  70, 130, 180  ,"SteelBlue" },
    "\xB0\xC4\xDE", // {}  176, 196, 222 ,"LightSteelBlue" },
    "\xB0\xE0\xE6", // {}  176, 224, 230 ,"PowderBlue" },
    "\xAD\xD8\xE6", // {}  173, 216, 230 ,"LightBlue" },
    "\x87\xCE\xEB", // {}  135, 206, 235 ,"SkyBlue" },
    "\x87\xCE\xFA", // {}  135, 206, 250 ,"LightSkyBlue" },
    "\x00\xBF\xFF", // {}  0, 191, 255   ,"DeepSkyBlue" },
    "\x1E\x90\xFF", // {}  30, 144, 255  ,"DodgerBlue" },
    "\x64\x95\xED", // {}  100, 149, 237 ,"CornflowerBlue" },
    "\x7B\x68\xEE", // {}  123, 104, 238 ,"MediumSlateBlue" },
    "\x41\x69\xE1", // {}  65, 105, 225  ,"RoyalBlue" },
    "\x00\x00\xFF", // {}  0, 0, 255     ,"Blue" },
    "\x00\x00\xCD", // {}  0, 0, 205     ,"MediumBlue" },
    "\x00\x00\x8B", // {}  0, 0, 139     ,"DarkBlue" },
    "\x00\x00\x80", // {}  0, 0, 128     ,"Navy" },
    "\x19\x19\x70", // {}  25, 25, 112   ,"MidnightBlue" },
    "\xFF\xFF\xFF", // {}  255, 255, 255 ,"White" },
    "\xFF\xFA\xFA", // {}  255, 250, 250 ,"Snow" },
    "\xF0\xFF\xF0", // {}  240, 255, 240 ,"Honeydew" },
    "\xF5\xFF\xFA", // {}  245, 255, 250 ,"MintCream" },
    "\xF0\xFF\xFF", // {}  240, 255, 255 ,"Azure" },
    "\xF0\xF8\xFF", // {}  240, 248, 255 ,"AliceBlue" },
    "\xF8\xF8\xFF", // {}  248, 248, 255 ,"GhostWhite" },
    "\xF5\xF5\xF5", // {}  245, 245, 245 ,"WhiteSmoke" },
    "\xFF\xF5\xEE", // {}  255, 245, 238 ,"Seashell" },
    "\xF5\xF5\xDC", // {}  245, 245, 220 ,"Beige" },
    "\xFD\xF5\xE6", // {}  253, 245, 230 ,"OldLace" },
    "\xFF\xFA\xF0", // {}  255, 250, 240 ,"FloralWhite" },
    "\xFF\xFF\xF0", // {}  255, 255, 240 ,"Ivory" },
    "\xFA\xEB\xD7", // {}  250, 235, 215 ,"AntiqueWhite" },
    "\xFA\xF0\xE6", // {}  250, 240, 230 ,"Linen" },
    "\xFF\xF0\xF5", // {}  255, 240, 245 ,"LavenderBlush" },
    "\xFF\xE4\xE1", // {}  255, 228, 225 ,"MistyRose" },
    "\xDC\xDC\xDC", // {}  220, 220, 220 ,"Gainsboro" },
    "\xD3\xD3\xD3", // {}  211, 211, 211 ,"LightGrey" },
    "\xD3\xD3\xD3", // {}  211, 211, 211 ,"LightGray" },
    "\xC0\xC0\xC0", // {}  192, 192, 192 ,"Silver" },
    "\xA9\xA9\xA9", // {}  169, 169, 169 ,"DarkGray" },
    "\xA9\xA9\xA9", // {}  169, 169, 169 ,"DarkGrey" },
    "\x80\x80\x80", // {}  128, 128, 128 ,"Gray" },
    "\x80\x80\x80", // {}  128, 128, 128 ,"Grey" },
    "\x69\x69\x69", // {}  105, 105, 105 ,"DimGray" },
    "\x69\x69\x69", // {}  105, 105, 105 ,"DimGrey" },
    "\x77\x88\x99", // {}  119, 136, 153 ,"LightSlateGray" },
    "\x77\x88\x99", // {}  119, 136, 153 ,"LightSlateGrey" },
    "\x70\x80\x90", // {}  112, 128, 144 ,"SlateGray" },
    "\x70\x80\x90", // {}  112, 128, 144 ,"SlateGrey" },
    "\x2F\x4F\x4F", // {}  47, 79, 79    ,"DarkSlateGray" },
    "\x2F\x4F\x4F", // {}  47, 79, 79    ,"DarkSlateGrey" },
    "\x00\x00\x00"  // {}  0, 0, 0       ,"Black" }
};
