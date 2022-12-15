#pragma once
#include <q3.h>

//YES. A CLASS WAS NEEDED BY THE PHYSICS LIBRARY.
//FOR THIS. 21 LINES OF CODE.
//THIS. IS. SO. UNNECESSARY.
class CollisionReport : public q3QueryCallback
{
public:
    int i;
    
    //Called if a block is colliding with another
    bool ReportShape( q3Box *shape ) override
    {
        //Counts how many objects the current
        //object is colliding with.
        i++;
        return true;
    }
};