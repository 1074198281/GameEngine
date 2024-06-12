#pragma once

namespace MyChemistry {
    typedef struct BaseProperty
    {
        // basic property
        int temperature;
        bool canFreeze;
        bool canBurn;
        bool isMetal;
        bool isWet;
        bool isFreeze;
        bool isOnfire;
    } BaseProperty;

}