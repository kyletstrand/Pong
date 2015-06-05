/***********************************/
/***********************************/
/*                                 */
/*             Pong                */
/*      written by Kyle Strand     */
/*      kyletstrand@gmail.com      */
/*          5 June 2015            */
/*                                 */
/***********************************/
/***********************************/


#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

extern volatile int key_shifts;
const float FPS = 60;
const int SCREEN_W = 800;                            //Screen width
const int SCREEN_H = 600;                            //Screen height
const int BOUNCER_SIZE = 8;                         //Setting ball size
const int PADDLE_W = 8;                             //Setting paddle width
const int PADDLE_H = 120;                            //Setting paddle height
enum MYKEYS {KEY_UP, KEY_DOWN}; //Key mappings

int main (int argc, char **argv) {

  ALLEGRO_DISPLAY *display = NULL;                   //Object for display window
  ALLEGRO_EVENT_QUEUE *event_queue = NULL;           //Object for event queue
  ALLEGRO_TIMER *timer = NULL;                       //Object for timer...might not be needed
  ALLEGRO_BITMAP *bouncer = NULL;                    //Sprite object for ball
  ALLEGRO_BITMAP *paddle1 = NULL;                    //Sprite object for plater paddle
  ALLEGRO_BITMAP *paddle2 = NULL;                    //Sprite object for computer paddle
  ALLEGRO_SAMPLE *bounce = NULL;                     //Object for bouncing audio
  ALLEGRO_SAMPLE *cheer = NULL;                      //Object for cheering audio
  ALLEGRO_SAMPLE *boo = NULL;                        //Object for booing audio

  float bouncer_x = SCREEN_W / 2.0 - BOUNCER_SIZE / 2.0; //Starting x-coord of ball 
  float bouncer_y = SCREEN_H / 2.0 - BOUNCER_SIZE / 2.0; //Starting y-coord of ball
  float bouncer_dx = -4.0, bouncer_dy = 4.0;             //Ball coord increments
  float paddle1_x = SCREEN_W / 16.0 - PADDLE_W / 2.0;     //Starting x-coord of paddle1
  float paddle1_y = SCREEN_H / 2.0 - PADDLE_H / 2.0;      //Starting y-coord of paddle1
  float paddle2_x = 15*SCREEN_W / 16.0 - PADDLE_W / 2.0;     //Starting x-coord of paddle2
  float paddle2_y = SCREEN_H / 2.0 - PADDLE_H / 2.0;      //Starting y-coord of paddle2
  //float paddle2_dx = -4.0, paddle2_dy = 4.0;              //Paddle2 coord increments
  bool speed = false;                                       //If the fastest move has been done
  bool key[4] = {false, false, false, false};
  bool redraw = true;
  bool doexit = false;

  if (!al_init()) {                                       //Initializing Allegro library
    fprintf(stderr, "Failed to initialize allegro!\n");
    return -1;
  }

  if (!al_install_audio()) {                             //Initializing audio
    fprintf(stderr, "Failed to initialize audio!\n");
    return -1;
  }

  if (!al_init_acodec_addon()) {
    fprintf(stderr, "Failed to initialized audio codecs!\n");
    return -1;
  }

  if (!al_reserve_samples(3)){
    fprintf(stderr, "Failed to reserve samples!\n");
    return -1;
  }

  bounce = al_load_sample("audio/bounce.ogg");          //load bounce clip
  if (!bounce) {
    printf("Audio clip sample not loaded!\n");
    return 01;
  }

  cheer = al_load_sample("audio/cheer.ogg");            //load cheer clip
  if (!cheer) {
    printf("Audio clip not loaded!\n");
    return -1;
  }

  boo = al_load_sample("audio/boo.ogg");                //load boo clip
  if (!boo) {
    printf("Audio clip not loaded\n");
    return -1;
  }

  if (!al_install_keyboard()) {                           //Initializing keyboard
    fprintf(stderr, "Failed to initialize keyboard!\n");
    return -1;
  }

  timer = al_create_timer(1.0 / FPS);                     //Initialzing timer
  if (!timer) {
    fprintf(stderr, "Failed to create timer!\n");
    return -1;
  }

  display = al_create_display(SCREEN_W, SCREEN_H);                  //Initializing display
  if (!display) {
    fprintf(stderr, "Failed to to create display!\n");
    al_destroy_timer(timer);
    return -1;
  }

  bouncer = al_create_bitmap(BOUNCER_SIZE, BOUNCER_SIZE); //Initializing ball sprite
  if (!bouncer) {
    fprintf(stderr, "Failed to create bouncer sprite!\n");
    al_destroy_display(display);
    al_destroy_timer(timer);
    return -1;
  }

  paddle1 = al_create_bitmap(PADDLE_W, PADDLE_H);        //Initialzing paddle1 sprite
  if (!paddle1) {
    fprintf(stderr, "Failed to create paddle1 sprite!\n");
    al_destroy_bitmap(bouncer);
    al_destroy_display(display);
    al_destroy_timer(timer);
    return -1;
  }

  paddle2 = al_create_bitmap(PADDLE_W, PADDLE_H);        //Initializing paddle2 sprite
  if (!paddle2) {
    fprintf(stderr, "Failed to create paddle2 sprite!\n");
    al_destroy_bitmap(paddle1);
    al_destroy_bitmap(bouncer);
    al_destroy_display(display);
    al_destroy_timer(timer);
    return -1;
  }

  al_set_target_bitmap(bouncer);                        //Setting colors for sprites, white
  al_clear_to_color(al_map_rgb(255,255,255));
  al_set_target_bitmap(paddle1);
  al_clear_to_color(al_map_rgb(255,255,255));
  al_set_target_bitmap(paddle2);
  al_clear_to_color(al_map_rgb(255,255,255));
  al_set_target_bitmap(al_get_backbuffer(display));     //Calling display
  al_clear_to_color(al_map_rgb(0,0,0));                 //Setting background color, black  

  event_queue = al_create_event_queue();
  if (!event_queue) {                                   //Initialzing event queue
    fprintf(stderr, "Failed to create event queue!\n"); 
    al_destroy_bitmap(paddle2);
    al_destroy_bitmap(paddle1);
    al_destroy_bitmap(bouncer);
    al_destroy_display(display);
    al_destroy_timer(timer);
  }

  al_register_event_source(event_queue, al_get_display_event_source(display));  //Setting display event source
  al_register_event_source(event_queue, al_get_timer_event_source(timer));      //Setting timer event source
  al_register_event_source(event_queue, al_get_keyboard_event_source());        //Setting keyboard event source    

  al_flip_display();                                                            //Move from screen buffer
  al_start_timer(timer);                                                        //Begin timer

  while(!doexit) {                                                              //Event loop
    ALLEGRO_EVENT ev;                                                           //Creating event object
    al_wait_for_event(event_queue, &ev);

    if (ev.type == ALLEGRO_EVENT_TIMER) {
      if (bouncer_x < 0) {                                                            //Checking if score was made by player
        al_play_sample(boo, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        al_rest(2);
        break;
      }

      if (bouncer_x > SCREEN_W - BOUNCER_SIZE) {                              //Checking if socre was made by computer
        al_play_sample(cheer, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        al_rest(2);
        break;
      }

      if (bouncer_x <= paddle1_x + BOUNCER_SIZE) {                            //Checking to see if the ball hits paddle1. The ball still clips the back corner...fix later
        if (bouncer_x <= paddle1_x) {}         
        else if (bouncer_y >= (paddle1_y - BOUNCER_SIZE)) {
          if (bouncer_y <= (paddle1_y + 60)) {   
            if (bouncer_y >= (paddle1_y - BOUNCER_SIZE) && bouncer_y <= (paddle1_y + 40)) {
              bouncer_dy = 6;
              if (bouncer_dx > -7) {
                bouncer_dx--;
                speed = false;
              }
            }
            if (bouncer_y >= (paddle1_y - BOUNCER_SIZE) && bouncer_y <= (paddle1_y + 20)) {
              bouncer_dy = 8;
              if (bouncer_dx > -6) {
                bouncer_dx = bouncer_dx - 2;
                speed = true;
              }
            }
            bouncer_dx = -bouncer_dx;
            if (bouncer_dy > 0) bouncer_dy = -bouncer_dy;                          //If the ball is coming from the top, reverse direction             
            al_play_sample(bounce, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);    //Play bounce sound when paddle1 is hit
          }
          if (bouncer_y >= (paddle1_y + 60) && bouncer_y <= (paddle1_y + PADDLE_H)) {
            if (bouncer_y >= paddle1_y + 60 && bouncer_y >= (paddle1_y + 80)) {
              bouncer_dy = 6;
              if (bouncer_dx > -7) {
                bouncer_dx--;
                speed = false;
              }
            }
            if (bouncer_y >= paddle1_y + 60 && bouncer_y >= (paddle1_y + 100)) {
              bouncer_dy = 8;
              if (bouncer_dx > -6) {
                bouncer_dx = bouncer_dx - 2;
                speed = true;
              }
            }
            bouncer_dx = -bouncer_dx;
            if (bouncer_dy < 0) bouncer_dy = -bouncer_dy;
            al_play_sample(bounce,1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
          }
        }
      }

      if (bouncer_x >= paddle2_x - BOUNCER_SIZE) {                           //Checking to see if ball hits paddle2 
        if (bouncer_x >= paddle2_x) {}
        else if (bouncer_y >= (paddle2_y - BOUNCER_SIZE))  {  // Trying to get it to change direction on hittiing top and bottom respectively 
          if (bouncer_y <= (paddle2_y + 60)) {
            if (bouncer_y >= (paddle2_y - BOUNCER_SIZE) && bouncer_y <= (paddle2_y + 40)) {
              bouncer_dy = 6;
              if (bouncer_dx < 7) { 
                bouncer_dx++;
                speed = false;
              }
            }
            if (bouncer_y >= (paddle2_y - BOUNCER_SIZE) && bouncer_y <= (paddle2_y + 20)) {
              bouncer_dy = 8;
              if (bouncer_dx < 6) {
                bouncer_dx = bouncer_dx + 2;
                speed = true;
              }
            }
            bouncer_dx = -bouncer_dx;
            if (bouncer_dy > 0) bouncer_dy = -bouncer_dy;                        //if ball comes from the top, reverse direction
            al_play_sample(bounce, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
          }
          if (bouncer_y >= (paddle2_y + 60) && bouncer_y < (paddle2_y + PADDLE_H)) {
            if (bouncer_y >= paddle2_y + 60 && bouncer_y >= (paddle2_y + 80)) {
              bouncer_dy = 6;
              if (bouncer_dx < 7) {
                bouncer_dx++;
                speed = false;
              }
            }
            if (bouncer_y >= paddle2_y + 60 && bouncer_y >= (paddle2_y + 100)) {
              bouncer_dy = 8;
              if (bouncer_dx < 6) {
                bouncer_dx = bouncer_dx + 2;
                speed = true;
              }
            }
            bouncer_dx = -bouncer_dx;
            if (bouncer_dy < 0) bouncer_dy = -bouncer_dy;                           //if coming from bottom, direction is reveresed
            al_play_sample(bounce, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);    //Play bounce sound when paddle2 is hit
          }
        }
      }

      if (bouncer_y <= 0 || bouncer_y >= SCREEN_H - BOUNCER_SIZE) {             //Checking to see if side was hit
        bouncer_dy = -bouncer_dy;                                            //Reverse direction
      } 

      bouncer_x += bouncer_dx;                                                //Increase x-coord
      bouncer_y += bouncer_dy;                                                //Increase y-coord

      if (key[KEY_UP] && paddle1_y > 4.0) {                                //Raise paddle1 if up is pressed
        paddle1_y -= 4.0;
      }

      if (key[KEY_DOWN] && paddle1_y <= SCREEN_H - PADDLE_H - 4.0) {       //Lower paddle1 if down is pressed
        paddle1_y += 4.0;
      }

      if (bouncer_dy >= 0) {                                               //If ball is traveling upwards, paddle2 will follow
        if (paddle2_y <= SCREEN_H - PADDLE_H - 4.0) {
          if (speed == true) {
            paddle2_y += 6.0;
          } else paddle2_y += 4.0;
        }
      }
 
      if (bouncer_dy <= 0) {                                               //If ball is traveling downwards, paddle2 will follow
        if (paddle2_y > 4.0) {
          if (speed == true) {
            paddle2_y -=6.0;
          } else paddle2_y -= 4.0;
        }
      }

      redraw = true;                                                           //Draw again 
    }

    else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {                        //Close if window is closed
      break;
    }

   /****** Keyboard events will go here ******/

    else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {                 //Check to see if key was pressed
      switch (ev.keyboard.keycode) {
        case ALLEGRO_KEY_UP:                                      //Check for up key
          key[KEY_UP] = true;
          break;

        case ALLEGRO_KEY_DOWN:                                    //check for down key
          key[KEY_DOWN] = true;
          break;
      }
    }

    else if (ev.type == ALLEGRO_EVENT_KEY_UP) {                   //Check if key was relesased
      switch (ev.keyboard.keycode) {
        case ALLEGRO_KEY_UP:                                      //check for up key
          key[KEY_UP] = false;
          break;
  
        case ALLEGRO_KEY_DOWN:                                    //check for down key
          key[KEY_DOWN] = false;
          break;

        case ALLEGRO_KEY_ESCAPE:                                  //Close if excape is pressed
          doexit = true;
          break;
      }
    }
    if(redraw && al_is_event_queue_empty(event_queue)) {                      //checking if event_queue is clear
      redraw = false;                                                         //If so, redraw display
      al_clear_to_color(al_map_rgb(0,0,0));                                   //Clear background
      al_draw_bitmap(bouncer, bouncer_x, bouncer_y, 0);                       //Redraw ball
      al_draw_bitmap(paddle1, paddle1_x, paddle1_y, 0);                       //Redraw paddle1
      al_draw_bitmap(paddle2, paddle2_x, paddle2_y, 0);                       //redraw paddle2
      al_flip_display();
    }
  }

  al_destroy_bitmap(paddle2);
  al_destroy_bitmap(paddle1);                          //Destroying objects
  al_destroy_bitmap(bouncer);
  al_destroy_timer(timer);
  al_destroy_display(display);
  al_destroy_event_queue(event_queue);

  return 0;
}

