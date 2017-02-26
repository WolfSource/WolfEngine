#ifndef __TIME_CODE_H__
#define __TIME_CODE_H__

class time_code
{
public:
	time_code(int f) : _fps(f), _frames(0), _seconds(0), _minutes(0), _hours(0) { }
	void update()
	{
		if (this->_frames >= (unsigned)this->_fps - 1)
		{
			this->_frames = 0;
			this->_seconds++;
		}
		else
		{
			this->_frames++;
		}

		if (this->_seconds >= 60)
		{
			this->_seconds = 0;
			this->_minutes++;
		}

		if (this->_minutes >= 60)
		{
			this->_minutes = 0;
			this->_hours++;
		}
		if (this->_hours >= 24)
		{
			this->_frames = 0;
			this->_seconds = 0;
			this->_minutes = 0;
			this->_hours = 0;
		}
	}
	int hours() { return this->_hours; }
	int minutes() { return this->_minutes; }
	int seconds() { return this->_seconds; }
	int frames() { return this->_frames; }

private:
	int _fps;
	unsigned long _frames;
	int _seconds;
	int _minutes;
	int _hours;
};

#endif
