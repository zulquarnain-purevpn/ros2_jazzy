#ifndef RCLCPP__NEW_INTERFACE_HPP_
#define RCLCPP__NEW_INTERFACE_HPP_

#include "rclcpp/rclcpp.hpp"

namespace rclcpp
{

class NewInterface
{
public:
  virtual ~NewInterface() = default;

  virtual void do_something() = 0;
};

}  // namespace rclcpp

#endif  // RCLCPP__NEW_INTERFACE_HPP_
