// [task_4.c]
// In this task, you'll learn how to use the allegro5 library to
// implement the event loop, reading the keyboard input. We'll also
// make use of a timer to update and redraw our display.
// You only need to enter codes below the 'TODO' comments.


#include<stdio.h>
#include<stdlib.h>
#include<allegro5/allegro.h>
#include<allegro5/allegro_primitives.h>
#include<allegro5/allegro_audio.h>
#include<allegro5/allegro_color.h>
#include<allegro5/allegro_ttf.h>
#include<allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>



// If defined, logs will be shown on console and written to file.
#define LOG_ENABLED

#define NORMAL_SPEED 1
#define SLOW_SPEED 0.4
#define HOLD_FPS 90
#define SEAL_BODY 60
enum STATUS{MENU, PLAYING, GG, WIN };
enum DIRECTION{L,R};
typedef struct {
    int x;
    int y;
    float speed ;
    char text[10];
    bool live;
    int body_x;
    int body_y;
}BadThing;

ALLEGRO_DISPLAY* game_display;
ALLEGRO_EVENT_QUEUE* game_event_queue;
// TODO: [Declare variables]
// Declare the variables that stores the timer.
ALLEGRO_TIMER* game_update_timer;
ALLEGRO_BITMAP* img;
ALLEGRO_FONT* font;
// Frame rate (frame per second)
const int FPS = 60;
// Define screen width and height as constants.
const int SCREEN_W = 1200;
const int SCREEN_H = 900;
// Keyboard state, whether the key is down or not.s
bool key_state[ALLEGRO_KEY_MAX];

// Define the block's coordinates.
float x=SCREEN_W-100, y=SCREEN_H/2-150;

#define numberBad 20
float sealSpeed = 1;
// Seal will lock when he rise or down
bool IsLockUp   = false;
bool IsLockDown = false;
bool IsLockRight   = false;


int  lockTime = 0; 
BadThing bad[numberBad];

int seal_lives = 3;

STATUS status = MENU;
DIRECTION direction = L;
/* Declare function prototypes. */

// Initialize variables and resources.
// Allows the game to perform any initialization it needs before
// starting to run.
void game_init(void);
// Initialize allegro5 library
void allegro5_init(void);
// Process events inside the event queue using an infinity loop.
void game_start_event_loop(void);
// Run game logic such as updating the world, checking for collision,
// switching scenes and so on.
// This is called when the game should update its logic.
void game_update(void);
// Draw to display.
// This is called when the game should draw itself.
void game_draw(void);
// Release resources.
// Free the pointers we allocated.
void game_destroy(void);


void InitBad(void);
void StartBad(void);
void UpdateBad(void);
void DrawBad(void);
void CollideBad(void);
/* Declare function prototypes for debugging. */

// Display error message and exit the program, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// If the program crashes unexpectedly, you can inspect "log.txt" for
// further information.
void game_abort(const char* format, ...);
// Log events for later debugging, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// You can inspect "log.txt" for logs in the last run.
void game_log(const char* format, ...);
// Called by 'game_abort', 'game_log' to deal with va_lists.
void game_vlog(const char* format, va_list arg);

// Program entry point.
// Returns program exit code.
int main(void) {
    srand(time(NULL));
    allegro5_init();
    game_log("Allegro5 initialized");
    game_log("Game begin");
    game_init();
    game_log("Game initialized");
    // Draw the first frame.
    game_draw();
    game_log("Game start event loop");
    // This call blocks until the game is finished.
    game_start_event_loop();
    game_log("Game end");
    game_destroy();
    return 0;
}

void allegro5_init(void) {
    // Initialize allegro5 library
    if (!al_init())
        game_abort("failed to initialize allegro");

    // Setup game display.
    game_display = al_create_display(SCREEN_W, SCREEN_H);
    if (!game_display)
        game_abort("failed to create display");

    // Setup update timer.
    game_update_timer = al_create_timer(1.0f / FPS);
    if (!game_update_timer)
        game_abort("failed to create timer");

    // Setup event queue.
    game_event_queue = al_create_event_queue();
    if (!game_event_queue)
        game_abort("failed to create event queue");

    // Initialize add-ons.
    if (!al_init_primitives_addon())
        game_abort("failed to initialize primitives add-on");
    // TODO: [Install keyboard]
    // Don't forget to check the return value.
    if (!al_install_keyboard())
        game_abort("failed to install keyboard");

    if (!al_init_font_addon())
        game_abort("failed to initialize font add-on");
    if (!al_init_ttf_addon())
        game_abort("failed to initialize ttf add-on");
    if (!al_init_image_addon())
        game_abort("failed to initialize image add-on");
    al_register_event_source(game_event_queue, al_get_display_event_source(game_display));
    // TODO: [Register keyboard to event queue]
    al_register_event_source(game_event_queue, al_get_keyboard_event_source());
    // TODO: [Register timer to event queue]
    al_register_event_source(game_event_queue, al_get_timer_event_source(game_update_timer));

    // TODO: [Start the timer]
    // Start the timer to update and draw the game.
    al_start_timer(game_update_timer);
}

void game_init(void) {
    font = al_load_ttf_font("ARCADECLASSIC.ttf", 52, 0);
    if (!font)
        game_abort("failed to load font: pirulen.ttf");
    img = al_load_bitmap("seal.png");
    if (!img)
        game_abort("failed to load image:");
    InitBad();
}

void game_start_event_loop(void) {
    bool done = false;
    ALLEGRO_EVENT event;
    while (!done) {
        al_wait_for_event(game_event_queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE )
            // Event for clicking the window close button.
            done = true;
        // TODO: [Process events]
        // 1) If the event's type is ALLEGRO_EVENT_KEY_DOWN, set the
        //    corresponding element in 'key_state' to true.
        // 2) If the event's type is ALLEGRO_EVENT_KEY_UP, set the
        //    corresponding element in 'key_state' to false.
        // 3) If the event's type is ALLEGRO_EVENT_TIMER and the source
        //    is your timer, call 'game_update' and 'game_draw'.
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            game_log("Key with keycode %d down", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = true;
        } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            game_log("Key with keycode %d up", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = false;
        } else if (event.type == ALLEGRO_EVENT_TIMER) {
            // Event for redrawing the display.
            if (event.timer.source == game_update_timer) {
                game_update();
                game_draw();
            }
        }
    }
}

void game_update(void) {
    // TODO: [Update coordinates]
    // Update 'x' and 'y' according to the current key state.
    int vx, vy;
    vx = vy = 0;

    if (IsLockDown){
        if (lockTime == 0) 
        {
            IsLockDown = false;
            sealSpeed = NORMAL_SPEED;
        }
        lockTime -= 1;
        vy += 1;
    }
    else if (IsLockUp)
    {
        if (lockTime == 0) 
        {
            IsLockUp = false;
            sealSpeed = NORMAL_SPEED;
        }
        lockTime -= 1;
        vy -= 1;
    }
    else if (IsLockRight)
    {
        if (lockTime == 0) 
        {
            IsLockRight = false;
            sealSpeed = NORMAL_SPEED;
        }
        lockTime -= 1;
        vx += 1;
    }

    else
    {
        if (key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W])
        {
            vy -= 1;
            IsLockUp = true;
            lockTime = HOLD_FPS;
            sealSpeed = SLOW_SPEED;
        }
        if (key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S])
        {
            vy += 1;
            IsLockDown = true;
            lockTime = HOLD_FPS;
            sealSpeed = SLOW_SPEED;
        }
        if (key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A])
        {
            vx -= 1;
            direction = L;
        }
            
        if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])
        {
            vx += 1;
            IsLockRight = true;
            lockTime = HOLD_FPS;
            sealSpeed = NORMAL_SPEED *2;
            direction = R;
        }
            
    }
    // 0.71 is (1/sqrt(2)).
    y += vy * (vx ? 3: 1);
    x += vx * (vy ? 3 : 1);
    StartBad();
    UpdateBad();
    CollideBad();

    if (x<-10) status = WIN;
}

void InitBad(){
    for(int i=0;i<numberBad;i++){
        bad[i].live = false;
        bad[i].speed = NORMAL_SPEED * 3.5;
        bad[i].body_x = 18;
        bad[i].body_y = 18;
        char str2[10] = "Homeworks";
        strcpy(bad[i].text, str2); 
    }
}

void StartBad(){
    for(int i = numberBad-1; i >= 0; i--){
        if(!bad[i].live){
            if(rand()%300 == 0){
                bad[i].live = true;
                bad[i].y = 30;
                bad[i].x = 30 + rand() % (SCREEN_W - 20);
                break;
            }
        }
    }
}
void UpdateBad(){
    for(int i = 0; i < numberBad; i++){
        
        if(bad[i].live){
            bad[i].y += bad[i].speed;
            if(bad[i].y > SCREEN_H)
                bad[i].live = false;
        }
    }
    
}

void DrawBad()
{
    for(int i=0;i<numberBad;i++){
        if(bad[i].live)
            al_draw_filled_circle(bad[i].x,bad[i].y,18,al_map_rgb(99,99,99));
    }
}

void CollideBad(){
    for(unsigned int i=0;i<numberBad;i++){
        if(bad[i].live){
            if(bad[i].x+bad[i].body_x > x  - SEAL_BODY &&
                bad[i].x-bad[i].body_x< x  + SEAL_BODY &&
                bad[i].y+bad[i].body_y> y &&
                bad[i].y-bad[i].body_y< y + 30)
                {
                    seal_lives--;
                    bad[i].live=false;
                }
        }
    }
    if (!seal_lives)
    {
        status = GG;
    }
}




void game_draw(void) {
    al_clear_to_color(al_map_rgb(153, 255, 255));
    if(direction==R)
        al_draw_bitmap(img, x-100 , y - 71 / 2, ALLEGRO_FLIP_HORIZONTAL);
    else
        al_draw_bitmap(img, x-100 , y - 71 / 2, 0);
    //al_draw_filled_rectangle(x, y, x + 25, y + 25, al_map_rgb(255, 255, 255));
    DrawBad();

    if (status == GG)
    {
        al_clear_to_color(al_map_rgb(225, 255, 255));
        al_draw_text(font, al_map_rgb(35, 35, 35), SCREEN_W / 2, 350,ALLEGRO_ALIGN_CENTER, "Seal Has Died!");
    }
    if (status == WIN)
    {
        al_clear_to_color(al_map_rgb(183, 255, 255));
        al_draw_text(font, al_map_rgb(35, 35, 35), SCREEN_W / 2, 350, ALLEGRO_ALIGN_CENTER, "Seal Still Alive !");
    }
    al_flip_display();
}

void game_destroy(void) {
    // TODO: [Release resources]
    // Destroy everything you have created.
    // Free the memories allocated by malloc or allegro functions.
    // We should destroy the timer we created.
    al_destroy_timer(game_update_timer);
    al_destroy_event_queue(game_event_queue);
    al_destroy_display(game_display);
}

// +=================================================================+
// | Code below is for debugging purpose, it's fine to remove it.    |
// | Deleting the code below and removing all calls to the functions |
// | doesn't affect the game.                                        |
// +=================================================================+

void game_abort(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
    fprintf(stderr, "error occured, exiting after 2 secs");
    // Wait 2 secs before exiting.
    al_rest(2);
    // Force exit program.
    exit(1);
}

void game_log(const char* format, ...) {
#ifdef LOG_ENABLED
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
#endif
}

void game_vlog(const char* format, va_list arg) {
#ifdef LOG_ENABLED
    static bool clear_file = true;
    vprintf(format, arg);
    printf("\n");
    // Write log to file for later debugging.
    FILE* pFile = fopen("log.txt", clear_file ? "w" : "a");
    if (pFile) {
        vfprintf(pFile, format, arg);
        fprintf(pFile, "\n");
        fclose(pFile);
    }
    clear_file = false;
#endif
}