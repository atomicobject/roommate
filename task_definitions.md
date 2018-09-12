# Task Definitions

## Calendar Sync Task
  - Creates an MQTT Client, and connects
  - Subscribes to ROOMMATE_CAL_SYNC_RESPONSE topic events. Callback sends result to a queue
  - Periodically sends an MQTT message requesting updated calendar event list
  - Waits for response to arrive in queue
  - Updates app state with most recent calendar event list

## UX Task
  - Waits for user interaction events, calendar updates, or calendar event transitions and initiates updates to the LEDs accordingly

## LED Control Task
  - Maintains a steady state LED pattern
  - Waits on a queue of either steady state pattern updates or sequence requests
  - When a pattern update is received update app state and the LEDs
  - When a sequence is received call the sequence functions (via func ptrs) to orchestrate the LEDs
    - When sequence is finished update the LEDs with the steady state pattern again









