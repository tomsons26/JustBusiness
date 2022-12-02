#include "vqaplay.h"
#include <cstring>

VQAConfig defaultconfig = {
    NULL,
    19,
    -1,
    NULL,
    320,
    200,
    -1,
    -1,
    -1,
    -1,
    -1,
    0,
    1,
    6,
    3,
    NULL,
    NULL,
    32768,
    -1,
    255,
    2048,
    -1,
    -1,
    -1,
    -1,
    -1,
};

// done
void VQA_DefaultConfig(VQAConfig *config)
{
    memcpy(config, &defaultconfig, sizeof(VQAConfig));
}
