#ifndef __STADIUM_H
#define __STADIUM_H

#define STADIUM_MARKINGLINE_WIDTH			0.2
#define STADIUM_FIELD_LENGTH				100.0
#define STADIUM_HALF_FIELD_LENGTH			STADIUM_FIELD_LENGTH / 2.0
#define STADIUM_FIELD_WIDTH					64.0
#define STADIUM_HALF_FIELD_WIDTH			STADIUM_FIELD_WIDTH / 2.0
#define STADIUM_CIRCLE_RADIUS				9.15
#define STADIUM_GOAL_LENGTH					7.32
#define STADIUM_HALF_GOAL_LENGTH			STADIUM_GOAL_LENGTH / 2.0
#define STADIUM_GOAL_HEIGHT					2.44
#define STADIUM_GOAL_UPPER_DEPTH			2.0
#define STADIUM_GOAL_LOWER_DEPTH			3.0
#define STADIUM_PENALTY_AREA_WIDTH			16.5
#define STADIUM_PENALTY_AREA_LENGTH			(STADIUM_PENALTY_AREA_WIDTH * 2.0 + STADIUM_GOAL_LENGTH)
#define STADIUM_HALF_PENALTY_AREA_LENGTH	STADIUM_PENALTY_AREA_LENGTH / 2.0
#define STADIUM_PENALTY_DISTANCE			11.0
#define STADIUM_GOAL_AREA_WIDTH				5.5
#define STADIUM_GOAL_AREA_LENGTH			(STADIUM_GOAL_AREA_WIDTH * 2.0 + STADIUM_GOAL_LENGTH)
#define STADIUM_HALF_GOAL_AREA_LENGTH		STADIUM_GOAL_AREA_LENGTH / 2.0
#define STADIUM_FIELD_TRACK_SPACING			1.0
#define STADIUM_CORNER_RADIUS				1.0
#define STADIUM_GOAL_NET_SIZE				0.25

void stadium_init();
void stadium_draw(double timeDiff);
void stadium_drawFieldMarkingLines();
void stadium_drawTrackMarkingLines();
void stadium_drawFieldGround();
void stadium_drawTrackGround();
void stadium_drawCurveStands();
void stadium_drawRoof();
void stadium_drawGrandStands();
void stadium_drawGoal();

#endif

