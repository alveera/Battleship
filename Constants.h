const char placement_wait[]="Please wait while the other player places their ships.";
const char invalid_placement[]="Invalid placement of ship.";
const char status_ok[]="OK";
const char fire_wait[]="Other player's turn";
const char status_str [6][8] = {"HIT", "MISS", "SINK", "WIN", "INVALID", "LOSE"};
const short int EMPTY = 0;
const short int SHOT = 9;
const short int SUNK = 10;
const short int OCCUPIED [5] = {1, 2, 3, 4, 5};
const short int SHOT_MISS = 8;