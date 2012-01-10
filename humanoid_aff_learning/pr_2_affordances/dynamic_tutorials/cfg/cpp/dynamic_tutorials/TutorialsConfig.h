//#line 2 "/opt/ros/diamondback/stacks/driver_common/dynamic_reconfigure/templates/ConfigType.h"
// *********************************************************
// 
// File autogenerated for the dynamic_tutorials package 
// by the dynamic_reconfigure package.
// Please do not edit.
// 
// ********************************************************/

/***********************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2008, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 ***********************************************************/

// Author: Blaise Gassend


#ifndef __dynamic_tutorials__TUTORIALSCONFIG_H__
#define __dynamic_tutorials__TUTORIALSCONFIG_H__

#include <dynamic_reconfigure/config_tools.h>
#include <limits>
#include <ros/node_handle.h>
#include <dynamic_reconfigure/ConfigDescription.h>
#include <dynamic_reconfigure/ParamDescription.h>
#include <dynamic_reconfigure/config_init_mutex.h>

namespace dynamic_tutorials
{
  class TutorialsConfigStatics;
  
  class TutorialsConfig
  {
  public:
    class AbstractParamDescription : public dynamic_reconfigure::ParamDescription
    {
    public:
      AbstractParamDescription(std::string n, std::string t, uint32_t l, 
          std::string d, std::string e)
      {
        name = n;
        type = t;
        level = l;
        description = d;
        edit_method = e;
      }
      
      virtual void clamp(TutorialsConfig &config, const TutorialsConfig &max, const TutorialsConfig &min) const = 0;
      virtual void calcLevel(uint32_t &level, const TutorialsConfig &config1, const TutorialsConfig &config2) const = 0;
      virtual void fromServer(const ros::NodeHandle &nh, TutorialsConfig &config) const = 0;
      virtual void toServer(const ros::NodeHandle &nh, const TutorialsConfig &config) const = 0;
      virtual bool fromMessage(const dynamic_reconfigure::Config &msg, TutorialsConfig &config) const = 0;
      virtual void toMessage(dynamic_reconfigure::Config &msg, const TutorialsConfig &config) const = 0;
    };

    typedef boost::shared_ptr<AbstractParamDescription> AbstractParamDescriptionPtr;
    typedef boost::shared_ptr<const AbstractParamDescription> AbstractParamDescriptionConstPtr;
    
    template <class T>
    class ParamDescription : public AbstractParamDescription
    {
    public:
      ParamDescription(std::string name, std::string type, uint32_t level, 
          std::string description, std::string edit_method, T TutorialsConfig::* f) :
        AbstractParamDescription(name, type, level, description, edit_method),
        field(f)
      {}

      T (TutorialsConfig::* field);

      virtual void clamp(TutorialsConfig &config, const TutorialsConfig &max, const TutorialsConfig &min) const
      {
        if (config.*field > max.*field)
          config.*field = max.*field;
        
        if (config.*field < min.*field)
          config.*field = min.*field;
      }

      virtual void calcLevel(uint32_t &comb_level, const TutorialsConfig &config1, const TutorialsConfig &config2) const
      {
        if (config1.*field != config2.*field)
          comb_level |= level;
      }

      virtual void fromServer(const ros::NodeHandle &nh, TutorialsConfig &config) const
      {
        nh.getParam(name, config.*field);
      }

      virtual void toServer(const ros::NodeHandle &nh, const TutorialsConfig &config) const
      {
        nh.setParam(name, config.*field);
      }

      virtual bool fromMessage(const dynamic_reconfigure::Config &msg, TutorialsConfig &config) const
      {
        return dynamic_reconfigure::ConfigTools::getParameter(msg, name, config.*field);
      }

      virtual void toMessage(dynamic_reconfigure::Config &msg, const TutorialsConfig &config) const
      {
        dynamic_reconfigure::ConfigTools::appendParameter(msg, name, config.*field);
      }
    };

//#line 10 "../cfg/Tutorials.cfg"
      int params;
//#line 11 "../cfg/Tutorials.cfg"
      int int_param;
//#line 12 "../cfg/Tutorials.cfg"
      double double_param;
//#line 14 "../cfg/Tutorials.cfg"
      std::string str_param;
//#line 16 "../cfg/Tutorials.cfg"
      bool bool_param;
//#line 24 "../cfg/Tutorials.cfg"
      int size;
//#line 138 "/opt/ros/diamondback/stacks/driver_common/dynamic_reconfigure/templates/ConfigType.h"

    bool __fromMessage__(dynamic_reconfigure::Config &msg)
    {
      const std::vector<AbstractParamDescriptionConstPtr> &__param_descriptions__ = __getParamDescriptions__();
      int count = 0;
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator i = __param_descriptions__.begin(); i != __param_descriptions__.end(); i++)
        if ((*i)->fromMessage(msg, *this))
          count++;
      if (count != dynamic_reconfigure::ConfigTools::size(msg))
      {
        ROS_ERROR("TutorialsConfig::__fromMessage__ called with an unexpected parameter.");
        ROS_ERROR("Booleans:");
        for (unsigned int i = 0; i < msg.bools.size(); i++)
          ROS_ERROR("  %s", msg.bools[i].name.c_str());
        ROS_ERROR("Integers:");
        for (unsigned int i = 0; i < msg.ints.size(); i++)
          ROS_ERROR("  %s", msg.ints[i].name.c_str());
        ROS_ERROR("Doubles:");
        for (unsigned int i = 0; i < msg.doubles.size(); i++)
          ROS_ERROR("  %s", msg.doubles[i].name.c_str());
        ROS_ERROR("Strings:");
        for (unsigned int i = 0; i < msg.strs.size(); i++)
          ROS_ERROR("  %s", msg.strs[i].name.c_str());
        // @todo Check that there are no duplicates. Make this error more
        // explicit.
        return false;
      }
      return true;
    }

    // This version of __toMessage__ is used during initialization of
    // statics when __getParamDescriptions__ can't be called yet.
    void __toMessage__(dynamic_reconfigure::Config &msg, const std::vector<AbstractParamDescriptionConstPtr> &__param_descriptions__) const
    {
      dynamic_reconfigure::ConfigTools::clear(msg);
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator i = __param_descriptions__.begin(); i != __param_descriptions__.end(); i++)
        (*i)->toMessage(msg, *this);
    }
    
    void __toMessage__(dynamic_reconfigure::Config &msg) const
    {
      const std::vector<AbstractParamDescriptionConstPtr> &__param_descriptions__ = __getParamDescriptions__();
      __toMessage__(msg, __param_descriptions__);
    }
    
    void __toServer__(const ros::NodeHandle &nh) const
    {
      const std::vector<AbstractParamDescriptionConstPtr> &__param_descriptions__ = __getParamDescriptions__();
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator i = __param_descriptions__.begin(); i != __param_descriptions__.end(); i++)
        (*i)->toServer(nh, *this);
    }

    void __fromServer__(const ros::NodeHandle &nh)
    {
      const std::vector<AbstractParamDescriptionConstPtr> &__param_descriptions__ = __getParamDescriptions__();
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator i = __param_descriptions__.begin(); i != __param_descriptions__.end(); i++)
        (*i)->fromServer(nh, *this);
    }

    void __clamp__()
    {
      const std::vector<AbstractParamDescriptionConstPtr> &__param_descriptions__ = __getParamDescriptions__();
      const TutorialsConfig &__max__ = __getMax__();
      const TutorialsConfig &__min__ = __getMin__();
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator i = __param_descriptions__.begin(); i != __param_descriptions__.end(); i++)
        (*i)->clamp(*this, __max__, __min__);
    }

    uint32_t __level__(const TutorialsConfig &config) const
    {
      const std::vector<AbstractParamDescriptionConstPtr> &__param_descriptions__ = __getParamDescriptions__();
      uint32_t level = 0;
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator i = __param_descriptions__.begin(); i != __param_descriptions__.end(); i++)
        (*i)->calcLevel(level, config, *this);
      return level;
    }
    
    static const dynamic_reconfigure::ConfigDescription &__getDescriptionMessage__();
    static const TutorialsConfig &__getDefault__();
    static const TutorialsConfig &__getMax__();
    static const TutorialsConfig &__getMin__();
    static const std::vector<AbstractParamDescriptionConstPtr> &__getParamDescriptions__();
    
  private:
    static const TutorialsConfigStatics *__get_statics__();
  };
  
  template <> // Max and min are ignored for strings.
  inline void TutorialsConfig::ParamDescription<std::string>::clamp(TutorialsConfig &config, const TutorialsConfig &max, const TutorialsConfig &min) const
  {
    return;
  }

  class TutorialsConfigStatics
  {
    friend class TutorialsConfig;
    
    TutorialsConfigStatics()
    {
//#line 10 "../cfg/Tutorials.cfg"
      __min__.params = 0;
//#line 10 "../cfg/Tutorials.cfg"
      __max__.params = 10;
//#line 10 "../cfg/Tutorials.cfg"
      __default__.params = 10;
//#line 10 "../cfg/Tutorials.cfg"
      __param_descriptions__.push_back(TutorialsConfig::AbstractParamDescriptionConstPtr(new TutorialsConfig::ParamDescription<int>("params", "int", 0, "Parameters", "", &TutorialsConfig::params)));
//#line 11 "../cfg/Tutorials.cfg"
      __min__.int_param = 0;
//#line 11 "../cfg/Tutorials.cfg"
      __max__.int_param = 100;
//#line 11 "../cfg/Tutorials.cfg"
      __default__.int_param = 50;
//#line 11 "../cfg/Tutorials.cfg"
      __param_descriptions__.push_back(TutorialsConfig::AbstractParamDescriptionConstPtr(new TutorialsConfig::ParamDescription<int>("int_param", "int", 1, "An Integer parameter", "", &TutorialsConfig::int_param)));
//#line 12 "../cfg/Tutorials.cfg"
      __min__.double_param = 0.0;
//#line 12 "../cfg/Tutorials.cfg"
      __max__.double_param = 1.0;
//#line 12 "../cfg/Tutorials.cfg"
      __default__.double_param = 0.5;
//#line 12 "../cfg/Tutorials.cfg"
      __param_descriptions__.push_back(TutorialsConfig::AbstractParamDescriptionConstPtr(new TutorialsConfig::ParamDescription<double>("double_param", "double", 1, "A double parameter", "", &TutorialsConfig::double_param)));
//#line 14 "../cfg/Tutorials.cfg"
      __min__.str_param = "";
//#line 14 "../cfg/Tutorials.cfg"
      __max__.str_param = "";
//#line 14 "../cfg/Tutorials.cfg"
      __default__.str_param = "Hello World";
//#line 14 "../cfg/Tutorials.cfg"
      __param_descriptions__.push_back(TutorialsConfig::AbstractParamDescriptionConstPtr(new TutorialsConfig::ParamDescription<std::string>("str_param", "str", 1, "A string parameter", "", &TutorialsConfig::str_param)));
//#line 16 "../cfg/Tutorials.cfg"
      __min__.bool_param = 0;
//#line 16 "../cfg/Tutorials.cfg"
      __max__.bool_param = 1;
//#line 16 "../cfg/Tutorials.cfg"
      __default__.bool_param = 1;
//#line 16 "../cfg/Tutorials.cfg"
      __param_descriptions__.push_back(TutorialsConfig::AbstractParamDescriptionConstPtr(new TutorialsConfig::ParamDescription<bool>("bool_param", "bool", 1, "A Boolean parameter", "", &TutorialsConfig::bool_param)));
//#line 24 "../cfg/Tutorials.cfg"
      __min__.size = 0;
//#line 24 "../cfg/Tutorials.cfg"
      __max__.size = 3;
//#line 24 "../cfg/Tutorials.cfg"
      __default__.size = 1;
//#line 24 "../cfg/Tutorials.cfg"
      __param_descriptions__.push_back(TutorialsConfig::AbstractParamDescriptionConstPtr(new TutorialsConfig::ParamDescription<int>("size", "int", 0, "A size parameter which is edited via an enum", "{'enum_description': 'An enum to set size', 'enum': [{'srcline': 18, 'description': 'A small constant', 'srcfile': '../cfg/Tutorials.cfg', 'cconsttype': 'const int', 'value': 0, 'ctype': 'int', 'type': 'int', 'name': 'Small'}, {'srcline': 19, 'description': 'A medium constant', 'srcfile': '../cfg/Tutorials.cfg', 'cconsttype': 'const int', 'value': 1, 'ctype': 'int', 'type': 'int', 'name': 'Medium'}, {'srcline': 20, 'description': 'A large constant', 'srcfile': '../cfg/Tutorials.cfg', 'cconsttype': 'const int', 'value': 2, 'ctype': 'int', 'type': 'int', 'name': 'Large'}, {'srcline': 21, 'description': 'An extra large constant', 'srcfile': '../cfg/Tutorials.cfg', 'cconsttype': 'const int', 'value': 3, 'ctype': 'int', 'type': 'int', 'name': 'ExtraLarge'}]}", &TutorialsConfig::size)));
//#line 239 "/opt/ros/diamondback/stacks/driver_common/dynamic_reconfigure/templates/ConfigType.h"
    
      for (std::vector<TutorialsConfig::AbstractParamDescriptionConstPtr>::const_iterator i = __param_descriptions__.begin(); i != __param_descriptions__.end(); i++)
        __description_message__.parameters.push_back(**i);
      __max__.__toMessage__(__description_message__.max, __param_descriptions__); 
      __min__.__toMessage__(__description_message__.min, __param_descriptions__); 
      __default__.__toMessage__(__description_message__.dflt, __param_descriptions__); 
    }
    std::vector<TutorialsConfig::AbstractParamDescriptionConstPtr> __param_descriptions__;
    TutorialsConfig __max__;
    TutorialsConfig __min__;
    TutorialsConfig __default__;
    dynamic_reconfigure::ConfigDescription __description_message__;
    static const TutorialsConfigStatics *get_instance()
    {
      // Split this off in a separate function because I know that
      // instance will get initialized the first time get_instance is
      // called, and I am guaranteeing that get_instance gets called at
      // most once.
      static TutorialsConfigStatics instance;
      return &instance;
    }
  };

  inline const dynamic_reconfigure::ConfigDescription &TutorialsConfig::__getDescriptionMessage__() 
  {
    return __get_statics__()->__description_message__;
  }

  inline const TutorialsConfig &TutorialsConfig::__getDefault__()
  {
    return __get_statics__()->__default__;
  }
  
  inline const TutorialsConfig &TutorialsConfig::__getMax__()
  {
    return __get_statics__()->__max__;
  }
  
  inline const TutorialsConfig &TutorialsConfig::__getMin__()
  {
    return __get_statics__()->__min__;
  }
  
  inline const std::vector<TutorialsConfig::AbstractParamDescriptionConstPtr> &TutorialsConfig::__getParamDescriptions__()
  {
    return __get_statics__()->__param_descriptions__;
  }

  inline const TutorialsConfigStatics *TutorialsConfig::__get_statics__()
  {
    const static TutorialsConfigStatics *statics;
  
    if (statics) // Common case
      return statics;

    boost::mutex::scoped_lock lock(dynamic_reconfigure::__init_mutex__);

    if (statics) // In case we lost a race.
      return statics;

    statics = TutorialsConfigStatics::get_instance();
    
    return statics;
  }

//#line 18 "../cfg/Tutorials.cfg"
      const int Tutorials_Small = 0;
//#line 19 "../cfg/Tutorials.cfg"
      const int Tutorials_Medium = 1;
//#line 20 "../cfg/Tutorials.cfg"
      const int Tutorials_Large = 2;
//#line 21 "../cfg/Tutorials.cfg"
      const int Tutorials_ExtraLarge = 3;
}

#endif // __TUTORIALSRECONFIGURATOR_H__
