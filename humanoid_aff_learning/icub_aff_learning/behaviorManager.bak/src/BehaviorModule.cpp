#include "../include/BehaviorModule.h"

BehaviorModule::BehaviorModule ()
{
  graspOrien.resize (4);
  graspDisp.resize (4);
  dOffs.resize (3);
  dLift.resize (3);
  home_x.resize (3);

  // default values for arm-dependent quantities
  graspOrien[0] = -0.171542;
  graspOrien[1] = 0.124396;
  graspOrien[2] = -0.977292;
  graspOrien[3] = 3.058211;

  graspDisp[0] = 0.0;
  graspDisp[1] = 0.0;
  graspDisp[2] = 0.05;

  dOffs[0] = -0.03;
  dOffs[1] = -0.07;
  dOffs[2] = -0.02;

  dLift[0] = 0.0;
  dLift[1] = 0.0;
  dLift[2] = 0.15;

  home_x[0] = -0.29;
  home_x[1] = -0.21;
  home_x[2] = 0.11;

  action = NULL;

  openPorts = false;
  firstRun = true;
  sim = false;

  options_left.put ("device", "remote_controlboard");
  options_left.put ("local", "/pos_ctrl_left_arm");
  options_left.put ("remote", "/icub/left_arm");

  options_right.put ("device", "remote_controlboard");
  options_right.put ("local", "/pos_ctrl_right_arm");
  options_right.put ("remote", "/icub/right_arm");

  driver_left.open (options_left);
  driver_right.open (options_right);

  if (!driver_left.isValid () || !driver_left.isValid ())
  {
    cerr << "A device is not available. Here are the known devices:" << endl;
    cerr << Drivers::factory ().toString ().c_str () << endl;
    exit (-1);
  }
  bool ok = true;
  ok = ok && driver_left.view (pos_ctrl_left);
  ok = ok && driver_right.view (pos_ctrl_right);
  ok = ok && driver_left.view (encoders_left);
  ok = ok && driver_right.view (encoders_right);

  if (!ok)
  {
    cerr << "error getting interfaces" << std::endl;
    exit (-1);
  }
  int n_jnts = 0;

  pos_ctrl_left->getAxes (&n_jnts);
  positions_left_cmd.resize (n_jnts);
  positions_left_enc.resize (n_jnts);

  pos_ctrl_right->getAxes (&n_jnts);
  positions_right_cmd.resize (n_jnts);
  positions_right_enc.resize (n_jnts);

//  driver_left.close();
//  driver_right.close();
}

BehaviorModule::~BehaviorModule ()
{
}

void
BehaviorModule::getArmDependentOptions (Bottle &b, Vector &_gOrien, Vector &_gDisp, Vector &_dOffs, Vector &_dLift,
                                        Vector &_home_x)
{
  if (Bottle *pB=b.find("grasp_orientation").asList())
  {
    int sz = pB->size ();
    int len = _gOrien.length ();
    int l = len < sz ? len : sz;

    for (int i = 0; i < l; i++)
      _gOrien[i] = pB->get (i).asDouble ();
  }

  if (Bottle *pB=b.find("grasp_displacement").asList())
  {
    int sz = pB->size ();
    int len = _gDisp.length ();
    int l = len < sz ? len : sz;

    for (int i = 0; i < l; i++)
      _gDisp[i] = pB->get (i).asDouble ();
  }

  if (Bottle *pB=b.find("systematic_error_displacement").asList())
  {
    int sz = pB->size ();
    int len = _dOffs.length ();
    int l = len < sz ? len : sz;

    for (int i = 0; i < l; i++)
      _dOffs[i] = pB->get (i).asDouble ();
  }

  if (Bottle *pB=b.find("lifting_displacement").asList())
  {
    int sz = pB->size ();
    int len = _dLift.length ();
    int l = len < sz ? len : sz;

    for (int i = 0; i < l; i++)
      _dLift[i] = pB->get (i).asDouble ();
  }

  if (Bottle *pB=b.find("home_position").asList())
  {
    int sz = pB->size ();
    int len = _home_x.length ();
    int l = len < sz ? len : sz;

    for (int i = 0; i < l; i++)
      _home_x[i] = pB->get (i).asDouble ();
  }
}

bool
BehaviorModule::configure (ResourceFinder &rf)
{
  string name = rf.find ("name").asString ().c_str ();
  setName (name.c_str ());

  string sim_or_real = rf.find ("sim").asString ().c_str ();
  if (sim_or_real == "on")
    sim = true;
  else
    sim = false;

  string partUsed = rf.find ("part").asString ().c_str ();
  if ((partUsed != "left_arm") && (partUsed != "right_arm"))
  {
    cout << "Invalid part requested!" << endl;
    return false;
  }

  Property config;
  config.fromConfigFile (rf.findFile ("from").c_str ());
  Bottle &bGeneral = config.findGroup ("general");
  if (bGeneral.isNull ())
  {
    cout << "Error: group general is missing!" << endl;
    return false;
  }

  // parsing general config options
  Property option;
  for (int i = 1; i < bGeneral.size (); i++)
  {
    Bottle *pB = bGeneral.get (i).asList ();
    if (pB->size () == 2)
      option.put (pB->get (0).asString ().c_str (), pB->get (1));
    else
    {
      cout << "Error: invalid option!" << endl;
      return false;
    }
  }

  option.put ("local", name.c_str ());
  option.put ("part", rf.find ("part").asString ().c_str ());
  option.put ("grasp_model_type", rf.find ("grasp_model_type").asString ().c_str ());
  option.put ("grasp_model_file", rf.findFile ("grasp_model_file").c_str ());
  option.put ("hand_sequences_file", rf.findFile ("hand_sequences_file").c_str ());

  printf ("%s\n", option.toString ().c_str ());

  // parsing arm dependent config options
  Bottle &bArm = config.findGroup ("arm_dependent");
  getArmDependentOptions (bArm, graspOrien, graspDisp, dOffs, dLift, home_x);

  cout << "***** Instantiating primitives for " << partUsed << endl;
  action = new ActionPrimitivesLayer2 (option);
  if (!action->isValid ())
  {
    delete action;
    return false;
  }

  deque<string> q = action->getHandSeqList ();
  cout << "***** List of available hand sequence keys:" << endl;
  for (size_t i = 0; i < q.size (); i++)
    cout << q[i] << endl;

  string fwslash = "/";
  inPort.open ((fwslash + name + "/in").c_str ());
  rpcPort.open ((fwslash + name + "/rpc").c_str ());
  port_simon_in.open ("/simon_cmd:i");
  attach (rpcPort);

  openPorts = true;

  // check whether the grasp model is calibrated,
  // otherwise calibrate it and save the results
  Model *model;
  action->getGraspModel (model);
  if (!model->isCalibrated ())
  {
    Property prop ("(finger all)");
    model->calibrate (prop);

    ofstream fout;
    fout.open (option.find ("grasp_model_file").asString ().c_str ());
    model->toStream (fout);
    fout.close ();
  }

  return true;
}

bool
BehaviorModule::close ()
{
  if (action != NULL)
    delete action;

  if (openPorts)
  {
    inPort.close ();
    rpcPort.close ();
  }

  return true;
}

double
BehaviorModule::getPeriod ()
{
  return 0.1;
}

void
BehaviorModule::init ()
{
  bool f;

  if (sim)
  {
    port_sim_rpc_out.open ("/sim:o");
    Network::connect ("/sim:o", "/icubSim/world");
  }

  action->disableContactDetection ();
  action->pushAction (home_x, "open_hand");
  action->checkActionsDone (f, true);
  action->enableContactDetection ();
//  action->enableArmWaving (home_x);
}

Vector
BehaviorModule::vectorAngle2Normal (Vector vec_angle_rep)
{
  Matrix R = iCub::ctrl::axis2dcm (vec_angle_rep);

  Vector v (3);
  v[0] = R[0][2];
  v[1] = R[1][2];
  v[2] = R[2][2];
  return v;
}

Vector
BehaviorModule::normal2VectorAngle (const Vector& hand_normal)
{
  double z_theta_hand = atan2 (hand_normal[1], hand_normal[0]);
  std::cout << "theta: " << z_theta_hand << std::endl;
  //    Vector oz (4);
  Vector ox (4);
  Vector oz_final (4);

  ox[0] = 1.0;
  ox[1] = 0.0;
  ox[2] = 0.0;
  ox[3] = -PI / 2;

  oz_final[0] = 0.0;
  oz_final[1] = 0.0;
  oz_final[2] = 1.0;
  oz_final[3] = z_theta_hand - PI / 2;

  //    Matrix Rz = iCub::ctrl::axis2dcm (oz); // from axis/angle to rotation matrix notation
  Matrix Rx = iCub::ctrl::axis2dcm (ox);
  Matrix Rz_final = iCub::ctrl::axis2dcm (oz_final);

  Matrix R = Rz_final * Rx;
  Vector vec_ang = iCub::ctrl::dcm2axis (R);
  return vec_ang;
}

Vector
BehaviorModule::angleXZToVectorAngle (const double x_ang, const double z_ang)
{
  Vector oz (4);
  Vector ox (4);

  oz[0] = 0.0;
  oz[1] = 0.0;
  oz[2] = 1.0;
  oz[3] = z_ang;// / 180 * PI;

  ox[0] = 1.0;
  ox[1] = 0.0;
  ox[2] = 0.0;
  ox[3] = x_ang;// / 180 * PI;

  Matrix Rz = iCub::ctrl::axis2dcm (oz); // from axis/angle to rotation matrix notation
  Matrix Rx = iCub::ctrl::axis2dcm (ox);

  Matrix R = Rz * Rx;
  Vector poi_orient = iCub::ctrl::dcm2axis (R); // from rotation matrix back to the axis/angle notation

  //    std::cout << poi_orient[0] << " " << poi_orient[1] << " " << poi_orient[2] << std::endl;

  return poi_orient;
}

Vector
BehaviorModule::angleXYZToVectorAngle (const double x_ang, const double y_ang, const double z_ang)
{
  Vector oz (4);
  Vector oy (4);
  Vector ox (4);

  oz[0] = 0.0;
  oz[1] = 0.0;
  oz[2] = 1.0;
  oz[3] = z_ang;// / 180 * PI;

  oy[0] = 0.0;
  oy[1] = 1.0;
  oy[2] = 0.0;
  oy[3] = y_ang;// / 180 * PI;

  ox[0] = 1.0;
  ox[1] = 0.0;
  ox[2] = 0.0;
  ox[3] = x_ang;// / 180 * PI;

  Matrix Rz = iCub::ctrl::axis2dcm (oz); // from axis/angle to rotation matrix notation
  Matrix Ry = iCub::ctrl::axis2dcm (oy);
  Matrix Rx = iCub::ctrl::axis2dcm (ox);

  Matrix R = Ry * Rz * Rx;
  //    Matrix R = Rz * Rx;
  Vector poi_orient = iCub::ctrl::dcm2axis (R); // from rotation matrix back to the axis/angle notation

  //    std::cout << poi_orient[0] << " " << poi_orient[1] << " " << poi_orient[2] << std::endl;

  return poi_orient;
}

//bb_pos and bb_dims specifies the bounding box properties of the object of interest
//push_dir is the angle(in radians). 0 -> right, PI/2 -> forward, PI -> left, 3*PI/2 -> backward
void
BehaviorModule::push (const Vector& bb_center, const Vector& bb_dims, double push_dir_angle, const double poi_shift,
                      bool spin)
{
  bool f;

  //just consider for now that pushes are done on the x-y plane, specifically on the table
  Vector hand_normal (3);

  //take the hypotenuse of the bounding box cross-section as the worst case scenario
  //similar to a bounding circle actually (radius away from the center)
  double poi_offset = sqrt (bb_dims[0] * bb_dims[0] + bb_dims[1] * bb_dims[1]) / 2.0 - 0.01;//0.03 for palm height

  Vector poi_off (3);
  poi_off[0] = 0.0;
  poi_off[1] = 0.0;
  poi_off[2] = 0.0;

  //first convert the angle into simulation coordinate system
  push_dir_angle += PI / 2;

  //make sure that the angle is between [0, 2*PI]
  if (push_dir_angle < 0)
    push_dir_angle += 2 * PI;
  std::cout << "push angle: " << push_dir_angle * 180 / PI << std::endl;

  double des_hand_normal_ang = 0;
  string hand_key;

  if (PI / 4 <= push_dir_angle && push_dir_angle < 3 * PI / 4)
  {
    des_hand_normal_ang = push_dir_angle + PI;
    hand_key = "open_hand";
    std::cout << hand_key << std::endl;
  }
  else if (3 * PI / 4 <= push_dir_angle && push_dir_angle < 5 * PI / 4)
  {
    des_hand_normal_ang = push_dir_angle + PI / 2;
    //      hand_key = "close_hand";
    hand_key = "fist_hand";
    std::cout << hand_key << std::endl;
  }
  else if (5 * PI / 4 <= push_dir_angle && push_dir_angle < 7 * PI / 4)
  {
    des_hand_normal_ang = push_dir_angle;
    hand_key = "open_hand";
    std::cout << hand_key << std::endl;
  }
  else
  {
    des_hand_normal_ang = push_dir_angle - PI / 2.0;
    poi_off[1] = sin (push_dir_angle - PI / 2.0) * poi_offset + sin (push_dir_angle + PI) * (-0.035);//0.035 is the half of the palm size
    poi_off[0] = cos (push_dir_angle - PI / 2.0) * poi_offset + cos (push_dir_angle + PI) * (-0.035);
    hand_key = "perpendicular_hand";
    std::cout << hand_key << std::endl;
  }

  hand_normal[0] = cos (des_hand_normal_ang);
  hand_normal[1] = sin (des_hand_normal_ang);
  hand_normal[2] = 0;

  poi_off[0] += cos (push_dir_angle + PI) * poi_offset;// x component of the direction vector * reach offset
  poi_off[1] += sin (push_dir_angle + PI) * poi_offset;// y component of the direction vector * reach offset

  //Find the vector-angle representation of this normal
  Vector vec_angle = normal2VectorAngle (hand_normal);

  //    std::cout << bb_center[0] << " " << bb_center[1] << " " << bb_center[2] << std::endl;
  //    std::cout << poi_off[0] << " " << poi_off[1] << " " << poi_off[2] << std::endl;
  //    action->touch (bb_center, vec_angle, poi_off);

  //first go above the point
  poi_off[2] = bb_dims[2] / 2 + 0.02;//0.02cm is object clearance factor
  action->pushAction (bb_center + poi_off, vec_angle, hand_key);
  action->checkActionsDone (f, true);

  //now, get down to the table
  poi_off[2] = -bb_dims[2] + 0.02;//0.02 table clearance factor
  action->pushAction (bb_center + poi_off, vec_angle, hand_key);
  action->checkActionsDone (f, true);

  //it is time to push the object
  Vector poi_sh (3);
  poi_sh[0] = cos (push_dir_angle) * poi_shift;// x component of the direction vector * reach offset
  poi_sh[1] = sin (push_dir_angle) * poi_shift;// y component of the direction vector * reach offset
  if (spin)
    poi_sh[2] = bb_dims[2];//enables spinning object
  else
    poi_sh[2] = 0.0;

  action->pushAction (bb_center + poi_off + poi_sh, vec_angle, hand_key);
  action->checkActionsDone (f, true);

  //get arm directly up if spin is not activated, this enables robot to avoid colliding
  //to the object while doing a homing action etc.
  if (!spin)
  {
    poi_off[0] += cos (push_dir_angle + PI) * 0.02;
    poi_off[1] += sin (push_dir_angle + PI) * 0.02;
    poi_off[2] += bb_dims[2];
  }

  action->pushAction (bb_center + poi_off + poi_sh, vec_angle, hand_key);
  action->checkActionsDone (f, true);
}

void
BehaviorModule::lift (const double poi_shift)
{
  Vector hand_pos;
  Vector hand_orient;
  action->getPose (hand_pos, hand_orient);
  hand_pos[2] += poi_shift;
  action->disableContactDetection ();
  action->pushAction (hand_pos, hand_orient);
}

void
BehaviorModule::hide (Vector bb_center, Vector bb_dims)
{
  Vector poi_off (3);
  poi_off[0] = 0.0;
  poi_off[1] = 0.0;
  poi_off[2] = bb_dims[2] / 2 + 0.02;//half of the height + safety distance

  Vector orient = angleXZToVectorAngle (0, PI);
  action->pushAction (bb_center + poi_off, orient, "karate_hand");
}

void
BehaviorModule::pointTo (Vector point)
{
  Vector left_shoulder (3);
  left_shoulder[0] = 0.0;
  left_shoulder[1] = -0.12;//to the left (crouch-left shoulder)
  left_shoulder[2] = 0.25;// to the up (crouch-neck)

  Vector shoulder_to_point = point - left_shoulder;
  double delta_dist = sqrt (pow (shoulder_to_point[0], 2) + pow (shoulder_to_point[1], 2) + pow (shoulder_to_point[2],
                                                                                                 2));

  //if the point is not reachable
  if (delta_dist > 0.35)
  {
    Vector des_position = (point - left_shoulder) * 0.35 / delta_dist + left_shoulder;
    Vector normalized_dir = (point - left_shoulder) / delta_dist;
    std::cout << normalized_dir[0] << " " << normalized_dir[1] << " " << normalized_dir[2] << std::endl;

    double z_angle = atan2 (normalized_dir[1], normalized_dir[0]);
    if (z_angle < 0)
      z_angle += 2 * PI;
    std::cout << z_angle / PI * 180 << std::endl;
    double y_angle = atan2 (normalized_dir[2], sqrt (pow (normalized_dir[0], 2) + pow (normalized_dir[1], 2)));
    std::cout << y_angle / PI * 180 << std::endl;

    //      Vector des_orient = angleXZToVectorAngle(-PI/2, z_angle);

    Vector des_orient = angleXYZToVectorAngle (-PI / 2, y_angle, z_angle);
    //      Vector des_orient = angleXYZToVectorAngle (0 , -PI/6,  PI);

    //action->disableTorsoDof ();
    action->pushAction (des_position, des_orient, "point_hand", 5);
    //action->enableTorsoDof ();
  }
}

void
BehaviorModule::release (Vector point, bool palm_upward)
{
  Vector hand_orient;

  if (palm_upward)
    hand_orient = angleXZToVectorAngle (PI, PI);
  else
    hand_orient = angleXZToVectorAngle (0, PI);

  action->pushAction (point, hand_orient);
  // release the object or just open the
  // hand (wait until it's done)
  bool f;
  action->pushAction ("open_hand");
  action->areFingersInPosition (f);
}

void
BehaviorModule::grasp (Vector bb_center)
{
  Vector xv (3);
  xv[0] = 0.0;
  xv[1] = 0.0;
  xv[2] = -0.0;

  //check if fingers contact
  //    bb_center[0]+= 0.02;
  bb_center[2] -= 0.03;

  bool f;

  // go and grasp (wait until it's done)
  action->enableContactDetection ();
  action->pushAction ("open_hand");
  action->grasp (bb_center, graspOrien, graspDisp, xv);
  action->checkActionsDone (f, true);
  action->areFingersInPosition (f);
}

void
BehaviorModule::graspLiftAndRelease (Vector bb_center, Vector bb_target_center, const double shift, bool palm_upward)
{
  grasp (bb_center);
  lift (shift);
  release (bb_target_center, palm_upward);
}

void
BehaviorModule::home ()
{
  Vector home_orient = angleXZToVectorAngle (-2 * PI / 5, PI);
  action->pushAction (home_x, home_orient);
}

void
BehaviorModule::tuckArms ()
{
  bool left_arm_cart_solver_active = false;
  bool right_arm_cart_solver_active = false;
  if (Network::isConnected ("/actionPrimitivesMod/left_arm/position/command:o","/icub/cartesianController/left_arm/command:i"))
    left_arm_cart_solver_active = true;

  if (Network::isConnected ("/actionPrimitivesMod/right_arm/position/command:o","/icub/cartesianController/right_arm/command:i"))
    right_arm_cart_solver_active = true;

  //first disconnect cartesian solvers
  if (!sim)
  {
    if (left_arm_cart_solver_active)
    {
      Network::disconnect ("/actionPrimitivesMod/left_arm/position/command:o","/icub/cartesianController/right_arm/command:i");
      Network::disconnect ("/actionPrimitivesMod/left_arm/position/rpc:o", "/icub/left_arm/rpc:i");
      Network::disconnect ("/icub/left_arm/state:o","/actionPrimitivesMod/left_arm/position/state:i");
//      driver_left.open (options_left);

    }

    if (right_arm_cart_solver_active)
    {
      Network::disconnect ("/actionPrimitivesMod/right_arm/position/command:o","/icub/cartesianController/right_arm/command:i");
      Network::disconnect ("/actionPrimitivesMod/right_arm/position/rpc:o", "/icub/right_arm/rpc:i");
      Network::disconnect ("/icub/right_arm/state:o","/actionPrimitivesMod/right_arm/position/state:i");
//      driver_right.open (options_right);
    }
  }
  else
  {
    //TODO for simulator

  }
  std::cout<<" ************** "<<std::endl;
  //now send target joint angles, assuming that it is already safe to do so
  encoders_left->getEncoders (positions_left_enc.data ());
  std::cout<<" --------------- "<<std::endl;
  encoders_right->getEncoders (positions_right_enc.data ());
  std::cout<<" !!!!!!!!!!!!!!! "<<std::endl;

  //assuming that both arms having the same #DOF
  for (uint i = 0; i < positions_left_enc.size (); i++)
  {
    pos_ctrl_left->setRefSpeed (i, 10.0);
    pos_ctrl_right->setRefSpeed (i, 10.0);

    pos_ctrl_left->setRefAcceleration (i, 50.0);
    pos_ctrl_right->setRefAcceleration (i, 50.0);
  }
  //set command positions
  Vector js;
  js.resize (positions_right_enc.size ());
  std::cout<<" ************** "<<js.size()<<" ************** "<<std::endl;
  js[0] = 10;
  js[1] = 20;
  js[2] = -30;
  js[3] = 60;
  js[4] = -60;
  js[5] = 0;
  js[6] = 20;
    for (uint i = 7; i < js.size (); i++)
      js[i] = positions_left_enc[i];
    pos_ctrl_left->positionMove (js.data ());

    for (uint i = 7; i < js.size (); i++)
      js[i] = positions_right_enc[i];
    pos_ctrl_right->positionMove (js.data ());

  bool done = false;
  while (!done && left_arm_cart_solver_active)
  {
    pos_ctrl_left->checkMotionDone (&done);
    Time::delay (0.001);
  }

  done = false;
  while (!done && right_arm_cart_solver_active)
  {
    pos_ctrl_right->checkMotionDone (&done);
    Time::delay (0.001);
  }

  //finally connect cartesian solvers back
  if (!sim)
  {
    if (left_arm_cart_solver_active)
    {

//      driver_left.close();

      Network::connect ("/actionPrimitivesMod/left_arm/position/command:o","/icub/cartesianController/right_arm/command:i");
      Network::connect ("/actionPrimitivesMod/left_arm/position/rpc:o", "/icub/left_arm/rpc:i");
      Network::connect ("/icub/left_arm/state:o","/actionPrimitivesMod/left_arm/position/state:i");
    }

    if (right_arm_cart_solver_active)
    {
//      driver_right.close();

      Network::connect ("/actionPrimitivesMod/right_arm/position/command:o","/icub/cartesianController/right_arm/command:i");
      Network::connect ("/actionPrimitivesMod/right_arm/position/rpc:o", "/icub/right_arm/rpc:i");
      Network::connect ("/icub/right_arm/state:o","/actionPrimitivesMod/right_arm/position/state:i");
    }
  }
  else
  {
    //TODO for simulator
  }
}

int
BehaviorModule::voiceCommand (Vector bb_center, Vector bb_dims)
{
  Bottle* b = port_simon_in.read (true);
  int cmd = b->get (0).asInt ();

  //    if(new_cmd_data_rcvd)
  //    {
  //      new_cmd_data_rcvd= false;
  //      cmd = cmd_id;
  //    }

  bool f;

  if (cmd == 1)//icub do push_left
  {
    push (bb_center, bb_dims, PI, 0.10);
  }
  else if (cmd == 2)//icub do push_right
  {
    push (bb_center, bb_dims, 0, 0.10);
  }
  else if (cmd == 3)//icub do rest
  {
    action->disableContactDetection ();
    action->pushAction (home_x, "open_hand");
    action->checkActionsDone (f, true);
    action->enableContactDetection ();
  }
  else if (cmd == 4)//icub do push_forward
  {
    push (bb_center, bb_dims, PI / 2, 0.10);
  }
  else if (cmd == 5)//icub do push_backward
  {
    push (bb_center, bb_dims, 3 * PI / 2, 0.10);
  }
  else if (cmd == 6)//icub do point
  {
    bb_center[0] = -0.5;
    bb_center[1] = -0.2;
    bb_center[2] = 0.1;
    pointTo (bb_center);
  }
  else if (cmd == 7)//icub do hide
  {
    hide (bb_center, bb_dims);
  }
  /*
   else if(s == "icub do pushforward")
   cmd = 4;
   else if(s == "icub do pushbackward")
   cmd = 5;
   else if(s == "icub do point")
   cmd = 6;
   else if(s == "icub do hide")
   cmd = 7;
   */
}

int
BehaviorModule::manualCommand (Vector bb_center, Vector bb_dims)
{
  int op_type;
  bool invalid_op = false;
  int push_direction;

  //    createObject (0, bb_center);
  //    pointTo (bb_center);

  bool f;

  while (true)
  {

    std::cout << "Select the behavior number: " << std::endl;
    std::cout << "Behavior #1: Point to the specified object" << std::endl;
    std::cout << "Behavior #2: Grasp the specified object" << std::endl;
    std::cout << "Behavior #3: Lift the specified object" << std::endl;
    std::cout << "Behavior #4: Release the specified object" << std::endl;
    std::cout << "Behavior #5: Hide the specified object" << std::endl;
    std::cout << "Behavior #6: Push the specified object" << std::endl;
    std::cout << "Behavior #7: Grasp, lift and release the specified object" << std::endl;
    std::cout << "Behavior #8: Shake the specified object" << std::endl;
    std::cout << "Behavior #9: Basket the specified object" << std::endl;
    std::cout << "Behavior #10: Go to home" << std::endl;
    std::cout << "Behavior #11: push through tunnel" << std::endl;
    if (invalid_op)
      std::cout << "INVALID BEHAVIOR NUMBER. Please re-enter the behavior number: ";
    else
      std::cout << "Enter number: ";
    std::cin >> op_type;

    deleteObject ();
    Vector v;
    switch (op_type)
    {
      case 1:
        createObject (0, bb_center);
        pointTo (bb_center);
        break;
      case 2:
        createObject (1, bb_center);
        grasp (bb_center);
        break;
      case 3:
        lift (0.25);
        break;
      case 4:
        release (bb_center);
        deleteObject ();
        break;
      case 5:
        createObject (1, bb_center);
        hide (bb_center, bb_dims);
        deleteObject ();
        break;
      case 6:
        createObject (1, bb_center);
        std::cout << "Enter which direction you want to push the object (forward=1, backward=2, left=3, right=4): ";
        std::cin >> push_direction;
        if (push_direction == 1)
        {
          push (bb_center, bb_dims, PI / 2, 0.10);
          action->disableContactDetection ();
          action->pushAction (home_x, "open_hand");
          action->checkActionsDone (f, true);
          action->enableContactDetection ();
        }
        else if (push_direction == 2)
        {
          push (bb_center, bb_dims, 3 * PI / 2, 0.10);
          action->disableContactDetection ();
          action->pushAction (home_x, "open_hand");
          action->checkActionsDone (f, true);
          action->enableContactDetection ();
        }
        else if (push_direction == 3)
        {
          push (bb_center, bb_dims, PI, 0.10);
          action->disableContactDetection ();
          action->pushAction (home_x, "open_hand");
          action->checkActionsDone (f, true);
          action->enableContactDetection ();
        }
        else if (push_direction == 4)
        {
          push (bb_center, bb_dims, 0, 0.10);
          action->disableContactDetection ();
          action->pushAction (home_x, "open_hand");
          action->checkActionsDone (f, true);
          action->enableContactDetection ();
        }
        deleteObject ();
        break;
      case 7:
        createObject (1, bb_center);
        v = bb_center;
        v[2] += 0.10;
        graspLiftAndRelease (bb_center, v, 0.10, true);
        break;
      case 8:
        break;
      case 9:
        createObject (1, bb_center);
        basket (bb_center, bb_dims);
        break;
      case 10:
        home ();
        break;
      case 11:
        createObject (2, bb_center);
        action->disableContactDetection ();
        push (bb_center, bb_dims, 0, 0.10, true);
        action->pushAction (home_x, "open_hand");
        action->checkActionsDone (f, true);
        action->enableContactDetection ();
        break;
      default:
        invalid_op = true;
    }
    if (!invalid_op)
      break;
  }

  bool done;
  action->checkActionsDone (done, true);
  std::cout << "ACTIONS DONE" << std::endl;
}

//from top or sides (left, right, back)
void
BehaviorModule::fetch (Vector bb_center, Vector bb_dims, Vector hand_orient)
{
  grasp (bb_center);
  home ();
}

void
BehaviorModule::shake (const Vector dir, const double shake_off)
{
  Vector delta_point (3);
  delta_point[0] = dir[0] * shake_off;
  delta_point[1] = dir[1] * shake_off;
  delta_point[2] = dir[2] * shake_off;

  Vector hand_pos;
  Vector hand_orient;
  action->getPose (hand_pos, hand_orient);

  for (uint i = 0; i < 10; i++)
  {
    action->pushAction (hand_pos + delta_point, 3);
    action->pushAction (hand_pos - delta_point, 3);
  }
}

void
BehaviorModule::basket (Vector bb_center, Vector bb_dims)
{
  //basket position: -0.45 w-0 0.0016

  Vector basket_pos (3);
  double lift_off = 0.15;
  basket_pos[0] = -0.45 - 0.03;//0.03 for palm
  basket_pos[1] = 0.0;
  basket_pos[2] = 0.0016 + lift_off;
  graspLiftAndRelease (bb_center, basket_pos, lift_off, false);
}

void
BehaviorModule::testHandSequences ()
{
  bool f;

  action->pushAction ("close_hand");
  action->checkActionsDone (f, true);
  action->areFingersInPosition (f);

  action->pushAction ("open_hand");
  action->checkActionsDone (f, true);
  action->areFingersInPosition (f);

  action->pushAction ("close_hand");
  action->checkActionsDone (f, true);
  action->areFingersInPosition (f);

  action->pushAction ("karate_hand");
  action->checkActionsDone (f, true);
  action->areFingersInPosition (f);
}

void
BehaviorModule::createObject (int op_type, Vector& bb_center)
{
  Bottle& obj = port_sim_rpc_out.prepare ();
  obj.clear ();
  string object_type;
  double a, b, c;
  double r;
  double red, green, blue;
  double x, y, z;
  int side;

  y = 0.53;
  r = 0.03;
  b = c = 0.06;
  a = 0.04;
  red = 1;
  green = blue = 0;

  std::cout << "Select the object type (sph, box): ";
  std::cin >> object_type;

  if (op_type == 0)
  { // point to behaviour
    std::cout << "Which side of the table (far=0, near=1): ";
    std::cin >> side;

    if (side == 0)
    {
      z = 0.6;
    }
    else
    {
      z = 0.3;
    }
    x = 0.3;
  }
  else if (op_type == 1)
  {
    std::cout << "Which side of the table (left=0, middle=1, right=2): ";
    std::cin >> side;
    if (side == 0)
    {
      x = 0.2;
      //        x = 0.5;
    }
    else if (side == 1)
    {
      x = 0;
    }
    else
    {
      x = -0.1;
    }
    z = 0.3;
    //      z = 0.6;
  }
  else
  {
    x = 0.10;
    z = 0.35;
  }

  obj.addString ("world");
  obj.addString ("mk");
  obj.addString (object_type.c_str ());

  if (object_type == "sph")
  {
    obj.addDouble (r);
  }
  else
  {
    obj.addDouble (a);
    obj.addDouble (b);
    obj.addDouble (c);
  }

  obj.addDouble (x);
  obj.addDouble (y);
  obj.addDouble (z);

  bb_center[0] = -z - 0.04;
  bb_center[1] = -x - 0.02;
  bb_center[2] = y - 0.5484;

  obj.addDouble (red);
  obj.addDouble (green);
  obj.addDouble (blue);

  port_sim_rpc_out.write ();
}

void
BehaviorModule::deleteObject ()
{
  Bottle& obj = port_sim_rpc_out.prepare ();
  obj.clear ();

  obj.addString ("world");
  obj.addString ("del");
  obj.addString ("all");

  port_sim_rpc_out.write ();
}

// we don't need a thread since the actions library already
// incapsulates one inside dealing with all the tight time constraints
bool
BehaviorModule::updateModule ()
{

  // do it only once
  if (firstRun)
  {
    init ();
    firstRun = false;
    tuckArms();
  }

  Vector bb_dims (3);
  bb_dims[0] = 0.06;
  bb_dims[1] = 0.06;
  bb_dims[2] = 0.06;

  // get a target object position from a YARP port
  //Bottle *b = inPort.read (); // blocking call

  Vector bb_center (3);
  bb_center[0] = -0.3;
  bb_center[1] = -0.2;
  bb_center[2] = 0.1;

  menu_mutex.wait ();
  //    voiceCommand (bb_center, bb_dims);

  menu_mutex.post ();

  return true;

  /*
   // do it only once
   if (firstRun)
   {
   init ();
   firstRun = false;
   }

   Vector bb_dims (3);
   bb_dims[0] = 0.06;
   bb_dims[1] = 0.06;
   bb_dims[2] = 0.06;

   // get a target object position from a YARP port
   //Bottle *b = inPort.read (); // blocking call

   Vector bb_center (3);

   int op_type;
   bool invalid_op = false;
   int push_direction;

   //    createObject (0, bb_center);
   //    pointTo (bb_center);

   bool f;
   menu_mutex.wait ();

   while (true)
   {
   std::cout << "Select the behavior number: " << std::endl;
   std::cout << "Behavior #1: Point to the specified object" << std::endl;
   std::cout << "Behavior #2: Grasp the specified object" << std::endl;
   std::cout << "Behavior #3: Lift the specified object" << std::endl;
   std::cout << "Behavior #4: Release the specified object" << std::endl;
   std::cout << "Behavior #5: Hide the specified object" << std::endl;
   std::cout << "Behavior #6: Push the specified object" << std::endl;
   std::cout << "Behavior #7: Grasp, lift and release the specified object" << std::endl;
   std::cout << "Behavior #8: Shake the specified object" << std::endl;
   std::cout << "Behavior #9: Basket the specified object" << std::endl;
   std::cout << "Behavior #10: Go to home" << std::endl;
   std::cout << "Behavior #11: push through tunnel" << std::endl;
   if (invalid_op)
   std::cout << "INVALID BEHAVIOR NUMBER. Please re-enter the behavior number: ";
   else
   std::cout << "Enter number: ";
   std::cin >> op_type;

   deleteObject ();
   Vector v;
   switch (op_type)
   {
   case 1:
   createObject (0, bb_center);
   pointTo (bb_center);
   break;
   case 2:
   createObject (1, bb_center);
   grasp (bb_center);
   break;
   case 3:
   lift (0.25);
   break;
   case 4:
   release (bb_center);
   deleteObject ();
   break;
   case 5:
   createObject (1, bb_center);
   hide (bb_center, bb_dims);
   deleteObject ();
   break;
   case 6:
   createObject (1, bb_center);
   std::cout << "Enter which direction you want to push the object (forward=1, backward=2, left=3, right=4): ";
   std::cin >> push_direction;
   if (push_direction == 1)
   {
   push (bb_center, bb_dims, PI / 2, 0.10);
   action->disableContactDetection ();
   action->pushAction (home_x, "open_hand");
   action->checkActionsDone (f, true);
   action->enableContactDetection ();
   }
   else if (push_direction == 2)
   {
   push (bb_center, bb_dims, 3 * PI / 2, 0.10);
   action->disableContactDetection ();
   action->pushAction (home_x, "open_hand");
   action->checkActionsDone (f, true);
   action->enableContactDetection ();
   }
   else if (push_direction == 3)
   {
   push (bb_center, bb_dims, PI, 0.10);
   action->disableContactDetection ();
   action->pushAction (home_x, "open_hand");
   action->checkActionsDone (f, true);
   action->enableContactDetection ();
   }
   else if (push_direction == 4)
   {
   push (bb_center, bb_dims, 0, 0.10);
   action->disableContactDetection ();
   action->pushAction (home_x, "open_hand");
   action->checkActionsDone (f, true);
   action->enableContactDetection ();
   }
   deleteObject ();
   break;
   case 7:
   createObject (1, bb_center);
   v = bb_center;
   v[2] += 0.10;
   graspLiftAndRelease (bb_center, v, 0.10, true);
   break;
   case 8:
   break;
   case 9:
   createObject (1, bb_center);
   basket (bb_center, bb_dims);
   break;
   case 10:
   home ();
   break;
   case 11:
   createObject (2, bb_center);
   action->disableContactDetection ();
   push (bb_center, bb_dims, 0, 0.10, true);
   action->pushAction (home_x, "open_hand");
   action->checkActionsDone (f, true);
   action->enableContactDetection ();
   break;
   default:
   invalid_op = true;
   }
   if (!invalid_op)
   break;
   }

   bool done;
   action->checkActionsDone (done, true);
   std::cout << "ACTIONS DONE" << std::endl;

   menu_mutex.post ();

   return true;
   */
}

bool
BehaviorModule::interruptModule ()
{
  // since a call to checkActionsDone() blocks
  // the execution until it's done, we need to
  // take control and exit from the waiting state
  action->syncCheckInterrupt (true);

  inPort.interrupt ();
  rpcPort.interrupt ();

  return true;
}
