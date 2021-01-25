#include <SoftwareSerial.h>
#define rxs 10 // LoraRF rx
#define txs 11 // LoraRF tx
#define led 13 // arduino nano led
#define m0 2 // LoraRF M0
#define m1 3 // LoraRF M1

const int device_id = 18;
 
const char cmd_connect[] = "cnn";
const char cmd_config[] = "cfn";
const char cmd_readings[] = "rds";
const char cmd_ack[] = "ack";
const char key[] = "lra";

// time related variables    // start
const int tx_connect = 15;
volatile int tx_period = 300;
volatile int tx_slot = 5;
volatile int rx_time = 5;
volatile int sync = 0;
// time related              // end

// output variables          // start
volatile int actuator1 = 0;
volatile int actuator2 = 0;
// output variables    // end

// sensor variables          // start
volatile int sensor1 = 0;
volatile int sensor2 = 0;
volatile int sensor3 = 0;
// sensor variables    // end

volatile bool flag_configured = false;

volatile int edges[2];

const int buffer_size = 40;
volatile char in_c;
volatile int in_i = 0;
char out_buffer[buffer_size];
char in_buffer[buffer_size];
char keyid[10];
SoftwareSerial SSerial(rxs,txs);

void setup(){
	pinMode(led, OUTPUT);
	pinMode(m0, OUTPUT);
	pinMode(m1, OUTPUT);
  	SSerial.begin(9600);
  	Serial.begin(115200);
}

void send_command(const char *message){
	SSerial.println(message);
	Serial.println(message);
}

void connect_frame(int id){
	sprintf(out_buffer, "%s%d%s", key, id, cmd_connect);
}

void data_frame(int id){
	sprintf(out_buffer, "%s%d%s,%d,%d,%d",
		key, id, cmd_readings,
		sensor1, sensor2, sensor3
	);
}

int strtoi(const char *text, const int start, const int end){
	// xx21xx
	int result = 0;
	int i = 0;
	while (i <= (end-start)){
		result = 10*result + text[start+i] - 48;
		i++;
	}
	// 21
	return result;
}

int find_chr(const char *text, const int start, const char chr){
	char * pch;
	pch = (char*) memchr (&text[start], chr, strlen(text));
	if (pch != NULL) return min(pch - text, strlen(text)-1);
}

void find_edges(const char *text, int order, const char chr){
	int start = 0;
	int end = 0;
	int i = 0;
	while ((find_chr(text, start, chr) != -1) && (i<=order)){
		start = end;
		end = find_chr(text, start+1, chr);
		i++;
	}
	edges[0] = start == 0 ? start : start+1;
	edges[1] = end - 1;
}

int split_chr(const char *text, const char chr, const int part){
	find_edges(text,part,chr);
	return strtoi(text,edges[0],edges[1]);
}

void proccess_config(){
	//config,20,5,5,15
	tx_period = split_chr(in_buffer, ',', 1);
	tx_slot = split_chr(in_buffer, ',', 2);
	rx_time = split_chr(in_buffer, ',', 3);
	sync = split_chr(in_buffer, ',', 4);
	flag_configured = true;
	rf_sleep();
}

void proccess_ack(){
	sync = split_chr(in_buffer, ',', 1);
	actuator1 = split_chr(in_buffer, ',', 2);
	actuator2 = split_chr(in_buffer, ',', 3);
	flag_configured=true;
}

void proccess_commands(){
	sprintf(keyid, "%s%d", key, device_id);
	if(memcmp(keyid, in_buffer, strlen(keyid)) == 0) {
		//secretidxxxxxxxxxxx
		memcpy(&in_buffer, &in_buffer[strlen(keyid)] ,strlen(in_buffer)-strlen(keyid));
		//xxxxxxxxxxxxxxxxxxx
		in_buffer[strlen(in_buffer)-strlen(keyid)] = 0;
		if(memcmp(cmd_config, in_buffer, strlen(cmd_config))==0) proccess_config();
		if(memcmp(cmd_ack, in_buffer, strlen(cmd_ack))==0) proccess_ack();
	}
}

void read_commands(){
	while (SSerial.available())
    {
	    in_c = SSerial.read();
	    in_buffer[in_i] = in_c;
	    in_i += 1;
	    if ((in_c == '\n') || (in_c == '\r')){
	    	in_buffer[in_i] = 0;
	    	in_i = 0;
	    	Serial.print(in_buffer);
	    	proccess_commands();
	    }
	}
}

void tic() {
	if (flag_configured) {
		digitalWrite(led, HIGH);delay(20);
		digitalWrite(led, LOW);	delay(980);
	} else {
		digitalWrite(led, HIGH);delay(20);
		digitalWrite(led, LOW);	delay(200);
		digitalWrite(led, HIGH);delay(20);
		digitalWrite(led, LOW);	delay(760);
	}
	
	Serial.println(">>>>>> Actuadores >>>>>>");
	Serial.println(actuator1);
	Serial.println(actuator2);
	Serial.println(">>>>>>>>>>>>");

	sync++;
	if (sync >= tx_period) sync = 0;

}

void rf_awake() {
	digitalWrite(m0, LOW);
	digitalWrite(m1, LOW);
}

void rf_sleep() {
	digitalWrite(m0, HIGH);
	digitalWrite(m1, HIGH);
}

void loop(){
	tic();
	if (sync != 0){
		
		if (!flag_configured && sync%tx_connect == 0) {
			rf_awake();
			connect_frame(device_id);
			send_command(out_buffer);
		}

		if (flag_configured && sync == tx_slot) {
			rf_awake();
		}

		if (flag_configured && sync == (tx_slot + 1)) {
			data_frame(device_id);
			send_command(out_buffer);
			flag_configured = false;
		}

		if (flag_configured && sync == (tx_slot + rx_time)) {
			rf_sleep();
		}
	}
	read_commands();
}
