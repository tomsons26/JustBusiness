#include "board.h"

#include "tempcode.h"

BoardLocationClass::BoardLocationClass(LocationType internal_id_num, unsigned init_x1, unsigned init_y1, unsigned init_x2,
    unsigned init_y2, LocExitDirType loc_exit_dir, BoardSpotType loc_type)
{
    internal_id = internal_id_num;
    x1 = init_x1;
    y1 = init_y1;
    x2 = init_x2;
    y2 = init_y2;
    location_exit_dir = loc_exit_dir;
    location_type = loc_type;
}