#include <HTTP_Method.h>
#include <Uri.h>
#include <WebServer.h>

#include <M5AtomS3.h>
#include "myDefs.h"

const int monCh1 = 5 ;
const int monCh2 = 6 ;
const int monCh3 = 7 ;
const int monCh4 = 8 ;
const int genCh1 = 38 ;
const int genCh2 = 39 ;

WebServer server(MYWIFIPORT) ;

const int SAMPLEMAX = 256 ;   // 128dot * 2
struct TSAMPLES {
  unsigned int datas[SAMPLEMAX] ;
  int trigIdx = -1 ;
  int afterTrig = 0 ;
  int smpIdx = 0 ;
  int displayed = 0 ;
} ;

TSAMPLES samples[2] ;
int samplePane = 0 ;

int sampleCount = 0 ;
void taskSampling(void* pvParameters) {  // Define the tasks to be executed in
    
  // thread 1.  定义线程1内要执行的任务
  TSAMPLES * psamp ;
  TSAMPLES * psampback ;

  int data = 0 ;
  while (1) {  // Keep the thread running.  使线程一直运行
    psamp = &samples[samplePane] ;
    data = 0 ;
    if ( digitalRead(monCh1) == HIGH )
      data = 2 ;
    else
      data = 1 ;
    if ( digitalRead(monCh2) == HIGH )
      data += 8 ;
    else
      data += 4 ;
    if ( digitalRead(monCh3) == HIGH )
      data += 32 ;
    else
      data += 16 ;
    if ( digitalRead(monCh4) == HIGH )
      data += 64 ;
    else
      data += 128 ;
    psamp->datas[psamp->smpIdx] |= data ;
    psamp->smpIdx ++ ;
    if ( psamp->smpIdx >= SAMPLEMAX )
      psamp->smpIdx = 0 ;
    
//  トリガ成立のダミー -->>
if (psamp->smpIdx == 150 )
  psamp->trigIdx = psamp->smpIdx ;
//  トリガ成立のダミー --<<

    if ( psamp->trigIdx >= 0 ){
      psamp->afterTrig ++ ;
      if ( psamp->afterTrig > SAMPLEMAX / 2 ){
        //  サンプル満タン完了
        //  裏に移動（表示完了していたら、ってこともあるかも）
        int nextPane ;
        if ( samplePane == 0 )
          nextPane = 1 ;
        else
          nextPane = 0 ;
        //  裏を初期化
        psampback = &samples[nextPane] ;
        if ( psampback->displayed != 0 ){
          //  裏が表示済みなら裏確定
          samplePane = nextPane ;
        }else{
          //  今のを上書きする
          psampback = psamp ;
        }
        int i ;
        psampback->trigIdx = -1 ;
        psampback->afterTrig = 0 ;
        psampback->smpIdx = 0 ;
        psampback->displayed = 0 ;
        for ( i=0 ; i < SAMPLEMAX ; i ++ )
          psampback->datas[i] = 0 ;        
        sampleCount ++ ;
        USBSerial.print("sample ");
        USBSerial.println(sampleCount) ;
      }
    }else
      psamp->afterTrig = 0 ;



//      USBSerial.print("taskSampling Uptime (ms): ");
//      USBSerial.println(
//          millis());  // The running time of the serial port
//                      // printing program.  串口打印程序运行的时间
//      delay(
//          100);  // With a delay of 100ms, it can be seen in the serial
                  // monitor that every 100ms, thread 1 will be executed once.
                  // 延迟100ms,在串口监视器内可看到每隔100ms,线程1就会被执行一次
  }
}

void taskDraw(void* pvParameters) {  // Define the tasks to be executed in
                                  // thread 1.  定义线程1内要执行的任务
  TSAMPLES * psamp ;
  TSAMPLES * psampback ;


  while (1) {  // Keep the thread running.  使线程一直运行
    if ( samplePane == 0 )
      psamp = &samples[1] ;
    else
      psamp = &samples[0] ;

    if ( psamp->displayed != 0 ){
      //  表示済みなら少し待つ
      delay(100) ;
    }else{
      //  表示前なら、新しいデータ。表示する
      int idx = psamp->trigIdx ;
      int finidx ;
      int i,x ;
      int d ;
      idx = idx - 64 ;
      finidx = idx + 128 ;
      x = 0 ;
      const int ch1Offset = 128-6 ;
      const int ch2Offset = 128-6*2 ;
      const int ch3Offset = 128-6*3 ;
      const int ch4Offset = 128-6*4 ;
      for ( i = idx ; i < finidx ; i ++){
        d = psamp->datas[i] ;

        //  0     *****
        //  1     *****
        //  2     *****
        //  3 ****    
        //  4 
        //  5

        // CH1 黄色
        M5.lcd.drawLine( x , ch1Offset + 4 , x , ch1Offset+0 ,BLACK ) ;
        if ( d & 1 )
          M5.lcd.drawLine( x , ch1Offset + 4 , x , ch1Offset+3 ,WHITE ) ;
        if ( d & 2 )
          M5.lcd.drawLine( x , ch1Offset + 2 , x , ch1Offset+0 ,WHITE ) ;
        // CH2 水色
        M5.lcd.drawLine( x , ch2Offset + 4 , x , ch2Offset+0 ,BLACK ) ;
        if ( d & 4 )
          M5.lcd.drawLine( x , ch2Offset + 4 , x , ch2Offset+3 ,WHITE ) ;
        if ( d & 8 )
          M5.lcd.drawLine( x , ch2Offset + 2 , x , ch2Offset+0 ,WHITE ) ;
        // CH3 マゼンダ
        M5.lcd.drawLine( x , ch3Offset + 4 , x , ch3Offset+0 ,BLACK ) ;
        if ( d & 16 )
          M5.lcd.drawLine( x , ch3Offset + 4 , x , ch3Offset+3 ,WHITE ) ;
        if ( d & 32 )
          M5.lcd.drawLine( x , ch3Offset + 2 , x , ch3Offset+0 ,WHITE ) ;
        // CH4 緑
        M5.lcd.drawLine( x , ch4Offset + 4 , x , ch4Offset+0 ,BLACK ) ;
        if ( d & 64 )
          M5.lcd.drawLine( x , ch4Offset + 4 , x , ch4Offset+3 ,WHITE ) ;
        if ( d & 128 )
          M5.lcd.drawLine( x , ch4Offset + 2 , x , ch4Offset+0 ,WHITE ) ;
        x ++ ;
      }



      //  表示済みフラグを立てる
      psamp->displayed = 1 ;

    }
      USBSerial.print("taskDraw Uptime (ms): ");
      USBSerial.println(
          millis());  // The running time of the serial port
                      // printing program.  串口打印程序运行的时间
      delay(
          100);  // With a delay of 100ms, it can be seen in the serial
                  // monitor that every 100ms, thread 1 will be executed once.
                  // 延迟100ms,在串口监视器内可看到每隔100ms,线程1就会被执行一次
  }
}

void WifiConnect(){
  WiFi.begin(MYWIFISSID,MYWIFIPASS);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    M5.Lcd.print('.');
  }
  M5.Lcd.print("\r\nWiFi connected\r\nIP address: ");
  M5.Lcd.println(WiFi.localIP());
}


void setup()
{
  M5.begin(true,true,false,false) ;

  M5.Lcd.fillRect(10,10,30,30,RED) ;
  M5.Lcd.println( "TEST") ;
  M5.Lcd.println( "TEST1") ;
  M5.Lcd.println( "TEST2") ;

  pinMode(monCh1,INPUT) ;
  pinMode(monCh2,INPUT) ;
  pinMode(monCh3,INPUT) ;
  pinMode(monCh4,INPUT) ;

  pinMode(genCh1,OUTPUT) ;
  pinMode(genCh2,OUTPUT) ;

  WifiConnect() ;
  server.on("/",handleRoot) ;
  server.onNotFound(handleNotFound) ;
  server.begin() ;
  M5.Lcd.println("HTTP Server Start") ;

    // Creat Task1.  创建线程1
    xTaskCreatePinnedToCore(
        taskSampling,    // Function to implement the task.
                  // 线程对应函数名称(不能有返回值)
        "taskSample",  //线程名称
        4096,     // The size of the task stack specified as the number of *
                  // bytes.任务堆栈的大小(字节)
        NULL,     // Pointer that will be used as the parameter for the task *
                  // being created.  创建作为任务输入参数的指针
        1,        // Priority of the task.  任务的优先级
        NULL,     // Task handler.  任务句柄
        1);  // Core where the task should run.  将任务挂载到指定内核

    // Task 2
    xTaskCreatePinnedToCore(taskDraw, "taskDraw", 4096, NULL, 2, NULL, 0);

}

void loop()
{
  //  GET BIT FROM PIO
  server.handleClient() ;
}

int temp = 0 ;
char buf[1024];

void handleRoot(){
  temp = temp+1 ;
  
  sprintf(buf, 
    "<html>\
     <head>\
        <title>Count</title>\
     </head>\
     <body>\
        <h1>M5AtomS3 CallCount</h1>\
        <p>Count : %d times</p>\
     </body>\
     </html>",
  temp);
  server.send(200, "text/html", buf);
  M5.Lcd.println("accessed on \"/\"");
}

void handleNotFound(){
  server.send(404, "text/plain", "File Not Found\n\n");
  M5.Lcd.println("File Not Found");
}
