#include <stdio.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "gui.h"

typedef struct {
    int event;
    void *arg;
} gui_event_t;

#define GUI_WIFI_EVENT 0
#define GUI_BATTERY_EVENT 1
#define GUI_TIME_EVENT 2
#define GUI_SOURCE_EVENT 3

static lv_obj_t * header;
static lv_obj_t * time_clock;
static lv_obj_t * wifi;
static lv_obj_t * battery;
static lv_obj_t * body;

typedef struct {
    lv_obj_t *obj;
    float val;
    uint8_t mask;
} source_t;

source_t source[3] = {0};
#define SOURCE_MASK_COLOR "ff3034"

static QueueHandle_t gui_event_queue = NULL;

static lv_disp_t **disp;
static lv_indev_t **indev;

static void header_create(void)
{
    header = lv_cont_create(lv_disp_get_scr_act(disp[0]), NULL);
    lv_obj_set_width(header, LV_HOR_RES);
    
    time_clock = lv_label_create(header, NULL);
    lv_obj_align(time_clock, NULL, LV_ALIGN_IN_LEFT_MID, LV_DPI/10, 0);
    lv_label_set_recolor(time_clock, true);
    lv_label_set_text(time_clock, "");

    lv_obj_t * state = lv_label_create(header, NULL);
    lv_label_set_text(state, LV_SYMBOL_SETTINGS);
    lv_obj_align(state, NULL, LV_ALIGN_CENTER, 0, 0);

    wifi = lv_label_create(header, NULL);
    lv_label_set_text(wifi, "");
    lv_obj_align(wifi, NULL, LV_ALIGN_IN_RIGHT_MID, -(LV_DPI/10) * 5, 0);

    battery = lv_label_create(header, NULL);
    lv_label_set_text(battery, "");
    lv_obj_align(battery, NULL, LV_ALIGN_IN_RIGHT_MID, -(LV_DPI/10) * 3, 0);

    lv_cont_set_fit2(header, false, true);   /*Let the height set automatically*/
    lv_obj_set_pos(header, 0, 0);
}

static void side_create(void)
{
    lv_obj_t * h = lv_page_create(lv_disp_get_scr_act(disp[0]), NULL);
    lv_obj_set_width(h, (LV_HOR_RES / 4) * 1);
    lv_obj_set_height(h, LV_VER_RES - lv_obj_get_height(header));
    lv_page_set_sb_mode(h, LV_SB_MODE_DRAG);
    lv_page_set_scrl_layout(h, LV_LAYOUT_CENTER);

    static lv_style_t style_txt;
    lv_style_copy(&style_txt, &lv_style_plain);
    LV_FONT_DECLARE(seg_font);
    style_txt.text.font = &seg_font;
    style_txt.text.opa = LV_OPA_100;

    lv_obj_t * lable;
    lable = lv_label_create(h, NULL);
    lv_obj_set_style(lable, &style_txt);
    lv_label_set_text(lable, "V");

    lable = lv_label_create(h, NULL);
    lv_obj_set_style(lable, &style_txt);
    lv_label_set_text(lable, "A");

    lable = lv_label_create(h, NULL);
    lv_obj_set_style(lable, &style_txt);
    lv_label_set_text(lable, "W");

    lv_obj_set_pos(h, LV_HOR_RES - (LV_HOR_RES / 4) * 1, lv_obj_get_height(header));
}

static void body_create(void)
{
    
    body = lv_page_create(lv_disp_get_scr_act(disp[0]), NULL);
    
    lv_obj_set_width(body, (LV_HOR_RES / 4) * 3);
    lv_obj_set_height(body, LV_VER_RES - lv_obj_get_height(header));
    lv_page_set_sb_mode(body, LV_SB_MODE_DRAG);
    lv_page_set_scrl_layout(body, LV_LAYOUT_CENTER);

    static lv_style_t style_txt;
    lv_style_copy(&style_txt, &lv_style_plain);
    LV_FONT_DECLARE(seg_font);
    style_txt.text.font = &seg_font;
    style_txt.text.opa = LV_OPA_100;

    lv_obj_t *txt;
    txt = lv_label_create(body, NULL);
    lv_obj_set_style(txt, &style_txt);
    lv_label_set_recolor(txt, true);
    source[0].obj = txt;

    txt = lv_label_create(body, NULL);
    lv_obj_set_style(txt, &style_txt);
    lv_label_set_recolor(txt, true);
    source[1].obj = txt;

    txt = lv_label_create(body, NULL);
    lv_obj_set_style(txt, &style_txt);
    lv_label_set_recolor(txt, true);
    source[2].obj = txt;

    lv_obj_set_pos(body, 0, lv_obj_get_height(header));
}

static void time_write()
{
    time_t now;
    struct tm timeinfo;
    char strftime_buf[64];
    
    time(&now);
    localtime_r(&now, &timeinfo);
    if (timeinfo.tm_sec % 2) {
        sprintf(strftime_buf, "%02d#000000 :#%02d", timeinfo.tm_hour, timeinfo.tm_min);
    } else {
        sprintf(strftime_buf, "%02d#ffffff :#%02d", timeinfo.tm_hour, timeinfo.tm_min);
    }
    
    lv_label_set_text(time_clock, strftime_buf);
}

static void source_write(source_t *source)
{
    char data[6], str[64];

    if (source->val < 10) {
        sprintf(data, "%05.3f", source->val);
    } else if (source->val < 100 && source->val > 10) {
        sprintf(data, "%05.2f", source->val);
    } else {
        sprintf(data, "E R R");
    }

    sprintf(str, "#%s %c##%s %c##%s %c##%s %c##%s %c#", \
                 (source->mask / 16) % 2 ? SOURCE_MASK_COLOR : "000000", data[0], \
                 (source->mask / 8)  % 2 ? SOURCE_MASK_COLOR : "000000", data[1], \
                 (source->mask / 4)  % 2 ? SOURCE_MASK_COLOR : "000000", data[2], \
                 (source->mask / 2)  % 2 ? SOURCE_MASK_COLOR : "000000", data[3], \
                 (source->mask / 1)  % 2 ? SOURCE_MASK_COLOR : "000000", data[4]  );
    
    lv_label_set_text(source->obj, str);
}

static void gui_task(lv_task_t * arg)
{
    int ret;
    gui_event_t e;
    ret = xQueueReceive(gui_event_queue, &e, 0);
    if (ret == pdFAIL) {
        return;
    }
    switch (e.event) {
        case GUI_WIFI_EVENT: {
            if (e.arg) {
                lv_label_set_text(wifi, LV_SYMBOL_WIFI);
            } else {
                lv_label_set_text(wifi, "");
            }
        }
        break;

        case GUI_BATTERY_EVENT: {
            lv_label_set_text(battery, (const char *)e.arg);
        }
        break;

        case GUI_TIME_EVENT: {
            time_write();
        }
        break;

        case GUI_SOURCE_EVENT: {
            source_write((source_t *)e.arg);
            
        }
        break;

    }
}

static int gui_event_send(int event, void *arg, int ticks_wait)
{
    int ret;
    if (gui_event_queue == NULL) {
        return -1;
    }

    gui_event_t e;
    e.event = event;
    e.arg = arg;
    ret = xQueueSend(gui_event_queue, &e, ticks_wait);
    if (ret == pdFALSE) {
        return -1;
    }

    return 0;
}

int gui_set_wifi_state(bool state, int ticks_wait)
{
    return gui_event_send(GUI_WIFI_EVENT, (void *)state, ticks_wait);
}

int gui_set_time_change(int ticks_wait)
{
    return gui_event_send(GUI_TIME_EVENT, NULL, ticks_wait);
}

int gui_set_battery_value(gui_battery_value_t value, int ticks_wait)
{   
    int ret;

    switch (value) {
        case BATTERY_FULL: {
            ret = gui_event_send(GUI_BATTERY_EVENT, (void *)LV_SYMBOL_BATTERY_FULL, ticks_wait);
        }
        break;

        case BATTERY_3: {
            ret = gui_event_send(GUI_BATTERY_EVENT, (void *)LV_SYMBOL_BATTERY_3, ticks_wait);
        }
        break;

        case BATTERY_2: {
            ret = gui_event_send(GUI_BATTERY_EVENT, (void *)LV_SYMBOL_BATTERY_2, ticks_wait);
        }
        break;

        case BATTERY_1: {
            ret = gui_event_send(GUI_BATTERY_EVENT, (void *)LV_SYMBOL_BATTERY_1, ticks_wait);
        }
        break;

        case BATTERY_EMPTY: {
            ret = gui_event_send(GUI_BATTERY_EVENT, (void *)LV_SYMBOL_BATTERY_EMPTY, ticks_wait);
        }
        break;

        default: {
            return -1;
        }
        break;
    }
    return ret;
}

int gui_set_source_value(char type, float value, uint8_t color_mask, int ticks_wait)
{   
    int ret;

    switch (type) {
        case 'V': {
            source[0].val = value;
            source[0].mask = color_mask;
            ret = gui_event_send(GUI_SOURCE_EVENT, (void *)&source[0], ticks_wait);
        }
        break;

        case 'A': {
            source[1].val = value;
            source[1].mask = color_mask;
            ret = gui_event_send(GUI_SOURCE_EVENT, (void *)&source[1], ticks_wait);
        }
        break;

        case 'W': {
            source[2].val = value;
            source[2].mask = color_mask;
            ret = gui_event_send(GUI_SOURCE_EVENT, (void *)&source[2], ticks_wait);
        }
        break;

        default: {
            return -1;
        }
        break;
    }
    return ret;
}

static lv_group_t * encoder_group;

void gui_init(lv_disp_t **disp_array, lv_indev_t **indev_array, lv_theme_t * th)
{
    disp = disp_array;
    indev = indev_array;
    gui_event_queue = xQueueCreate(5, sizeof(gui_event_t));
    lv_theme_set_current(th);
    th = lv_theme_get_current();    /*If `LV_THEME_LIVE_UPDATE  1` `th` is not used directly so get the real theme after set*/
    lv_cont_set_style(lv_disp_get_scr_act(disp[0]), LV_CONT_STYLE_MAIN, th->style.bg);
    lv_cont_set_style(lv_disp_get_scr_act(disp[1]), LV_CONT_STYLE_MAIN, th->style.bg);

    header_create();
    side_create();
    body_create();
    lv_task_create(gui_task, 10, LV_TASK_PRIO_MID, NULL);
    gui_set_source_value('V', 0, 16, portMAX_DELAY);
    gui_set_source_value('A', 0, 16, portMAX_DELAY);
    gui_set_source_value('W', 0, 16, portMAX_DELAY);

    encoder_group = lv_group_create();
    // lv_group_set_focus_cb(encoder_group, group_focus_cb);
    lv_indev_set_group(indev[0], encoder_group);
    lv_obj_t * spinbox = lv_spinbox_create(lv_disp_get_scr_act(disp[1]), NULL);
    // lv_obj_set_event_cb(obj, general_event_handler);
    lv_spinbox_set_digit_format(spinbox, 5, 2);
    lv_spinbox_step_prev(spinbox);
    lv_obj_set_width(spinbox, 160);
    lv_obj_align(spinbox, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_group_add_obj(encoder_group, spinbox);
}
