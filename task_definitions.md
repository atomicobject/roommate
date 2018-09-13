## Task Definitions

### Calendar Sync Task
	- Creates an MQTT Client, and connects
	- Subscribes to applicable roommate calendar topics events. Callback sends result to a queue
	- Updates app state with most recent calendar event list

### UX Task
	- Waits for user interaction events, calendar updates, or calendar event transitions and initiates updates to the LEDs accordingly

### LED Control Task
	- Maintains a steady state LED pattern
	- Waits on a queue of either steady state pattern updates or sequence requests
	- When a pattern update is received update app state and the LEDs
	- When a sequence is received call the sequence functions (via func ptrs) to orchestrate the LEDs
	- When sequence is finished update the LEDs with the steady state pattern again


## Proposed data structures

	// This is the type of object that would be sent to the LED Control Task message queue
	struct led_control_request {
		enum led_control_request_type type;
		union {
			struct led_sequence sequence_request_data;
			struct led_state steady_state_update_request_data;
		}
	}  
	
	enum led_control_request_type { LED_CONTROL_SEQUENCE_REQUEST, LED_CONTROL_STEADY_STATE_REQUEST };
	
	struct led_sequence {
		led_sequence_executor_func executor;       // Called in a loop to get the next LED state (frame) until return value is null
		led_sequence_state_init_func initializer;  // Called to get initial state for sequence 
		uint8_t max_frames;
	}
	
	typedef struct led_frame (*led_sequence_executor_func)(void *sequence_state);
	
	typedef void *(*led_sequence_state_init_func)(void);  // Ideally, this should return a struct instead of void ptr...
	
	struct led_state {
		uint32_t leds[NUM_LEDS];
	};
	
	struct led_frame {
		struct led_state new_state;
		uint8_t duration_ms
	};







