/* -------------------------------
 * Node:    merge_arrays_node
 * Package: merge_arrays
 * Author:  Matthew Kasper
 * Email:   mtkasper@wisc.edu
 * ------------------------------- */

#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32_multi_array.hpp"

/*
 * Takes two arrays from /input/array1 and /input/array2 topics and publishes a sorted,
 * merged array of the two arrays to /output/array
 */
class MergeArraysNode : public rclcpp::Node {
    public:
        MergeArraysNode() : Node("merge_arrays_node") {
            // Initialize the subscribers
            array1Subscription_ = this->create_subscription<std_msgs::msg::Int32MultiArray>(
                    "/input/array1", 10, std::bind(&MergeArraysNode::array1CallBack, this, std::placeholders::_1));

            array2Subscription_ = this->create_subscription<std_msgs::msg::Int32MultiArray>(
                    "/input/array2", 10, std::bind(&MergeArraysNode::array2CallBack, this, std::placeholders::_1));

            // Initialize the publisher
            mergedArrayPublisher_ = this->create_publisher<std_msgs::msg::Int32MultiArray>("/output/array", 10);
        }

    private:
        // Updates array values and publishes a new merged array
        void array1CallBack(const std_msgs::msg::Int32MultiArray & msg) const {
            // Stores array data in a vector
            RCLCPP_INFO(this->get_logger(), "Received: ");
            printVector(msg.data);
            
            lastArray1 = msg.data;
            mergeAndPublish();
        }

        // Updates array values and publishes a new merged array
        void array2CallBack(const std_msgs::msg::Int32MultiArray & msg) const {
            // Stores array data in a vector
            RCLCPP_INFO(this->get_logger(), "Received: ");
            printVector(msg.data);

            lastArray2 = msg.data;
            mergeAndPublish();
        }

        // Merged and sorts the two held arrays
        void mergeAndPublish() const {
            std::vector<int32_t> mergedArray;
            // Copy elements from both arrays into the first empty slot
            
            std::copy(lastArray1.begin(), lastArray1.end(), std::back_inserter(mergedArray));
            std::copy(lastArray2.begin(), lastArray2.end(), std::back_inserter(mergedArray));

            // Sort the merged array
            std::sort(std::begin(mergedArray), std::end(mergedArray));

            // Convert to ROS2 Message and load merged array into message
            auto message = std_msgs::msg::Int32MultiArray();
            message.data = mergedArray;
            
            // Load in dimension data
            message.layout.dim.push_back(std_msgs::msg::MultiArrayDimension()); 
            message.layout.dim[0].size = mergedArray.size();
            message.layout.dim[0].stride = mergedArray.size();
            message.layout.dim[0].label = "/output/array";

            RCLCPP_INFO(this->get_logger(), "Published: ");
            printVector(message.data);
    
            mergedArrayPublisher_->publish(message);
        }

        void printVector(std::vector<int32_t> vector) const {
            std::string vectorString;
            
            for(int32_t integer : vector) {
                vectorString += std::to_string(integer) + " ";
            }

            RCLCPP_INFO(this->get_logger(), "%s", vectorString.c_str());
        }

        // Create the subscription fields
        rclcpp::Subscription<std_msgs::msg::Int32MultiArray>::SharedPtr array1Subscription_;
        rclcpp::Subscription<std_msgs::msg::Int32MultiArray>::SharedPtr array2Subscription_;

        // Create the publisher field
        rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr mergedArrayPublisher_;

        // Create caches that store the last known input arrays
        mutable std::vector<int32_t> lastArray1;
        mutable std::vector<int32_t> lastArray2;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MergeArraysNode>());
    rclcpp::shutdown();
    return 0;
}