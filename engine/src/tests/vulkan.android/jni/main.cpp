/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <jni.h>
#include <errno.h>
#include <cstdlib>
#include <stdlib.h>

#include <android/sensor.h>
#include <android/log.h>
#include "../native_app_glue/android_native_app_glue.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

#define LOG_ACCELEROMETER false

/**
 * Our saved state data.
 */
struct saved_state 
{
    float angle;
	int32_t x;
	int32_t y;
};

/**
 * Shared state for our app.
 */
struct engine 
{
    struct android_app* app;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

    int animating;
	int32_t width;
	int32_t height;
    struct saved_state state;
};

static int engine_init_display(struct engine* pEngine) 
{
	ANativeActivity* nativeActivity = pEngine->app->activity;
	//const char* internalPath = nativeActivity->externalDataPath;
	//std::string dataPath(internalPath);

	/*wolf::system::w_time_span _time;
	wolf::system::w_time_span _t(0, 1, 1);

	_time = _time.add(wolf::system::w_time_span(1, 1, 1));
	LOGI(_time.to_string().c_str());
	_time -= _t;
	
	LOGI(_time.to_string().c_str());
	
	wolf::system::w_xml_data _d;
	_d.node = L"Pooya";
	_d.attributes.push_back({ L"name", L"Pooya" });
	_d.attributes.push_back({ L"id", L"17" });
	
	wolf::system::w_xml_data _c;
	_c.node = L"Pooya";
	_c.attributes.push_back({ L"name", L"Ray" });
	_c.attributes.push_back({ L"id", L"2" });

	_d.children.push_back(_c);

	wolf::system::w_xml::save((dataPath + "/pooya.xml").c_str(), false, _d);

	auto _hr = wolf::system::io::get_is_file((dataPath + "/pooya.xml").c_str());
	_hr = wolf::system::io::get_is_directory((dataPath + "/test/").c_str());
	if (_hr == S_OK)
	{
		LOGI("OK");
	}
	else
	{
		LOGI("FALSE");
	}

	auto _u_name = wolf::system::io::get_unique_name();
	LOGI(_u_name.c_str());

	wolf::system::io::create_directory((dataPath + "/" + _u_name).c_str());
	auto _ext = wolf::system::io::get_file_extention("c:\\b\\c.txt");
	LOGI(_ext.c_str());

	_ext = wolf::system::io::get_file_extention((dataPath + "/pooya.xml").c_str());
	LOGI(_ext.c_str());

	_ext = wolf::system::io::get_file_name((dataPath + "/pooya.xml").c_str());
	LOGI(_ext.c_str());

	_ext = wolf::system::io::get_base_file_name((dataPath + "/pooya.xml").c_str());
	LOGI(_ext.c_str());

	_ext = wolf::system::io::get_parent_directory((dataPath + "/pooya.xml").c_str());
	LOGI(_ext.c_str());

	int _status = -1;
	auto _read = wolf::system::io::read_text_file((dataPath + "/pooya.xml").c_str(), _status);
	LOGI(_read);
	
	auto _color = w_color::from_hex(100);

	wolf::system::w_event<int> __e;
	auto _ff = [](int a)
	{
		if (a == 10)
		{
			LOGI("IIIII");
		}
	};

	__e += _ff;

	__e += [](int a)
	{
		if (a == 20)
		{
			LOGI("IIIII");
		}
	};

	__e(10);
	__e(20);

	__e -= _ff;

	__e(10);
	__e(20);

	logger.initialize("Wolf.Engine", nativeActivity->externalDataPath);
	logger.write("Pooya");
	

	wolf::system::w_task::execute_async([&]()
	{
		logger.write("A");
		int k = 0;
		for (size_t i = 0; i < 1000000; i++)
		{
			k++;
		}
	}, []()
	{
		logger.write("DONE");
	});


	wolf::system::w_timer_callback t;
	t.do_sync(1000, []()
	{
		logger.write("timer");
	});

	logger.write("here");

	tc.do_sync(1000, []()
	{
		logger.write("SYNC");
	});

	tc.do_async(1000, []()
	{
		logger.write("ASYNC");
	});*/

	pEngine->width = ANativeWindow_getWidth(pEngine->app->window);
	pEngine->height = ANativeWindow_getHeight(pEngine->app->window);

	/*w_window_info _w_info;
	_w_info.width = static_cast<UINT>(pEngine->width);
	_w_info.height = static_cast<UINT>(pEngine->height);
	_w_info.window = pEngine->app->window;
	std::vector<w_window_info> _v;
	_v.push_back(_w_info);

	sWindowsInfos[0] = _v;

	sScene = new scene(nativeActivity->internalDataPath);*/

    return 0;
}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) 
{
	//run the main loop
	//sScene->run(sWindowsInfos);

	//sGameTime.tick([]
	//{
		/*logger.write("fps: " + std::to_string(sGameTime.get_frames_per_second()));
		logger.write("elapsed: " + std::to_string(sGameTime.get_elapsed_seconds()));
		logger.write("total: " + std::to_string(sGameTime.get_total_seconds()));*/
	//});

	//usleep(16);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine) {
   
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    struct engine* engine = (struct engine*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        engine->animating = 1;
        engine->state.x = AMotionEvent_getX(event, 0);
        engine->state.y = AMotionEvent_getY(event, 0);
        return 1;
    }
    return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            engine->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)engine->app->savedState) = engine->state;
            engine->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (engine->app->window != nullptr) {
                engine_init_display(engine);
                engine_draw_frame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            if (engine->accelerometerSensor != nullptr) {
                ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                        engine->accelerometerSensor);
                // We'd like to get 60 events per second (in us).
                ASensorEventQueue_setEventRate(engine->sensorEventQueue,
                        engine->accelerometerSensor, (1000L/60)*1000);
            }
            break;
        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            if (engine->accelerometerSensor != nullptr) {
                ASensorEventQueue_disableSensor(engine->sensorEventQueue,
                        engine->accelerometerSensor);
            }
            // Also stop animating.
            engine->animating = 0;
            engine_draw_frame(engine);
            break;
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) {
    struct engine engine;

    // Make sure glue isn't stripped.
    app_dummy();

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;

    // Prepare to monitor accelerometer
    engine.sensorManager = ASensorManager_getInstance();
    engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
            ASENSOR_TYPE_ACCELEROMETER);
    engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
            state->looper, LOOPER_ID_USER, nullptr, nullptr);

    if (state->savedState != nullptr) {
        // We are starting with a previous saved state; restore from it.
        engine.state = *(struct saved_state*)state->savedState;
    }

    // loop waiting for stuff to do.

    while (1) {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

		//sGameTime.set_fixed_time_step(true);
		//sGameTime.set_target_elapsed_seconds(1.0 / 30.0);

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, nullptr, &events,
                (void**)&source)) >= 0) 
		{
            // Process this event.
            if (source != nullptr) {
                source->process(state, source);
            }

            // If a sensor has data, process it now.
            if (ident == LOOPER_ID_USER) {
                if (engine.accelerometerSensor != nullptr) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
                            &event, 1) > 0) {
                        if (LOG_ACCELEROMETER) {
                            LOGI("accelerometer: x=%f y=%f z=%f",
                                event.acceleration.x, event.acceleration.y,
                                event.acceleration.z);
                        }
                    }
                }
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine_term_display(&engine);
                return;
            }
        }

        if (engine.animating) {
            // Done with events; draw next animation frame.
            engine.state.angle += .01f;
            if (engine.state.angle > 1) {
                engine.state.angle = 0;
            }

            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.
            engine_draw_frame(&engine);
        }
    }
}
//END_INCLUDE(all)
