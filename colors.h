#ifndef COLORS_H
#define COLORS_H

extern const char *random_colors[];
#define random_color() random_colors[std::rand() % 164];

#endif
