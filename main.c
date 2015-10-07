#include <pololu/3pi.h>
#include <avr/pgmspace.h>
#include "bargraph.h"
#include "maze-solve.h"
#include "follow-segment.h"
#include "turn.h"


const char welcome_line1[] PROGMEM = " Pololu";
const char welcome_line2[] PROGMEM = "3\xf7 Robot";
const char demo_name_line1[] PROGMEM = "Maze";
const char demo_name_line2[] PROGMEM = "solver";
const char welcome[] PROGMEM = ">g32>>c32";
const char go[] PROGMEM = "L16 cdegreg4";


int d =1;　//dの値により、探索手法を定める
char path[1000] = "";//経路を記憶
unsigned char path_length = 0; //経路長
void time_reset();

int total = 0;
int n = 0;
int mukis[100];
int tates[100];
int yokos[100];

int main()
{
    //初期化
    initialize();
    
    //走行を開始
    maze_solve();
    
    while(1);
}


void maze_solve()
{
    int muki =21;
    int tate = 1;
    int yoko =0;
    
    int p=0;
    int q=0;
    
    int j=0;
    int l=0;
    
    long int t1,t2 = 0;
    time_reset();
    t1 = get_ms();//スタートした時間を格納
    
    while(1)
    {
        //LCDに表示
        clear();
        print_long(d);
        print_long(muki);
        print_long(tate);
        print_long(yoko);
        
        //ライントレースにより走行
        follow_segment();
        set_motors(50,50);
        delay_ms(50);
        
        //走行可能な方向があれば、1を格納する
        unsigned char found_left=0;
        unsigned char found_straight=0;
        unsigned char found_right=0;
        unsigned int sensors[5];
        read_line(sensors,IR_EMITTERS_ON);
        if(sensors[0] > 100)
        found_left = 1;
        if(sensors[4] > 100)
        found_right = 1;
        set_motors(40,40);
        delay_ms(100);
        
        //直進可能ならば、1を格納する
        read_line(sensors,IR_EMITTERS_ON);
        if(sensors[1] > 200 || sensors[2] > 200 || sensors[3] > 200)
        found_straight = 1;
        
        //ゴールと判断し、走行を終了する
        if(sensors[1] > 600 && sensors[2] > 600 && sensors[3] > 600)
        break;
        
        
        
        //優先度より、進む方向を決める
        unsigned char dir = select_turn(found_left, found_straight, found_right,d);
        
        //方向転換する
        turn(dir);
        
        //方向転換して進んだロボットの向きを、値として記憶
        switch(dir)
        {
            case 'L':
            muki += 1;
            break;
            
            case 'R':
            muki -= 1;
            break;
            
            case 'B':
            muki += 2;
            break;
            
            case 'S':
            break;
        }
        
        //記憶したロボットの向きの値より、現在の座標を更新する
        switch(muki%4)
        {
            case(1):
            tate += 1;
            break;
            case(2):
            yoko -= 1;
            break;
            case(3):
            tate -= 1;
            break;
            case(0):
            yoko +=1;
            break;
        }
        
        
        mukis[p]=muki%4;
        tates[p]=tate;
        yokos[p]=yoko;
        l=0;
        
        //ロボットが同じ向き、座標に来ていないか(ループに入っていないか)チェック
        for(q=0;q<p;q++)
        {
            if(mukis[q]==muki%4 && tates[q]==tate && yokos[q] == yoko)
            {
                l++;
            }
            
        }
        //ループに入っていた場合、dの値が変わり、探索方法が変更される
        d=1+l;
        
        p++;
        
        //経路長を更新
        path[path_length] = dir;
        path_length ++;
        j++;
        
        //経路を簡略化
        simplify_path();
        
        
    }
    
    
    //探索後
    long int t3,t4,t5,t6;
    
    while(1)
    {
        set_motors(0,0);
        play(">>a32");
        
        //終了時間を格納
        t2 = get_ms();
        t3 = (t2 - t1)/1000;
        t4 = t2 - t1 - t3*1000;
        t5 = t4%100;
        t6 = (t4 - t5)/100;
        
        //探索結果を表示
        while(!button_is_pressed(BUTTON_B))
        {
            if(get_ms() % 2000 < 1000)
            {
                clear();
                print_long(t3);
                print(".");
                print_long(t6);
                lcd_goto_xy(0,1);
                print_long(path_length);
                print("  ");
                print_long(j);
            }
            else
            display_path();
            delay_ms(30);
        }
        while(button_is_pressed(BUTTON_B));
        
        
        
        //簡略化した経路(最短経路)を再び走行
        delay_ms(1000);
        time_reset();
        t1 = get_ms();
        
        int i;
        for(i=0;i<path_length;i++)
        {
            
            follow_segment();
            
            set_motors(50,50);
            delay_ms(50);
            set_motors(40,40);
            delay_ms(100);
            
            turn(path[i]);
        }
        
        follow_segment();
        
    }
    
}

//LCDに経路長を表示
void display_path()
{
    
    path[path_length] = 0;
    
    clear();
    print(path);
    
    if(path_length > 8)
    {
        lcd_goto_xy(0,1);
        print(path+8);
    }
}


//方向転換の値を返す
char select_turn(unsigned char found_left, unsigned char found_straight, unsigned char found_right,int d)
{
    if(d%2 == 1)//int dの値が奇数なら左手法、偶数なら右手法
    {
        if(found_left)
        return 'L';
        else if(found_straight)
        return 'S';
        else if(found_right)
        return 'R';
        else
        return 'B';
    }
    else{
        if(found_right)
        return 'R';
        else if(found_straight)
        return 'S';
        else if(found_left)
        return 'L';
        else
        return 'B';
        
    }
}

//記憶した経路を簡略化し、最短経路を再び記憶する
void simplify_path()
{
    if(path_length < 3 || path[path_length-2] != 'B')
    return;
    
    int total_angle = 0;//ロボットの向き
    int i;
    
    //記憶した最新の3経路の向きを、角度に変換し、足す
    for(i=1;i<=3;i++)
    {
        switch(path[path_length-i])
        {
            case 'R':
            total_angle += 90;
            break;
            case 'L':
            total_angle += 270;
            break;
            case 'B':
            total_angle += 180;
            break;
        }
    }
    
    total_angle = total_angle % 360; //角度の合計を360度以下にする
    
    //合計の角度を1経路の角度とし、その向きを再び記憶する
    switch(total_angle)
    {
        case 0:
        path[path_length - 3] = 'S';
        break;
        case 90:
        path[path_length - 3] = 'R';
        break;
        case 180:
        path[path_length - 3] = 'B';
        break;
        case 270:
        path[path_length - 3] = 'L';
        break;
    }
    
    // 3経路を1経路に簡略化したので、2経路減らす
    path_length -= 2;
}




//初期化
void initialize()
{
	unsigned int counter; // used as a simple timer
	unsigned int sensors[5]; // an array to hold sensor values

	// This must be called at the beginning of 3pi code, to set up the
	// sensors.  We use a value of 2000 for the timeout, which
	// corresponds to 2000*0.4 us = 0.8 ms on our 20 MHz processor.
	pololu_3pi_init(2000);
	load_custom_characters(); // load the custom characters
	
	// Play welcome music and display a message
	print_from_program_space(welcome_line1);
	lcd_goto_xy(0,1);
	print_from_program_space(welcome_line2);
	play_from_program_space(welcome);
	delay_ms(1000);

	clear();
	print_from_program_space(demo_name_line1);
	lcd_goto_xy(0,1);
	print_from_program_space(demo_name_line2);
	delay_ms(1000);

	// Display battery voltage and wait for button press
	while(!button_is_pressed(BUTTON_B))
	{
		int bat = read_battery_millivolts();

		clear();
		print_long(bat);
		print("mV");
		lcd_goto_xy(0,1);
		print("Press B");

		delay_ms(100);
	}

	// Always wait for the button to be released so that 3pi doesn't
	// start moving until your hand is away from it.
	wait_for_button_release(BUTTON_B);
	delay_ms(1000);

	// Auto-calibration: turn right and left while calibrating the
	// sensors.
	for(counter=0;counter<80;counter++)
	{
		if(counter < 20 || counter >= 60)
			set_motors(40,-40);
		else
			set_motors(-40,40);

		// This function records a set of sensor readings and keeps
		// track of the minimum and maximum values encountered.  The
		// IR_EMITTERS_ON argument means that the IR LEDs will be
		// turned on during the reading, which is usually what you
		// want.
		calibrate_line_sensors(IR_EMITTERS_ON);

		// Since our counter runs to 80, the total delay will be
		// 80*20 = 1600 ms.
		delay_ms(20);
	}
	set_motors(0,0);

	// Display calibrated values as a bar graph.
	while(!button_is_pressed(BUTTON_B))
	{
		// Read the sensor values and get the position measurement.
		unsigned int position = read_line(sensors,IR_EMITTERS_ON);

		// Display the position measurement, which will go from 0
		// (when the leftmost sensor is over the line) to 4000 (when
		// the rightmost sensor is over the line) on the 3pi, along
		// with a bar graph of the sensor readings.  This allows you
		// to make sure the robot is ready to go.
		clear();
		print_long(position);
		lcd_goto_xy(0,1);
		display_readings(sensors);

		delay_ms(100);
	}
	wait_for_button_release(BUTTON_B);
	clear();
	print("Go!");		

	// Play music and wait for it to finish before we start driving.
	play_from_program_space(go);
	while(is_playing());
}
