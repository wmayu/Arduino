#include <SPI.h>
#include <Ethernet.h>
#include <Stewitter.h>

byte mac[] = { 0xA8, 0xA61, 0x0A, 0xAE, 0x75, 0x33 };//MACアドレス
Stewitter twitter("0b3601d03fd0c1a5b8ae8c6711629d86");//Twiterのトークン

String last_msg;//1番最後にメンションされたツイート
String second_msg;//2番目に最後にメンションされたツイート
String tweet_msg;//tweetする内容
char* tweet_ms;
boolean tweet;//ツイートするかどうか

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
  if(now_time > 60000){
    Serial.println("確認中...");
    if(twitter.lastMention()){
      int status = twitter.wait();  
      if(status == 200){
        check_lastmention();
        select_tweet(); 
        tweet_ms = tweet_msg.c_str();
        if(tweet&&twitter.post(tweet_ms)){
          Serial.print("tweet_msg:");
          Serial.println(tweet_msg);
          Serial.println("OK!");
          tweet = false;
        }else{
          Serial.println("スルー");//本来要らないコード
        }
      }else{
                    Serial.print("failed : code ");
            Serial.println(status);
      }
    }
    last_time = millis();
  }
}

void check_lastmention(){//最後にメンションされたツーイート内容が新しいかの確認
  last_msg = twitter.response();//最後にメンションされたツーイート内容を取得
  if(last_msg != second_msg){
    Serial.print("last_msg:");
    Serial.println(last_msg); 
    second_msg = last_msg;
    tweet = true;
  }
}

void select_tweet(){//ツイートするメッセージの決定
  if(last_msg.indexOf(c_hello) != -1){
    Serial.println("tweet:hello");
    tweet_msg = "こんにちは！気温をツイートします。%0D分からない場合は'help'とつぶやいてください";
  }else if(last_msg.indexOf(c_tempe) != -1){
    Serial.println("tweet:temp");
    tweet_msg = "気温はxx℃です。";
  }else if(last_msg.indexOf(c_help) != -1){
    Serial.println("tweet:help");
    tweet_msg = "気温は?と聞くと気温をつぶやきます。";
  }else{
    Serial.println("error");
  }
}