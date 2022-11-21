#include "haptics.h"

// Static members for Serial listening mode
bool SerialListen::active;
int16_t SerialListen::torqueIn;
int16_t SerialListen::angleOut;
char SerialListen::serial_data[SERIAL_BUFSIZE];
int SerialListen::serial_data_length;
std::string SerialListen::serial_data_str;
std::string SerialListen::serial_data_str_buffer;

TorqueTuner::TorqueTuner() {
	// active_mode = mode_list[0];
	num_modes = mode_list.size();
}

void TorqueTuner::update() {
	update_angle();

	// Filter and gate velocity for output
	velocity_out = gate(filter(velocity), 5, 0);
	calc_acceleration(velocity_out);

	// Calculate index to transfer function
	active_mode->calc_index(this);

	// Calculate trigger and discrete output
	float resolution = active_mode->max / stretch;

	angle_discrete = round(round(angle_out / resolution) * resolution);
	if (abs(angle_discrete - angle_discrete_last) >= resolution) {
		update_trig();
		angle_discrete_last = angle_discrete;
	};
	if (active_mode == &wall) {
		torque = static_cast<int16_t>(active_mode->calc(this));
	} else
		torque = static_cast<int16_t>(active_mode->calc(this) - active_mode->damping * velocity);
}

void TorqueTuner::update_angle() {
	// Wrap angle
	angle_delta = angle - angle_last;
	if ((angle_delta) < -1800) {
		wrap_count += 1;
		angle_delta += 3600;
	} else if ((angle_delta) > 1800) {
		wrap_count -= 1;
		angle_delta -= 3600;
	};
	angle_last = angle;

	angle_unclipped += angle_delta;

	// Clip with parameter range
	angle_out += angle_delta;
	if (active_mode->wrap_output) {
		angle_out = mod(angle_out, 3600); // CHANGE to min, max
	} else {
		angle_out = static_cast<int32_t>(clip(angle_out, active_mode->min, active_mode->max));
	}
}

void TorqueTuner::set_mode(int mode_idx) {
	if (active_mode != mode_list[mode_idx]) {
		if (active_mode != nullptr) active_mode->set_active(false);
		active_mode = mode_list[mode_idx];
		active_mode->reset(angle);
		active_mode->set_active(true);
		angle_last = angle;
		angle_out = 0;
		angle_unclipped = 0;
		set_defaults(active_mode);
		
		// Init discrete angle
		// float resolution = active_mode->max / stretch;
		// angle_discrete = floor(floor(angle_out / resolution) * resolution);
		print_mode(static_cast<MODE>(mode_idx));
	}
}

void TorqueTuner::set_mode(MODE mode_) {
	int mode_idx = static_cast<int>(mode_);
	set_mode(mode_idx);
}

void TorqueTuner::set_stretch(float stretch_) {
	if (abs(stretch_ - stretch) > 0.1) {
		stretch = stretch_;
	}
}

void TorqueTuner::set_defaults(Mode * mode) {
	scale = mode->scale_default;
	stretch = mode->stretch_default;
	target_velocity = mode->target_velocity_default;
}

void TorqueTuner::print_mode(MODE mode_) {
	printf("Switched mode to : \n");
	switch (mode_) {
	case CLICK:
		printf("Click \n");
		break;
	case INERTIA:
		printf("Inertia\n");
		break;
	case WALL:
		printf("Wall\n");
		break;
	case MAGNET:
		printf("Magnet\n");
		break;
	case LINSPRING:
		printf("Linear Spring \n");
		break;
	case EXPSPRING:
		printf("Exponential Spring \n");
		break;
	case FREE:
		printf("Free\n");
		break;
	case SPIN:
		printf("Spin \n");
		break;
	case SERIAL_LISTEN:
		printf("Serial listen \n");
		break;
	}
}

float TorqueTuner::calc_acceleration(float velocity_) {
	static float last = 0;
	acceleration = last - velocity_;
	last = velocity_;
	return acceleration;
}

float TorqueTuner::filter(float x) {
	// Cannonical form - https://ccrma.stanford.edu/~jos/filters/Direct_Form_II.html
	// w[n] = x[n] - a1*w[n-1] - a2*w[n-2]
	// y[n] = b0*w[n] + b1*w[n-1] + b2*w[n-2]
	static float w[3];
	w[0] = x - a[1] * w[1] - a[2] * w[2];
	x = b[0] * w[0] + b[1] * w[1] + b[2] * w[2];
	w[2] = w[1];
	w[1] = w[0];
	return x;
}

float TorqueTuner::gate(float val, float threshold, float floor) {
	return abs(val) > threshold ? val : floor;
}

void TorqueTuner::update_trig() {
	trigger++;
	trigger = fold(trigger, 0, 1);
}

int32_t TorqueTuner::getTime() {
	return esp_timer_get_time();
}

int16_t Wall::calc(void* ptr) {
	TorqueTuner* knob = (TorqueTuner*)ptr;
	float val = 0;
	float delta_angle_min = (knob->angle_unclipped - min) / 10.0;
	if (delta_angle_min < 0 && delta_angle_min > - threshold) {
		val = WALL_TORQUE * stiffness * abs(delta_angle_min) - damping * knob->velocity;
	} else {
		float delta_angle_max = (knob->angle_unclipped - max) / 10.0;
		if (delta_angle_max > 0 && delta_angle_max < threshold) {
			val = -WALL_TORQUE * stiffness * abs(delta_angle_max) - damping * knob->velocity;
		}
	}
	return static_cast<int16_t> (round(val));
}

int16_t Click::calc(void* ptr) {
	TorqueTuner* knob = (TorqueTuner*)ptr;
	float val;
	if (knob->angle_out <= min) {
		val = WALL_TORQUE;
	} else if (knob->angle_out >= max) {
		val = -WALL_TORQUE;
	} else {
		val = static_cast<float>((tf_click_2[idx])) / TABLE_RESOLUTION * knob->scale;
	}
	return static_cast<int16_t> (round(val));
}

int16_t Magnet::calc(void* ptr) {
	TorqueTuner* knob = (TorqueTuner*)ptr;
	return static_cast<float>(tf_magnet[idx]) / TABLE_RESOLUTION * knob->scale; // Magnet
}

int16_t Inertia::calc(void* ptr) {
	TorqueTuner* knob = (TorqueTuner*)ptr;
	if (knob->velocity > 0) {
		return round((- (knob->angle_out / 3600.0) * knob->velocity) * knob->scale / MAX_VELOCITY);
	} else {
		return 0;
	}
}

int16_t LinSpring::calc(void* ptr) {
	TorqueTuner* knob = (TorqueTuner*)ptr;
	float val = - (knob->angle_out - 1800) / 1800.0;
	if (knob->angle_unclipped <= min) {
		val = 1;
	} else if (knob->angle_unclipped >= max) {
		val = -1;
	}
	val *= knob->scale;
	return static_cast<int16_t> (round(val));
}

int16_t ExpSpring::calc(void* ptr) {
	TorqueTuner* knob = (TorqueTuner*)ptr;
	float val = static_cast<float>(tf_exp_spring[idx]) / TABLE_RESOLUTION;

	if (knob->angle_unclipped <= min) {
		val = 1;
	} else if (knob->angle_unclipped >= max) {
		val = -1;
	}

	val *= knob->scale;
	return static_cast<int16_t> (round(val));
}


int16_t Free::calc(void* ptr) {
	TorqueTuner* knob = (TorqueTuner*)ptr;
	return knob->scale;
}

int16_t Spin::calc(void* ptr) {
	TorqueTuner* knob = (TorqueTuner*)ptr;
	return knob->target_velocity;
}

 SerialListen::SerialListen(): Mode(MAX_TORQUE) {
	torqueIn = 0;
    uart_config_t uart_config0 = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,    //UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_APB,
    };

    //Configure UART1 parameters
    uart_param_config(0, &uart_config0);

    uart_set_pin(0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        
    //Install UART driver (we don't need an event queue here)
    //In this example we don't even use a buffer for sending data.
    uart_driver_install(0, UART_FIFO_LEN + 1, 0, 0, NULL, 0);

    xTaskCreate(serial_monitor, "serial_monitor", 2048, NULL, 10, NULL);
}

int16_t SerialListen::calc(void* ptr) {
	TorqueTuner* knob = (TorqueTuner*)ptr;
	angleOut = knob->angle;
	return torqueIn;
}

void SerialListen::set_active(bool is_active) {
	active = is_active;
}

void SerialListen::serial_monitor(void *pvParameters) {
    while(1) {
        //Read torque ints from UART
        serial_data_length = uart_read_bytes(0, serial_data, SERIAL_BUFSIZE, 20 / portTICK_RATE_MS);
        if (serial_data_length > 0) {
            serial_data_str = convertToString(serial_data);
            memset(serial_data, 0, sizeof serial_data);
            uart_flush(0);
        }

		if (!active) continue;

		// Write angle to serial
		std::string angleMessage = std::to_string(angleOut) + std::string(" ");
		uart_write_bytes(0, angleMessage.data(), angleMessage.size() * sizeof(char));
		
		// Interpret data
		if (serial_data_str.empty()) {
            continue;
        }
        // Convert torque value string to int
		char *endptr;
		const char *serial_data_cstr = serial_data_str.c_str();
		int possibleTorque = std::strtol(serial_data_cstr, &endptr, 10);
		if (endptr == serial_data_cstr) {
			uart_write_bytes(0, "bad\n", 4);
		} else {
			torqueIn = possibleTorque;
		}
        serial_data_str.clear();
    }
}

// Calculates an index for the look-up table based transfer functions
int16_t Mode::calc_index(void* ptr) {
	TorqueTuner* knob = (TorqueTuner*)ptr;

	state += static_cast<int16_t> (round(knob->angle_delta * knob->stretch));
	if (wrap_haptics)
	{
		idx = mod(state, 3600);
	} else {
		idx = clip(state, 0,  3600);
	}
	return idx;

};

void Mode::reset(int16_t angle_) {
	idx = offset; // apply mode specific offset to idx
	state = 0;
};

// Hybrid mode - under construction
