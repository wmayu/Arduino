#include <SPI.h>
#include <Ethernet.h>
#include <Stewitter.h>

byte mac[] = { 0xA8, 0xA61, 0x0A, 0xAE, 0x75, 0x33 };//MACアドレス
Stewitter twitter("0b3601d03fd0c1a5b8ae8c6711629d86");//Twiterのトークン

String last_msg;//1番最後にメンションされたツイート
String second_msg;//2番目に最後にメンションされたツイート

String c_hello("@arduinoprojec16  hello");
String c_tempe("arduinoprojec16|@arduinoprojec16  気温は？");
String c_help("@arduinoprojec16  help");

unsigned long last_time = millis();

void setup() {
  Serial.begin(9600);//9600bpsでシリアルポートを開く
  Ethernet.begin(mac);//MACアドレスの設定
  Serial.println("program start");
}

void loop() {  
  unsigned long now_time = millis()-last_time;
  if(now_time > 30000){
    Serial.println("確認中...");
    if(twitter.lastMention()){
      int status = twitter.wait();
      if(status == 200){
        char* tweet = select_tweet(check_lastmention());
        twitter.post(tweet);
        Serial.println("OK!");
      }else{ 
        Serial.print("failed : code ");
        Serial.println(status);
      }
    }
    last_time = millis();
  }
}


String check_lastmention(){//最後にメンションされたツーイート内容が新しいかの確認
  second_msg = last_msg;
  last_msg = twitter.response();//最後にメンションされたツーイート内容を取得
  if(last_msg != second_msg){
    Serial.println(last_msg); 
    return last_msg;
  }
  return "0";
}

char *select_tweet(String msg){//ツイートするメッセージの決定
  String message1 = "こんにちは！気温をツイートします。%0D分からない場合は'help'とつぶやいてください";
  char* hello = message1.c_str();
  String message2 = "気温はxx℃です。";
  char* temp = message2.c_str();
  if(msg.indexOf(c_hello) != -1){
    Serial.println("1");
    return hello;
  }else if(last_msg.indexOf(c_tempe) != -1){
    Serial.println("2");
    return temp;
  }else if(last_msg.indexOf(c_help) != -1){
    Serial.println("3");
    return "help";
  }else{
    Serial.println("0");
    return "error";
  }
}