#pragma once

namespace MyChemistry {
    typedef struct BaseProperty
    {
        // basic property
        int temperature;
        bool isMetal;
        bool isWet;
        bool isFreeze;
        bool isOnfire;
    } BaseProperty;

}