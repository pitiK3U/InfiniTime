#include "displayapp/screens/SportsTracking.h"

#include "components/heartrate/HeartRateController.h"

#include <lvgl/src/lv_core/lv_obj.h>

using namespace Pinetime::Applications::Screens;

namespace {
    static void record_button_callback(lv_obj_t* obj, lv_event_t event) {
        auto sportsTracking = static_cast<SportsTracking*>(obj->user_data);
        sportsTracking->recordButtonCallback(event);
    }

    static void stop_button_callback(lv_obj_t* obj, lv_event_t event) {
        auto sportsTracking = static_cast<SportsTracking*>(obj->user_data);
        sportsTracking->stopButtonCallback(event);
    }
}

SportsTracking::SportsTracking(Controllers::HeartRateController& heartRateController)
    : heartRateController {heartRateController}
{
    lv_obj_t* pulseLabel = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text_static(pulseLabel, "Pulse");
    lv_label_set_align(pulseLabel, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(pulseLabel, nullptr, LV_ALIGN_IN_TOP_MID, 0, 0);

    heartbeatIcon = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text_static(heartbeatIcon, Symbols::heartBeat);
    lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
    lv_obj_align(heartbeatIcon, pulseLabel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    heartbeatValue = lv_label_create(lv_scr_act(), nullptr);
    lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
    lv_label_set_text_static(heartbeatValue, "-");
    lv_obj_align(heartbeatValue, heartbeatIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t* durationLabel = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text_static(durationLabel, "Duration");
    lv_label_set_align(pulseLabel, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(durationLabel, heartbeatValue, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    lv_obj_t* durationIcon = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text_static(durationIcon, Symbols::clock);
    lv_obj_align(durationIcon, durationLabel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    lv_obj_t* durationValue = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text_fmt(durationValue, "%.2d:%.2d", 0, 0);
    lv_obj_align(durationValue, durationIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t* distanceLabel = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text_static(distanceLabel, "Distance");
    lv_label_set_align(distanceLabel, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(distanceLabel, durationIcon, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    lv_obj_t* distanceIcon = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text_static(distanceIcon, Symbols::map);
    lv_obj_align(distanceIcon, distanceLabel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    lv_obj_t* distanceValue = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text_fmt(distanceValue, "%f km", 0.0f);
    lv_obj_align(distanceValue, distanceIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t* speedLabel = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text_static(speedLabel, "Speed");
    lv_obj_align(speedLabel, distanceIcon, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    lv_obj_t* speedIcon = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text_static(speedIcon, Symbols::running);
    lv_obj_align(speedIcon, speedLabel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    lv_obj_t* speedValue = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text_fmt(speedValue, "%d km/h", 0);
    lv_obj_align(speedValue, speedIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    pauseButton = lv_btn_create(lv_scr_act(), nullptr);
    lv_obj_align(pauseButton, nullptr, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
    pauseIcon = lv_label_create(pauseButton, nullptr);
    lv_label_set_text_static(pauseIcon, ""); // Symbols::pause);
    lv_label_set_align(pauseIcon, LV_ALIGN_CENTER);
    lv_obj_set_style_local_bg_opa(pauseButton, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
    lv_btn_set_state(pauseButton,  LV_BTN_STATE_DISABLED);

    stopButton = lv_btn_create(lv_scr_act(), nullptr);
    stopButton->user_data = this;
    lv_obj_set_event_cb(stopButton, stop_button_callback);
    lv_obj_align(stopButton, nullptr, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    stopIcon = lv_label_create(stopButton, nullptr);
    lv_label_set_text_static(stopIcon, ""); // Symbols::stop);
    lv_obj_set_style_local_bg_opa(stopButton, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
    lv_btn_set_state(stopButton, LV_BTN_STATE_DISABLED);

    recordButton = lv_btn_create(lv_scr_act(), nullptr);
    recordButton->user_data = this;
    lv_obj_set_event_cb(recordButton, record_button_callback);
    lv_obj_align(recordButton, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    recordIcon = lv_label_create(recordButton, nullptr);
    lv_label_set_text_static(recordIcon, Symbols::record);

    taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
    Refresh();
}

SportsTracking::~SportsTracking() {
    lv_task_del(taskRefresh);
    lv_obj_clean(lv_scr_act());
}

void SportsTracking::Refresh() {
    heartbeat = heartRateController.HeartRate();
    heartbeatRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;
    if (heartbeat.IsUpdated() || heartbeatRunning.IsUpdated()) {
        if (heartbeatRunning.Get()) {
            lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
            lv_label_set_text_fmt(heartbeatValue, "%d", heartbeat.Get());
        } else {
            lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x1B1B1B));
            lv_label_set_text_static(heartbeatValue, "");
        }

        lv_obj_realign(heartbeatIcon);
        lv_obj_realign(heartbeatValue);
    }
}

void SportsTracking::recordButtonCallback(lv_event_t event) {
    switch (event) {
        case LV_EVENT_CLICKED: {
            lv_label_set_text_static(recordIcon, "");
            lv_obj_set_style_local_bg_opa(recordButton, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
            lv_btn_set_state(recordButton, LV_BTN_STATE_DISABLED);

            lv_label_set_text_static(stopIcon, Symbols::stop);
            lv_obj_set_style_local_bg_opa(stopButton, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
            lv_btn_set_state(stopButton, LV_BTN_STATE_RELEASED);

            lv_label_set_text_static(pauseIcon, Symbols::pause);
            lv_obj_set_style_local_bg_opa(pauseButton, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
            // TODO: opentracks + gadget bridge doesn't allow for pausing for now
            // lv_btn_set_state(pauseButton, LV_BTN_STATE_RELEASED);
            break;
        }
        default: {
            break;
        }
    }
}

void SportsTracking::stopButtonCallback(lv_event_t event) {
    const uint8_t MAX_PRESS_COUNT = 5;
    pressed_count++;

    switch (event) {
        case LV_EVENT_PRESSED: {
            /*
            pressed_arc = lv_arc_create(stopButton, nullptr);
            lv_obj_set_size(pressed_arc, 20, 20);
            lv_obj_align(pressed_arc, stopButton, LV_ALIGN_CENTER, 0, 0);
            lv_arc_set_rotation(pressed_arc, 270);
            lv_arc_set_bg_angles(pressed_arc, 0, 360);
            lv_arc_set_range(pressed_arc, 0, 5);
            */
            // lv_obj_remove_style(pressed_arc, NULL, LV_ARC_PART_KNOB);   /*Be sure the knob is not displayed*/
            // lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
            break;
        }
        case LV_EVENT_LONG_PRESSED: {
            // lv_arc_set_value(pressed_arc, pressed_count);
            break;
        }
        case LV_EVENT_LONG_PRESSED_REPEAT: {
            //lv_arc_set_value(pressed_arc, pressed_count);
            if (pressed_count < MAX_PRESS_COUNT) {
                break;
            }
            lv_label_set_text_static(stopIcon, "");
            lv_obj_set_style_local_bg_opa(stopButton, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
            lv_btn_set_state(stopButton, LV_BTN_STATE_DISABLED);
            // lv_obj_del(pressed_arc);
            pressed_arc = nullptr;

            lv_label_set_text_static(pauseIcon, "");
            lv_obj_set_style_local_bg_opa(pauseButton, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
            // TODO: opentracks + gadget bridge doesn't allow for pausing for now
            lv_btn_set_state(pauseButton, LV_BTN_STATE_DISABLED);

            lv_label_set_text_static(recordIcon, Symbols::record);
            lv_obj_set_style_local_bg_opa(recordButton, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
            lv_btn_set_state(recordButton, LV_BTN_STATE_RELEASED);

            break;
        }
        case LV_EVENT_CLICKED: {
            pressed_count = 0;
            // lv_obj_del(pressed_arc);
            pressed_arc = nullptr;
            break;
        }
        default: {
            break;
        }
    }
}