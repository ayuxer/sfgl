#include <felidae/felidae.h>
#include <stdlib.h>

static constexpr int W = 25;
static constexpr int A = 38;
static constexpr int S = 39;
static constexpr int D = 40;

static bool key_state[256] = { 0 };

void listen(struct felidae_event *event)
{
    if (event->kind == KEY_PRESS) {
        key_state[event->data.key_press.key_code] = 1;
    } else if (event->kind == KEY_RELEASE) {
        key_state[event->data.key_release.key_code] = 0;
    }
    free(event);
}

void update_camera(void)
{
    if (key_state[W]) {
        CameraTurnUp(.1f);
    }
    if (key_state[A]) {
        CameraTurnLeft(.1f);
    }
    if (key_state[S]) {
        CameraTurnDown(.1f);
    }
    if (key_state[D]) {
        CameraTurnRight(.1f);
    }
}

int main()
{
    MakeWindow(640, 480, "Hello, world!", 0, 0, 0);
    RevealWindow();
    while (!ShouldWindowClose()) {
        struct felidae_event *event = PollEvent();
        if (event)
            listen(event);
        update_camera();
        BeginRendering();
        DrawBackground(37, 109, 123, 1);
        FinishRendering();
    }
    FelidaeFree();
    return 0;
}
