#include "advent_of_code.h"
#include "solvers.h"

void update_results(AdventOfCode* aoc, FuriString* part_1, FuriString* part_2) {
    furi_check(furi_mutex_acquire(aoc->model_mutex, FuriWaitForever) == FuriStatusOk);
    if(part_1) {
        furi_string_set(aoc->model->part_1, part_1);
        if(aoc->model->img_1->data) {
            free(aoc->model->img_1->data);
            aoc->model->img_1->data = NULL;
        }
    }
    if(part_2) {
        furi_string_set(aoc->model->part_2, part_2);
        if(aoc->model->img_2->data) {
            free(aoc->model->img_2->data);
            aoc->model->img_2->data = NULL;
        }
    }
    furi_mutex_release(aoc->model_mutex);
    view_port_update(aoc->view_port);
};

void update_images(AdventOfCode* aoc, Image* img_1, Image* img_2) {
    furi_check(furi_mutex_acquire(aoc->model_mutex, FuriWaitForever) == FuriStatusOk);

    if(img_1) {
        if(aoc->model->img_1->data) free(aoc->model->img_1->data);

        aoc->model->img_1->width = img_1->width;
        aoc->model->img_1->height = img_1->height;

        size_t size = (size_t)img_1->width * (size_t)img_1->height;
        if(img_1->width % 8 > 0) size += img_1->height;

        aoc->model->img_1->data = malloc(size);
        memcpy(aoc->model->img_1->data, img_1->data, size);
    }

    if(img_2) {
        if(aoc->model->img_2->data) free(aoc->model->img_2->data);

        aoc->model->img_2->width = img_2->width;
        aoc->model->img_2->height = img_2->height;

        size_t size = (size_t)img_2->width * (size_t)img_2->height;
        if(img_2->width % 8 > 0) size += img_2->height;

        aoc->model->img_2->data = malloc(size);
        memcpy(aoc->model->img_2->data, img_2->data, size);
    }

    furi_mutex_release(aoc->model_mutex);
    view_port_update(aoc->view_port);
};

void unimplemented(AdventOfCode* aoc, Stream* file_stream) {
    UNUSED(file_stream);
    FuriString* str = furi_string_alloc_set("unimplemented");
    update_results(aoc, str, str);
    furi_string_free(str);
}

static void render_callback(Canvas* canvas, void* ctx) {
    FURI_LOG_D(TAG, "Render callback");

    AdventOfCode* aoc = ctx;
    furi_check(furi_mutex_acquire(aoc->model_mutex, FuriWaitForever) == FuriStatusOk);
    int day = aoc->model->day;
    FuriString* part_1 = furi_string_alloc_set(aoc->model->part_1);
    FuriString* part_2 = furi_string_alloc_set(aoc->model->part_2);
    Image* img_1 = aoc->model->img_1;
    Image* img_2 = aoc->model->img_2;
    furi_mutex_release(aoc->model_mutex);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 12, "Advent of Code 2022");

    char tmp_str[24];
    snprintf(tmp_str, 24, " * Day %d *", day);
    canvas_draw_str(canvas, 0, 24, tmp_str);

    canvas_set_font(canvas, FontSecondary);

    if(img_1->data) {
        canvas_draw_bitmap(canvas, 32, 36, img_1->width, img_1->height, img_1->data);
        furi_string_set(part_1, "");
    } else if(furi_string_size(part_1) == 0) {
        furi_string_set(part_1, "calculating...");
    }

    if(img_2->data) {
        canvas_draw_bitmap(canvas, 32, 48, img_2->width, img_2->height, img_2->data);
        furi_string_set(part_2, "");
    } else if(furi_string_size(part_2) == 0) {
        furi_string_set(part_2, "calculating...");
    }

    snprintf(tmp_str, 24, "Part 1: %s", furi_string_get_cstr(part_1));
    canvas_draw_str(canvas, 0, 42, tmp_str);
    if(day == 25)
        snprintf(tmp_str, 24, "(cont.) %s", furi_string_get_cstr(part_2));
    else
        snprintf(tmp_str, 24, "Part 2: %s", furi_string_get_cstr(part_2));
    canvas_draw_str(canvas, 0, 54, tmp_str);

    furi_string_free(part_2);
    furi_string_free(part_1);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    FURI_LOG_D(TAG, "Input callback");

    AdventOfCode* aoc = ctx;
    if(input_event->type == InputTypeShort) {
        furi_message_queue_put(aoc->input_queue, input_event, 0);
    }
}

void advent_of_code_reset_day(AdventOfCode* aoc, int day) {
    furi_check(furi_mutex_acquire(aoc->model_mutex, FuriWaitForever) == FuriStatusOk);
    aoc->model->day = day;
    furi_string_reset(aoc->model->part_1);
    if(aoc->model->img_1->data) {
        free(aoc->model->img_1->data);
        aoc->model->img_1->data = NULL;
    }
    furi_string_reset(aoc->model->part_2);
    if(aoc->model->img_2->data) {
        free(aoc->model->img_2->data);
        aoc->model->img_2->data = NULL;
    }
    furi_mutex_release(aoc->model_mutex);
    view_port_update(aoc->view_port);
}

AdventOfCode* advent_of_code_alloc() {
    AdventOfCode* aoc = malloc(sizeof(AdventOfCode));

    AdventOfCodeModel* model = malloc(sizeof(AdventOfCodeModel));
    model->day = 0;
    model->part_1 = furi_string_alloc();
    model->part_2 = furi_string_alloc();

    Image* img_1 = malloc(sizeof(Image));
    img_1->width = 0;
    img_1->height = 0;
    img_1->data = NULL;
    Image* img_2 = malloc(sizeof(Image));
    img_2->width = 0;
    img_2->height = 0;
    img_2->data = NULL;

    model->img_1 = img_1;
    model->img_2 = img_2;

    aoc->model = model;

    aoc->model_mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    aoc->input_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    aoc->view_port = view_port_alloc();
    view_port_draw_callback_set(aoc->view_port, render_callback, aoc);
    view_port_input_callback_set(aoc->view_port, input_callback, aoc);

    // Open GUI and register view_port
    aoc->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(aoc->gui, aoc->view_port, GuiLayerFullscreen);

    return aoc;
}

void advent_of_code_free(AdventOfCode* aoc) {
    FURI_LOG_D(TAG, "Freeing");

    gui_remove_view_port(aoc->gui, aoc->view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(aoc->view_port);

    furi_message_queue_free(aoc->input_queue);

    furi_mutex_free(aoc->model_mutex);

    if(aoc->model->img_2->data) free(aoc->model->img_2->data);
    free(aoc->model->img_2);
    if(aoc->model->img_1->data) free(aoc->model->img_1->data);
    free(aoc->model->img_1);

    furi_string_free(aoc->model->part_2);
    furi_string_free(aoc->model->part_1);
    free(aoc->model);
    free(aoc);

    FURI_LOG_D(TAG, "Free at last");
}

int32_t advent_of_code_2022_app(void* p) {
    FURI_LOG_D(TAG, "Starting");

    AdventOfCode* aoc = advent_of_code_alloc();

    FURI_LOG_D(TAG, "Allocated");

    FuriString* file_path;
    file_path = furi_string_alloc();
    FuriString* file_name;
    file_name = furi_string_alloc();

    do {
        FURI_LOG_D(TAG, "Checking file path");
        if(p && strlen(p)) {
            FURI_LOG_D(TAG, "File path set");
            furi_string_set(file_path, (const char*)p);
        } else {
            furi_string_set(file_path, ADVENT_OF_CODE_APP_BASE_PATH);

            FURI_LOG_D(TAG, "Opening file browser");

            DialogsFileBrowserOptions browser_options;
            dialog_file_browser_set_basic_options(
                &browser_options, ADVENT_OF_CODE_APP_EXTENSION, &I_txt_10px);
            browser_options.base_path = ADVENT_OF_CODE_APP_BASE_PATH;
            browser_options.hide_ext = false;

            DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
            bool res = dialog_file_browser_show(dialogs, file_path, file_path, &browser_options);

            furi_record_close(RECORD_DIALOGS);
            if(!res) {
                FURI_LOG_E(TAG, "No file selected");
                break;
            }
        }

        path_extract_filename(file_path, file_name, true);
        FURI_LOG_D(
            TAG,
            "Selected file \'%s\' (%s)",
            furi_string_get_cstr(file_path),
            furi_string_get_cstr(file_name));

        Storage* storage = furi_record_open(RECORD_STORAGE);
        Stream* file_stream = buffered_file_stream_alloc(storage);
        furi_record_close(RECORD_STORAGE);

        int day = atoi(furi_string_get_cstr(file_name));

        if(day < 1 || day > 25) {
            FURI_LOG_E(TAG, "Not a valid day: %d", day);
            continue;
        }

        advent_of_code_reset_day(aoc, day);

        if(!buffered_file_stream_open(
               file_stream, furi_string_get_cstr(file_path), FSAM_READ, FSOM_OPEN_EXISTING)) {
            FURI_LOG_E(TAG, "Unable to open file");
        } else {
            solvers[day - 1](aoc, file_stream);
        }

        buffered_file_stream_close(file_stream);
        stream_free(file_stream);

        InputEvent input;
        while(furi_message_queue_get(aoc->input_queue, &input, FuriWaitForever) == FuriStatusOk) {
            if(input.key == InputKeyBack) {
                break;
            }
        }
    } while(1);

    furi_string_free(file_path);
    advent_of_code_free(aoc);

    return 0;
}
