#include <felidae/felidae.h>
#include <stdio.h>
#include <stdlib.h>

void listen(struct felidae_event *event)
{
    if (event->kind != KEY_PRESS && event->kind != KEY_RELEASE)
        return;
    struct felidae_window_dimensions dimensions = GetWindowDimensions();
    int width = dimensions.width, height = dimensions.height;
    const char *title = GetWindowTitle();
    printf(
        "%d: width=%d,height=%d,title=%s\n", event->kind, width, height, title
    );
    char *type = event->kind == KEY_PRESS ? "pressed" : "released";
    printf(
        "Key %s: code=%d,timestamp=%d\n", type, event->data.key_press.key_code,
        event->timestamp
    );
    fflush(stdout);
}

int main()
{
    MakeWindow(640, 480, "Hello, world!", 0, 0, 0);
    RevealWindow();
    while (!ShouldWindowClose()) {
        struct felidae_event *event = PollEvent();
        if (event) {
            listen(event);
            free(event);
        }
        BeginRendering();
        DrawBackground(37, 109, 123, 1);
        FinishRendering();
    }
    FelidaeFree();
    return 0;
}
