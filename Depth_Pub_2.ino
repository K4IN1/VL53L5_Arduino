#include <ros.h>
#include <ros/time.h>
#include "I2CScanner.hpp"
#include "VL53L5_MOD.hpp"
#include <Wire.h>
#include <sensor_msgs/Image.h>
#include "math.h"
// #include <std_msgs/Int16MultiArray.h>
#include <sensor_msgs/CameraInfo.h>

// 实例化nh和信息对象
ros::NodeHandle nh;
sensor_msgs::Image img_1;
sensor_msgs::Image img_2;

ros::Publisher image_pub1("depth_image1", &img_1);
ros::Publisher image_pub2("depth_image2", &img_2);

sensor_msgs::CameraInfo cam_info1;
sensor_msgs::CameraInfo cam_info2;

ros::Publisher info_pub1("cam_info1", &cam_info1);
ros::Publisher info_pub2("cam_info2", &cam_info2);

ros::Time syncstamp;
// 确定LPN的PIN
const uint8_t LPn_1 = D0;
const uint8_t LPn_2 = D1;
// 生成ToF对应的实例和数据
byte tofAddress1 = 0x52;
// byte tofAddress2 = 0x29;
VL53L5CX_MOD tof1(LPn_1);
// VL53L5CX_MOD tof2(LPn_2);
VL53L5CX_ResultsData measurementData1;
// VL53L5CX_ResultsData measurementData2;
sensor_msgs::Image::_data_type image_raw[128];  
// std_msgs::Int16MultiArray::_data_type data_raw[64];
const char frame1[] = "sensor1";
const char frame2[] = "sensor2";
// 生成相机信息
sensor_msgs::CameraInfo get_camera_info(const char frame[]) {
  sensor_msgs::CameraInfo msg;
  msg.header.frame_id = frame;
  msg.height = 8;
  msg.width = 8;
  msg.distortion_model = "plumb_bob";
  msg.D_length = 5;
  sensor_msgs::CameraInfo::_D_type D[5] = { 0, 0, 0, 0, 0 };

  float fov = 45;
  float c = (msg.width - 1) / 2.0;
  float f = c / tan(fov * M_PI / 180.0 / 2.0);
  float K[9] = { f, 0, c, 0, f, c, 0, 0, 1 };
  float P[12] = { f, 0, c, 0, 0, f, c, 0, 0, 0, 1, 0 };
  float R[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
  // 该类型需要直接赋值
  msg.D = D;
  // 该类型需要循环赋值
  for (int i = 0; i < 9; i++) {
    msg.K[i] = K[i];
    msg.R[i] = R[i];
  }

  for (int i = 0; i < 12; i++) {
    msg.P[i] = P[i];
  }
  return msg;
}

// 设置ToF传感器参数，已验证
void set_tof_param(VL53L5CX_MOD &sensor) {
  sensor.setRangingFrequency(15);
  sensor.setResolution(8 * 8);
  sensor.setSharpenerPercent(20);
  sensor.setRangingMode(SF_VL53L5CX_RANGING_MODE::CONTINUOUS);
  // sensor.setIntegrationTime(50); 
}
// 设置ROS数据格式,已验证
void set_ros_data(sensor_msgs::Image &img, const char frame[]) {
  img.header.frame_id = frame;
  img.data_length = 128;
  img.height = 8;
  img.width = 8;
  img.encoding = "16UC1";
  img.step = 16;
}
// 传感器数据转换为ROS数据,已验证
void tof_data_to_ros(sensor_msgs::Image &img, VL53L5CX_MOD &sensor) {
  if (sensor.isDataReady()) {
    uint8_t j = 0;
    int16_t distance;
    sensor.getRangingData(&measurementData1);
    for (int y = 0;y <= 8 * 7;y += 8) {
      for (int x = 0;x <= 7;x++) {
        switch (measurementData1.target_status[x + y]) {
        case 5:
          distance = measurementData1.distance_mm[x + y];
          break;
        default:
          distance = 2000;
          break;
        }
        image_raw[j] = distance & 0xFF;
        image_raw[j + 1] = distance >> 8;
        j += 2;
      }
    }
    img.data = image_raw;
  }

}

void setup() {
  nh.initNode();
  nh.advertise(image_pub1);
  nh.advertise(info_pub1);
  Wire.begin();
  Wire.setClock(1000000);

  I2CScanner::scan(Wire);
  tof1.disable();
  tof1.enable();
  // tof2.disable();
  tof1.setaddress(tofAddress1);
  // tof1.disable();
  // tof2.setaddress(tofAddress2);
  tof1.enable();
  set_tof_param(tof1);
  tof1.startRanging();
  // tof2.startRanging();
}

void loop() {
  cam_info1 = get_camera_info(frame1);
  set_ros_data(img_1, frame1);
  syncstamp = nh.now();
  img_1.header.stamp = syncstamp;
  // img1.header.stamp = syncstamp;
  cam_info1.header.stamp = syncstamp;
  tof_data_to_ros(img_1, tof1);
  image_pub1.publish(&img_1);
  info_pub1.publish(&cam_info1);
  nh.spinOnce();
  delay(80);
}
