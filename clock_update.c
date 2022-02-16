#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "clock.h"

/*
Translates a number of seconds into hours, minutes and seconds and stores it in tod
Only happens if total seconds are within the acceptable range (0 - 86400)
returns 1 if outside range, returns 0 if function is succesful
*/
int set_tod_from_secs(int time_of_day_sec, tod_t *tod)
{
    //Verifies that the seconds are within bounds
    if (time_of_day_sec < 0 || time_of_day_sec >= 86400){
        return 1;
    }

    //Checks whther seconds represent am or pm
    //sets the ampm field of tod to its corresponding number. Start seconds at midnight or noon
    if (time_of_day_sec < 43200){
        tod->ampm = 1;
    }
    else {
        tod->ampm = 2;
        time_of_day_sec = time_of_day_sec - 43200;
    }
    //Breaks down seconds intoo hours minutes and seconds and fills in corresponding fields of tod
    tod->hours = time_of_day_sec / 3600;
    //Cannot have hours be 0, makes it 12
    if (tod->hours == 0){
        tod->hours = 12;
    }

    //Breaks down the seconds into minutes:seconds
    time_of_day_sec = time_of_day_sec % 3600;
    tod->minutes = time_of_day_sec / 60;
    time_of_day_sec = time_of_day_sec % 60;
    tod->seconds = time_of_day_sec;
    return 0;
}

//fills the integer display with bits representing the bits to display the appropriate time of day
//returns 1 if the function was unsuccesful/time is out of bounds, returns 0 if function is succesful
int set_display_from_tod(tod_t tod, int *display)
{
    //begins display at 0 across all bits
    *display = 0x0000000;
    //Helper function, takes a single digit integer and translates it into the bits to display said number
    int help_translate_display(int number){
        if (number == 0){
            return 0b0111111;
        }
        else if (number == 1){
            return 0b0110000;
        }
        else if (number == 2){
            return 0b1101101;
        }
        else if (number == 3){
            return 0b1111001;
        }
        else if (number == 4){
            return 0b1110010;
        }
        else if (number == 5){
            return 0b1011011;
        }
        else if (number == 6){
            return 0b1011111;
        }
        else if (number == 7){
            return 0b0110001;
        }
        else if (number == 8){
            return 0b1111111;
        }
        else if (number == 9){
            return 0b1111011;
        }
        return 0;
    }
    
    //checks bounds of each time value
    if ((tod.hours < 0 || tod.minutes < 0 || tod.seconds < 0) || (tod.hours > 23 || tod.minutes > 59 || tod.seconds > 59) || (tod.ampm != 1 && tod.ampm != 2)){
        return 1;
    }
    //uses the helper functions and shift bits to set all bits to their place
    *display = *display | (help_translate_display(tod.minutes % 10));
    *display = *display | (help_translate_display(tod.minutes / 10) << 7);
    *display = *display | (help_translate_display(tod.hours % 10) << 14);
    //Only sets the 2nd digit of hours if it is not 0
    if (help_translate_display(tod.hours / 10) != 0b0111111){
        *display = *display | (help_translate_display(tod.hours / 10) << 21);
    }
    if (tod.ampm == 1){
        *display = *display | (1 << 28);
    }
    if (tod.ampm == 2){
        *display = *display | (1 << 29);
    }
    return 0;
}


//Updates clock using two previous function
//returns 0 upon success and 1 upon failure
int clock_update(){
    //initialize a time of day struct
    tod_t today;
    //determine current time using total second global variable
    if(set_tod_from_secs(TIME_OF_DAY_SEC, &today)){
        return 1;
    }
    //finds bits for current time and sets it into the global display variable
    if(set_display_from_tod(today, &CLOCK_DISPLAY_PORT)){
        return 1;
    }
    return 0;
}
