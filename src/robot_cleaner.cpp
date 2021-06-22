
#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Pose.h"

const double PI=3.1415926535897934;

void move(double speed,double distance,bool isForward);
void rotate(double angular_speed,double angle,bool is_clock);
double deg_to_rad(double degree);
void setDesiredOrientation(double desired_ang_rad);
void move_to_goal(turtlesim::Pose goal_pos,double dist_tol);
void spiral_move();

turtlesim::Pose turtle_pose; 

void pos_call_back(const turtlesim::Pose::ConstPtr& pos_msg)
{
    turtle_pose.theta=pos_msg->theta;
    turtle_pose.x=pos_msg->x;
    turtle_pose.y=pos_msg->y;
}

ros::Publisher velocity_publisher;
ros::Subscriber pose_subscriber;


int main(int argc, char **argv)
{
  
  ros::init(argc, argv, "robot_cleaner");
  ros::NodeHandle n;
  
  velocity_publisher=n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel",10);
  pose_subscriber=n.subscribe("/turtle1/pose",10,pos_call_back);

/*
  double sp,dis;bool forw;
  std::cout<<"Enter speed,distance,direction : \n";
  std::cin>>sp>>dis>>forw;
  move(sp,dis,forw);

  double asp,ang;bool is_cl;
  std::cout<<"Enter angular speed, angle (in terms of degrees) , is_clockwise \n";
  std::cin>>asp>>ang>>is_cl;
  rotate(deg_to_rad(asp),deg_to_rad(ang),is_cl);*/

  /*
  ros::Rate laps(0.5);
  laps.sleep();
  
  setDesiredOrientation(deg_to_rad(90));
  laps.sleep();
  setDesiredOrientation(deg_to_rad(0));
  */

  /*
  ros::Rate loop12(0.5);

  turtlesim::Pose goal_pos;

  std::cout<<"Enter x y theta \n";
  double a1,b1,c1;
  std::cin>>a1>>b1>>c1;
  goal_pos.x=a1;
  goal_pos.y=b1;
  goal_pos.theta=c1;

  move_to_goal(goal_pos,0.1);

    loop12.sleep();
  */

  spiral_move(); 
  ros::spin();

  return 0;
}

double deg_to_rad(double degree)
{
    double val=degree*PI/180;
    return val;
}

double get_distance(double x1,double y1,double x2,double y2)
{
    return sqrt(pow((x1-x2),2)+pow((y1-y2),2));
}

void move(double speed,double distance,bool isForward)
{
    geometry_msgs::Twist vel;

    if(isForward)
        vel.linear.x=abs(speed);
    else
        vel.linear.x=-abs(speed);
    vel.linear.y=0;
    vel.linear.z=0;
    vel.angular.x=0;
    vel.angular.y=0;
    vel.angular.z=0;

    double t0=ros::Time::now().toSec();
    double cur_dist=0;
    
    ros::Rate loop_rate(1000);

    do
    {
        velocity_publisher.publish(vel);
        double t1=ros::Time::now().toSec();
        cur_dist=(t1-t0)*abs(speed);

        ros::spinOnce(); 
        loop_rate.sleep();

    }while(cur_dist<distance);

    vel.linear.x=0;
    velocity_publisher.publish(vel);
    

}

void rotate(double angular_speed,double angle,bool is_clock)
{
    geometry_msgs::Twist vel;

    ros::Rate loop_rate(1000);

    double cur_ang=0;
    double t0=ros::Time::now().toSec();

    vel.linear.x=0;vel.linear.y=0;vel.linear.z=0;
    vel.angular.x=0;vel.angular.y=0;vel.angular.z=is_clock?angular_speed:-angular_speed;

    do
    {
        double t1=ros::Time::now().toSec();
        cur_ang=(t1-t0)*abs(angular_speed);

        velocity_publisher.publish(vel);
        ros::spinOnce();
        loop_rate.sleep();
    } while (cur_ang<angle);
    vel.angular.z=0;
    velocity_publisher.publish(vel);

}


void setDesiredOrientation(double desired_ang_rad)
{
    double rel_ang=desired_ang_rad-turtle_pose.theta;
    bool clock=rel_ang>0?1:0;
    /*std::cout<<rel_ang<<"\n";*/
    rotate(abs(rel_ang),abs(rel_ang),clock);
}

void move_to_goal(turtlesim::Pose goal_pos,double dist_tol)
{
    geometry_msgs::Twist vel;
    vel.linear.y=0;
    vel.linear.z=0;
    vel.angular.x=0;
    vel.angular.y=0;

    ros::Rate loop_rate(10);

    do{
        vel.linear.x=0.5*(get_distance(turtle_pose.x,turtle_pose.y,goal_pos.x,goal_pos.y));
        vel.angular.z=4*(atan2(goal_pos.y-turtle_pose.y,goal_pos.x-turtle_pose.x) - turtle_pose.theta );

        velocity_publisher.publish(vel);

        ros::spinOnce();
        loop_rate.sleep();

    }while(get_distance(turtle_pose.x,turtle_pose.y,goal_pos.x,goal_pos.y)>dist_tol);
    vel.linear.x=0;
    vel.angular.z=0;
    velocity_publisher.publish(vel);
    
}

void spiral_move()
{
    geometry_msgs::Twist vel;
    double ang_vel=4.0;
    double lin_vel=0.0;
    vel.linear.x=lin_vel;
    vel.linear.y=0.0;
    vel.linear.z=0.0;
    vel.angular.x=0.0;
    vel.angular.y=0.0;
    vel.angular.z=ang_vel;

    


    ros::Rate loop_rate(1);

    do
    {
        vel.linear.x+=0.5;
        velocity_publisher.publish(vel);
        ros::spinOnce();
        loop_rate.sleep();
    } while ((turtle_pose.x<9) && (turtle_pose.y<9));
    double t0=ros::Time::now().toSec();
    vel.linear.x=-0.0;
    vel.angular.z=0;
    velocity_publisher.publish(vel);
    

}