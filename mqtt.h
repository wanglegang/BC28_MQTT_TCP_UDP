#include <SoftwareSerial.h> 
#define DEBUG
#define _baudrate   9600
// USB TTL 检错用
#define _rxpin      8
#define _txpin      9
SoftwareSerial debug( _rxpin, _txpin ); // RX, TX

void(* resetFunc) (void) = 0; //重启
void sendDebug(String cmd)
{
  // 传到 USB TTL
  debug.print("SEND: ");
  debug.println(cmd);
  Serial.println(cmd);
} 
void InitAT(){
  //初始化at指令
  //模块重启
  sendDebug("AT+NRB");
  delay(11000);
  while (!Serial.find("OK"))
  {
    sendDebug("AT");
    delay(500);    
  }
  delay(1000); 
  //读取sim卡信息
  sendDebug("AT+CIMI");
  if (Serial.find("ERROR")) {
    for(int i = 0; i < 5 ; i++) //根据gps数据长度循环
    {
      sendDebug("AT+CIMI");
      delay(500); 
    }
  }
  delay(500); 
  //查看网络附着状态
  sendDebug("AT+CGATT?");
  delay(500); 
  //查看信号强度
  sendDebug("AT+CSQ");   
  //断开与其他平台连接
  sendDebug("AT+QREGSWT=2");
  delay(150); 
  sendDebug("AT+QREGSWT=1");
  delay(150); 
  //获取模组的IP地址
  sendDebug("AT+CGPADDR");
  delay(500); 
  //切换MQTT的协议版本，否则会被拒绝连接 
  sendDebug(" AT+QMTCFG=\"version\",0,4 ");
  //MQTT服务器地址和端口号
  sendDebug("AT+QMTOPEN=0,\"129.28.200.*\",1883 ");
  delay(500); 
  //MQTT客户端号
  sendDebug("AT+QMTCONN=0,\"525716215\"");
  delay(2500); 
  //订阅名称为RECEIVE的TOPIC
  sendDebug("AT+QMTSUB=0,1,\"topic\",2 ");
}

void setup() {  
  Serial.begin( _baudrate );
  debug.begin( _baudrate );
  InitAT();
}

void loop() {

    delay(8000); 
  //推送消息到RECEIVE，发送指令后返回>
  sendDebug("AT+QMTPUB=0,0,0,1,\"topic\"");
  delay(2000); 
  if(Serial.find("ERROR"))
  {
    Serial.println("RST");
    resetFunc(); //重启单片机
  } 
  else
  {  
    sendDebug("hello world!");
    delay(100); 
    Serial.write(0x1A);  // 串口输出0x1A
    delay(100);    
  }
}


