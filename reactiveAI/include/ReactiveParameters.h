//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __PPR_PARAMETERS_H__
#define __PPR_PARAMETERS_H__

/// @file PPRParameters.h
/// @brief Declares many constants used by PPRAgent.


// un-comment this to allow performance-costly but informative drawings on agents that you select with the mouse.
// note that this adds to the size of each agent, to store all information about its path plans.
//#define USE_ANNOTATIONS



/// @todo put INFINITY, M_PI, etc. macro definitions somewhere in steerlib so that they are accessible everywhere, and play nicely with math.h
#ifndef INFINITY
#define INFINITY FLT_MAX
#endif



// these are measured in meters/second
#define PED_MAX_SPEED                2.6f
#define PED_TYPICAL_SPEED            1.3f

// mass * meters / (second^2)
#define PED_MAX_FORCE                14.0f

// these are intended to be multiplied by "typical speed"
#define PED_MAX_SPEED_FACTOR             1.7f
#define PED_FASTER_SPEED_FACTOR          1.31f
#define PED_SLIGHTLY_FASTER_SPEED_FACTOR 1.15f
#define PED_TYPICAL_SPEED_FACTOR         1.0f
#define PED_SLIGHTLY_SLOWER_SPEED_FACTOR 0.77f
#define PED_SLOWER_SPEED_FACTOR          0.5f


// TODO: the exact values for turning rates may have to tweaked again 
#define PED_CORNERING_TURN_RATE 1.9f
#define PED_ADJUSTMENT_TURN_RATE 0.16f
#define PED_FASTER_AVOIDANCE_TURN_RATE 0.55f
#define PED_TYPICAL_AVOIDANCE_TURN_RATE 0.26f


// notice the braking rate is not negative, instead, just explicitly negate when using the macro to actually brake
#define PED_BRAKING_RATE    0.95f
#define PED_COMFORT_ZONE    1.5f
#define PED_QUERY_RADIUS    10.0f

// threshold for cosTheta dot product between normalized vectors that tell us whether two agents are facing almost the same direction
#define PED_SIMILAR_DIRECTION_DOT_PRODUCT_THRESHOLD 0.94f
#define PED_SAME_DIRECTION_DOT_PRODUCT_THRESHOLD 0.99f

// threshold for cosTheta dot product between normalized vectors that tell us whether two agents are facing opposite directions
#define PED_ONCOMING_PREDICTION_THRESHOLD -0.95f
#define PED_ONCOMING_REACTION_THRESHOLD -0.95f

// threshold for cosTheta dot product between targetdirection and current direction to tell us if we're going in a completely wrong direction
#define PED_WRONG_DIRECTION_DOT_PRODUCT_THRESHOLD 0.55f

// threshold that tells us how close the threat needs to be before we care about it
#define PED_THREAT_DISTANCE_THRESHOLD 8.0f

// threshold that tells us how long we need to continue caring about a particular threat.  one possibility is to relate the two thresholds by PED_TYPICAL_SPEED = PED_THREAT_DISTANCE_THRESHOLD / PED_THREAT_MAX_TIME_THRESHOLD.
// TODO: this threshold is not currently defined ???  note that it IS different than the two thresholds immediately below...


// threshold that tells us how soon/far in advance that the agent is capable of predicting threats.
//#define PED_THREAT_MIN_TIME_THRESHOLD 1.5f
#define PED_THREAT_MIN_TIME_THRESHOLD 0.8f
#define PED_THREAT_MAX_TIME_THRESHOLD 4.0f

// scaling factor to determine how "far" each agent looks ahead based on its current direction
#define PED_PREDICTIVE_ANTICIPATION_FACTOR  5.0f
#define PED_REACTIVE_ANTICIPATION_FACTOR 1.1f

// factor that determines the strength of cooperative crowd forces
#define PED_CROWD_INFLUENCE_FACTOR 0.3f

// threshold that tells us the maximum "difference" between a symmetric left ray and a right ray for which we should decide to slow down because we are facing the object.
#define PED_FACING_STATIC_OBJECT_THRESHOLD 0.3f

// three scaling factors that determine how strong the reactive steering forces are in the specific situations
// ** note these values are completely unrelated to each other, and should NOT be compared.
#define PED_ORDINARY_STEERING_STRENGTH 0.05f
#define PED_ONCOMING_THREAT_AVOIDANCE_STRENGTH 0.15f
#define PED_CROSS_THREAT_AVOIDANCE_STRENGTH 0.9f

// turning is a slight "hack" to avoid using sin/cos, instead just adds a fraction of the side() vector to the forward() vector, and then re-normalizes
// note this approach is not linear, but doesn't matter
// we use this turning rate so that the AI can use -1.0 to 1.0 for the "amount of turning".
// also note this assumes that side() and forward() are both normalized, which is probably a good idea anyway.
#define PED_MAX_TURNING_RATE 0.1f

// integer threshold that defines how many objects (only agents?) need to be classified as a "temporary crowd" before the agent begins to feel "crowded"... which alters steering behavior.
#define PED_FEELING_CROWDED_THRESHOLD 3

// factor that decides how strong the side-to-side scooting forces will be when an agent realizes it needs to scoot to the side.
#define PED_SCOOT_RATE 0.4f

// threshold distance for when to consider that a goal target is reached.
#define PED_REACHED_TARGET_DISTANCE_THRESHOLD 0.5f

// padding distance (in meters) that is added to collision queries, to give characters some natural space.
#define PED_DYNAMIC_COLLISION_PADDING 0.2f

// planning constants, used for waypoint management.
#define PED_FURTHEST_LOCAL_TARGET_DISTANCE 20
#define PED_NEXT_WAYPOINT_DISTANCE 256 // MUBBASIR this was 30 // Glen this was 60...
#define PED_MAX_NUM_WAYPOINTS 20



#endif
