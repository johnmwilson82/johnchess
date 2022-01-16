import numpy as np

def knight_moves_from_square(sq):
    x = int(sq / 8)
    y = int(sq % 8)

    attacks = [np.array((x, y)) + np.array(mv) for mv in 
        [(1, 2), (2, 1), (-1, 2), (-2, 1), (1, -2), (2, -1), (-1, -2), (-2, -1)]]
    
    legal_attacks = [np.uint64(1) << np.uint64(ax * 8 + ay) for (ax, ay) in attacks if((ax >= 0) and (ax < 8) and (ay >= 0) and (ay < 8))]

    return np.sum(legal_attacks)

for i in range(8):
    print(" ".join(["0x%016x," % knight_moves_from_square(j) for j in range(i*8, (i+1)*8)]))