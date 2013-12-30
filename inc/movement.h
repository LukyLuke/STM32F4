#ifndef MOVEMENT_H
#define MOVEMENT_H

// Move in xyz-axis; negative values means backward
void move_x(const i8 value);
void move_y(const i8 value);
void move_z(const i8 value);

// special moves; negative values means backward
void rotate(const i8 value);

#endif // MOVEMENT_H