#ifndef INPUT_H
#define INPUT_H

typedef enum {
    ACT_NONE = 0,
    ACT_UP,
    ACT_DOWN,
    ACT_LEFT,
    ACT_RIGHT,
    ACT_UNDO,
    ACT_RESTART,
    ACT_CONTINUE,
    ACT_QUIT
} InputAction;

void input_init(void);
void input_cleanup(void);
InputAction input_read_action(void);

#endif /* INPUT_H */
