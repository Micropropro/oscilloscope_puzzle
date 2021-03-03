#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "hardware/adc.h"

#define Morse_Pin 1 //Pin sends morse signal to be read
#define Reach_Two_Pin 28 //Pin to be pulled high
#define Reach_Four_Pin 19 //Pin to be pulled high
#define LED_PIN 25 

//----------GLOBAL VARIABLE SETUPS----------//
static int state = 1; //Variable indicates current state of the switch statement 
static uint32_t start_time = 0; //Timekeeping Variable

const float conversion_factor = 3.3f / (1 << 12); //conversion factor from voltage to numeric 
uint16_t result = 0;
float temp = 0;

//----------FUNCTION SETUPS----------//
//Morse Code functions. Barbaric, yet simple
int dot(){ 
  gpio_put(Morse_Pin, 1);
  sleep_ms(1);
  gpio_put(Morse_Pin, 0);
  sleep_ms(1);
}

int dash(){ //Will blink LED for 3x period of time to indicate a dash
  gpio_put(Morse_Pin, 1);
  sleep_ms(3);
  gpio_put(Morse_Pin, 0);
  sleep_ms(1);
}

void Write_Morse(){ //Even more barbaric. Sends message by pinging the hard coded dots and dashes
    //Current message: "SERLCNSL" (Serial Console)
    //S
    dot();
    dot();
    dot();
    //E
    dot();  
    //R
    dot();
    dash();
    dot();
    //L
    dot();
    dash();
    dot();
    dot();

    //C
    dash();
    dot();
    dash();
    dot();
    //N
    dash();
    dot();
    //S
    dot();
    dot();
    dot();
    //L
    dot();
    dash();
    dot();
    dot();
    sleep_ms(800);
}

//Blink LED depending on current state of switch statement
void Blink_LED_1() { //State 1
    gpio_put(LED_PIN, 1); 
    sleep_ms(100); //Light on for this amount of time
    gpio_put(LED_PIN, 0);
}

void Blink_LED_2() { //State 2
    gpio_put(LED_PIN, 1); 
    sleep_ms(100); //Light on for this amount of time
    gpio_put(LED_PIN, 0);
    sleep_ms(100); //Light off for this amount of time
    gpio_put(LED_PIN, 1); 
    sleep_ms(100);
    gpio_put(LED_PIN, 0);
}

void Blink_LED_3() { //State 3
    gpio_put(LED_PIN, 1); 
    sleep_ms(100); //Light on for this amount of time
    gpio_put(LED_PIN, 0);
    sleep_ms(100); //Light off for this amount of time
    gpio_put(LED_PIN, 1); 
    sleep_ms(100);
    gpio_put(LED_PIN, 0); 
    sleep_ms(100);
    gpio_put(LED_PIN, 1); 
    sleep_ms(100);
    gpio_put(LED_PIN, 0);
}

void Blink_LED_4() { //State 4
    gpio_put(LED_PIN, 1); 
    sleep_ms(100); //Light on for this amount of time
    gpio_put(LED_PIN, 0);
}

void Update_State(){
    uint32_t curr_time = to_ms_since_boot(get_absolute_time()); - start_time; //Calculate time since program started
    result = adc_read(); //RP2040 Chip temperature reading
    temp = (((result * conversion_factor)-0.076)/0.001721)-290; //False conversion from Voltage to Celsius (heating up Pico decreases the values read)
    
    if (gpio_get(Reach_Two_Pin) == 1) { //"Reach_Two_Pin" must be pulled HIGH to enter state 2
        state = 2;

    } else if (temp <= 6) { //Heating up the Pico will allow entry into state 3
        state = 3;

    } else if(gpio_get(Reach_Four_Pin) == 1 && curr_time >= 1800000 && curr_time <= 2019000){ //Time-gated. Will only enter final state when pin 19 pulled HIGH within 30-30.65 minutes of the program starting
        state = 4;
    
    } else{
        ;
    }
    
    switch (state)
    {
    case 1: //Initial state for the code. 
        Blink_LED_1();
        Write_Morse();
        puts("\nNice, you figured out what the signal read! Here's your clue to enter Stage 2:\n");
        puts("    'The MCU is pretty great. Lots of movies, new shows, and plenty of one-off villains. Endgame also made a TON of $$$$. \n    Figure out how much BO money it made, and do something to that corresponding pin' \n    (Hint:'HIGH-er than Avatar')");
        sleep_ms(800);
        break;
    
    case 2: //Reached when pulling pin 28 HIGH
        Blink_LED_2();
        puts("\nAlright, You made it to State 2! Not bad. All it took was finding (and maybe rounding up) that box office value to 2.8 Billion");
        puts("\nHere's your next clue:  \n    Not too hot, not too cold, this is the classic Goldilocks tale involving temperature. Let's INVERT that and instead 'cool down' the Pico \n    (Hint: Maybe make sure you don't have any wires from Stage 1 still plugged in)");
        sleep_ms(700);
        break;

    case 3: //Reached by heating up the Pico. (FIRE!)
        Blink_LED_3();
        puts("\nJust one more left to go! Let things 'warm up' before trying this last one. This one's my favorite:");
        puts("\n    Doing things on time is common, doing things WITHIN a certain range of time may be a little less so");
        puts("\n    PULL the Covid-XX Pin within this time range to enter the Final Stage");
        puts("\n    Try sooner than Tufts' founding year, it won't work. Take longer than the year Covid started, you'll have to restart. The years represent seconds" );
        sleep_ms(500);
        break; //Basically, pull pin 19 within the time period of 30 minutes and 33.65 minutes (1852 seconds and 2019 seconds) 

    case 4: //Reached when pulling gpio 19 HIGH within 30-33.65 minutes of booting up the Pico. Was thinking of implementing a $15 paywall instead, but decided I'm not a monster. #OpenSource&Free 
        Blink_LED_4();
        puts("\nWooo you've done it! You've made it to the end! Well done. Here's the name of the rando who put you through all this:");
        puts("\n      40202862468ed7db6bd255763dcde6fdf0ca9dbe64ad7a3c0383bb7903ae440c");
        sleep_ms(100);
        break;

    default:
        break;
    }
}

int main(){
    //Pin setups
    gpio_init(LED_PIN);
    gpio_init(Morse_Pin); //Initilizing gpio
    gpio_init(Reach_Two_Pin);
    gpio_init(Reach_Four_Pin);

    gpio_set_dir(LED_PIN, GPIO_OUT); //Setting corresponding out/inputs
    gpio_set_dir(Morse_Pin, GPIO_OUT); 
    gpio_set_dir(Reach_Two_Pin, GPIO_IN);
    gpio_set_dir(Reach_Four_Pin, GPIO_IN);

    gpio_pull_down(Morse_Pin); //Pulling down to false readings 
    gpio_pull_down(Reach_Two_Pin);
    gpio_pull_down(Reach_Four_Pin); //may already be pulled down anyways

    stdio_init_all();

    //Initializing adc temperature readings and conversions
    adc_init();
    adc_gpio_init(26);
    adc_select_input(4); //Value to indicate Temperature sensor is desired
    
    start_time = to_ms_since_boot(get_absolute_time()); //"Starting time" for the program

    while (1){
        Update_State();
    }
}
