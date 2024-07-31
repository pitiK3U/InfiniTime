#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"

#include "Symbols.h"
#include "utility/DirtyValue.h"


#include <lvgl/lvgl.h>

namespace Pinetime {
    namespace Applications {
        namespace Screens {
            class SportsTracking : public Screen {
            public:
                SportsTracking(Controllers::HeartRateController& heartRateController);
                ~SportsTracking() override;
                void Refresh() override;

                void recordButtonCallback(lv_event_t event);
                void stopButtonCallback(lv_event_t event);
            private:

                Controllers::HeartRateController& heartRateController;

                Utility::DirtyValue<uint8_t> heartbeat {};
                Utility::DirtyValue<bool> heartbeatRunning {};

                lv_obj_t* heartbeatIcon;
                lv_obj_t* heartbeatValue;
                lv_obj_t* recordButton;
                lv_obj_t* recordIcon;
                lv_obj_t* pauseButton;
                lv_obj_t* pauseIcon;
                lv_obj_t* stopButton;
                lv_obj_t* stopIcon;

                uint8_t pressed_count = 0;
                lv_obj_t* pressed_arc;

                lv_task_t* taskRefresh;
            };
        }

        template <>
        struct AppTraits<Apps::SportsTracking> {
            static constexpr Apps app = Apps::SportsTracking;
            static constexpr const char* icon = Screens::Symbols::running;
            static Screens::Screen* Create(AppControllers& controllers) {
                return new Screens::SportsTracking(controllers.heartRateController);
            }
        };
    }
}