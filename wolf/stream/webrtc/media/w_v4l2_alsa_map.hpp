/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifndef WIN64
#include <dirent.h>
#include <fstream>

std::string get_device_id(const std::string& evt) {
    std::string deviceid;
    std::istringstream f(evt);
    std::string key;
    while (getline(f, key, '=')) {
            std::string value;
	    if (getline(f, value)) {
		    if ( (key =="PRODUCT") || (key == "PCI_SUBSYS_ID") ) {
			    deviceid = value;
			    break;
		    }
	    }
    }
    return deviceid;
}

std::map<std::string,std::string> get_video_devices() 
{
	std::map<std::string,std::string> videodevices;
	std::string video4linuxPath("/sys/class/video4linux");
	DIR *dp = opendir(video4linuxPath.c_str());
	if (dp != nullptr) 
    {
		struct dirent *entry = nullptr;
		while((entry = readdir(dp))) 
        {
			std::string devicename;
			std::string deviceid;
			if (strstr(entry->d_name,"video") == entry->d_name) 
            {
				std::string devicePath(video4linuxPath);
				devicePath.append("/").append(entry->d_name).append("/name");
				std::ifstream ifsn(devicePath.c_str());
				devicename = std::string(std::istreambuf_iterator<char>{ifsn}, {});
				devicename.erase(devicename.find_last_not_of("\n")+1);

				std::string ueventPath(video4linuxPath);
				ueventPath.append("/").append(entry->d_name).append("/device/uevent");
				std::ifstream ifsd(ueventPath.c_str());
				deviceid = std::string(std::istreambuf_iterator<char>{ifsd}, {});
				deviceid.erase(deviceid.find_last_not_of("\n")+1);
			}

			if (!devicename.empty() && !deviceid.empty()) 
            {
				videodevices[devicename] = get_device_id(deviceid);
			}
		}
		closedir(dp);
	}
	return videodevices;
}

std::map<std::string,std::string> get_audio_devices() 
{
	std::map<std::string,std::string> audiodevices;
	std::string audioLinuxPath("/sys/class/sound");
	DIR *dp = opendir(audioLinuxPath.c_str());
	if (dp != nullptr) 
    {
		struct dirent *entry = nullptr;
		while((entry = readdir(dp))) 
        {
			std::string devicename;
			std::string deviceid;
			if (strstr(entry->d_name,"card") == entry->d_name) 
            {
				std::string devicePath(audioLinuxPath);
				devicePath.append("/").append(entry->d_name).append("/device/uevent");	

				std::ifstream ifs(devicePath.c_str());
				std::string deviceid = std::string(std::istreambuf_iterator<char>{ifs}, {});
				deviceid.erase(deviceid.find_last_not_of("\n")+1);
				deviceid = get_device_id(deviceid);

				if (!deviceid.empty()) 
                {
					if (audiodevices.find(deviceid) == audiodevices.end()) 
                    {
						std::string audioname(entry->d_name);
						int deviceNumber = atoi(audioname.substr(strlen("card")).c_str());

						std::string devname = "audiocap://";
						devname += std::to_string(deviceNumber);
						audiodevices[deviceid] = devname;
					}
				}							
			}
		}	
		closedir(dp);	
	}
	return audiodevices;
}

std::map<std::string,std::string>  get_v4l2_alsa_map() 
{
	std::map<std::string,std::string> videoaudiomap;

	std::map<std::string,std::string> videodevices = get_video_devices();
	std::map<std::string,std::string> audiodevices = get_audio_devices();

	for (auto & id : videodevices) 
    {
		auto audioDevice = audiodevices.find(id.second);
		if (audioDevice != audiodevices.end()) 
        {
			//std::cout <<  id.first << "=>" << audioDevice->second << std::endl;
			videoaudiomap[id.first] = audioDevice->second;
		}
	}
	return videoaudiomap;
}
#else
std::map<std::string,std::string>  get_v4l2_alsa_map() 
{ 
    return std::map<std::string,std::string>(); 
};
#endif