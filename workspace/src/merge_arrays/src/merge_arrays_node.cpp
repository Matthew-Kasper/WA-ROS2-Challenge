/* -------------------------------
 * Node: merge_arrays_node
 * Package: merge_arrays
 * Author: Matthew Kasper
 * Email: mtkasper@wisc.edu
 * ------------------------------- */

#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

/*
 * Takes two arrays from /input/array1 and /input/array2 topics and publishes a sorted,
 * merged array of the two arrays to /output/array
 */
class MergeArraysNode : public rclcpp::Node
{
public:
    MergeArraysNode()
            : Node("merge_arrays_node"), count_(0)
    {
        publisher_ = this->create_publisher<std_msgs::msg::String>("/output/array", 10);
        timer_ = this->create_wall_timer(
                500ms, std::bind(&MergeArraysNode::timer_callback, this));
    }

private:
    void timer_callback()
    {
        auto message = std_msgs::msg::String();
        message.data = "Hello, world! " + std::to_string(count_++);
        RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
        publisher_->publish(message);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    size_t count_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MergeArraysNode>());
    rclcpp::shutdown();
    return 0;
}