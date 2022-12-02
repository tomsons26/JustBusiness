#include "defines.h"

class BoardLocationClass
{
public:
    __cdecl BoardLocationClass(LocationType internal_id_num, unsigned int init_x1, unsigned int init_y1,
        unsigned int init_x2, unsigned int init_y2, LocExitDirType loc_exit_dir, BoardSpotType loc_type);

private:
    LocationType internal_id;
    unsigned int x1;
    unsigned int y1;
    unsigned int x2;
    unsigned int y2;
    LocExitDirType location_exit_dir;
    BoardSpotType location_type;
};
