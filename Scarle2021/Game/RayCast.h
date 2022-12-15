#pragma once
#include <q3.h>

//I may end up not using this.
class RayCast : public q3QueryCallback
{
public:
    //Raycast data
    q3RaycastData data;
    r32 tfinal;
    q3Vec3 nfinal;
    //Body we are checking collision with
    q3Body *impactBody;

    //Raycastable shapes
    bool ReportShape( q3Box *shape ) override
    {
        //Toi = Time Of Impact
        if ( data.toi < tfinal )
        {
            tfinal = data.toi;
            nfinal = data.normal;
            impactBody = shape->body;
        }

        data.toi = tfinal;
        return true;
    }

    void Location( const q3Vec3& spot, const q3Vec3& dir )
    {
        //Start point
        data.start = spot;
        //Direction the raycast is heading
        data.dir = q3Normalize( dir );

        //Initializing some raycast data
        data.t = r32( 10000.0 );
        tfinal = FLT_MAX;
        data.toi = data.t;
        impactBody = NULL;
    }
};