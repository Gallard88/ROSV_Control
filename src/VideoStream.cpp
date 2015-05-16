

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "VideoStream.h"
#include "EventMessages.h"

#define BUFFER_SIZE	(1024)
const char *VideoFileName = "/home/pi/Video.h264";

VideoStreamer::VideoStreamer(int port):
  VideoWidth(1024), VideoHeight(768),
  VideoFrame(20), VideoDuration(60*60),
  Port(port), Recording(false)
{
  struct sockaddr_in serv_addr;
  Msg = EventMsg::Init();

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0) {
    exit(-1);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  if (bind(listen_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    exit(-1);
  }
  listen( listen_fd, 1);
}

VideoStreamer::~VideoStreamer()
{
  close(listen_fd);
  listen_fd = -1;
}

std::string VideoStreamer::GetClientName(void)
{
  std::string s;
  mtx.lock();
  s = Client_Name;
  mtx.unlock();
  return s;
}

unsigned int VideoStreamer::Get_Port(void)
{
  return Port;
}

unsigned int VideoStreamer::Get_VideoWidth(void)
{
  return VideoWidth;
}

unsigned int VideoStreamer::Get_VideoHeight(void)
{
  return VideoHeight;
}

unsigned int VideoStreamer::Get_VideoFramerate(void)
{
  return VideoFrame;
}

unsigned int VideoStreamer::Get_VideoDuration(void)
{
  return VideoDuration;
}

void VideoStreamer::Set_VideoWidthHeight(unsigned int width, unsigned int height)
{
  VideoHeight = height;
  VideoWidth = width;
}

void VideoStreamer::Set_VideoFramerate(unsigned int frame)
{
  VideoFrame = frame;
}

void VideoStreamer::Set_VideoDuration(unsigned int duration)
{
  VideoDuration = duration;
}

bool VideoStreamer::isRecording(void)
{
  return Recording;
}

int VideoStreamer::WaitForClient(void)
{
  // First we wait for a client to come along.
  struct sockaddr_in cli_addr;

  socklen_t clilen = sizeof(cli_addr);
  int fp = accept(listen_fd, (struct sockaddr *) &cli_addr, &clilen);
  if ( fp < 0 )
    return -1;
  mtx.lock();
  Client_Name = std::string(inet_ntoa(cli_addr.sin_addr));
  mtx.unlock();
  Msg->Log(EventMsg::NOTICE, "New client: %s", Client_Name.c_str());
  return fp;
}

FILE *VideoStreamer::OpenLocalFile(void)
{
  FILE * file = fopen(VideoFileName, "w");
  if ( file == NULL ) {
    perror("fopen");
    Msg->Log(EventMsg::ERROR,"fopen");
    exit(1);
  }
  return file;
}

FILE *VideoStreamer::OpenPipe(void)
{
  char buffer[BUFFER_SIZE];

//  snprintf(buffer, sizeof(buffer), "ls -l /home/pi/");
  snprintf(buffer, sizeof(buffer), "raspivid -fps %u -t %u -w %u -h %u -o -", (unsigned int) VideoFrame, VideoDuration*1000, (unsigned int)VideoWidth, (unsigned int)VideoHeight);
  Msg->Log(EventMsg::NOTICE, "Command: %s", buffer);
  FILE * fp = popen(buffer, "r");
  if ( fp == NULL) {
    Msg->Log(EventMsg::ERROR,"popen");
    exit(1);
  }
  return fp;
}

int VideoStreamer::RunStream(void)
{
  char buffer[BUFFER_SIZE];
  int client_fp = WaitForClient();
  ssize_t written;
  FILE *pipe_fp = OpenPipe();
  FILE *local_file = OpenLocalFile();

  Recording = true;

  Msg->Log(EventMsg::NOTICE, "While(1)");

  while( !feof(pipe_fp) ) {

    ssize_t bytes_read = fread(buffer, 1, BUFFER_SIZE, pipe_fp);

    written = fwrite(buffer, 1, bytes_read, local_file);
    if ( written != bytes_read ) {
      Msg->Log(EventMsg::ERROR,"Write to file: %d:%u", written, bytes_read);
      break;
    }

    written = write(client_fp, buffer, bytes_read);
    if ( written != bytes_read ) {
      Msg->Log(EventMsg::ERROR,"Write to TCP: %d:%u", written, bytes_read);
      break;
    }
  }

  Msg->Log(EventMsg::NOTICE, "End...");


  Recording = false;

  fclose(local_file);
  pclose(pipe_fp);
  close(client_fp);

  mtx.lock();
  Client_Name.clear();
  mtx.unlock();

  return 0;
}

