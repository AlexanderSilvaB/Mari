#include "GameController.h"

GameController::GameController(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
    pressed = false;
    team_red = false;
    lastState = STATE_INVALID;
    myLastPenalty = PENALTY_NONE;
    actOnWhistle = true;
}

void GameController::OnStart()
{
    spellBook->behaviour.Started = true;
}

bool GameController::whistleHeard(int numSeconds)
{
    return false;
}

void GameController::Tick(float ellapsedTime, const SensorValues &sensor)
{
    Blackboard *blackboard = InitManager::GetBlackboard();

    uint8_t previousGameState = readFrom(gameController, gameState);
    data = readFrom(gameController, data);
    teamNumber = readFrom(gameController, our_team).teamNumber;
    playerNumber = readFrom(gameController, player_number);

    if(sensor.sensors[Sensors::ChestBoard_Button] > 0)
    {
        pressed = true;
    }
    else if(pressed)
    {
        pressed = false;
        switch(data.state)
        {
            case STATE_INITIAL:
            case STATE_PLAYING:
                data.state = STATE_PENALISED;
                data.teams[team_red].players[playerNumber - 1].penalty = PENALTY_MANUAL;
                break;
            default:
                data.state = STATE_PLAYING;
                data.teams[team_red].players[playerNumber - 1].penalty = PENALTY_NONE;
        }
    }

    spellBook->strategy.Started = data.state != STATE_INITIAL;
    spellBook->strategy.Penalized = data.state == STATE_PENALISED;
    spellBook->behaviour.Penalized = spellBook->strategy.Penalized;

    // make our_team point to the my actual team, based on teamNumber
    TeamInfo *our_team = NULL;
    if (data.teams[TEAM_BLUE].teamNumber == teamNumber) 
    {
        our_team = &(data.teams[TEAM_BLUE]);
        team_red = false;
    } 
    else if (data.teams[TEAM_RED].teamNumber == teamNumber) 
    {
        our_team = &(data.teams[TEAM_RED]);
        team_red = true;
    }

    uint8_t gameState = data.state;


    // If we previously heard a whistle and changed our gameState to playing
    // Then don't let it get overriden by the game controller
    if (gameState == STATE_SET && previousGameState == STATE_PLAYING) 
    {
        gameState = previousGameState;
    }

    // Heard whistles - if we are in SET and whistle heard in last 3 seconds
    if (gameState == STATE_SET && whistleHeard(3)) 
    {
        if (actOnWhistle == true) 
        {
            gameState = STATE_PLAYING;
            cout << "WHISTLE HEARD, TELLING TEAM MATES" << endl;
            SAY("Whistle heard");
        }
        else 
        {
            SAY("Heard whistle not moving");
        }
    }

    // Check the team opinion on whether a whistle has been heard
   if (data.state == STATE_SET) 
   {
        float numTeammatesPlaying = 0;
        float numActiveTeammates = 0;
        for (int i = 0; i < ROBOTS_PER_TEAM; ++i) 
        {
            if (!readFrom(receiver, incapacitated)[i]) 
            {
                ++numActiveTeammates;
            if (readFrom(receiver, data)[i].gameState == STATE_PLAYING) 
            {
                ++numTeammatesPlaying;
            }
        }
    }
    cout << "    Whistle: Heard / Active Players = " << numTeammatesPlaying
           << " / " << numActiveTeammates << endl;

    AbsCoord robot_pos = readFrom(localisation, robotPos);
    bool nearCenterCircle = abs(robot_pos.x()) < 1500 && abs(robot_pos.y()) < 3000;

    if (numActiveTeammates > 0) 
    {
        // If enough of the team thinks we should play,
        // and we're localised and close to center circle, lets play
        float ratio = numTeammatesPlaying / numActiveTeammates;
        if (ratio >= 0.30 && nearCenterCircle && actOnWhistle) 
        {
            data.state = STATE_PLAYING;
            cout << "WHISTLE HEARD BY TEAMMATES AND NEAR CENTER CIRCLE, ENTERING STATE PLAYING YAY" << endl;
            SAY("Whistle heard by teammates");
         }
      }
   }

   writeTo(gameController, data, data);
   writeTo(gameController, our_team, *our_team);
   writeTo(gameController, team_red, team_red);
   writeTo(gameController, gameState, data.state);

   // In the case where we've heard a whistle, but haven't decided to play yet
   // We want to keep the official data as set, but tell the team we think its play time
   // So override our gameState variable, thus making gameState != data.state
    if (gameState == STATE_PLAYING && data.state == STATE_SET) 
    {
        writeTo(gameController, gameState, gameState);
    }
}

void GameController::OnStop()
{
    
}

GameController::~GameController()
{
    
}