#include <simplecpp>
#include "shooter.h"
#include "bubble.h"
#include "bullet.h"
/* Simulation Vars */
double STEP_TIME ;

/* Game Vars */
const int PLAY_Y_HEIGHT = 450;
const int LEFT_MARGIN = 70;
const int TOP_MARGIN = 20;
const int BOTTOM_MARGIN = (PLAY_Y_HEIGHT+TOP_MARGIN);


void move_bullets(vector<Bullet> &bullets){
    // move all bullets
    for(unsigned int i=0; i<bullets.size(); i++){
        if(!bullets[i].nextStep(STEP_TIME)){
            bullets.erase(bullets.begin()+i);
        }
    }
}

void move_bubbles(vector<Bubble> &bubbles){
    // move all bubbles
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        bubbles[i].nextStep(STEP_TIME);
    }
}

vector<Bubble> create_bubbles(int level , int radius , int x1_velocity , int x2_velocity)
{
    // create initial bubbles in the game
    vector<Bubble> bubbles;
    //repeat the number of bubbles created twice the number of current level
    for(int i=1;i<=level;i++)
    {
    bubbles.push_back(Bubble(BUBBLE_DEFAULT_RADIUS + WINDOW_X/(2.0*i), BUBBLE_START_Y, radius , x1_velocity , 0, COLOR(255-50*level,105+50*level,180-30*level)));
    bubbles.push_back(Bubble(BUBBLE_DEFAULT_RADIUS + WINDOW_X/(4.0*i), BUBBLE_START_Y, radius , x2_velocity, 0, COLOR(255-50*level,105+50*level,180-30*level)));
    }
    return bubbles;
}


int main()
{
    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
    b1.setColor(COLOR(0, 0, 255));

    string msg_cmd("Cmd: _");
    Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);

    //Initialising the levels

    for( int level=1 ; level<=3 ; level++)
    {
    // Intialize the shooter
    Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);

    int RADIUS = BUBBLE_DEFAULT_RADIUS - 2*(level -1);

    // Initialize the bubbles
    vector<Bubble> bubbles = create_bubbles(level , RADIUS ,-BUBBLE_DEFAULT_VX -50*(level-1),BUBBLE_DEFAULT_VX + 50*(level-1));

    // Initialize the bullets (empty)
    vector<Bullet> bullets;

    XEvent event;



    STEP_TIME = 0.02*(level);

    //prints the score of the level on the top
    string score("SCORE: _");
      Text scoreCollected(420,10,score);

    int bubble_count = 0;

    if (level==1)
          {
          Text level_info(200,200," LEVEL 1");
          wait(2);
          level_info.hide();
          }
    if (level==2)
          {
          Text level_info(200,200," LEVEL 2");
          wait(2);
          level_info.hide();

          }
    if (level==3)
          {
          Text level_info(200,200," FINAL LEVEL 3 @_@");
          wait(2);
          level_info.hide();

           }

       //keeps track number of times bubble hits the shooter
      int health = level;
      string msg_health("HEALTH: _");
      Text health_text(300,10,msg_health);

      //keeps the track of bullets left in the level;
      string msg_bullets("BULLETS LEFT: __");
      Text bullets_text(100,10,msg_bullets);

      //keeps track of time;
      int t=0 , time = 60/level;
      string msg_time("Time left: __");
      Text time_text(420,BOTTOM_MARGIN,msg_time);

      //keeps track of level;
      string msg_level("LEVEL : _/3");
      Text level_text(WINDOW_X/2,BOTTOM_MARGIN,msg_level);
      msg_level[msg_level.size()-3] = level + 48;
      level_text.setMessage(msg_level);

     //keeps a track of number of bullets used in a level
     int bullet_count = 10*level;
    // Main game loop
    while (true)
    {
       //initialising bubble count
        if(bullet_count<10)
            {
            msg_bullets[msg_bullets.size()-1] = bullet_count + 48;
            msg_bullets[msg_bullets.size()-2] =  48;
            bullets_text.setMessage(msg_bullets);
            }
        else if (bullet_count >= 10)
            {
            msg_bullets[msg_bullets.size()-1] = bullet_count%10 + 48;
            msg_bullets[msg_bullets.size()-2] = bullet_count/10 + 48;
            bullets_text.setMessage(msg_bullets);
            }

        bool pendingEvent = checkEvent(event);
        if (pendingEvent)
        {
            // Get the key pressed
            char c = charFromEvent(event);
            msg_cmd[msg_cmd.length() - 1] = c;
            charPressed.setMessage(msg_cmd);

            // Update the shooter
            if(c == 'a')
                shooter.move(STEP_TIME, true);
            else if(c == 'd')
                shooter.move(STEP_TIME, false);
            else if(c == 'w')
                {
                bullet_count--;
                bullets.push_back(shooter.shoot());
                }
            else if(c == 'q')
                return 0;
        }

    //blasting of bubble due to collision
    /* if bullet hits a bubble , then distance between their centres should be less than maximum possible
        distance between them during their contact, which is radius of circle + length of half of bullet's diagonal*/
        //count the number of bubbles left in the game;


    for(unsigned int j=0 ; j < bubbles.size() ; j++)
    {
     //length of hypotenuse is pow((pow(5,2) + pow(5,2)),0.5)
      double maximum_distance_bullet_bubble = bubbles[j].get_radius()+ pow((pow(5,2) + pow(5,2)),0.5);

    for(unsigned int i=0; i < bullets.size(); i++)
    {
     //distance between centres of bubble and rectangle
       double distance_x = bullets[i].get_center_x() -  bubbles[j].get_center_x();
       double distance_y = bullets[i].get_center_y() -  bubbles[j].get_center_y();
       double distance  = pow((pow(distance_x,2) + pow(distance_y,2)),0.5);
       if (distance < maximum_distance_bullet_bubble )
       {  if(bubbles[i].get_radius() == RADIUS)
          create_bubbles(level , RADIUS/2 , bubbles[i].get_center_x() ,bubbles[i].get_center_y() );
          bubbles.erase(bubbles.begin() + j);
          bullets.erase(bullets.begin() + i);
          bubble_count = bubble_count + 1;
       }


    }
    }

         if (bubble_count == 2*level)
        {
        Text win(200,200,"CONGRATULATIONS :)");
        wait(2);
         break;

        }

         //printing the scores
         score[score.size()-1] = bubble_count + 48;
         scoreCollected.setMessage(score);



        //collision between gun and bubble
      /* if bubble hits the shooter , then distance between their centres should be less than maximum possible
        distance between them during their contact, which is radius of circle + length of half of shooter's diagonal*/
       //length of hypotenuse is pow((pow(15,2) + pow(9,2)),0.5)
      double maximum_distance_bubble_shooter = RADIUS + pow((pow(15,2) + pow(9,2)),0.5);
      for(unsigned int i=0 ; i < bubbles.size() ; i++)
      {
         double distance_x = shooter.get_body_center_x() -  bubbles[i].get_center_x();
         double distance_y = shooter.get_body_center_y() -  bubbles[i].get_center_y();
         double distance  = pow((pow(distance_x,2) + pow(distance_y,2)),0.5);
         double distance_x_shooter_head_bubble = shooter.get_head_center_x() - bubbles[i].get_center_x();
         double distance_y_shooter_head_bubble = shooter.get_head_center_y() - bubbles[i].get_center_y();
         /*to avoid collision the circular tip of shooter should not be in contact with bubble,
         distance between centres is greater than sum of their radii */
         double distance_shooter_head_bubble = pow((pow(distance_x_shooter_head_bubble,2) + pow(distance_y_shooter_head_bubble,2)),0.5);
         double radial_distance = RADIUS + shooter.get_head_radius();
         if ((distance < maximum_distance_bubble_shooter)||(distance_shooter_head_bubble < radial_distance ))
         {
              bubbles.erase(bubbles.begin() + 1);
              health = health -1;
              bubble_count++;
         }

         msg_health[msg_health.size()-1] = health + 48;
         health_text.setMessage(msg_health);

         //ending the game after health is zero
         if(health==0)
         {
         Text lost(200,200,"GAME OVER :(");
         wait(2);
         return 0;
         }

            if (bubble_count == 2*level)
        {
        Text win(200,200,"CONGRATULATIONS :)");
        wait(2);
         break;
        }

      }
      //number of bullets should not reach more than a specified number
      if(bullet_count == 0)
      {
      Text bullet_notleft(200,200,"sorry, you don't have more bullets ");
      wait(2);
      bullet_notleft.hide();
      Text lost(200,200,"GAME OVER :(");
      wait(2);
      return 0;
      }

       // Update the bubbles
        move_bubbles(bubbles);

        // Update the bullets
        move_bullets(bullets);

        /*take taken to execute a single while loop is not negligible with respect to 0.02 seconds,
           so , for every 50 execution of while loop my time should increases by 1 seconds.
           So , we need to consider that also.
           since in my computer my 29 seconds of time is equal to 71.6 seconds;
           therefore, by unitory method 1 actual second is 0.4 second of hypothetical one.
           therefore for every 0.4*50 = 20 iteration of loop, my time increases by 1 seconds*/
        t++;
        if(t%20 ==0)
        {
         time--;

         if(time<10)
            {
            msg_time[msg_time.size()-1] = time + 48;
            msg_time[msg_time.size()-2] = 48;
            time_text.setMessage(msg_time);
            }
        else if (time >= 10)
            {
            msg_time[msg_time.size()-1] = time%10 + 48;
            msg_time[msg_time.size()-2] = time/10 + 48;
            time_text.setMessage(msg_time);
            }

        }

        if(time == 0)
          {
          Text lost_timeUp(200,200,"Time is Up");
          wait(2);
          Text lost_timeup(200,200,"GAME OVER :(");
          wait(2);
          return 0;
          }
        wait(STEP_TIME);
    }
    create_bubbles(level , RADIUS , -BUBBLE_DEFAULT_VX -50*(level-1),BUBBLE_DEFAULT_VX + 50*(level-1));
}
 }
