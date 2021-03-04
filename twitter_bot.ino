#include <SPI.h>
#include <Ethernet.h>
#include <Stewitter.h>
#include <DHT.h> 
#define DHT_PIN 8
#define DHT_TYPE DHT11

byte mac[] = { 0x--, 0x--, 0x--, 0x--, 0x--, 0x-- };//MACアドレス
Stewitter twitter("your token");//Twiterのトークン

String last_msg;//1番最後にメンションされたツイート
String second_msg;//2番目に最後にメンションされたツイート
String tweet_msg;//tweetする内容
char* tweet_ms;
boolean tweet;//ツイートするかどうか

String c_hello("@xxxxxx  hello");
String c_tempe("xxxxxx|@xxxxxx  気温は？");
String c_help("@xxxxxx  help");

void setup() {
  Serial.begin(9600);//9600bpsでシリアルポートを開く
  Ethernet.begin(mac);//MACアドレスの設定
  Serial.println("program start");
}

void loop(){
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
        Serial.println("新しいメンションはありません.");
      }
    }else{
      Serial.print("failed : code ");
      Serial.println(status);
    }
  }
  delay(60000);
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
    DHT dht(DHT_PIN,DHT_TYPE);//センサの初期化
    dht.begin();
    delay(2000);//正確なデータを取るための時間
    float temperature = dht.readTemperature();//温度
    float humidity    = dht.readHumidity();//湿度
    Serial.println("tweet:temp");
    tweet_msg = "気温は" + String(temperature) + "℃%0D湿度は" + String(humidity) + "%です。";
  }else if(last_msg.indexOf(c_help) != -1){
    Serial.println("tweet:help");
    tweet_msg = "気温を教えてくださいと聞くと気温をつぶやきます。";
  }else{
    Serial.println("error");
  }
}
