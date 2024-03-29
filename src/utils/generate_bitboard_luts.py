import numpy as np

def knight_moves_from_square(sq):
    x = int(sq % 8)
    y = int(sq / 8)

    attacks = [np.array((x, y)) + np.array(mv) for mv in 
        [(1, 2), (2, 1), (-1, 2), (-2, 1), (1, -2), (2, -1), (-1, -2), (-2, -1)]]
    
    legal_attacks = [np.uint64(1) << np.uint64(ax + ay * 8) for (ax, ay) in attacks if((ax >= 0) and (ax < 8) and (ay >= 0) and (ay < 8))]

    return np.sum(legal_attacks)


def king_moves_from_square(sq):
    x = int(sq % 8)
    y = int(sq / 8)

    attacks = [np.array((x, y)) + np.array(mv) for mv in 
        [(1, 1), (1, 0), (1, -1), (0, 1), (0, -1), (-1, 1), (-1, 0), (-1, -1)]]
    
    legal_attacks = [np.uint64(1) << np.uint64(ax + ay * 8) for (ax, ay) in attacks if((ax >= 0) and (ax < 8) and (ay >= 0) and (ay < 8))]

    return np.sum(legal_attacks)

def ray_attacks_from_square(sq, dir):
    x = int(sq % 8)
    y = int(sq / 8)

    attacks = [np.array((x, y)) + a * np.array(dir) for a in range(1,8)]

    legal_attacks = [np.uint64(1) << np.uint64(ax + ay * 8) for (ax, ay) in attacks if((ax >= 0) and (ax < 8) and (ay >= 0) and (ay < 8))]

    return int(np.sum(legal_attacks))

"""print("knight moves")
for i in range(8):
    print(" ".join(["0x%016x," % knight_moves_from_square(j) for j in range(i*8, (i+1)*8)]))
print("====================================================")
    
print("king moves")
for i in range(8):
    print(" ".join(["0x%016x," % king_moves_from_square(j) for j in range(i*8, (i+1)*8)]))
print("====================================================")

print("ray moves")
for dir in [(-1, -1), (0, -1), (1, -1), (-1, 0), (1, 0), (1, 1), (0, 1), (-1, 1)]:
    print(dir)
    for i in range(8):
        print(" ".join(["0x%016x," % ray_attacks_from_square(j, dir) for j in range(i*8, (i+1)*8)]))
    print("====================================================")"""

dir = (-1, 0)

for i in range(8):
    print(" ".join(["0x%016x," % ray_attacks_from_square(j, dir) for j in range(i*8, (i+1)*8)]))

def calc_west_ray_attacks(sq):
    return (0x7fffffffffffffff >> (63 - sq)) - (((2 << (sq | 7)) - 1) >> 8)

print("..." + "%x" %((0xff << 10) & ~((2 << (10 | 7)) - 1)))
for i in range(8):
    print(" ".join(["0x%016x," % calc_west_ray_attacks(j) for j in range(i*8, (i+1)*8)]))
