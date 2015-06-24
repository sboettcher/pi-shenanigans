#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

#include <string>
#include <map>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "./Adafruit_GFX.h"
#include "./Adafruit_ILI9340.h"

#define _dc 24
#define _rst 25

Adafruit_ILI9340 tft = Adafruit_ILI9340(_dc, _rst);
int running = 1;
static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

void sig_handler(int signo) {
  if (signo == SIGINT || signo == SIGTERM) {
    printf("statsMain recieved INT or TERM, exiting...\n");
    running = 0;
  }
}

const std::string currentDateTime() {
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%Y-%m-%d - %R", &tstruct);
  return buf;
}

void CPU_init(){
  FILE* file = fopen("/proc/stat", "r");
  fscanf(file, "cpu %Ld %Ld %Ld %Ld", &lastTotalUser, &lastTotalUserLow, &lastTotalSys, &lastTotalIdle);
  fclose(file);
}
double CPU_getCurrentValue(){
  double percent;
  FILE* file;
  unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

  file = fopen("/proc/stat", "r");
  fscanf(file, "cpu %Ld %Ld %Ld %Ld", &totalUser, &totalUserLow, &totalSys, &totalIdle);
  fclose(file);
  

  if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
    totalSys < lastTotalSys || totalIdle < lastTotalIdle){
    //Overflow detection. Just skip this value.
    percent = -1.0;
  }
  else{
    total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) + (totalSys - lastTotalSys);
    percent = total;
    total += (totalIdle - lastTotalIdle);
    percent /= total;
    percent *= 100;
  }
  
  lastTotalUser = totalUser;
  lastTotalUserLow = totalUserLow;
  lastTotalSys = totalSys;
  lastTotalIdle = totalIdle;
  
  return percent;
}

std::map<std::string, std::string> getIPs() {
  struct ifaddrs * ifAddrStruct=NULL;
  struct ifaddrs * ifa=NULL;
  void * tmpAddrPtr=NULL;
  std::map<std::string, std::string> IPs;

  getifaddrs(&ifAddrStruct);

  for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
    if (!ifa->ifa_addr) {
      continue;
    }
    if (ifa->ifa_addr->sa_family == AF_INET) { // check if it is IP4
      tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      char addressBuffer[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
      IPs[ifa->ifa_name] = addressBuffer;
    } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check if it is IP6
      tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
      char addressBuffer[INET6_ADDRSTRLEN];
      inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
      IPs[ifa->ifa_name] = addressBuffer;
    } 
  }
  if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
  return IPs;
}

float RAM_getTotalValue() {
  struct sysinfo memInfo;
  sysinfo (&memInfo);
  long long totalPhysMem = memInfo.totalram;
  // Multiply in next statement to avoid int overflow on right hand side...
  totalPhysMem *= memInfo.mem_unit;
  return totalPhysMem;
}

float RAM_getCurrentValue() {
  struct sysinfo memInfo;
  sysinfo (&memInfo);
  long long physMemUsed = memInfo.totalram - memInfo.freeram;
  // Multiply in next statement to avoid int overflow on right hand side...
  physMemUsed *= memInfo.mem_unit;
  return physMemUsed;
}

int main(int argc, char** argv) {
  signal(SIGINT, sig_handler);
  signal(SIGTERM, sig_handler);

  tft.begin();
  tft.fillScreen(ILI9340_BLACK);
  tft.drawRect(0, 0, ILI9340_TFTWIDTH, ILI9340_TFTHEIGHT, ILI9340_WHITE);
  tft.drawRect(1, 1, ILI9340_TFTWIDTH-2, ILI9340_TFTHEIGHT-2, ILI9340_WHITE);
  tft.drawRect(2, 2, ILI9340_TFTWIDTH-4, ILI9340_TFTHEIGHT-4, ILI9340_WHITE);
  tft.drawFastVLine(35, 0, ILI9340_TFTHEIGHT, ILI9340_WHITE);
  tft.drawFastVLine(90, 0, ILI9340_TFTHEIGHT, ILI9340_WHITE);

  tft.setRotation(3);
  tft.setTextColor(ILI9340_WHITE, ILI9340_BLACK);
  tft.setTextSize(2);

  std::map<std::string, std::string> IPs;
  std::stringstream cpu;
  std::stringstream ram;
  
  CPU_init();
  while (running == 1) {
    tft.setCursor(10, 15);
    tft.print(currentDateTime());

    IPs = getIPs();
    tft.setCursor(10,45);
    tft.print("eth0: ");
    if (IPs.find("eth0") == IPs.end()) {
      IPs["eth0"] = "               ";
    }
    tft.print(IPs["eth0"]);
    tft.setCursor(10,70);
    tft.print("wlan0: ");
    if (IPs.find("wlan0") == IPs.end()) {
      IPs["wlan0"] = "               ";
    }
    tft.print(IPs["wlan0"]);

    ram.str("");
    ram << std::setfill(' ') << std::setw(3) << (int)(RAM_getCurrentValue()/(1024*1024)) << "/" << (int)(RAM_getTotalValue()/(1024*1024));
    tft.setCursor(10, 100);
    tft.print("RAM: ");
    tft.print(ram.str());
    tft.print(" MiB");

    cpu.str("");
    cpu << std::setfill(' ') << std::setw(3) << (int)CPU_getCurrentValue();
    tft.setCursor(10, 125);
    tft.print("CPU: ");
    tft.print(cpu.str());
    tft.print("%");
    usleep(2000000);
  }
  
  return 0;
}

