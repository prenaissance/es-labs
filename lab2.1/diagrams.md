Scheduler diagram

```cpp
typedef void (*Task)(void);

class Scheduler
{
private:
    const size_t MAX_MILLIS_PER_TASK = 50;
    Task *tasks;
    size_t size = 0;
    uint16_t executionMillis = 0;
    uint16_t lastTickMillis = millis();
    size_t currentTaskIndex = 0;

public:
    Scheduler(size_t max_size)
    {
        tasks = new Task[max_size];
    }
    ~Scheduler()
    {
        delete[] tasks;
    }
    void addTask(Task task)
    {
        tasks[size++] = task;
    }
    void advanceTick()
    {
        uint16_t currentMillis = millis();
        uint16_t elapsedMillis = currentMillis - lastTickMillis;
        lastTickMillis = currentMillis;
        executionMillis += elapsedMillis;
        if (executionMillis >= MAX_MILLIS_PER_TASK)
        {
            executionMillis = 0;
            tasks[currentTaskIndex]();
            currentTaskIndex = (currentTaskIndex + 1) % size;
        }
    }
};
```

```mermaid
flowchart TD
    start([Start]) --> loop[/Advance Tick, gets called by controller/]
    loop --> task_check{ 50 milliseconds passed since task began?}
    task_check --> |"false"| loop
    task_check --> |"true"| execute_task[Execute next task]
    execute_task --> loop
```

Led flicker chart

```cpp
void flicker_task()
{
    if (!ledButtonPressed)
    {
        if (millis() - lastFlicker > flickerInterval)
        {
            flickerLedIsOn = !flickerLedIsOn;
            lastFlicker = millis();
        }
    }
    else
    {
        flickerLedIsOn = false;
    }
}
```

```mermaid
flowchart TD
    start([Start]) --> check_led_button{Button pressed?}
    check_led_button --> |"false"| check_flicker_interval{interval passed between flickers?}
    check_led_button --> |"true"| end_state([End])
    check_flicker_interval --> |"false"| increase_interval[Increase interval]
    check_flicker_interval --> |"true"| flicker_led[/Flicker LED/]
    increase_interval --> end_state
    flicker_led --> reset_timer[Reset timer]
    reset_timer --> end_state
```