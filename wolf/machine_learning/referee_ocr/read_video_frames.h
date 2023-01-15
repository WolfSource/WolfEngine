#ifndef READ_VIDEO_FRAMES_H
#define READ_VIDEO_FRAMES_H

#include <iostream>
#include <opencv2/opencv.hpp>

#include "dlib_export.h"

//! read frames of the video class.
/*! \brief This class is responsible for reading the frame of the input video.

	This class contains functions, structures, and variables that use for reading
	the frames of the input video.
*/
class read_video_frames
{
  public:
	/*!<The path of the video.*/
	std::string path;
	/*!<Video Capture APIs.*/
	cv::VideoCapture cap;
	/*!<The frame number in the video frames.*/
	int frame_number = 0;
	/*!<Number of frames of the video.*/
	double frame_count;

	/*!
	The constructor of the class.

	\param  video_path    The path of video.
  */
	W_SYSTEM_EXPORT read_video_frames(std::string video_path);
	/*!
	The read_video_frame_by_frame function extracts frames of the input video one by one in order.

	\return    The next frame of video
  */
	W_SYSTEM_EXPORT cv::Mat read_video_frame_by_frame();
	/*!
	The write_image_to_video function puts related images next to each other for creating a video.

	\param  out_video_path    The path of output video.
  */
	W_SYSTEM_EXPORT void write_image_to_video(std::string out_video_path);
	/*!
	The read_specific_frame function reads a specific frame of the video.

	\param  input_frame_number     Frame number of desired frame in the video frames.
	\return    Specific frame of the video
  */
	W_SYSTEM_EXPORT cv::Mat read_specific_frame(int input_frame_number);
	/*!
	The get_current_frame_number function returns the current frame number.

	\return    Current frame number.
  */
	W_SYSTEM_EXPORT int get_current_frame_number();
	/*!
	The get_frame_amount function returns the number of frames of the video.

	\return    Number of frames of the video.
  */
	W_SYSTEM_EXPORT double get_frame_amount();
	/*!
	The video_player function implements a simple video player based on OpenCV.
  */
	[[noreturn]] void video_player();
	/*!
	The release function call cv::VideoCapture::release() function.
  */
	void release();
};

#endif	// READ_VIDEO_FRAMES_H
